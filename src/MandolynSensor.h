#ifndef _MANDOLYNSENSOR_INCLUDED
#define _MANDOLYNSENSOR_INCLUDED

#include "Message.h"
#include "json.hpp"


//using namespace nlohmann;

using nlohmann::json;

class MandolynSensor
{
    private:
    
    enum class MANDOLYNSENSORTYPE {
        TEMPHUMIDITY,
        TEMP
			 };
    int m_Id;                    //should be unique
    float m_Temp;                //actual temperature in C
    unsigned int m_Humidity;     //actual Humidity in %
    MANDOLYNSENSORTYPE m_Type;   //Type of the Sensor Data
    
    
    public:
    MandolynSensor();
    MandolynSensor(int id, float temp, unsigned int humidity);
    MandolynSensor(int id, float temp);
	~MandolynSensor();
    
    bool IsTempHumidity() { return m_Type == MANDOLYNSENSORTYPE::TEMPHUMIDITY; }
    bool IsTemp() { return m_Type == MANDOLYNSENSORTYPE::TEMP; }
    
    float Temp() { return m_Temp; }
    unsigned int Humidity() { return m_Humidity; }
    int Id() { return m_Id; }
    
	//return true if the sensor id is equal, false otherwise
    inline bool operator == (const MandolynSensor &other) const { return other.m_Id == m_Id; }
    
    //assignment operator just results in updateing values
    inline MandolynSensor& operator=(const MandolynSensor& other)
    {
        // check for self-assignment
        if(&other == this)
            return *this;

        //update values
        m_Id = other.m_Id;
        m_Type = other.m_Type;
        m_Temp = other.m_Temp;
        m_Humidity = other.m_Humidity;
        return *this;
    }

	json toJson();

	static MandolynSensor from_json(const std::string& str) {
		try
		{
			auto json = json::parse(str);
			auto id = json.at("id").get<int>();
			auto temp = json.at("temp").get<float>();
			auto type = json.at("type").get<std::string>();

			if (type == "temphumidity")
			{
				auto humidity = json.at("humidity").get<int>();

				return MandolynSensor(id, temp, humidity);
			}
			else
			{
				return MandolynSensor(id, temp);
			}
		}
		catch (...)
		{
			return MandolynSensor();
		}
	}
};


#endif //_MANDOLYNSENSOR_INCLUDED
