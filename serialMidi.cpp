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
	static int8_t goal_cnt = -1;
	static int8_t cnt = 0;
	static uint8_t data[2];
	
	while(!sdGetWouldBlock(&SD2)) 
	{
        uint8_t byte = sdGet(&SD2);
		
		if(byte & 0x80) //command byte
		{
			cnt = 0; //clear any old data
			cmd = byte;
			uint8_t cmd_type = cmd & 0xF0;
			
			//ignore sysex messages (for now... not sure what they are used for)
			if(cmd_type == 240)
			{
				goal_cnt = -1;
			}
			//types that only have 1 data byte
			if(cmd_type == MIDI_PROGRAM_CHANGE || cmd_type == MIDI_CHANNEL_PRESSURE)
			{
				goal_cnt = 1;
			}
			//all others have 2 data bytes
			else
			{
				goal_cnt = 2;
			}		
		}
		else if(goal_cnt > 0) //data byte
		{
			
			data[cnt] = byte;
			++cnt;
			
			//end of message: pass it on
			if(cnt == goal_cnt)
			{
				addToNotesQueue(cmd, data[0], data[1]);
				cnt = 0;
				
				if(SHIFTMASK(MAINTOG, bitMidiThru))
				{
					if(goal_cnt == 2)
					{
						//serial_MidiSend2(cmd, data[0], data[1]);
						MidiSend3((midi_device_t) MIDI_DEVICE_DIN, 0, cmd, data[0], data[1]);
					} 
					else
					{
						//serial_MidiSend2(cmd, data[0]);
						MidiSend2((midi_device_t) MIDI_DEVICE_DIN, 0, cmd, data[0]);
					}
				}
				
			}
		}
	}
}








#endif 




	