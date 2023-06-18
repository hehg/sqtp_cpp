#pragma once
#include <thread>
#include "mq_cli.h"
#if SUPPORT_MQ
#include "rdkafkacpp.h"

namespace sq
{
	class ExampleDeliveryReportCb : public RdKafka::DeliveryReportCb {
	public:
		void dr_cb(RdKafka::Message &message);
	};
	class ExampleRebalanceCb : public RdKafka::RebalanceCb {
	public:
		void rebalance_cb(RdKafka::KafkaConsumer *consumer,
			RdKafka::ErrorCode err,
			std::vector<RdKafka::TopicPartition*> &partitions);
	};
	class ExampleEventCb : public RdKafka::EventCb {
	public:
		void event_cb(RdKafka::Event &event);
	};
	class MyHashPartitionerCb : public RdKafka::PartitionerCb {
	public:
		int32_t partitioner_cb(const RdKafka::Topic *topic, const std::string *key,
			int32_t partition_cnt, void *msg_opaque);

	};

	/*

	class ExampleConsumeCb : public RdKafka::ConsumeCb {
	public:
		void consume_cb(RdKafka::Message &msg, void *opaque);
	};
*/
	class mq_cli_kafka:public mq_cli
	{
		struct productor_conf {
			RdKafka::Conf *m_conf = nullptr;
			RdKafka::Conf *m_topic_conf = nullptr;
		};
		struct customer_conf {
			RdKafka::Conf *m_conf = nullptr;
			RdKafka::Conf *m_topic_conf = nullptr;
			//std::string topic;
			//int32_t partion = 0;// RdKafka::Topic::PARTITION_UA;
			int64_t offset= RdKafka::Topic::OFFSET_BEGINNING;
		};
	public:
		mq_cli_kafka();
		~mq_cli_kafka();
		virtual int open(const char *server_addr,const char* group_id);
		virtual void sub_topic(const char*topic, int64_t start_offset);
		virtual int close();
		virtual int send_message(const char*topic,void *msg, int msg_len);

		void join();
	private:
		int create_productor();
		int create_customer();
		void on_custom_msg(RdKafka::Message* message);
		void run();
	private:
		
		std::string  m_err_msg;
		
		std::list<RdKafka::Topic*> m_sub_topics;

		productor_conf m_productor_conf;
		customer_conf  m_custom_conf;

		ExampleDeliveryReportCb ex_dr_cb;
		ExampleRebalanceCb ex_rebalance_cb;
		ExampleEventCb ex_event_cb;

		RdKafka::Producer *m_producer = nullptr;
		RdKafka::Consumer* m_customer = nullptr;

		std::thread* m_thread=nullptr;
	};
} // namespace bt
#endif