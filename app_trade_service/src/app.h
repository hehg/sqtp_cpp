#pragma once

#include "sq_strategy_app.h"
#include "sq_app.h"
#include "sq_memory_db.h"
#include "./base/server.h"
using namespace sq;

class app : public sq::sq_app
{

public:
	app();
	~app();
	void init(int argc, char **argv) override;
	void run() override;
    //void on_package(server_cmd*cmd);
	void load_services(const char *xml_path);
	server* get_server(const char* name);

    std::vector<server*> m_servers;
	std::map<std::string, server*> m_servers_map;

	sq::sq_memory_db* m_memory_db=nullptr;
};

extern app the_app;
#define APP (the_app)
