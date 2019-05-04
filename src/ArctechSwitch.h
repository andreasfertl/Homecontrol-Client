#ifndef _ARCTECHSWITCH_INCLUDED
#define _ARCTECHSWITCH_INCLUDED

#include "Message.h"
#include "json.hpp"

//using namespace nlohmann;

using nlohmann::json;
class ArctechSwitch
{
    private:
    
    int m_House;                    //House Code
    int m_Unit;                     //Unit Code
    int m_Group;                    //Group
                                    //these 3 makes a arctech switch unique
    bool m_On;                      //true = ON / false = OFF

    public:
    ArctechSwitch();
    ArctechSwitch(int house, int unit, int group, bool on);
	~ArctechSwitch();
    
    int House() { return m_House; } const
	bool IsOn() { return m_On; } const
	bool IsOff() { return !m_On; } const
	int Unit() {return m_Unit; } const
	int Group() {return m_Group; } const
        
	//return true if the arctech switch is the same, false otherwise
    inline bool operator == (const ArctechSwitch &other) const { return other.m_House == m_House && other.m_Unit == m_Unit && other.m_Group == m_Group; }
    
    //assignment operator just results in updateing values
    inline ArctechSwitch& operator=(const ArctechSwitch& other)
    {
        // check for self-assignment
        if(&other == this)
            return *this;

        //update values
        m_House = other.m_House;
        m_Unit = other.m_Unit;
        m_Group = other.m_Group;
        m_On = other.m_On;
        return *this;
    }

	json toJson();

	static ArctechSwitch from_json(const std::string& str) {
		try
		{
			auto json = json::parse(str);
			auto house = json.at("HouseId").get<int>();
			auto unit = json.at("UnitId").get<int>();
			auto group = json.at("GroupId").get<int>();
			auto on = json.at("State").get<bool>();

			return ArctechSwitch(house, unit, group, on);
		}
		catch (...)
		{
			return ArctechSwitch();
		}
	}


};



#endif //_ARCTECHSWITCH_INCLUDED
