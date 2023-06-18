#include "sq_contract_table.h"
namespace sq {
    #if 0
    contract_table::~contract_table()
    {
        clear();
    }
    bool contract_table::add_contract(const char* contract)
    {
        auto it = m_table.find(contract);
        if (it == m_table.end()) {
            sq_contract*val = new sq_contract();
            memset(val, 0, sizeof(sq_contract));
            val->contract = contract;
            m_table[contract] = val;
            return true;
        }
        return false;
    }
    bool contract_table::add_contract(sq_contract* contract)
    {
        auto it = m_table.find(contract->contract.to_string());
        if (it == m_table.end()) {
            sq_contract*val = new sq_contract();
            memcpy(val, contract, sizeof(sq_contract));
            m_table[contract->contract.to_string()] = val;
            return true;
        }
        return false;
    }
    void contract_table::clear()
    {
        for (auto i : m_table) {
            delete i.second;
        }
        m_table.clear();
    }
    sq_contract*contract_table::find(const char* contract)
    {
        auto it = m_table.find(contract);
        if (it != m_table.end()) {
            return it->second;
        }
        return nullptr;
    }
    bool contract_table::update(sq_contract&c)
    {
        auto it = m_table.find(c.contract.data());
        if (it != m_table.end()) {
            memcpy(it->second, &c, sizeof(sq_contract));
        }
        return false;
    }
	void contract_table::load_from_csv(const char*csv_path)
	{
		csv_file_reader reader;
		reader.open(csv_path, true);

		while (reader.read_row()) {
			sq_contract *info=new sq_contract();
			strcpy(info->contract.data(), reader['contract'].as_string().c_str());
			strcpy(info->market.data(), reader['market'].as_string().c_str());
            info->tick=reader['tick'].as_int();
			m_table[info->contract.data()] = info;
		}
	}
    #endif 
}