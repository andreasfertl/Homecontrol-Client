#include <iostream>
#include "project.h"

#include "MandolynSensor.h"

MandolynSensor::MandolynSensor() :
    m_Id(-1),
    m_Temp(0),
    m_Humidity(0),
    m_Type(MANDOLYNSENSORTYPE::TEMPHUMIDITY)
{
}

MandolynSensor::MandolynSensor(int id, float temp, unsigned int humidity) :
    m_Id(id),
    m_Temp(temp),
    m_Humidity(humidity),
    m_Type(MANDOLYNSENSORTYPE::TEMPHUMIDITY)
{
}

MandolynSensor::MandolynSensor(int id, float temp) :
    m_Id(id),
    m_Temp(temp),
    m_Humidity(0),
    m_Type(MANDOLYNSENSORTYPE::TEMP)
{
}


MandolynSensor::~MandolynSensor()
{

}

json MandolynSensor::toJson() {
	return json{ { "$type", "Messanger.Msg, Messanger"}, { "Value",{ { "$type", "Messanger.MandolynSensor, Messanger" }, { "Temp", m_Temp },{ "Humidity", m_Humidity } ,{ "Id", m_Id } } },{ "Destination", THREADSID::ThreadID::ALL_SUBSCRIBERS },{ "Source", 0 },{ "RemoteHandle", 0 },{ "Command", MESSAGE::CMD::SET },{ "CommandType", MESSAGE::ID::MANDOLYN_SENSOR },{ "ObjectType", "Messanger.MandolynSensor, Messanger, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null" } };
}













	  