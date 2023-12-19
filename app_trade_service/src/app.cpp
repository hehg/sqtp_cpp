#include "app.h"
#include "xml/rapidxml.hpp"
#include "net/reactor.h"
#include "xml/rapidxml_utils.hpp"
#include "gate_server/gate_server.h"
#include "md_server/md_server.h"
#include "query_server/query_server.h"
#include "data_server/data_server.h"
using namespace sq;

app the_app;
app::app(/* args */)
{
}

app::~app()
{
}
void app::init(int argc, char **argv)
{
	sq_app::init(argc, argv);
	REGISTER_SERVER(query_server);
	REGISTER_SERVER(gate_server);
	REGISTER_SERVER(md_server);
	REGISTER_SERVER(data_server);

	m_memory_db = new sq_memory_db();
	//加载服务
	load_services("../conf/config.xml");

	//消息订阅
	server::subscribe(tid_query_contract_req,get_server("query_server"));
	//启动服务
	auto it = m_servers.begin();
	for (; it != m_servers.end(); it++)
	{
		(*it)->start();
	}
}

// void app::on_package(server_cmd*cmd)
// {
// 	if (cmd->target_server != 0)
// 	{
// 		auto it = m_servers.begin();
// 		for (; it != m_servers.end(); it++)
// 		{
// 			if ((*it)->id() == cmd->target_server)
// 			{
// 				(*it)->put((const char *)cmd, sizeof(server_cmd));
// 			}
// 		}
// 	}
// 	else if (cmd->target_server == 0)
// 	{
// 		auto it = m_servers.begin();
// 		for (; it != m_servers.end(); it++)
// 		{
// 			(*it)->put((const char *)cmd, sizeof(server_cmd));
// 		}
//    }
//    else{
// 	 //sq_panic("not support")
//    }
// }
void app::run()
{
	while (true)
	{
		get_default_reactor()->loop_once();
	}
}
server* app::get_server(const char*name)
{
	auto it = m_servers_map.find(name);
	if(it != m_servers_map.end())
	{
		return it->second;
	}
	return nullptr;

}
/**
 * 启动各个服务
*/
void app::load_services(const char*xml_path)
{
	using namespace std::placeholders;
	using namespace rapidxml;
    rapidxml::file<> xml_file(xml_path);
    xml_document<> doc;
    doc.parse<0>(xml_file.data());

    rapidxml::xml_node<>* root_node = doc.first_node();
    rapidxml::xml_node<>* service_node = root_node->first_node("service");
    assert(service_node);
	rapidxml::xml_node<>* server_node=service_node->first_node("server");
	while(server_node)
	{
		std::string class_name = server_node->first_attribute("class")->value();
		std::string enable = server_node->first_attribute("enable")->value();
		if (enable == "true")
		{
			server *tmp_server = server::create(class_name);
			if(tmp_server==nullptr){
				sq_panic("create server error,%s\n",class_name.c_str());
			}
			assert(tmp_server);
			rapidxml::xml_node<>*setting_note=server_node->first_node();
			while(setting_note){
				std::string key=setting_note->name();
				std::string val=setting_note->value();
				tmp_server->set_option(key.c_str(),(char*)val.c_str());
				setting_note=setting_note->next_sibling();
			}
			tmp_server->open(xml_path);
			//tmp_server->m_post_funct = std::bind(&app::on_package, this, _1);
			m_servers.push_back(tmp_server);
			m_servers_map[class_name]=tmp_server;
		}
		server_node=server_node->next_sibling("server");
			
	}
}
