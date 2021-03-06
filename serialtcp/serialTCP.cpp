#include "Arduino.h"
#include <inttypes.h>
#include "serialTCP.h"
#include <WiFiUdp.h>

serialUDP SerialUDP;
serialTCP SerialTCP;

//=====================================================================================
//=====================================================================================
//=====================================================================================
serialUDP::serialUDP() {
	_ip="";
	_port=0;
}

serialUDP::~serialUDP() {
}

//=====================================================================================
size_t serialUDP::write(uint8_t data)
{
	_client.beginPacket(_ip.c_str(),_port); 
	_client.write(&data,sizeof(data));
	_client.endPacket();
	delay(1);
	
	Serial.write(data);

	return 1;
}

#define udpsendlimit 512  // more or less random, I found no data for the esp8266.

//=====================================================================================
size_t serialUDP::write(const char *str)
{
	if (str == NULL) return 0; 

	_client.beginPacket(_ip.c_str(),_port); 
	int i = strlen(str);
	if (i>udpsendlimit) {
		i=udpsendlimit;
	}
	_client.write((const uint8_t *)str, i);
	_client.endPacket();
	delay(1);
	
	return Serial.write((const uint8_t *)str, strlen(str)); 
}

//=====================================================================================
size_t serialUDP::write(const uint8_t *buffer, size_t size)
{
	_client.beginPacket(_ip.c_str(),_port); 
	int i = size;
	if (i>udpsendlimit) {
		i=udpsendlimit;
	}
	_client.write(buffer,i);
	_client.endPacket();
	delay(1);

	return Serial.write((const uint8_t *)buffer, size);
}

//=====================================================================================
void serialUDP::begin(long baud, String ip, uint16_t port, bool reconnect)
{
	_port=port;
	_ip=ip;
	
	if (baud>0) {
		Serial.begin(baud);
	}
}


//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
serialTCP::serialTCP() {
	_reconnect=true;
	_ip="";
	_port=0;
}

serialTCP::~serialTCP() {
	closetcp();
}

//*************************************************************************************
size_t serialTCP::write(uint8_t data)
{
	if (! _client) {
		opentcp();  
	}

	if (_client) {
		_client.write(data);
	} 
	
	Serial.write(data);

	return 1;
}

//*************************************************************************************
size_t serialTCP::write(const char *str)
{
	if (str == NULL) return 0; 

	if (! _client) {
		opentcp();  
	}

	if (_client) {
		_client.write((const uint8_t *)str, strlen(str)); 
	} 
	
	return Serial.write((const uint8_t *)str, strlen(str)); 
}

//*************************************************************************************
size_t serialTCP::write(const uint8_t *buffer, size_t size)
{
	if (! _client) {
		opentcp();  
	}

	if (_client) {
		_client.write((const uint8_t *)buffer, size);
	} 
	
	return Serial.write((const uint8_t *)buffer, size);
}


//*************************************************************************************
void serialTCP::opentcp()
{
	if (_ip!="") {
		if (!_client.connect(_ip.c_str(),_port)) {
			Serial.println("Debug cant open ");
			if (! _reconnect) {
				_ip="";
			}
			return;
		}
	}
}

//*************************************************************************************
void serialTCP::closetcp()
{
	if (_client) {
		_client.stop();
	}
}

//*************************************************************************************
void serialTCP::begin(long baud, String ip, uint16_t port, bool reconnect)
{
	_port=port;
	_ip=ip;
	_reconnect=reconnect;
	closetcp();
	
	if (baud>0) {
		Serial.begin(baud);
	}
}
