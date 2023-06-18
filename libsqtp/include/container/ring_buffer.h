/**
Copyright (c) 2003-2015
@brief 无锁队列

@author
*/
#pragma once
#include <atomic>
#include <stdint.h>
#include <assert.h>
#include <thread>
#include "sync/sq_spin_lock.h"
using namespace std;

namespace sq
{
	/** 
	 * @brief 无锁队列,固定大小
	 */
	template <typename T>
	class ring_buffer
	{
	public:
		explicit ring_buffer(int count = 4096)
		{
			assert(((count > 0) && ((count & (~count + 1)) == count)));
			m_w_count = count;
			m_r_count = count;
			m_buffer = new T[count];
			m_w_count_mask = count - 1;
			m_r_count_mask = count - 1;
			m_writer_idx.store(0);
			m_reader_idx.store(0);
		}

		~ring_buffer()
		{
			delete[] m_buffer;
		}
		// 申请内存 写入
		T *back()
		{
			if (full())
				return nullptr;
			auto tmp = m_writer_idx.load(std::memory_order_relaxed);
			return &m_buffer[tmp & m_w_count_mask];
		}
		// 移动writer
		bool push()
		{
			auto tmp = m_writer_idx.load(std::memory_order_relaxed);
			m_writer_idx.store(tmp + 1, std::memory_order_relaxed);
			return true;
		};
		// 读取
		T *front()
		{
			if (empty())
				return nullptr;
			auto tmp = m_reader_idx.load(std::memory_order_relaxed);
			return &m_buffer[tmp & m_r_count_mask];
		}
		// 移动读取游标
		bool pop()
		{
			auto tmp = m_reader_idx.load(std::memory_order_relaxed);
			m_reader_idx.store(tmp + 1, std::memory_order_relaxed);
			return true;
		}

		T *operator[](int64_t idx)
		{
			int64_t real_idx = m_reader_idx.load(std::memory_order_relaxed) + idx;
			return &m_buffer[real_idx & m_r_count_mask];
		}
		T *at(int64_t idx)
		{
			int64_t real_idx = m_reader_idx.load(std::memory_order_relaxed) + idx;
			return &m_buffer[real_idx & m_r_count_mask];
		}

		int get_real_count() { return m_writer_idx.load(std::memory_order_relaxed) - m_reader_idx.load(std::memory_order_relaxed); }

	private:
		bool inline full() { return (m_writer_idx.load(std::memory_order_relaxed) -
									 m_reader_idx.load(std::memory_order_relaxed)) == m_w_count; }
		bool inline empty()
		{
			return (m_writer_idx.load(std::memory_order_relaxed) == m_reader_idx.load(std::memory_order_relaxed));
		}
		T *m_buffer;

		alignas(64) atomic<int64_t> m_writer_idx;
		int64_t m_w_count;
		int64_t m_w_count_mask;

		alignas(64) atomic<int64_t> m_reader_idx;
		int64_t m_r_count;
		int64_t m_r_count_mask;

		ring_buffer(const ring_buffer &) = delete;
		ring_buffer &operator=(const ring_buffer &) = delete;
	};

	/**
	 * 无锁队列,动态大小
	 * |-----^----------^--------|
	 */
	struct MsgHeader
	{
		uint16_t size;
	};
	template <int MemBytes>
	class ring_buffer_var
	{
		static constexpr uint32_t BLK_CNT = MemBytes / 64;
		static_assert(BLK_CNT && !(BLK_CNT & (BLK_CNT - 1)), "BLK_CNT must be a power of 2");

	public:
		char *alloc(uint16_t size)
		{
			size += sizeof(MsgHeader);
			uint32_t blk_sz = (size + sizeof(Block) - 1) / sizeof(Block);
			if (blk_sz > BLK_CNT)
				return nullptr;
			asm volatile(""
						 : "=m"(read_idx)
						 :
						 :); // force read memory
			uint32_t used_sz = write_idx - read_idx;
			if (used_sz == BLK_CNT)
			{
				// full
				// printf("rb full write_idx=%d\n", write_idx);
				return nullptr;
			}

			uint32_t tail_padding_sz = 0;
			uint32_t head_padding_sz = 0;
			if (read_idx % BLK_CNT == 0)
			{
				if (read_idx == write_idx)
					head_padding_sz = BLK_CNT;
				else
				{
					head_padding_sz = 0;
				}
			}
			else
			{
				head_padding_sz = (read_idx % BLK_CNT);
			}

			if (write_idx % BLK_CNT == 0)
			{
				tail_padding_sz = 0;
			}
			else
			{
				tail_padding_sz = BLK_CNT - (write_idx % BLK_CNT);
			}
#if 0
			printf("blk_sz=%u,write_idx=%u,readd_idx=%u,head_padding_sz=%d,tail_padding_sz=%d\n",
			 blk_sz, write_idx, read_idx, head_padding_sz,
			 tail_padding_sz);
#endif
			if (blk_sz > tail_padding_sz && blk_sz > head_padding_sz)
			{
				return nullptr;
			}
			if (blk_sz > tail_padding_sz)
			{
				blk[write_idx % BLK_CNT].header.size = 0;
				write_idx += tail_padding_sz;
			}

			MsgHeader &header = blk[write_idx % BLK_CNT].header;
			header.size = size;
			char *body = ((char *)&header) + sizeof(MsgHeader);
			// return &header;
			return body;
		}

		void push()
		{
			asm volatile(""
						 :
						 : "m"(blk), "m"(write_idx)
						 :); // memory fence
			uint32_t blk_sz = (blk[write_idx % BLK_CNT].header.size + sizeof(Block) - 1) / sizeof(Block);
			write_idx += blk_sz;
			// printf("write_idx change to %u\n",write_idx);
			asm volatile(""
						 :
						 : "m"(write_idx)
						 :); // force write memory
		}

		char *front()
		{

			asm volatile(""
						 : "=m"(write_idx), "=m"(blk)
						 :
						 :); // force read memory
			if (read_idx == write_idx)
			{
				return nullptr;
			}
			uint16_t size = blk[read_idx % BLK_CNT].header.size;
			if (size == 0)
			{ // rewind
				read_idx += BLK_CNT - (read_idx % BLK_CNT);
				if (read_idx == write_idx)
				{
					return nullptr;
				}
			}
			MsgHeader &header = blk[read_idx % BLK_CNT].header;
			char *body = ((char *)&header) + sizeof(MsgHeader);
			return body;
			// return &blk[read_idx % BLK_CNT].header;
		}

		void pop()
		{
			asm volatile(""
						 : "=m"(blk)
						 : "m"(read_idx)
						 :); // memory fence
			uint32_t blk_sz = (blk[read_idx % BLK_CNT].header.size + sizeof(Block) - 1) / sizeof(Block);
			read_idx += blk_sz;
			asm volatile(""
						 :
						 : "m"(read_idx)
						 :); // force write memory
		}

		// private:
		struct Block
		{
			alignas(64) MsgHeader header;
		} blk[BLK_CNT];

		alignas(128) uint32_t write_idx = 0;
		alignas(128) uint32_t read_idx = 0;
	};

	// 上面的两个buffer 要通过两次函数调用，这个buffer 通过一次调用

	template <int COUNT>
	struct sq_ring_buffer
	{
		alignas(64) volatile atomic<uint32_t> m_prod_head;
		volatile atomic<uint32_t> m_prod_tail;
		int m_prod_size;
		int m_prod_mask;

		alignas(64) volatile atomic<uint32_t> m_cons_head;
		volatile atomic<uint32_t> m_cons_tail;
		int m_cons_size;
		int m_cons_mask;

		alignas(64) void *m_ring[COUNT];

		sq_ring_buffer()
		{
			m_prod_size = COUNT;
			m_prod_mask = COUNT - 1;
			m_prod_head = m_prod_tail = 0;

			m_cons_size = COUNT;
			m_cons_mask = COUNT - 1;
			m_cons_head = m_cons_tail = 0;
		}

		int push(void *buf)
		{
			uint32_t prod_head, prod_next, cons_tail;

			do
			{
				prod_head = m_prod_head.load();
				prod_next = (prod_head + 1) & m_prod_mask;
				cons_tail = m_cons_tail.load();

				if (prod_next == cons_tail)
				{
					if (prod_head == m_prod_head.load() &&
						cons_tail == m_cons_tail.load())
					{
						return (ENOBUFS);
					}
					continue;
				}
			} while (!m_prod_head.compare_exchange_weak(prod_head, prod_next));

			m_ring[prod_head] = buf;

			while (m_prod_tail.load() != prod_head)
			{
				std::this_thread::yield();
			}

			m_prod_tail.store(prod_next);

			return (0);
		}

		char *pop()
		{
			uint32_t cons_head, cons_next;
			void *buf;

			do
			{
				cons_head = m_cons_head.load();
				cons_next = (cons_head + 1) & m_cons_mask;

				if (cons_head == m_prod_tail.load())
				{
					return (NULL);
				}
			} while (!m_cons_head.compare_exchange_weak(cons_head, cons_next));

			buf = m_ring[cons_head];

			while (m_cons_tail.load() != cons_head)
			{
				std::this_thread::yield();
			}

			m_cons_tail.store(cons_next);

			return (char*)(buf);
		}

		bool full()
		{

			return (((m_prod_head + 1) & m_prod_mask) == m_cons_tail);
		}

		bool empty()
		{

			return (m_cons_head == m_prod_tail);
		}
	};

} // namespace sq
