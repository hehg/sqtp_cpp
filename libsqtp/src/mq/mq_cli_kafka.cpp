#include "mq/mq_cli_kafka.h"
#include <assert.h>
#include <iostream>
#include "log/sq_logger.h"
#if SUPPORT_MQ
namespace sq{
	void ExampleDeliveryReportCb::dr_cb(RdKafka::Message &message)
	{

	}
	void ExampleRebalanceCb::rebalance_cb(RdKafka::KafkaConsumer *consumer,
		RdKafka::ErrorCode err,
		std::vector<RdKafka::TopicPartition*> &partitions) {


	}
	
	void  ExampleEventCb::event_cb(RdKafka::Event &event) {

	}
	int32_t MyHashPartitionerCb::partitioner_cb(const RdKafka::Topic *topic,
		const std::string *key,
		int32_t partition_cnt, void *msg_opaque)
	{
		return 0;
	}


	/*static inline unsigned int djb_hash(const char *str, size_t len) {
		unsigned int hash = 5381;
		for (size_t i = 0; i < len; i++)
			hash = ((hash << 5) + hash) + str[i];
		return hash;
	}*/

#if 0
	void ExampleConsumeCb::consume_cb(RdKafka::Message &msg, void *opaque)
	{
		const RdKafka::Headers *headers;
		RdKafka::Message *message = &msg;

		switch (message->err()) {
		case RdKafka::ERR__TIMED_OUT:
			break;

		case RdKafka::ERR_NO_ERROR:
			/* Real message */
			std::cout << "Read msg at offset " << message->offset() << std::endl;
			if (message->key()) {
				std::cout << "Key: " << *message->key() << std::endl;
			}
			headers = message->headers();
			if (headers) {
				std::vector<RdKafka::Headers::Header> hdrs = headers->get_all();
				for (size_t i = 0; i < hdrs.size(); i++) {
					const RdKafka::Headers::Header hdr = hdrs[i];

					if (hdr.value() != NULL)
						printf(" Header: %s = \"%.*s\"\n",
							hdr.key().c_str(),
							(int)hdr.value_size(), (const char *)hdr.value());
					else
						printf(" Header:  %s = NULL\n", hdr.key().c_str());
				}
			}
			printf("%.*s\n",
				static_cast<int>(message->len()),
				static_cast<const char *>(message->payload()));
			break;

		case RdKafka::ERR__PARTITION_EOF:
			/* Last message */
			/*if (exit_eof) {
				run = false;
			}*/
			break;

		case RdKafka::ERR__UNKNOWN_TOPIC:
		case RdKafka::ERR__UNKNOWN_PARTITION:
			std::cerr << "Consume failed: " << message->errstr() << std::endl;
//			run = false;
			break;

		default:
			/* Errors */
			std::cerr << "Consume failed: " << message->errstr() << std::endl;
//			run = false;
		}
	}
#endif


	mq_cli_kafka::mq_cli_kafka()
	{
	}
	mq_cli_kafka::~mq_cli_kafka() 
	{
	}
	int mq_cli_kafka::open(const char *server_addr, const char* group_id)
	{
		/*printf("open %s\n", conf_path);
		boost::property_tree::ptree pt;
		boost::property_tree::ini_parser::read_ini(conf_path, pt);*/
		//m_custom_conf.topic = pt.get<std::string>("custom.topic");
		//get config
		
		m_server_addr = server_addr;
		m_group_id = group_id;

		//m_product_conf.topic = pt.get<std::string>("product.topic");
		
		create_productor();
		create_customer();

		m_thread = new std::thread(std::bind(&mq_cli_kafka::run, this));
	    return 0;
	}
	void mq_cli_kafka::sub_topic(const char*topic, int64_t start_offset)
	{
		RdKafka::Topic *kafka_topic=RdKafka::Topic::create(m_customer, topic,
			m_custom_conf.m_topic_conf, m_err_msg);
		m_sub_topics.push_back(kafka_topic);
		if (start_offset == 0) {
			auto ret2 = m_customer->start(kafka_topic, 0, RdKafka::Topic::OFFSET_BEGINNING);
		}
		else if (start_offset == -1) {
			auto ret2 = m_customer->start(kafka_topic, 0, RdKafka::Topic::OFFSET_END);
		}
		else if(start_offset>0) {
			auto ret2 = m_customer->start(kafka_topic, 0, start_offset);
		}
		else {
			assert(false);
		}
		//
		//auto ret2 = m_customer->start(kafka_topic, 0, RdKafka::Topic::OFFSET_END);
		//auto ret2 = m_customer->start(kafka_topic, 0, RdKafka::Topic::OFFSET_BEGINNING);
	}
	int mq_cli_kafka::create_productor()
	{
		//create config
		m_productor_conf.m_conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
		m_productor_conf.m_topic_conf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

		auto ret = m_productor_conf.m_conf->set("metadata.broker.list", m_server_addr, m_err_msg);

		m_productor_conf.m_conf->set("event_cb", &ex_event_cb, m_err_msg);

		ret = m_productor_conf.m_conf->set("dr_cb", &ex_dr_cb, m_err_msg);
		ret= m_productor_conf.m_conf->set("default_topic_conf", 
			m_productor_conf.m_topic_conf, m_err_msg);

		
		m_producer = RdKafka::Producer::create(m_productor_conf.m_conf, m_err_msg);
		
		return 0;
	}
	int mq_cli_kafka::create_customer()
	{
		m_custom_conf.m_conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
		m_custom_conf.m_topic_conf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
		auto ret = m_custom_conf.m_conf->set("group.id", m_group_id, m_err_msg);
		ret = m_custom_conf.m_conf->set("default_topic_conf",
			m_custom_conf.m_topic_conf, m_err_msg);
		 ret= m_custom_conf.m_conf->set("rebalance_cb", &ex_rebalance_cb, m_err_msg);
		// ret = m_conf->set("enable.partition.eof", "true", m_err_msg);
		ret = m_custom_conf.m_conf->set("enable.auto.commit", "true", m_err_msg);
		 ret = m_custom_conf.m_conf->set("metadata.broker.list", m_server_addr, m_err_msg);
		m_customer = RdKafka::Consumer::create(m_custom_conf.m_conf, m_err_msg);
		
		return 0;
	}

    int mq_cli_kafka:: close()
    {
		if (m_customer) {
			//m_customer->stop(m_topic, m_custom_conf.partion);
			delete m_customer;
			m_customer = nullptr;
		}
		if (m_producer) {
			delete m_producer;
			m_producer = nullptr;
		}
		auto it = m_sub_topics.begin();
		for (; it != m_sub_topics.end(); ++it)
		{
			delete *it;
		}
		m_sub_topics.clear();

        return 0; 
    }
    int mq_cli_kafka::send_message(const char*topic,void *msg, int msg_len)
    {
		if (m_producer == nullptr)
			return 1;
		RdKafka::Headers *headers = RdKafka::Headers::create();
		
		log_info(log_info,"send package to kafka len={},topic={}\n", msg_len, topic);
		//headers->add("my header", "header value");
		//headers->add("other header", "yes");

		RdKafka::ErrorCode resp =
			m_producer->produce(topic, RdKafka::Topic::PARTITION_UA,
				RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
				/* Value */
				msg, msg_len,
				/* Key */
				NULL, 0,
				/* Timestamp (defaults to now) */
				0,
				/* Message headers, if any */
				nullptr,/*headers,*/
				/* Per-message opaque value passed to
				* delivery report */
				NULL);
		if (resp != RdKafka::ERR_NO_ERROR) {
			std::cerr << "% Produce failed: " <<
				RdKafka::err2str(resp) << std::endl;
			//delete headers; /* Headers are automatically deleted on produce()
							
		}
		m_producer->poll(0);
		while (m_producer->outq_len() > 0) {
			//std::cerr << "Waiting for " << m_producer->outq_len() << std::endl;
			m_producer->poll(1000);
		}
        return 0;
    }
	void mq_cli_kafka::on_custom_msg(RdKafka::Message* message)
	{
		const RdKafka::Headers *headers;
		if (message) {
			switch (message->err()) {
			case RdKafka::ERR__TIMED_OUT:
				break;

			case RdKafka::ERR_NO_ERROR:
				/* Real message */
				/*std::cout << "Read msg at offset " << message->offset() << std::endl;
				if (message->key()) {
					std::cout << "Key: " << *message->key() << std::endl;
				}*/
				
				headers = message->headers();
				if (headers) {
					std::vector<RdKafka::Headers::Header> hdrs = headers->get_all();
					for (size_t i = 0; i < hdrs.size(); i++) {
						const RdKafka::Headers::Header hdr = hdrs[i];

						if (hdr.value() != NULL)
							printf(" Header: %s = \"%.*s\"\n",
								hdr.key().c_str(),
								(int)hdr.value_size(), (const char *)hdr.value());
						else
							printf(" Header:  %s = NULL\n", hdr.key().c_str());
					}
				}
				/*printf("%.*s\n",
					static_cast<int>(message->len()),
					static_cast<const char *>(message->payload()));*/
				if (m_hander) {
					//message->topic_name();
					m_hander(message->topic_name().c_str(),
						message->payload(),
						static_cast<int>(message->len()));
				}
				break;

			case RdKafka::ERR__PARTITION_EOF:
				/* Last message */
				/*if (exit_eof) {
					run = false;
				}*/
				printf("RdKafka::ERR__PARTITION_EOF\n");
				break;

			case RdKafka::ERR__UNKNOWN_TOPIC:
			case RdKafka::ERR__UNKNOWN_PARTITION:
				std::cerr << "Consume failed: " << message->errstr() << std::endl;

				break;

			default:
				/* Errors */
				std::cerr << "Consume failed: " << message->errstr() << std::endl;

			}
		}
	}
	void mq_cli_kafka::run()
	{
		while (true) {
			if (m_producer) {
				m_producer->poll(0);
			}
			if (m_customer) {
				auto it = m_sub_topics.begin();
				for(;it!= m_sub_topics.end();++it){

					RdKafka::Message *msg = m_customer->consume(*it, 0, 10);
					if (msg) {
						on_custom_msg(msg);
						delete msg;
					}
					else {
						break;
					}
					m_customer->poll(0);
				}
				
			}
			if(m_producer==nullptr && m_customer==nullptr)
				std::this_thread::sleep_for(std::chrono::microseconds(10));
		}
	}

	void mq_cli_kafka::join()
	{
		if (m_thread) {
			m_thread->join();
		}
	}
    
} // bt

#endif

 