#include <iostream>
#include "project.h"
#include "ArctechSwitch.h"

ArctechSwitch::ArctechSwitch() :
    m_House(-1),
    m_Unit(-1),
    m_Group(-1),
    m_On(false)
{
}

ArctechSwitch::ArctechSwitch(int house, int unit, int group, bool on) :
    m_House(house),
    m_Unit(unit),
    m_Group(group),
    m_On(on)
{
}


ArctechSwitch::~ArctechSwitch()
{
}

json ArctechSwitch::toJson() {
	auto house = House();
	auto unit = Unit();
	auto group = Group();
	auto on = IsOn();

	return json{ { "$type", "Messanger.Msg, Messanger" },{ "Value",{ { "$type", "Messanger.ArctechSwitch, Messanger" }, { "HouseId", house },{ "UnitId", unit } ,{ "GroupId", group },{ "State", on } } },{ "Destination", THREADSID::ThreadID::ALL_SUBSCRIBERS },{ "Source", 0 },{ "RemoteHandle", 0 },{ "Command", MESSAGE::CMD::SET },{ "CommandType", MESSAGE::ID::ARCTECH_SWITCH },{ "ObjectType", "Messanger.ArctechSwitch, Messanger, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null" } };
}











	  