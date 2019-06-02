#ifndef SERIALMIDI_C
#define SERIALMIDI_C

#include "./settings.h"
#include "./midiHandler.h"

static const SerialConfig sd2Cfg = {31250, 0, 0, 0};
		
void initSerialMidi()
{
	palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7)|PAL_MODE_INPUT);// RX
	sdStart(&SD2, &sd2Cfg);
}



void checkSerialMidi()
{
	static uint8_t cmd;
	static uint8_t cnt = 0;
	static uint8_t data[2];
	
	while(!sdGetWouldBlock(&SD2)) 
	{
        uint8_t byte = sdGet(&SD2);
		
		if(byte & 0x80) //command byte
		{
			cnt = 0; //clear any old data
			
			uint8_t upr = byte & 0xF0;
			if(upr != 240 && upr != 192) 
			{
				cmd = byte;
			}
			
		}
		else //data byte
		{
			data[cnt] = byte;
			cnt++;
			if(cnt > 1)
			{
				//LogTextMessage("a");
				addToNotesQueue(cmd, data[0], data[1]);
				//if((cmd & 0xF0) == NOTEON || (cmd & 0xF0) == NOTEOFF) addToNotesQueue(cmd, data[0], data[1]);
				//else addToCCQueue(cmd, data[0], data[1]);
				//else cc stuff
				//LogTextMessage("hi %u %u %u", cmd, data[0], data[1]);
				cnt = 0;
			}
		}
	}
}








#endif 




	