#pragma once
#include <atomic>
namespace sq
{
	class sq_spin_lock
	{
	public:
	private:
		std::atomic < bool > flag = ATOMIC_VAR_INIT(false);
	public:
		sq_spin_lock() = default;
		sq_spin_lock(const sq_spin_lock &) = delete;
		sq_spin_lock & operator= (const sq_spin_lock) = delete;
		void lock()
		{ 
			bool expected = false;
			while (flag.compare_exchange_strong(expected, true));
			expected = false;
		}
		void unlock()
		{ 
			flag.store(false);
		}
	};
}