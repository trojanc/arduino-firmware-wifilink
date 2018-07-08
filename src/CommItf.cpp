//Communication Interface
#include "CommItf.h"
#include "utility/wifi_utils.h"
#include "SPISlave.h"

/**
 * SPI buffer (limited to 128 bytes)
 */
uint8_t raw_pckt[SPI_BUFFER_SIZE];

/**
 * Check received data (maximum 128 byte (4 * 32))
 */
uint8_t data_received_size = 0;

/**
 * Check data sent for lenght greater than 32 byte (SPI)
 */
bool req_send = false;

/**
 * check data ready to process (SPI)
 */
bool processing= false;

/**
 * need for SPI event
 */
CommItf* This;

/**
 * need to Serial
 */
unsigned long _startMillis;

/**
 * 1 Second Serial Timeout for Serial
 */
unsigned long _timeout = 1000;

CommItf::CommItf(){
}

bool CommItf::begin(){

	Serial.begin(APP_BAUDRATE);
	while(!Serial);
	#ifdef DEBUG
	Serial1.begin(BAUDRATE_DEBUG);
	Serial1.println("--SERIAL started--");
	#endif
	return true;
}

bool CommItf::available(){
	return Serial.available();
}

int CommItf::read(tMsgPacket *_pckt){
	return createPacket(_pckt);
}

/* Cmd Struct Message */
/* _________________________________________________________________________________  */
/*| START CMD | C/R  | CMD  |[TOT LEN]| N.PARAM | PARAM LEN | PARAM  | .. | END CMD | */
/*|___________|______|______|_________|_________|___________|________|____|_________| */
/*|   8 bit   | 1bit | 7bit |  8bit   |  8bit   |   8bit    | nbytes | .. |   8bit  | */
/*|___________|______|______|_________|_________|___________|________|____|_________| */

int CommItf::createPacket(tMsgPacket *_reqPckt){

		#if defined ESP_CH_UART
		String raw_pckt_serial = readStringUntil(END_CMD);
		memcpy(raw_pckt, raw_pckt_serial.c_str(),raw_pckt_serial.length());
		#endif
		
		int idx = 0;
		unsigned char tmp;
		if(raw_pckt[idx] != START_CMD){			//TODO
			return -1;
		}
		_reqPckt->cmd = raw_pckt[idx];
		//The command
		_reqPckt->tcmd = raw_pckt[++idx];

		//The number of parameters for the command
		tmp = raw_pckt[++idx];
		_reqPckt->nParam = tmp;
		//Get each parameter
		for(int a=0; a<(int)_reqPckt->nParam; a++){
			//Length of the parameter
			if( _reqPckt->tcmd >= 0x40 && _reqPckt->tcmd < 0x50 ){
				//16bit tParam
				tmp = (uint16_t)((raw_pckt[++idx] << 8) + (uint8_t)raw_pckt[++idx]);
				_reqPckt->paramsData[a].dataLen = tmp;
				memcpy(_reqPckt->paramsData[a].data,raw_pckt+(++idx),tmp);
				idx = idx+(tmp-1);
			}
			else{
				//8bit tParamData
				tmp = raw_pckt[++idx];
				_reqPckt->params[a].paramLen = tmp;
				memcpy(_reqPckt->params[a].param,raw_pckt+(++idx),tmp);
				idx = idx+(tmp-1);
			}
		}
		return 0;

}

void CommItf::write(uint8_t* _pckt,int transfer_size){
	Serial.write(_pckt,transfer_size);
}

/** Private Functions **/

int CommItf::timedRead(){
	int c;
	_startMillis = millis();
	do {
		c = Serial.read();
		if (c >= 0) return c;
	} while(millis() - _startMillis < _timeout);
	return -1;     // -1 indicates timeout
}

String CommItf::readStringUntil(char terminator){
	String ret;
	int c = timedRead();

	while (c >= 0 && (char)c != terminator)
	{
		ret += (char)c;
		c = timedRead();
	}
	return ret;
}
CommItf CommunicationInterface;
