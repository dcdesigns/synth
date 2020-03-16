#ifndef LCDLIB_C
#define LCDLIB_C


#include "./settings.h"
#include "./helperfunctions.c"

const uint8_t maxBytes = 64;
static uint8_t i2cWritePtr[maxBytes + 1] __attribute__ ((section (".sram2")));	//the right type of memory for i2c write buffer										
static uint8_t *readByte;
const uint8_t graphCols = 128;
int8_t vals[12000] = {0};

//static const uint8_t rowAddrs[] = {0x00, 0x40, 0x14, 0x54};
/* const uint8_t rcl[18][6][3] = {
	//wav
	{
		{1,0,9},{1,10,9},
		{2,0,9},{2,10,9},
		{3,0,9},{3,10,9}
	},
	//amp
	{
		{1,9,3},{1,16,3},
		{2,9,3},{2,16,3},
		{2,9,3},{2,16,3},
	},
	//pitch
	{
		{1,7,3},{0,0,0},
		{2,11,7},{0,0,0},
		{3,9,3}, {3,16,3}
	},
	//pit env
	{
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0},
		{3,6,3}, {0,0,0}
	},
	//filt
	{
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0},
		{3,4,3}, {0,0,0}
	},
	//filt env
	{
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0}
	},
	//arp
	{
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0}
	},
	//arpnotes
	{
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0}
	},
	//patchload
	{
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0}
	},
	//patch save
	{
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0}
	},
	//midi
	{
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0}
	},
	//midicc
	{
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0}
	},
	//pan
	{
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0}
	},
	//mod
	{
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0}
	},
	//notes
	{
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0}
	},
	//arprec
	{
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0}
	},
	//fav
	{
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0},
		{0,0,0},{0,0,0}
	},
	//harmonics
	{
		{1,6,0},{0,0,0},
		{2,5,1},{2,17,2}, 
		{3,6,3},{3,17,4}
	}
		
}; */


 

//const char waveStr[2][6] = {"NOISE", "FILE"};
const char saveCopyStr[2][7] = {"SAVED.", "DONE."};
const char oscStr[6][5] = {"POL1", "POL2", "MON1", "MON2", "MON3", "MON4"}; 
const char lvlStr[3][4] = {"LVL", "MST"};
const char arpNoteLeader[3][4] = {" P", " E:", " V:"};
const char yesNoStr[2][4] = {"NO", "YES"};
const char onOffStr[2][4] = {"OFF", "ON"};
const char startStopStr[2][7] = {"*START", "*STOP:"};
const char copyWhat[2][4] = {"ALL", "ARP"};
const char envStr[5][6] = {"PITCH", "F.CUT", "RES"};
const char filtStr[3][4] = {"LPF", "BPF", "HPF"};
const char stageStr[5][4] = {"1ST", "2ND", "3RD", "SUS", "REL"};
const char notesStr[2][4] = {"PIT", "VEL"};
const char trackStr[2][4] = {"YES", "ENV"};
const char velStr[3][7] = {"NONE","KEYVEL","WINDCC"};
const char loopStr[4][6] = {"TRIG-", "LOOP-", "SKIP", "ALL"};
const char modStrA[5][5] = {"----", "MAIN", "AUDM", "AUDL", "AUDR"};
const char modStrB[9][4] = {"WND", "MDW", "PBD", "SUS", "OUT", "AMP", "PIT", "FLT", "ARP"};
const char modStrO[6][3] = {"P1", "P2", "M1", "M2", "M3", "M4"}; 

const char recStr[5] = "*REC";
const char timeStr[6] = "TIME:";
const char typeStr[6] = "TYPE:";
const char favStr[2][21] = {"MAKE SURE THIS PATCH", "  HAS BEEN SAVED!   "};
const char nts[12][4] = {"C  ", "C #", "D  ", "D #","E  ","F  ","F #", "G  ","G #", "A  ","A #", "B  "};
const char units[3][3] = {"s", "Hz", "kH"};


//global variables
//LCDelem LCD_layout[ENCODERS + 3];
static int8_t backlightStatus;										//backlight setting
static int8_t displayStatus;										//display setting
static const I2CConfig i2cfg = {									//i2c object
	OPMODE_I2C,
	100000,			// 100000 is the value of the Arduino Wire library
	FAST_DUTY_CYCLE_2,	
};

void __attribute__(( noinline )) sendGraphicCmd(uint8_t byte)
{
	i2cWritePtr[0] = 0x80;
	i2cWritePtr[1] = byte;
	i2cMasterTransmit(&I2CD1, 0x3c, i2cWritePtr, 2, readByte, 0);
}



void __attribute__(( noinline ))  updateGraphic()
{
	int32_t *wavPtr = NULL;
	int8_t graphic[graphCols+1][2] = {0};
	uint8_t suslen = 10;
	GRAPH_update = 0;
	uint32_t strt = ticks;
	uint8_t vertA = 200;
	uint8_t vertB = 200;
		
	if(screenInd == AMPENV)
	{
		uint8_t *ptr = &amp_env_knobs[oscInd].rate[0];
		int8_t sh = 6;
		uint32_t cycles = 0;
		uint16_t majCycle[7];
		uint8_t majPos[7];
		float curCycle = 0;
		uint8_t curPos = 0;
		uint8_t majInd = 0;
		uint8_t curVal = 0;
		uint32_t val = 0;
		
		//attack
		vals[0] = val;
		majCycle[0] = 0;
		uint32_t att = (ATTACK_K[*ptr] < MAX_INT32>>sh)? ATTACK_K[*ptr]<<sh: MAX_INT32;
		while(val < MAX_INT32)
		{
			cycles++;
			val += att;
			if(val > MAX_INT32) val = MAX_INT32;
			vals[cycles] = val>>graphShift;
		}
		majCycle[1] = cycles;
		
		//remaining portions
		for(uint8_t i = 0; i < 3; ++i)
		{
			int32_t goal;
			int32_t rate = SEEK[*(ptr + 1 + i)];
			if(i < 2) goal = GAIN[*(ptr + 4 + i)];
			else goal = 0;
			int32_t diff = goal-val;
			rate = (rate > (MAX_INT32 >>sh))? MAX_INT32 : rate <<sh;
			do
			{
				val += (___SMMUL(diff, rate))<<1;
				
				cycles++;
				diff = goal-val;
				vals[cycles] = (val > MAX_INT32)? 0: val>>graphShift;

			} while(diff > 40000 || diff < -40000);
			majCycle[2+i] = cycles;
		}
		
		float perCol = (float)cycles/(graphCols-suslen);
		if(perCol < 1)
		{
			perCol = 1;
			suslen = graphCols - cycles;
		}
		
		//insert sus line and add an extra slot (since we count one extra cycle)
		majCycle[5] = majCycle[4];
		majCycle[6] = majCycle[4];
		majCycle[4] = suslen;
		
		
		//get the rounded versions of major points (to make sure everything gets visible)
		for(uint8_t i = 0; i < 7; ++i)
		{
			majPos[i] = float(majCycle[i])/perCol;
		}
		
		//get the interpolated graph points
		for(uint8_t i = 0; i < graphCols+1; ++i)
		{
			//if we're on the sus line, don't increment the cycle and hold the same value
			if(majInd == 4)
			{
				if(!(--suslen)) majInd++;
			}
			else
			{
				uint16_t ind = curCycle;
				
				//if we're at a major point, ignore the actual cycle and use the major cycle
				if(curPos >= majPos[majInd])
				{
					ind = majCycle[majInd];
					majInd++;
				}
				curVal =vals[ind];
				curCycle += perCol;
				++curPos;
			}
			graphic[i][0] = curVal;
		}
	}
	else if(screenInd == PITENV || screenInd == FILTENV)
	{
		/* struct PIT_ENV_KNOBS {
			uint8_t time[FREE_STAGES];
			uint8_t glide[FREE_STAGES + 2];
			int32_t pitch[FREE_STAGES + 2];	
		}; */
		PIT_ENV_KNOBS *ptr = (screenInd == PITENV)? &pit_env_knobs[oscInd] : &filt_env_knobs[oscInd];
		int8_t sh = 5;
		uint32_t cycles = 0;
		uint16_t majCycle[7];
		uint8_t majPos[7];
		float curCycle = 0;
		uint8_t curPos = 0;
		int8_t curVal = 0;
		int32_t val = 0;
		uint8_t majInd = 1;
		uint8_t susMaj;
		uint8_t max = 1;
		float scl;
		majCycle[0] = 0;
		
		for(uint8_t i = 0; i < FREE_STAGES + 2; ++i)
		{
			uint16_t limit;
			if(i < FREE_STAGES)
			{
				limit = TIME[ptr->time[i]] >> sh;
				if(ptr->time[i] && limit < 1) limit = 1;
			}
			else limit = 0x7FFF;
			int32_t rate = SEEK[ptr->glide[i]];
			int32_t goal = ptr->pitch[i]; 

			int32_t diff = goal-val;
			
			rate = (rate > (MAX_INT32 >>sh))? MAX_INT32 : rate <<sh;
			uint16_t curCyc = 0;
			if(limit)
			{
				do
				{
					val += (___SMMUL(diff, rate))<<1;
					cycles++;
					diff = goal-val;
					vals[cycles] = (val>>21);
					int8_t abs = val>>21;
					if(abs < 0) abs = -abs;
					if(abs > max) max = abs;
					curCyc++;

				} while((i<FREE_STAGES && curCyc < limit) || (i>=FREE_STAGES && (diff > 40000 || diff < -40000)));
				majCycle[majInd++] = cycles;
				if(i == FREE_STAGES) susMaj = majInd++;
			}
		}
		scl = (float)31.5/max;
		if(scl > 4) scl = 4;
		//LogTextMessage("cyc %u %u %f" , cycles, max, scl);
		majCycle[majInd] = majCycle[majInd-1];
		vals[0] = vals[cycles];
		float perCol = (float)cycles/(graphCols-suslen);
		if(perCol < 1)
		{
			perCol = 1;
			suslen = graphCols - cycles;
		}
		majCycle[susMaj] = suslen;

		//get the rounded versions of major points (to make sure everything gets visible)
		for(uint8_t i = 0; i < majInd + 1; ++i)
		{
			majPos[i] = float(majCycle[i])/perCol;
		}
		
		majInd = 0;
		
		//get the interpolated graph points
		for(uint8_t i = 0; i < graphCols+1; ++i)
		{
			//if we're on the sus line, don't increment the cycle and hold the same value
			if(majInd == susMaj)
			{
				if(!(--suslen)) majInd++;
			}
			else
			{
				uint16_t ind = curCycle;
				
				//if we're at a major point, ignore the actual cycle and use the major cycle
				if(curPos >= majPos[majInd])
				{
					ind = majCycle[majInd];
					majInd++;
				}
				curVal =vals[ind];
				curCycle += perCol;
				++curPos;
			}
			//LogTextMessage("%d", curVal);
			graphic[i][0] = curVal*scl + 32;
		}
	}
	/* else if(screenInd == ARPEGSETUP || screenInd == ARPEGNOTES)
	{

		ARP_KNOBS *ptr = &arpeggio[oscInd];
		int8_t sh = 5;
		uint32_t cycles = 0;
		uint16_t majCycle[65];
		uint8_t majPos[65];
		float curCycle = 0;
		uint8_t curPos = 0;
		int8_t curVal = 0;
		int32_t val = 0;
		uint8_t majInd = 1;
		uint8_t susMaj;
		uint8_t max = 1;
		float scl;
		majCycle[0] = 0;
		uint16_t limit = ptr->T >> sh;
		//LogTextMessage("h %d %d", 0x7ffffff, 0x7fffffff>>1);
		int32_t rate = SEEK[ptr->G];
		rate = (rate > (MAX_INT32 >>sh))? MAX_INT32 : rate <<sh;
		uint8_t isVel = 1;
		for(uint8_t i = 0; i < 4; ++i)
		{
			if(arpToggle[i] != VEL_TOG)
			{
				isVel = 0;
				break;
			}
		}
		
		for(uint8_t i = 0; i < ptr->steps; ++i)
		{
			//int32_t goal = ptr->P[i]<<PITCH_COARSE; 
			int32_t goal = (isVel)? ((int32_t)VELGAIN[ptr->V[i]]-(0x7fffffff>>1))>>3 : ptr->P[i]<<PITCH_COARSE; 
			//LogTextMessage("g %d", goal);
			int32_t diff = goal-val;	
			uint16_t curCyc = 0;

			do
			{
				val += (___SMMUL(diff, rate))<<1;
				cycles++;
				diff = goal-val;
				vals[cycles] = (val>>21);
				int8_t abs = val>>21;
				if(abs < 0) abs = -abs;
				if(abs > max) max = abs;
				curCyc++;

			} while(curCyc < limit);
			majCycle[majInd++] = cycles;
		}
		scl = (float)31.5/max;
		if(scl > 4) scl = 4;
		//LogTextMessage("cyc %u %u %f" , cycles, max, scl);
		majCycle[majInd] = majCycle[majInd-1];
		vals[0] = 0;
		float perCol = (float)cycles/(graphCols);

		//get the rounded versions of major points (to make sure everything gets visible)
		for(uint8_t i = 0; i < majInd + 1; ++i)
		{
			majPos[i] = float(majCycle[i])/perCol;
		}
		
		majInd = 0;
		
		//get the interpolated graph points
		for(uint8_t i = 0; i < graphCols+1; ++i)
		{
			uint16_t ind = curCycle;
				
			//if we're at a major point, ignore the actual cycle and use the major cycle
			if(curPos >= majPos[majInd])
			{
				ind = majCycle[majInd];
				majInd++;
			}
			curVal =vals[ind];
			curCycle += perCol;
			++curPos;

			//LogTextMessage("%d", curVal);
			graphic[i][0] = curVal*scl + 32;
		}
	} */
	else if(screenInd == WAVETBL || screenInd == PHASE || screenInd == HARMONIC)
	{
		if(SHIFTMASK(oscInd, bitWave))
		{
			//wavPtr = (SHIFTMASK(oscInd, bitHarms))? &harmArray[oscInd][0]: &wavArray[oscInd][0];
			wavPtr = &harmArray[oscInd][0];


			for(uint16_t i = 0; i < graphCols + 1; ++i)
			{
				graphic[i][0] = (*(wavPtr + ( (i<<2) & 0x1ff)) >> 26) + 32; 
			}
			
			if(screenInd == PHASE && SHIFTMASK(oscInd, bitPhase))
			{
				if(phase_knobs[oscInd].before_harm) vertA = phase_knobs[oscInd].before_phase >> 1;
				if(phase_knobs[oscInd].after_harm) vertB = phase_knobs[oscInd].after_phase >> 1;
			}
		}
	}
	
	uint8_t next = graphic[0][0];
	uint8_t cur;

	for(uint8_t i = 0; i < graphCols; ++i)
	{
		cur = next;
		next = graphic[i+1][0];
		
		if(cur <= next)
		{
			graphic[i][1] = next;
		}
		else
		{
			graphic[i][0] = next;
			graphic[i][1] = cur;
		}
	}
		
	for(uint8_t page = 0; page < 8; page++)
	{
		sendGraphicCmd(LOW_COLUMN_ADDRESS | 2);
		sendGraphicCmd(HIGH_COLUMN_ADDRESS | 0);
		sendGraphicCmd(SET_PAGE_ADDRESS | page);
		
		uint8_t upr = (8-page) * 8 - 1;
		uint8_t rem = graphCols;
		uint8_t ind = 0;
		//LogTextMessage("h");
		while(rem)
		{
			uint8_t amt = (rem < maxBytes)? rem: maxBytes;
			
			memset(&i2cWritePtr[1], 0x0, amt);
			i2cWritePtr[0] = 0x40;
			//if(wavPtr != NULL)
			{
				for(uint8_t i = 0; i < amt; ++i)
				{
					int8_t start = upr - graphic[i + ind][1];
					if(start < 0) start = 0;
					int8_t end = upr - graphic[i + ind][0];
					if(end  > 7) end = 7;
	
					if(start <= end && start < 8)
					{
						i2cWritePtr[i + 1] = (0xff << start) & (0xff >> (7-end));
						start++;
					}
					if(i + ind == vertA || i + ind == vertB)
					{
						i2cWritePtr[i + 1] = 0xff;
					}
				}
			}
			
			i2cMasterTransmit(&I2CD1, 0x3c, i2cWritePtr, amt + 1, readByte, 0);
			rem -= amt;
			ind += amt;
		}
		
		  
	}
	//LogTextMessage("%u t", ticks-strt);
	//LogTextMessage("max %d", max);
}

/* void updateGraphic()
{
	sendGraphicCmd(SET_DISP | 1);
	
	
	int32_t *wavPtr = NULL;
	int8_t wave[graphCols] = {0};
	
	GRAPH_update = 0;
	
	if(SHIFTMASK(oscInd, bitWave))
	{
		wavPtr = (SHIFTMASK(oscInd, bitHarms))? &harmArray[oscInd][0]: &wavArray[oscInd][0];
	}
	
	int32_t max = 0;
	for(uint16_t i = 0; i < graphCols; ++i)
	{
		wave[i] = ((*(wavPtr + 4 * i)) >> 26) + 32;
	}
	uint8_t cnt = 0;
	for(uint8_t page = 0; page < 8; page++)
	{
		sendGraphicCmd(LOW_COLUMN_ADDRESS | 2);
		sendGraphicCmd(HIGH_COLUMN_ADDRESS | 0);
		sendGraphicCmd(SET_PAGE_ADDRESS | page);
		
		uint8_t upr = (8-page) * 8 - 1;
		uint8_t rem = graphCols;
		uint8_t ind = 0;
		
		while(rem)
		{
			uint8_t amt = (rem < maxBytes)? rem: maxBytes;
			
			memset(&i2cWritePtr[1], 0x0, amt);
			i2cWritePtr[0] = 0x40;
			if(wavPtr != NULL)
			{
				for(uint8_t i = 0; i < amt; ++i)
				{
					int8_t dist = upr - wave[i + ind];
					if(dist > -1 && dist < 8)
					{
						cnt++;
						i2cWritePtr[i + 1] |= (1 << dist);
						//LogTextMessage("page %u, ind %u, %d, %u", page, i + ind, wave[i + ind], cnt);
					}
				}
			}
			
			i2cMasterTransmit(&I2CD1, 0x3c, i2cWritePtr, amt + 1, readByte, 0);
			rem -= amt;
			ind += amt;
		}
		  
	}
	//LogTextMessage("max %d", max);
} */

//higher level functions
void initLCD()
{	
	//configure I2C pins
	palSetPadMode(GPIOB, 8, PAL_MODE_ALTERNATE(4)|PAL_STM32_PUDR_PULLUP|PAL_STM32_OTYPE_OPENDRAIN);// SCL
	palSetPadMode(GPIOB, 9, PAL_MODE_ALTERNATE(4)|PAL_STM32_PUDR_PULLUP|PAL_STM32_OTYPE_OPENDRAIN);// SDA
	
	//start an i2c object?
	i2cStart(&I2CD1, &i2cfg);
	
	//turn on backlight
	toggleBackLight(1);
	
	//wait to make sure power is fully up
	chThdSleepMilliseconds(200);
	toggleBackLight(1); // backlight on
	
	//initialization sequence
	uint8_t codes[4] = {0x30, 0x30, 0x30, 0x20};
	for(uint8_t i = 0; i < 4; ++i)
	{
		send4BitsAndClear(codes[i]);
		chThdSleepMilliseconds(5);
	}

	
	//set to 4-bit mode
	/* send4BitsAndClear(0x20);
	chThdSleepMilliseconds(5); */
	
	//now in 4-bit mode
	
	//set lines and font
	sendByte(0x28, Cmd); //2 or more lines
/* 	else
		sendByte(0x20, Cmd); //1 line */
	
	toggleDisplay(0); 		//display off
	clearDisplay(); 		//clear
	sendByte(0x06, Cmd); 	//increment on write, don't bit display
	toggleDisplay(1); 		//display back on
	
	sendGraphicCmd(SCAN_DIR | flip_scan);
	sendGraphicCmd(RE_MAP | flip_map);
	sendGraphicCmd(SET_DISP | 1);
	GRAPH_update = 1;	
	
}



void __attribute__(( noinline )) writeStr(uint8_t row, uint8_t col, uint8_t len, char *str)
{	
	//move cursor to the right position
	setCursorPosition(row, col);
	//LogTextMessage("writing");
	//write the string
	uint8_t skip = 0;
	for(int i = 0; i < len; ++i)
	{
		uint8_t ch;
		if(i >= strlen(str)) ch = ' ';//sendByte((uint8_t)(' '), Rs);
		else if(str[i] == '>') ch = 126;//sendByte(126, Rs);
		else if(str[i] == '<') ch = 127;//sendByte(127, Rs);
		else ch = str[i];//sendByte((uint8_t)(str[i]), Rs);
		if(ch == '@') skip = 1;
		else
		{
			if(skip)
			{
				//LogTextMessage("r");
				setCursorPosition(row, col + i);
				skip = 0;
			}
			sendByte(ch, Rs);
		}
	}
}

	

void  __attribute__(( noinline )) setCursorPosition(uint8_t row, uint8_t col)
{
	uint8_t rowAddrs[] = {0x00, 0x40, 0x14, 0x54};
	if (row > ROWS-1) 
		row = ROWS - 1;

	if(col > COLS-1)
		col = COLS-1;

	sendByte(0x80 | (rowAddrs[row] + col), Cmd);
}

void toggleBackLight(uint8_t on)
{
	backlightStatus = (on > 0) ? Bl : 0;
	sendByte(0, Cmd);
}

void toggleDisplay(uint8_t on)
{
	displayStatus = (on > 0) ? 0x04 : 0;
	sendByte(0x08 | displayStatus, Cmd);
}

void clearDisplay()
{
	sendByte(0x01, Cmd);
}

void  __attribute__(( noinline )) toggleBlinkCursor(uint8_t on)
{
	uint8_t tByte = (on > 0) ? (0x01) : 0;
	sendByte(0x08 | displayStatus | tByte, Cmd);
}

void  __attribute__(( noinline )) updateLCDelems(uint8_t first, uint8_t last)
{
	for(uint8_t i = first; i <= last; ++i)
		LCD_update[i] = 1;
}

void __attribute__(( noinline )) checkWriteElem()
{
	char tempStr[21];
	strcpy(tempStr, "");
	static uint8_t curLCD = 0;
	
	
	uint8_t isPitchEnv = 0;
	uint8_t simple = LCD_update[curLCD];
	if(LCD_update[curLCD] > 0)
	{
		LCD_update[curLCD] = 0;
		//GRAPH_update = 1;
		//osc number
		
		if(curLCD == OSC)
		{ 
			//oscStr(oscInd, tempStr);
			writeStr(0, 0, 5, (char *)oscStr[oscInd]);

		}
		//volume/level
		else if(curLCD == LVL)
		{
			intToStr((char *)lvlStr[isMainLVL],(isMainLVL)? (int16_t)main_gain: (int16_t)(osc_gain[oscInd]), 3, 0, 0, tempStr);
			writeStr(0, 5, 7, tempStr);
		}
		
		//screen label
		else if(curLCD == SCRN)
		{
			toggleBlinkCursor(screenInd == ARPEGSETUP || screenInd == PATCHSV);
			writeStr(0, 12, 8, (char *)SCREENS[screenInd]); 
			for(uint8_t i = 0; i < 3; ++i) writeStr(i+1, 0, 20, (char *)screens[screenInd][i]);
		} 
		

		
		//object labels
		/* else if(simple > 1)
		{
			uint8_t ind = curLCD-OBJ1;
			LogTextMessage("obj %u val %u %u %u %u", ind, simple, rcl[screenInd][ind][0], rcl[screenInd][ind][1], rcl[screenInd][ind][2]);
			writeBasicInt((int16_t)simple,rcl[screenInd][ind][2], 0, rcl[screenInd][ind][0],rcl[screenInd][ind][1]);
		} */
		else
		{
			
			switch(screenInd)
			{
				case WAVETBL:
				case PATCHLD:
				{	
					//"01234567890123456789"
					//"@@@@@@@@@ @@@@@@@@@ ",
					//"@@@@@@@@@>@@@@@@@@@<",
					//"@@@@@@@@@ @@@@@@@@@ "
					filsList *fil;
					uint8_t fType;
					if(screenInd == WAVETBL)
					{
						fType = WAVE;
						fil = curWavFile[oscInd];
					}
					else
					{
						fType = PATCH;
						fil = curPatchFile;
					}
					if(browseCnt[fType].files)
					{
						uint8_t line = ((curLCD-OBJ1)>>1);
						  
						if((curLCD-OBJ1) & 1)
						{
							if(line == 0) fil = fil->prev;
							else if(line == 2) fil = fil->next;
							writeNumSpaceStr(fil->filInd, fil->name, line + 1, 10, 9);
						}
						else 
						{
							uint8_t dir = fil->dirInd;
							do
							{
								dir = indexIncrement(dir, line-1, browseCnt[fType].dirs);
							}while(dirs[fType][dir].numFiles == 0 && line != 1 && dir != fil->dirInd);
							
							writeNumSpaceStr(dir + 1, dirs[fType][dir].name, line + 1, 0, 9);
						}
					}
				} 
				break;
				
				case AMPENV:
				{
					//"01234567890123456789"
					//" TIME: A:@@@  D:@@@ ",
					//"       S:@@@  R:@@@ ",
					//" TRGT: D:@@@  S:@@@ "
					
					
					uint8_t ind = curLCD - OBJ1;
					writeBasicInt((int16_t)*((&amp_env_knobs[oscInd].rate[0]) + ind),3, 0, (ind >> 1) + 1, (ind & 0x01)? 16: 9);
				}
				break;
				
				case PITCH:
				{
					// "01234567890123456789"
					// "LEGATO:@@@          ",
					// "TRANSPOSE: @@@@@@@  ",
					// "GLIDE: P:@@@  V:@@@ "
		
		
					PIT_KNOBS *curPits = &pit_knobs[oscInd];
					uint8_t arrInd = curLCD - OBJ1;
					if(arrInd > 0) arrInd--;
					
					switch(curLCD)
					{
						case OBJ1: writeStr(1, 7, 3, (char *)yesNoStr[SHIFTMASK(oscInd, bitLgto)]); break;
						
						case OBJ3:								
							pitchStr(tempStr, curPits->pitch + ((oscInd < POLY_CNT)? 0: note[firstChild[oscInd]]), (oscInd < POLY_CNT) || SHIFTMASK(oscInd, bitNotes), MIDI_KEY_0, monoPitch[oscInd]);
							writeStr(2, 11, 7, tempStr);
							break;
							
						case OBJ5: writeBasicInt((int16_t)curPits->pit_glide,3, 0, 3, 9); break;	
						case OBJ6: writeBasicInt((int16_t)curPits->vel_glide,3, 0, 3, 16); break;
						
						
					}
				}
				break;
					
				case PITENV: isPitchEnv = 1;
				case FILTENV:
				{
					//"01234567890123456789"
					//"STAGE:@@@ @@@@@@@@@@",
					//"GOAL:@@@@@@@ @@@@@@@",
					//"GLIDE:@@@ @@@@@@@@@@"
					PIT_ENV_KNOBS *curEnv = (isPitchEnv)? &pit_env_knobs[oscInd] : &filt_env_knobs[oscInd];
					switch(curLCD)
					{
						case OBJ1: writeStr(1, 6, 3, (char *)stageStr[envInd]); break;	
						
						case OBJ2:
						{
							if(!isPitchEnv) strCatCat((char *)typeStr, (char *)envStr[(SHIFTMASK(oscInd, bitFECut))? 1: 2], tempStr);
							writeStr(1, 10, 10, tempStr);
						}
						break;
							
						
							
						case OBJ3: 
							pitchStr(tempStr, curEnv->pitch[envInd], 1, 0, 0);
							writeStr(2, 5, 7, tempStr);
							break;
						
						/* case OBJ4:
							if(SHIFTMASK(MAINTOG, bitRecEnv))
							{
								//LogTextMessage("p");							
								intToStr((char *)startStopStr[1],(int16_t)(recNotes), 1, 0, 0, tempStr);
							}
							else strcpy(tempStr, recStr);
							writeStr(2, 13, 7, tempStr);
							break; */
							
						case OBJ5: writeBasicInt((int16_t)(curEnv->glide[envInd]),3, 0, 3, 6); break;
						
						case OBJ6:
							if(envInd < FREE_STAGES)
							{
								strcpy(tempStr, timeStr);
								float time = (float)TIME[curEnv->time[envInd]]/(float)3000;
								uint8_t leading = 0;
								if(time >= 100) leading++;
								if(time >= 10) leading++;
								if(time >= 1) leading++;

								floatToStr(time, leading, 3-leading, 0, tempStr);
								strcat(tempStr, "s");
							}
							writeStr(3, 10, 10, tempStr);
							break;
							
					}		
				}
				break;	
					
				
				case ARPEGSETUP:
				{					
					//"01234567890123456789"
					//"STEPS:@@  TYPE:@@@@@",
					//"SPM:@@@@@@ GLIDE:@@@",
					//"STYLE:@@@@-@@@@     "
					
					ARP_KNOBS *curArp = &arpeggio[oscInd];
					switch(curLCD)
					{
						case OBJ1: writeBasicInt((uint16_t)(curArp->steps), 2, 0, 1, 6); break;
						case OBJ2: writeStr(1, 15, 5, (char *)envStr[SHIFTMASK(oscInd, bitArpFilt)]); break;
							
						case OBJ3:
							floatToStr((curArp->BPM),4, 1, 0, tempStr);
							writeStr(2, 4,6,tempStr);
							break;
						
						case OBJ4: writeBasicInt((uint16_t)(curArp->G), 3, 0, 2, 17); break;
						
						case OBJ5: writeStr(3, 6, 4, (char *)loopStr[(SHIFTMASK(oscInd, bitArpTrig))? 0: 1]); break;
						case OBJ6: writeStr(3, 11, 4, (char *)loopStr[(SHIFTMASK(oscInd, bitArpSkip))? 2: 3]); break;
	
					}
				}
				break;
				
				case ARPEGNOTES:
				{
					//"01234567890123456789"
					//"POS:@@@ *PIT/ENF/VEL",
					//"@@@@@@@@   @@@@@@@@ ",
					//"@@@@@@@@   @@@@@@@@ "
					
					ARP_KNOBS *curArp = &arpeggio[oscInd];
					
					uint8_t baseInd = arp_page[oscInd] << 2;
					if(curLCD == OBJ1) writeBasicInt((uint16_t)(baseInd + 1), 2, 0, 1, 4);
					else if(curLCD > OBJ2)
					{
						uint8_t posInd = curLCD - OBJ3;
						baseInd += posInd;

						if(baseInd < curArp->steps)
						{
							intToStr("",(int16_t)(baseInd + 1), 2, 0, 0, tempStr);
							
							strcat(tempStr, arpNoteLeader[arpToggle[posInd]]);
							if(arpToggle[posInd] == PIT_TOG)
							{
								if(SHIFTMASK(oscInd, bitArpSkip) && !curArp->E[baseInd]) strcat(tempStr, ":SKP");
								else intToStr("",(int16_t)(curArp->P[baseInd]), 3, 0, 1, tempStr);
							}
							else if(arpToggle[posInd] == ENV_TOG)
							{
								arpEnvStr(curArp->E[baseInd], tempStr);
							}
							else
							{
								if(SHIFTMASK(oscInd, bitArpSkip) && !curArp->E[baseInd]) strcat(tempStr, "SKP");
								else intToStr("",(int16_t)(curArp->V[baseInd]), 3, 0, 0, tempStr);
							}							
						}
						writeStr((posInd >> 1) + 2, (posInd & 0x01)? 11: 0, 8, tempStr);
					}
				}
				break;
				
				case ARPREC:
				{
					// "01234567890123456789"
					// "RYTHM:@@@ VEL:@@@@@@",
					// "ENV:@@@             ",
					// "           @@@@@@@@ "
					switch(curLCD)
					{
						case OBJ1: writeStr(1, 6, 3, (char *)yesNoStr[(recRhythm)? 1: 0]); break;
						case OBJ2: writeStr(1, 14, 6, (char *)velStr[recVel]); break;
						
						case OBJ3: 
							arpEnvStr(recEnv, tempStr);
							writeStr(2, 4, 3, tempStr);
							break;
							
						case OBJ6:
							strcpy(tempStr, startStopStr[SHIFTMASK(MAINTOG, bitRecArp)]);
							if(SHIFTMASK(MAINTOG, bitRecArp)) intToStr("",(int16_t)(recNotes), 2, 0, 0, tempStr);
							writeStr(3, 11, 8, tempStr); 
							break;
					}
				}
				break;

				case FILTER:
				{
					FILT_KNOBS *curFilt = &filt_knobs[oscInd];
					switch(curLCD)
					{
							
						case OBJ1: writeStr(1,5,5, (char *)filtStr[curFilt->TYPE]); break;
						case OBJ2: writeStr(1, 17, 3, (char *)yesNoStr[SHIFTMASK(oscInd, bitFTrack)]); break;

						case OBJ3:
							pitchStr(tempStr, curFilt->FRQ, SHIFTMASK(oscInd, bitFTrack), 0, 0);
							writeStr(2, 7, 7, tempStr);
						break;
						
						case OBJ5: writeBasicInt((int16_t)(curFilt->RES),3, 0, 3, 4); break;
					}
				}
				break;
					

					
				case PATCHSV:
					
					// "01234567890123456789"
					// " DIR: @@@@@ @@@@@@  ",
					// " EDIT/*A>a     MOVE ",
					// "  NAME:@@@@@@ *SAVE "
					switch(curLCD)
					{
						case OBJ1: writeNumSpaceStr(saveDirInd + 1, dirs[PATCH][saveDirInd].name, 1, 6, 9); break;
						case OBJ5:
							if(isSaved > 0) strcpy(tempStr, saveCopyStr[0]);
							else strcpy(tempStr, saveName);
							
							writeStr(3,7,6, tempStr);
							isSaved = 0;
							break;
					}
					
					break;
				
				  case MIDIINS:
				{
					// "01234567890123456789"
					// " MIDICH:@@@ HKEY:@@@",
					// " VEL:@@@@@@ LKEY:@@@",
					// " LEGATO:@@@         "
					
					MIDI_PARAMS *curMidi = &midi_knobs[oscInd];
					switch(curLCD)
					{
						case OBJ1:
							if(!curMidi->chan) strcat(tempStr, "ALL");
							else intToStr("",(int16_t)(curMidi->chan), 2, 0, 0, tempStr);
							writeStr(1, 8, 3, tempStr);
							break;
							
						case OBJ2:
							pitchNumtoStr(curMidi->keyMax + MIDI_KEY_0, tempStr);
							writeStr(1, 17, 3, tempStr);
							break;
						
						case OBJ4:
							pitchNumtoStr(curMidi->keyMin + MIDI_KEY_0, tempStr);
							writeStr(2, 17, 3, tempStr);
							break;
							
						//case OBJ3: writeStr(2, 5, 6, (char *)velStr[(SHIFTMASK(oscInd, bitWind))? 2: SHIFTMASK(oscInd, bitKeyVel)]); break;
						//case OBJ3: writeStr((char *)velStr[curMidi->velType]); break;
						
						case OBJ5: writeStr(3, 8, 3, (char *)yesNoStr[SHIFTMASK(oscInd, bitLgto)]); break;
					}
				}
				break; 
					
				case MIDICCS:
				{
					// "01234567890123456789"
					// "CC#:WIND:@@@ MDW:@@@",
					// "RNG:WIND:@@@ MDW:@@@",
					// "    PBND:@@@ SUS:@@@"
					
					uint8_t ind = curLCD-OBJ1;
					writeBasicInt((int16_t)(*(&(midi_knobs[oscInd].CC_nums[0]) + ind)), 3, 0, (ind >> 1) + 1, (ind & 0x01)? 17: 9);
				}
				break;
				
				 case MODA:
				{
					// "01234567890123456789"
					// "PIT:@@@@  F.CUT:@@@@",
					// "AMP:@@@@  F.RES:@@@@",
					// "                    "
					
					//if(curLCD < OBJ5)
					{						
						uint8_t screenInd = curLCD-OBJ1;
						uint8_t srcInd = mod_src[oscInd][screenInd];
						
						uint8_t eInd = 5;
						if(srcInd == 0) eInd = 0;
						else if(srcInd == MOD_MAIN_OUT) eInd = 1;
						else if(srcInd == MOD_AUDIO_MX) eInd = 2;
						else if(srcInd == MOD_AUDIO_L) eInd = 3;
						else if(srcInd == MOD_AUDIO_R) eInd = 4;

						if(eInd < 5) strcpy(tempStr, modStrA[eInd]);
						else
						{
							eInd = (--srcInd) % TOTAL_MOD_SRC;
							uint8_t oInd = (srcInd) / TOTAL_MOD_SRC;
							//LogTextMessage("%u, %u", oInd, eInd);
							strCatCat((char *)modStrO[oInd], (char *)modStrB[eInd], tempStr);
							//strcpy(tempStr, modStrB[eInd]);
							//tempStr[3] = oInd + '1';//(srcInd, tempStr);
						}
						writeStr((screenInd >> 1) + 1, (screenInd & 0x01)? 15: 4, 5, tempStr);
					}
				}
				break;
				
				case OUTS:
					// "01234567890123456789"
					// " PAN: L@@@ R@@@     ",
					//"                    ",
					//"                    "
					switch(curLCD)
					{
						case OBJ1: writeBasicInt((int16_t)panLeft[oscInd], 3, 0, 1, 7); break;
						case OBJ2: writeBasicInt((int16_t)(127 - panLeft[oscInd]), 3, 0, 1, 12); break;
						//case OBJ3: writeBasicInt((int16_t)(delay_left_knobs[oscInd]), 3, 0, 2, 7); break;
						//case OBJ4: writeBasicInt((int16_t)(delay_right_knobs[oscInd]), 3, 0, 2, 12); break;
					}
					break;
				 
				case NOTES:
				{
					//"01234567890123456789"
					//"NOTES:@@@   EDIT:@@@",
					//" @@@@@@@@@ @@@@@@@@@",
					//" @@@@@@@@@ @@@@@@@@@"
					
					if(oscInd >= POLY_CNT) notesPage = 0;
					if(curLCD == OBJ1)
					{
						intToStr("",(int16_t)((notesPage << 2) + 1), 1, 0, 0, tempStr);
						if(oscInd < POLY_CNT) intToStr("-",(int16_t)(notesPage << 2) + 4, 1, 0, 0, tempStr);
						writeStr(1, 6, 3, tempStr);
					}
					else if(curLCD == OBJ2) writeStr(1, 17, 3, (char *)notesStr[notesTog]);
						
					else if(curLCD > OBJ2)
					{
						uint8_t scrnInd = curLCD-OBJ3;
						uint8_t stepInd = (notesPage << 2) + scrnInd;
						
						if(stepInd < childCnt(oscInd))
						{
							
							uint8_t child = firstChild[oscInd] + stepInd;
							if(!notesTog)
							{								
								//LogTextMessage("l %d %u", note[child], child);
								//num: monopoly track, str: everything else
								pitchStr(tempStr, note[child], oscInd < POLY_CNT && !SHIFTMASK(oscInd, bitPoly) && SHIFTMASK(oscInd, bitNotes), 0, monoPitch[oscInd] + pit_knobs[oscInd].pitch);
							//pitchStr(tempStr, note[child], 0, 0, monoPitch[oscInd] + pit_knobs[oscInd].pitch);
								//intToStr("P",(int16_t)(stepInd + 1), 1, 0, 0, tempStr);
								//pitchStr(tempStr, notes[spread[oscInd].pit[stepInd], 1, 0, 0);
							}
							else
							{								
								intToStr("V:",(int16_t)__USAT(vel[child] + monoVel[oscInd], 7), 3, 0, 0, tempStr);
							}
							
						}
						writeStr((scrnInd >> 1) + 2, (scrnInd & 0x01)? 11: 1, 9, tempStr);
					}
				}
				break;
				
				 case FAVS:
				{
					//"01234567890123456789"
					//"FAV @@: @@@@@@      ",
					//"@@@@@@@@@@@@@@@@@@@@",
					//"@@@@@@@@@@@@@@@@@@@@"
					
					switch(curLCD)
					{
						case OBJ1: writeBasicInt(favInd, 2, 0, 1, 4); break;
						case OBJ2: writeStr(1,8,6, saveName); break;
						case OBJ3: 
							if(favSave) strcpy(tempStr, favStr[0]);
							writeStr(2,0,20, tempStr);
							break;
						case OBJ5:
							if(favSave) strcpy(tempStr, favStr[1]);
							writeStr(3,0,20, tempStr);
							break;
					}
					
				}
				break; 
				
				case HARMONIC:
				{
					//"G.FUND @@@ PRTL1 @@@",
					//"G.PTL1 @@@ STEP  @@@",
					//"G.LAST @@@ COUNT @@@"
					
					// struct HARMONICS {
						// uint8_t first;
						// uint8_t step;
						// uint8_t cnt;
						// uint8_t gainFirst;
						// uint8_t gainLast;
					// };
					uint8_t ind = curLCD - OBJ1;
					writeBasicInt((int16_t)*((&harmParams[oscInd].gainFund) + ind),3, 0, (ind >> 1) + 1, (ind & 0x01)? 17: 7);
					
/* 					uint8_t rcs[6][3] = {{1,6,0},{0,0,0},{2,5,1},{2,17,2}, {3,6,3},{3,17,4}};
					uint8_t *rc = &rcs[curLCD-OBJ1][0];
					if(*rc > 0)
					{
						writeBasicInt(*(&(harmParams[oscInd].first) + *(rc + 2)), 3, 0, *rc, *(rc + 1)); 
					} */
				}
				break;
				
				case PHASE:
				{
					//"PRE-HARM  POST-HARM ",
					//" TOG:@@@    TOG:@@@ ",
					//"@@@@@@     @@@@@@   "
					
					uint8_t ind = curLCD - OBJ1;
					if(ind < 2) writeStr(2,  (ind & 0x01)? 15: 5, 3, (char *)onOffStr[*((&phase_knobs[oscInd].before_harm) + ind)]); 
					else if(ind < 4)
					{
						uint8_t val = *((&phase_knobs[oscInd].before_harm) + ind);
						float pct = float(val + 1)/float(2.56);
						
						if(!val) pct = 0;
						else if(val == 255) pct = 100;
						
						floatToStr(pct, 3, 1, 0, tempStr);
						strcat(tempStr, "%");
						writeStr(3, (ind & 0x01)? 11: 1, 6, tempStr);
						//writeBasicInt((int16_t)*((&phase_knobs[oscInd].before_harm) + ind),3, 0, 3, (ind & 0x01)? 16: 6);
					}
				}
				break;
							
				
			}
		}
		if(screenInd == PATCHSV) setCursorPosition(3, 7 + saveNameInd);
		else if(screenInd == ARPEGSETUP) setCursorPosition(2, 4 + posBPM[indBPM]);
		//else if(screenInd == ARPEGSETUP) setCursorPosition(2, 4 + BPM_pos);
	}	
	curLCD = indexIncrement(curLCD, 1, LCDelems);
	if(!curLCD && GRAPH_update) updateGraphic();

}


void  __attribute__(( noinline )) send4Bits(uint8_t data)
{
	//static uint8_t i2cWritePtr[1] __attribute__ ((section (".sram2")));	//the right type of memory for i2c write buffer										
	//static uint8_t *readByte;
	
	i2cWritePtr[0] = data | backlightStatus ;
	i2cMasterTransmit(&I2CD1, address, i2cWritePtr, 1, readByte, 0);	
}

void  __attribute__(( noinline )) clearFlag(uint8_t data)
{
	//pulse high
	send4Bits(data | En);
	//chThdSleepMicroseconds(MICRODELAY);
	
	//pulse low
	send4Bits(data & ~En);
	//chThdSleepMicroseconds(MICRODELAY);
}

void  __attribute__(( noinline )) send4BitsAndClear(uint8_t data)
{
	send4Bits(data);
	clearFlag(data);
	//chThdSleepMicroseconds(MICRODELAY);
}

void __attribute__(( noinline ))  sendByte(uint8_t data, uint8_t mode)
{
	send4BitsAndClear((data&0xF0)|mode);
	send4BitsAndClear(((data<<4)&0xF0)|mode);
}

void __attribute__(( noinline ))  pitchStr(char *str, int32_t pitch, uint8_t isNum, uint8_t center, int32_t note)
{
	if(isNum) intToStr("",(int16_t)((pitch>>PITCH_COARSE)), 3, center, 1, str);
	else pitchNumtoStr((pitch + note)>>PITCH_COARSE, str);
	intToStr("",(int16_t)((pitch>>PITCH_FINE)&0x3F), 2, 0, 1, str);
}

void __attribute__(( noinline )) pitchNumtoStr(int16_t num, char *str)
{
	
	static const uint8_t zone1Lim = MIDI_KEY_0 + A0 -1;
	static const uint8_t zone2Lim = MIDI_KEY_0 + C8;
	
	if(num > zone1Lim && num <= zone2Lim)
	{
		uint8_t octaves = -1 + '0';
		uint8_t rem = num;
		uint8_t sharp = 0;
		char tStr[4];
		tStr[3] = '\0';
		rem -= MIDI_KEY_0;
		while(rem >= 12) 
		{
			rem-= 12;
			octaves++;
		}
		
		//char tStr[4];
		strcpy(tStr, nts[rem]);
		tStr[1] = octaves;
		strcat(str, tStr);
		
	}
	else
	{
		
		if(num > zone1Lim) num = num - (zone2Lim + 1) + SET2;
		//if(num > maxNoteLabelInd) num = maxNoteLabelInd;
		strcat(str, noteLabels[num]);
		if(num < noteLabelTypes[0]) strcat(str, units[0]);
		else if(num < noteLabelTypes[1]) strcat(str, units[1]);
		else strcat(str, units[2]);
	}
		
}



void __attribute__(( noinline )) strCatCat(char *cat1, char *cat2, char *str)
{
	strcat(str, cat1);
	strcat(str, cat2);
}




void __attribute__(( noinline )) arpEnvStr(uint8_t env, char *str)
{
	if(env & AMP_MASK) strcat(str, "A");
	else strcat(str, "-");
	if(env & PIT_MASK) strcat(str, "P");
	else strcat(str, "-");
	if(env & FILT_MASK) strcat(str, "F");
	else strcat(str, "-");
}

void __attribute__(( noinline )) writeBasicInt(int16_t num, uint8_t digits, uint8_t leadingSign, uint8_t row, uint8_t col)
{
	char str[5];
	strcpy(str, "");
	intToStr("", num, digits, 0, leadingSign, str);
	writeStr(row, col, digits + leadingSign, str);
}

void __attribute__(( noinline )) intToStr(char *before, int16_t num, uint8_t digitsOut, int16_t center, uint8_t leadingSign, char *str)
{
	strcat(str, before);
	int16_t tNum = num - center;
	uint8_t digits[4] = {0,0,0,0};
	char temp[2];
	temp[1] = '\0';

	if(leadingSign)
	{
		temp[0] = (tNum >= 0)? '+' : '-';
		strcat(str, temp);
	}
	
	
	if(tNum < 0) 
	{
		if(leadingSign) tNum = -tNum;
		else tNum = 0;
	}
	
	uint16_t vals[3] = {1000,100,10};
	for(uint8_t i = 0; i < 3; ++i)
	{

		while(tNum >= vals[i])
		{
			tNum -= vals[i];
			digits[i]++;
		}
	}

	digits[3] = tNum;
	int32_t some = 0;
	//LogTextMessage("%u %u %u %s %d", digits[0], digits[1], digits[2], str, tNum);
	for(uint8_t i = 0; i < 4; ++i)
	{
		if(digitsOut >= 4-i)
		{
			if(i < 3 && !some && !digits[i])
			{
				if(leadingSign)
				{
					temp[0] = str[strlen(str) -1];
					str[strlen(str) -1] = ' ';
				}
				else temp[0] = ' ';
			}
			else
			{
				some = 1;
				temp[0] = digits[i] + '0';
			}
			strcat(str, temp);
			//
		}
	}
	//LogTextMessage("string %s", str);
	

}

void __attribute__(( noinline )) writeNumSpaceStr(uint8_t num, char *str, uint8_t row, uint8_t col, uint8_t len)
{
	char tStr[21];
	strcpy(tStr, "");
	intToStr("",(int16_t)num, 2, 0, 0, tStr);
	strCatCat(" ", str, tStr);
	//LogTextMessage("row %u", row);
	writeStr(row, col, len, tStr);
}





#endif 




	