#include "bus_client.h"

namespace bus
{

   bus_client::bus_client(int id, sq::ws_channel *ch)
   {
      m_channel = ch;
      m_id = id;
   }

   bus_client::~bus_client()
   {
      m_channel = nullptr;
   }
   /**
    * @brief 订阅 topic
   */
   void bus_client::on_sub_topic( bus_subcribe_req *req)
   {
      m_sub_topics.insert(req->topic);
   }

   void bus_client::on_pub_topic(sq::sq_package*package,bus_client*sender)
   {
      if(sender == this)
      {
         return;
      }
      //support this topic
      if(m_sub_topics.find(package->get_topic()) != m_sub_topics.end())
      {
         //转发给客户端
        m_channel->send_ws_package(package->get_data(),package->get_size());
      }
   }
}