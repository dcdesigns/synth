/*
* SerialMidi.cpp
* Created by Ethan Bailey, 03/14/20
*/

#include <Bela.h>
#include "SerialMidi.hpp"
#include "lcdLib.h"
#include <string.h>
#include "settings.h"
#include "midiHandler.h"
#include "helperFunctions.h"

void SerialMidi::setup (Midi *m, const char dev[], int bd)
{
    _midi = m;
    _serial_fd.fd = setupSerialDevice (dev, bd);
    _serial_fd.events = POLLIN;
    
}

int SerialMidi::setupSerialDevice (const char dev[], int bd)
{
    /*
    derived from this code:
    https://gist.githubusercontent.com/peterhurley/fbace59b55d87306a5b8/raw/220cfc2cb1f2bf03ce662fe387362c3cc21b65d7/anybaud.c
    */

    int fd;
    struct termios2 tio;

    // open device for read
    fd = open(dev, O_RDWR | O_NOCTTY | O_ASYNC);

    //if can't open file
    if (fd < 0) {
        //show error and exit
        perror(dev);
        return (-1);
    }

    if (ioctl(fd, TCGETS2, & tio) < 0)
        perror("TCGETS2 ioctl");

    tio.c_cflag &= ~CBAUD;
    tio.c_cflag |= BOTHER | CS8 | CLOCAL | CREAD; // Baud rate, 8N1, local modem, receive chars
    tio.c_iflag = IGNPAR; // ignore parity errors
    tio.c_oflag = 0; // raw output
    tio.c_lflag = 0; // non-canonical
    tio.c_cc[VTIME] = 0; // don't use inter-char timer
    tio.c_cc[VMIN] = 1; // block read until 1 char arrives
    tio.c_ispeed = bd;
    tio.c_ospeed = bd;

    if (ioctl(fd, TCSETS2, & tio) < 0)
        perror("TCSETS2 ioctl");

    return fd;
}

void SerialMidi::checkSerialMidi(uint8_t byte)
{
	static int8_t goal_cnt = -1;
	static int8_t cnt = 0;
	static uint8_t bytes[3];
    static char buf[22] = { 0 };
    static int locked = 0;

    if (SHIFTMASK(MAINTOG, bitMidiThru))
    {
        write(_serial_fd.fd, &byte, 1);

    }
    //rt_printf("raw %u\n", byte);

	if(byte & 0x80) //command byte
	{
		
		uint8_t cmd_type = byte & 0xF0;
        
        //ignore sysex messages (for now... not sure what they are used for)
        if (cmd_type == 0xF0) return;
	
		//types that only have 1 data byte
		if(cmd_type == 0xC0 || cmd_type == 0xD0)
		{
			goal_cnt = 2;
		}
		//all others have 2 data bytes
		else
		{
			goal_cnt = 3;
		}		
        cnt = 0;
	}
	
    if(goal_cnt > 0) //data byte
	{
		
		bytes[cnt] = byte;
		++cnt;
        
		//end of message: pass it on
		if(cnt == goal_cnt)
		{
            
            addToNotesQueue(bytes[0], bytes[1], bytes[2]);
            //rt_printf("midi %u %u %u\n", bytes[0], bytes[1], bytes[2]);
            cnt = 1;
		}
	}
}


void SerialMidi::readSerialDevice()
{
    uint8_t byte;
    //rt_printf("reading midi\n");
	
    // read midi message
   while (!gShouldStop)
    {
        // 1 second timeout on MIDI messages
        poll (&_serial_fd, 1, 1000);
        if (_serial_fd.revents & POLLIN)
        {
            read(_serial_fd.fd, &byte, 1);
            checkSerialMidi(byte);
        }
    }
}

/*
Bela aux task callback for serial reads
*/
void readTask (void *arg)
{
    SerialMidi *sm = (SerialMidi *) arg;
    sm->readSerialDevice();
}

void SerialMidi::startRead()
{
    //rt_printf("starting midi\n");
    //Bela_scheduleAuxiliaryTask (Bela_createAuxiliaryTask (readTask, 85, "serial MIDI", this));
}

void SerialMidi::stopRead()
{
    close(_serial_fd.fd);
}