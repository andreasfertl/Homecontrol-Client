# Homecontrol-Client
Homecontrol Client application running on Linux which provides support for HW dependent modules.

This is an earlier version of the Homecontrol-Server project and has been transformed to a Client module to abstract the HW dependencies / SW dependencies.

This client support local telldus/tellstick lightswitches/remote controllers and does have some dependencies to older SW versions of supporting libraries.


See a more precise description of the whole project in the Homecontrol-Server repository
https://github.com/andreasfertl/Homecontrol-Server

## Prerequisites:
Linux

gcc (on Linux)

Git

boost

JSON for Modern C++
https://github.com/nlohmann/json


# Instructions
### Linux
On Ubuntu 18.10 i installed via apt these packages to be able to compile the software

telldus
pre dependencie libconfuse1
```
Linux:~/$ dpkg --install http://ftp.de.debian.org/debian/pool/main/c/confuse/libconfuse-common_3.0+dfsg-2+deb9u1_all.deb
Linux:~/$ dpkg --install http://ftp.us.debian.org/debian/pool/main/c/confuse/libconfuse1_3.0+dfsg-2+deb9u1_amd64.deb
```
prepare apt to be able to install telldus
```
Linux:~/$ wget -q http://download.telldus.com/debian/telldus-public.key -O- | sudo apt-key add -
```
edit /etc/apt/sources.list to include (add):
```
deb http://download.telldus.com/debian/ unstable main
```
install telldus
```
Linux:~/$ apt install telldus-core libtelldus-core2 libtelldus-core-dev -y
```
install other dependencies
```
Linux:~/$ apt install build-essential libbluetooth-dev libcurl4-openssl-dev libboost-graph-dev libboost-iostreams-dev -y
```

In the src folder you find the Makefile.
```
Linux:~/LinuxClient$ cd src
Linux:~/LinuxClient/src$ make
Linux:~/LinuxClient/src$ ./LinuxClient
```

# Configuration
The configuration of this software is done by a .xml file (LinuxClientConfig.xml).

### Example config
See the provided LinuxClientConfig.xml in the homecontrol folder.

#### Explanation:
Since this software communicates via messages between modules every configuration item which ends up as a message (e.g. Lights) need to be configured with a unique id (InternalId).

```
<?xml version="1.0"?>
<Configuration>

<Logger>
	<DbgLvl>3</DbgLvl>
	<LogLvl>3</LogLvl>
</Logger>

<TCPClient>
  <IP>10.0.1.127</IP>
  <Port>5005</Port>
  <MsgID1>ARCTECH_SWITCH</MsgID1>
  <MsgID2>BLUETOOTHDEVICE_IN_RANGE</MsgID2>
  <MsgID3>MANDOLYN_SENSOR</MsgID3>
</TCPClient>
  
<Person>
	<Name>Phone of 123</Name>
	<BTDeviceAddr>12:34:56:78:9A:BC</BTDeviceAddr>
</Person>
  
<Person>
  <Name>Phone of ABC</Name>
	<BTDeviceAddr>AA:BB:CC:DD:EE:FF</BTDeviceAddr>
</Person>

<Threads>
	<ThreadID1>TELLDUSMANAGER</ThreadID1>
	<CycleTime1>50</CycleTime1>
	<HystereseTime1>5</HystereseTime1>

	<ThreadID2>BLUETOOTHMANAGER</ThreadID2>
	<CycleTime2>3000</CycleTime2>
	<HystereseTime2>500</HystereseTime2>

  <ThreadID3>TCPMANAGER</ThreadID3>
	<CycleTime3>1</CycleTime3>
	<HystereseTime3>0</HystereseTime3>
</Threads>

<MsgMap>
	<ThreadID>TELLDUSMANAGER</ThreadID>
  <MsgID1>TELLDUSDEVICE_SET_STATE</MsgID1>
</MsgMap>
  
<MsgMap>
	<ThreadID>TCPMANAGER</ThreadID>
	<MsgID1>ARCTECH_SWITCH</MsgID1>
  <MsgID2>BLUETOOTHDEVICE_IN_RANGE</MsgID2>
  <MsgID3>MANDOLYN_SENSOR</MsgID3>
</MsgMap>

</Configuration>

```

