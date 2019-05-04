#ifndef _MESSAGES_INCLUDED
#define _MESSAGES_INCLUDED

#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <typeinfo>
#include "threadsid.h" 


namespace MESSAGE
{
	enum class ID {									//DATA type
				SUBSCRIBE,
				RUN,
				LogMessage,
				LogLevel,
				STOP_ALL,
				LIGHTCONTROL, 
				BLUETOOTHDEVICE_IN_RANGE, ////BLUETOOTHDEVICE::BluetoothDeviceMessage
				CodeswitchButtonMessage,           
				TempMessage,         
				SunriseSunset,
				SSHClientControl,
				MusicControl,
				MusicMessageState,
				ConfigurationMessage,
				MusicMessageVolume,
				ALIVE_MESSAGE,
				ARCTECH_SWITCH, //ArctechSwitch with on/off
				MANDOLYN_SENSOR, // MandolySensor with temp & humidity
				TELLDUSDEVICE_SET_STATE, //TelldusDeviceSetMessage bool with handle corresponding to /etc/tellstick.conf
				INVALID,
			 };
			 
	enum class CMD {
				SET,
				GET,
				ANSW,
				ANY,
				ACK,
				INVALID
			 };
			 
	typedef unsigned int MsgDataType;
	
	std::string IdToString(ID id);
	ID IdFromString(std::string id);
}




class Message
{
	private:
		MESSAGE::CMD         m_Cmd;        //SET/GET
		THREADSID::ThreadID  m_DestId;     //thread id from
		THREADSID::ThreadID  m_SrcId;      //thread id to
		MESSAGE::ID          m_MsgId;
		boost::any           m_Val;        //the actual value itself
		
		template <typename T>
		T ConvertAnyVal()
		{
			try 
			{ 
				return boost::any_cast<T>(this->m_Val);
			} 
			catch (boost::bad_any_cast &e) 
			{ 
				std::ostringstream strMsg;
				strMsg << "CastException: " << e.what();
				Logger_Write(LoggLevel::ERROR, strMsg.str());
			}
			catch (...) 
			{ 
				Logger_Write(LoggLevel::ERROR, (std::string)"unhandled Exception in ConvertAnyVal");
			}
			return T {}; 
		}
		

		
	protected:

	public:
		Message(MESSAGE::CMD Cmd, THREADSID::ThreadID DestId, THREADSID::ThreadID SrcId, MESSAGE::ID MsgId, MESSAGE::MsgDataType Handle, boost::any Val) : m_Cmd(Cmd), m_DestId(DestId), m_SrcId(SrcId), m_MsgId(MsgId), m_Val(Val){}
		Message(MESSAGE::CMD Cmd, THREADSID::ThreadID DestId, THREADSID::ThreadID SrcId, MESSAGE::ID MsgId, boost::any Val) : m_Cmd(Cmd), m_DestId(DestId), m_SrcId(SrcId), m_MsgId(MsgId), m_Val(Val){}
		Message(MESSAGE::CMD Cmd, THREADSID::ThreadID DestId, THREADSID::ThreadID SrcId, MESSAGE::ID MsgId) : m_Cmd(Cmd), m_DestId(DestId), m_SrcId(SrcId), m_MsgId(MsgId){}
		Message(Message &other)
		{
			m_Cmd     = other.GetCmd();
			m_DestId  = other.GetDest();
			m_SrcId   = other.GetSrc();
			m_MsgId   = other.GetMsgId();
			m_Val     = other.GetAnyValue();
		}
		Message() { };
		~Message();
		
		THREADSID::ThreadID   GetDest()   { return m_DestId; }
		THREADSID::ThreadID   GetSrc()    { return m_SrcId;  }
		MESSAGE::ID           GetMsgId()  { return m_MsgId;  }
		MESSAGE::CMD          GetCmd()    { return m_Cmd; }
		
		boost::any GetAnyValue() { return m_Val; }
		template <typename T>
		T GetValue() { return ConvertAnyVal<T>(); }
		std::string TypeName() { const std::type_info &ti = m_Val.type(); return ti.name(); }
        const std::type_info& Type() { return m_Val.type(); }
        bool IsType(const std::type_info& type) { return m_Val.type() == type; }

};



#endif // _MESSAGES_INCLUDED
