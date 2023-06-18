#include "sq_frame.h"
#include "sq_struct_def.h"
#include "sq_struct_def.h"
#include "sq_mdb.h"
#include "error/sq_err.h"
#include "log/sq_logger.h"
#include "xml/rapidxml.hpp"
#include "xml/rapidxml_utils.hpp"
#include "plug/sq_plug_obj.h"
extern void plugs_callback_func(uint16_t tid, char*data, uint16_t size, void*param, int plug_id);
 static std::vector<plug_obj*> m_md_plugs;

 void load_md_plugs(const char*cfg_path)
{
	using namespace rapidxml;
	rapidxml::file<> fdoc(cfg_path);
	rapidxml::xml_document<> doc;
	doc.parse<0>(fdoc.data());

	xml_node<> *root = doc.first_node();
	xml_node<> *plugs = root->first_node("plug_md");
	xml_node<>*plug = plugs->first_node("plug");
	int plug_id = 0;
	while (plug)
	{
		xml_attribute<> *att_enable = plug->first_attribute("enable");
		xml_attribute<> *att_path = plug->first_attribute("path");
		if (att_enable && strcmp(att_enable->value(), "true") == 0)
		{
			plug_obj *obj = new plug_obj();

			int load_ret = obj->load(att_path->value());
			if (load_ret != 0)
			{
				assert(false);
				delete obj;
			}

			obj->plug_create();
			obj->plug_id_ = plug_id;
			++plug_id;
			m_md_plugs.push_back(obj);
			obj->plug_set_option(PLUG_OPT_id, &obj->plug_id_);
            obj->plug_set_option(PLUG_OPT_callback,(void*)plugs_callback_func);
           // obj->plug_set_option(PLUG_OPT_callback_param,);

			xml_node<> *opt = plug->first_node();
			while (opt)
			{
				obj->plug_set_option(opt->name(), opt->value());
				opt = opt->next_sibling();
			}
		}

		plug = plug->next_sibling();
	}
	{
		auto it = m_md_plugs.begin();
		for (; it != m_md_plugs.end(); ++it)
		{
			(*it)->plug_open();
		}
	}
}
void unload_md_plugs()
{
    auto it = m_md_plugs.begin();
    for (; it != m_md_plugs.end(); ++it)
    {
        (*it)->plug_close();
        (*it)->plug_destory();
    }
    m_md_plugs.clear();
}