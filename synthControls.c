#ifndef SYNTHCONTROLS_C
#define SYNTHCONTROLS_C


#include "./settings.h"
#include "./helperfunctions.c"
#include "./fileLoader.h"
#include "./pitchTables.c"
#include "./lcdLib.h"
#include "./midiHandler.h"



void __attribute__(( noinline )) get_port(const char* port_code, ioportid_t *port, uint32_t *pin)
{
	switch(port_code[0])
	{
		case 'A':
		case 'a':
			*port = GPIOA;
			break;
		case 'B':
		case 'b':
			*port = GPIOB;
			break;
		case 'C':
		case 'c':
			*port = GPIOC;
			break;
			
	}
	
	*pin = uint32_t(port_code[1] - '0');
}

void __attribute__(( noinline )) setup_pin(const char *port_code, uint32_t is_input)
{
	ioportid_t port;
	uint32_t pin;
	get_port(port_code, &port, &pin);
	
	if(is_input)
	{
		palSetPadMode(port, pin, PAL_MODE_INPUT);
	}
	else
	{
		palSetPadMode(port, pin, PAL_MODE_OUTPUT_PUSHPULL);
		palWritePad(port, pin, 0);
	}
}

uint32_t __attribute__(( noinline )) read_pin(const char *port_code)
{
	ioportid_t port;
	uint32_t pin;
	get_port(port_code, &port, &pin);
	return palReadPad(port, pin);
}

void __attribute__(( noinline )) write_pin(const char *port_code, uint32_t val)
{
	ioportid_t port;
	uint32_t pin;
	get_port(port_code, &port, &pin);
	palWritePad(port, pin, val);
}
//runs once at startup
void initSynthStuff()
{	

	#if LOADTABLES
	save_data_arrays();
	#endif
	read_data_arrays();
	//get the pointers to the settings variables
	void *ptrs[] = {
		toggles, osc_gain, panLeft, midi_knobs,
		pit_knobs, amp_env_knobs, pit_env_knobs, filt_env_knobs, 
		filt_knobs, mod_src, arpeggio, harmParams,
		phase_knobs,
		amp_env, pit_env, filt_env, arp_env, 
		vel, note, monoPitch, monoVel 
	};
	

	memcpy(varPtrs, ptrs, sizeof(ptrs));
	
	for(int32_t i = 0; i < 5; ++i)
	{
		if(i < 2)
		{
			setup_pin(lower_knob_pins[i], 1);
			setup_pin(upper_knob_pins[i], 1);
		}
		if(i < 4)
		{
			setup_pin(chan_pins[i], 0);
			setup_pin(led_pins[i], 0);
		}
		setup_pin(mx_pins[i], 1);
	}
	
	//clear the note and cc events arrays
	memset(&midiEvents, DEAD_MIDI_EVENT, sizeof(midiEvents));

	//initialize buttons/knobs
	memset(&MX, 255, sizeof(MX));
	memset(&LED, 0, sizeof(LED));
	memset(&KNOB_A, 3, sizeof(KNOB_A));
	memset(&KNOB_DIR, 0, sizeof(KNOB_DIR));
	memset(&KNOB_TICKS, 0, sizeof(KNOB_TICKS));
	KNOB_S = ~0;//memset(&KNOB_S, 1, sizeof(KNOB_S));
	
	
	//for(int32_t osc = 0; osc < OSC_CNT; ++osc)
	//{
	//	delay_read_right[osc] = 0;
	//	delay_read_left[osc] = 0;
	//}
	//memset(delay_left_knobs, 0, sizeof(delay_left_knobs));
	//memset(delay_right_knobs, 0, sizeof(delay_right_knobs));

	//memset(delay_lefts, 0, sizeof(delay_lefts));
	//memset(delay_rights, 0, sizeof(delay_rights));

	//delay_write = 1;


	
	
	//give presets to editing variables
	strcpy(saveName, saveCopyStr[2]);
	pitchShift = PITCH_COARSE;
	lastLetter = 'A';
	saveNameInd = 0;
	saveDirInd = 0;
	recording = 0;
	recEnv = 0x7;
	indBPM = 2;
	
	//initialize all indexes
	oscInd = 0;
	screenInd = WAVETBL;
	envInd = 0;
	isMainLVL = 0;
	
	//initialize the dummy pointer
	zeroMod = 0;
	maxMod = 0x7FFFFFFF;
	
	//turn off blinking
	blinkInd = -1;
	blinkGrp = 0;
	routeTog = -1;
	drumPage = 0;
	
	main_gain = 127;
	initOscMidi(0, OSC_CNT -1);
	
}	


	
//runs when a patch is loaded/reset/randomized	
void __attribute__(( noinline )) initPatch(uint8_t first, uint8_t last)
{
	resetArpPages(first, last);
	updateAllMod(first, last);

	memset(&FIL_update[first], 1, last-first + 1);
	memset(&HARM_update[first], -1, last-first + 1);

	toggleSelected(oscInd);
	updateLEDs();
};

//uint8_t onTogs[3] = {bitArpTrig, bitFTrack, bitFECut};


//clear patch to default settings
void __attribute__(( noinline )) resetPatch()
{
	uint32_t midiThru = SHIFTMASK(MAINTOG, bitMidiThru);
	
	for(uint8_t i = 0; i < resetCnt; ++i)
	{
		uint16_t rem = ptrSizes[i];
		char *pos = (char *)varPtrs[i];
		while(rem)
		{
			uint8_t amt = (rem < 200)? rem: 200;
			memset(pos, resetVals[i], amt);
			pos += amt;
			rem -= amt;
		}
	}
	
	uint8_t resetTogs[7] = {bitOsc, bitMain, bitNotes, bitEnvs, bitAEnv, bitWave, bitPoly};
	SETBITS(0, resetTogs, sizeof(resetTogs));
	SETBIT(1, bitPoly);	

	for(uint8_t i = 0; i < OSC_CHILD_CNT; ++i)
	{
		amp_env[i].stage = 3;
		amp_env[i].val = 0;
		pit_env[i].stage = FREE_STAGES + 1;
		filt_env[i].stage = FREE_STAGES + 1;
	}
	
	//turn on notes/vel for poly
	for(uint8_t i = 0; i < POLY_CHILD_CNT; ++i)
	{
		note[i] = A4 << PITCH_COARSE;
		vel[i] = 127;
	}
	
	//turn on notes/vel for mono
	for(uint8_t i = 0; i < MONO_CNT; ++i)
	{
		monoPitch[i + POLY_CNT] = A4 << PITCH_COARSE;
		monoVel[i + POLY_CNT] = 127;
	}
	
	for(uint8_t i = 0; i < OSC_CNT; ++i)
	{
		setFileIndexFromName(WAVE, i, (char *)def_wave);
		//curWavFile[i] = &files[WAVE][0];
		pit_knobs[i].vel_glide = 11;
		pit_knobs[i].pitch = MIDI_KEY_0<<PITCH_COARSE;
		SETBIT(i, bitArpTrig);
		//SETBIT(i, bitFTrack);
		SETBIT(i, bitFECut);

		arpeggio[i].steps = 1;
		arpeggio[i].BPM = 120;
		arpeggio[i].T = 1500;
		memset(&arpeggio[i].V, 127, sizeof(arpeggio[i].V));
		
		midi_knobs[i].keyMax = C8;
		midi_knobs[i].keyMin = A0;
		midi_knobs[i].CC_nums[WND_EVENT] = 11;
		midi_knobs[i].CC_nums[MW_EVENT] = 74;
		
		harmParams[i].gainFund = 110;
		harmParams[i].gainFirst = 90;
		harmParams[i].gainLast = 20;
		harmParams[i].first = 2;
		harmParams[i].cnt = 1;
		harmParams[i].step = 1;
		
		phase_knobs[i].before_phase = 127;
		phase_knobs[i].before_harm = 0;
		phase_knobs[i].after_phase = 127;
		phase_knobs[i].after_harm = 1;
		
		filt_knobs[i].FRQ = (A4 + MIDI_KEY_0)<<PITCH_COARSE;

		
	}

	
	osc_gain[0] = 50;
	amp_env_knobs[0].rate[0] = 2;
	if(midiThru) SETBIT(MAINTOG, bitMidiThru);
	
	initPatch(0, OSC_CNT-1);
	
}



void __attribute__(( noinline )) phaseWidth(uint32_t width, int32_t *in_arr, int32_t *out_arr)
{
	uint32_t incA;
	uint32_t incB;
	
	//width = phase_width;
	
	//if(width < 64)
	{
		incA = phase_width_incs[width][0];
		incB = phase_width_incs[width][1];
	}
	/* else
	{
		incA = phase_width_incs[127 - width][1];
		incB = phase_width_incs[127 - width][0];
	} */
	uint32_t phase = 0;
	if(width == 0) phase = 0xFFFFFFFF >> 1;
	
	for(uint32_t i = 0; i < 512; ++i)
	{
		
		
		int32_t wavInd = (phase)>>WAVE_SHIFT;
		int32_t amp1 = *(in_arr + wavInd);
		int32_t amp2 = *(in_arr + ((wavInd+1) & 0x1FF));
		uint32_t off = ((phase) & WAVE_MASK) << WAVE_INTERP;
		*(out_arr + i) = ___SMMUL(amp1,INT_MAX-off);
		*(out_arr + i) = ___SMMLA(amp2,off,*(out_arr + i))<<1; 
		
		if(i >> 1 <= width) phase += incA;
		else phase += incB;
	}
}
		
	
	

	
void __attribute__(( noinline )) checkHarmQueue()
{
	static uint8_t curO = 0;
	HARMONICS *curHarm = &harmParams[curO];
	PHASE_KNOBS *cur_phase = &phase_knobs[curO];
	int8_t *curCnt = &HARM_update[curO];
	
	static uint8_t curPart = 1;
	static int32_t tArr[WAVE_RES] __attribute__ ((section (".sdram")));
	static int32_t base_arr[WAVE_RES] ;
	uint32_t gain;
	if(*curCnt < curHarm->cnt)
	{
		//mark count as completed
		*curCnt = curHarm->cnt;

		//apply before phasing to base wave
		if(SHIFTMASK(curO, bitPhase) && cur_phase->before_harm)
		{
			phaseWidth(cur_phase->before_phase, wavArray[curO], tArr);
		}
		//otherwise copy base wave
		else
		{
			memcpy(tArr, wavArray[curO], sizeof(tArr));
		}
		
		//add harmonics
		if(SHIFTMASK(curO, bitHarms))
		{
			memcpy(base_arr, tArr, sizeof(base_arr));
			
			gain = GAIN[curHarm->gainFund];
			for(uint16_t i = 0; i < WAVE_RES; ++i)
			{
				tArr[i] = ___SMMUL(base_arr[i], gain);//<<1;	
			}

			curPart = curHarm->first;	
			//*curCnt += 1;

			//add in remaining partials
			for(uint8_t p = 0; p < curHarm->cnt; p++)
			{
				if(curHarm-> cnt < 2)
				{
					gain = GAIN[curHarm->gainFirst];
				}
				else
				{
					gain = GAIN[int8_t((curHarm->gainLast - curHarm->gainFirst) * float(p)/float((curHarm->cnt)-1)) + curHarm->gainFirst];
				}
					
				uint16_t j = 0;
				
				//add in the current partial
				for(uint16_t i = 0; i < WAVE_RES; ++i)
				{
					tArr[i] = __SSAT(tArr[i] + (___SMMUL(base_arr[j], gain)), 31);	
					j = (j + curPart) & WAVE_RES_MASK;
				}

				curPart += curHarm->step;
			}
			
			for(uint16_t i = 0; i < WAVE_RES; ++i)
			{
				tArr[i] <<= 1;
			}
		}
		
		if(SHIFTMASK(curO, bitPhase) && cur_phase->after_harm)
		{
			phaseWidth(cur_phase->after_phase, tArr, harmArray[curO]);
		}
		else
		{
			memcpy(harmArray[curO], &tArr, sizeof(tArr));
		}
		GRAPH_update = 1;

	}
	else
	{
		curO = indexIncrement(curO, 1, OSC_CNT);
	}
}

	
/* void __attribute__(( noinline )) checkHarmQueue()
{
	static uint8_t curO = 0;
	HARMONICS *curHarm = &harmParams[curO];
	int8_t *curCnt = &HARM_update[curO];
	static uint8_t curPart = 1;
	static int32_t tArr[WAVE_RES];
	uint32_t gain;
	if(*curCnt < curHarm->cnt)
	{
		*curCnt = curHarm->cnt;
		//if it's been reset, reset the array
		gain = GAIN[curHarm->gainFund];
		for(uint16_t i = 0; i < WAVE_RES; ++i)
		{
			tArr[i] = ___SMMUL(wavArray[curO][i], gain);//<<1;	
		}

		curPart = curHarm->first;	
		//*curCnt += 1;

		//add in remaining partials
		for(uint8_t p = 0; p < curHarm->cnt; p++)
		{
			if(curHarm-> cnt < 2)
			{
				gain = GAIN[curHarm->gainFirst];
			}
			else
			{
				gain = GAIN[int8_t((curHarm->gainLast - curHarm->gainFirst) * float(p)/float((curHarm->cnt)-1)) + curHarm->gainFirst];
			}
				
			uint16_t j = 0;
			
			//add in the current partial
			for(uint16_t i = 0; i < WAVE_RES; ++i)
			{
				tArr[i] = __SSAT(tArr[i] + (___SMMUL(wavArray[curO][j], gain)), 31);	
				j = (j + curPart) & WAVE_RES_MASK;
			}

			curPart += curHarm->step;
		}
		
		for(uint16_t i = 0; i < WAVE_RES; ++i)
		{
			tArr[i] <<= 1;
		}
		memcpy(harmArray[curO], &tArr, sizeof(tArr));
		GRAPH_update = 1;

	}
	else
	{
		curO = indexIncrement(curO, 1, OSC_CNT);
	}
}
 */
 
 // void check_knob_turns(uint8_t knobInd, const char pin_group[2][3])
 // { 
	// uint8_t tIn = read_pin(pin_group[0]);
	// tIn |= (read_pin(pin_group[1]) << 1);
	
	// //check for knob turns
	// if((tIn) != (KNOB_A[knobInd]))
	// {
		// uint8_t chg = tIn ^ KNOB_A[knobInd];
		// uint8_t isDone = 0;
		// uint32_t elapsed = ticks - KNOB_TICKS[knobInd];
		
		// //starting a new turn, reset direction
		// if(elapsed > 400) KNOB_DIR[knobInd] = 0;
		
		// //both signals changed--garbage data-- add it as an acceleration of the current direction
		// if(chg == 3) 
		// {
			// if(KNOB_DIR[knobInd]) isDone = 2;
		// }
		// else
		// {
			// //compare signal A to signal B to get direction
			// int8_t dir = ((tIn & 1) == (tIn >> 1))? 1 : -1;
			
			// //if A changed, flip the result 
			// if(chg & 1)
			// {
				// dir = -dir;
				
				// //if A returned to high, it landed back in a detent
				// if(tIn & 1) isDone = 1;
			// }
			
			// //direction change is either an acceleration (if elapsed is small) or direction change
			// if(dir != KNOB_DIR[knobInd] && elapsed < 48) isDone = 3;
			// else KNOB_DIR[knobInd] = dir;
		// }
		
		// //apply knob turns
		// if(isDone && KNOB_DIR[knobInd])
		// {
			// LogTextMessage("knob %u, val %d\n", knobInd, KNOB_DIR[knobInd]<<(isDone-1));
			// //addToInputQueue(KNOB_GRP, knobInd, KNOB_DIR[knobInd]<<(isDone-1), 0);
		// }

		// KNOB_A[knobInd] = tIn;
		// KNOB_TICKS[knobInd] = ticks;
	// }
 // }
 
  // void check_knob_turns(uint32_t knobInd, const char pin_group[2][3])
 // { 
	// static uint8_t A_seq[8] = {0};
	// static uint8_t B_seq[8] = {0};
	// static uint8_t events[8] = {0};
	// static uint32_t initialized = 0;
	// static int32_t last_dir[8] = {0};
	// static uint8_t knob_val[8] = {0};
	// uint32_t tA = read_pin(pin_group[0]);
	// uint32_t tB = read_pin(pin_group[1]);
	
	
	
	// if(!((initialized >> knobInd) & 1))
	// {
		// initialized |= (1 << knobInd);
		// A_seq[knobInd] = tA;
		// B_seq[knobInd] = tB;
		// KNOB_TICKS[knobInd] = ticks;
	// }
	
	// uint32_t A_chg = tA != (A_seq[knobInd] & 1);
	// uint32_t B_chg = tB != (B_seq[knobInd] & 1);
	
	// //check for knob turns
	// if(A_chg || B_chg)
	// {	
		// A_seq[knobInd] <<= 1;
		// A_seq[knobInd] |= tA;
		// B_seq[knobInd] <<= 1;
		// B_seq[knobInd] |= tB;
		
		// if(A_chg && tA)
		// {
			// uint32_t b_val = B_seq[knobInd] & 0xF;
			// uint32_t elapsed = ticks - KNOB_TICKS[knobInd];
			// int32_t dir = elapsed < 40? last_dir[knobInd] : (b_val == 0x03) ? -1 : (b_val == 0x0C) ? 1 : (elapsed < 100) ? last_dir[knobInd] : 0; 
			// int32_t mult = elapsed < 20? 4: elapsed < 40? 3 : (b_val == 0x03 || b_val == 0x0C) ? 1 : 2; 
			// addToInputQueue(KNOB_GRP, knobInd, dir * mult, 0);
			// KNOB_TICKS[knobInd] = ticks;
			// last_dir[knobInd] = dir;
		// }

	// }
 // }
 
   void check_knob_turns(uint32_t knobInd, uint8_t sig)
 { 
	static uint8_t A_seq[8] = {0};
	static uint8_t B_seq[8] = {0};
	static uint8_t events[8] = {0};
	static uint32_t initialized = 0;
	static int32_t last_dir[8] = {0};
	static uint8_t knob_val[8] = {0};
	uint32_t tA = sig & 1;
	uint32_t tB = (sig >> 1) & 1;
	
	
	
	if(!((initialized >> knobInd) & 1))
	{
		initialized |= (1 << knobInd);
		A_seq[knobInd] = tA;
		B_seq[knobInd] = tB;
		KNOB_TICKS[knobInd] = ticks;
	}
	
	uint32_t A_chg = tA != (A_seq[knobInd] & 1);
	uint32_t B_chg = tB != (B_seq[knobInd] & 1);
	
	//check for knob turns
	if(A_chg || B_chg)
	{	
		A_seq[knobInd] <<= 1;
		A_seq[knobInd] |= tA;
		B_seq[knobInd] <<= 1;
		B_seq[knobInd] |= tB;
		
		if(A_chg && tA)
		{
			uint32_t b_val = B_seq[knobInd] & 0xF;
			uint32_t elapsed = ticks - KNOB_TICKS[knobInd];
			int32_t dir = elapsed < 40? last_dir[knobInd] : (b_val == 0x03) ? -1 : (b_val == 0x0C) ? 1 : (elapsed < 100) ? last_dir[knobInd] : 0; 
			int32_t mult = elapsed < 20? 4: elapsed < 40? 3 : (b_val == 0x03 || b_val == 0x0C) ? 1 : 2; 
			addToInputQueue(KNOB_GRP, knobInd, dir * mult, 0);
			KNOB_TICKS[knobInd] = ticks;
			last_dir[knobInd] = dir;
		}

	}
 }
 
/*  void __attribute__(( noinline )) check_knob(const char pins[2][3], uint8_t *states, int32_t ind, int32_t grp)
 {
	uint32_t a_sig = read_pin(pins[0]);
	uint32_t b_sig = read_pin(pins[1]);
	int32_t b_ind = ind + 4;
	
	if(b_sig != ((*states >> b_ind) & 1))
	{
		*states ^= (1 << b_ind);
	}
	
	if(a_sig != ((*states >> ind) & 1))
	{
		*states ^= (1 << ind);
		
		if(a_sig)
		{
			if(b_sig)
			{
				LogTextMessage("knob %u left\n", ind + (grp << 2));
			}
			else
			{
				LogTextMessage("knob %u right\n", ind + (grp << 2));
			}
		}
	}
	
 } */
void __attribute__(( noinline )) scanInputs()
{
	
	
	static uint8_t ind = 0;
	static uint8_t grp = 0;
	static uint8_t l_ind = 0;
	static uint16_t mxs[5] = {0};
	static uint8_t k_lower = 0;
	static uint8_t k_upper = 0;
	static uint8_t tM[16];
	
	if(!grp)
	{
		tM[ind] = palReadGroup(GPIOC, 0x1F, 0);
	}

	uint8_t tk = palReadGroup(GPIOB, 0xC3, 0);
	check_knob_turns(ind & 0x03, (tk >> 6) & 3);
	check_knob_turns((ind & 0x03) + 4, tk & 3);

	checkSwitch(grp, ind, (tM[ind] >> grp) & 1);
	
	
	// check_knob_turns(ind & 0x03, lower_knob_pins);
	// check_knob_turns((ind & 0x03) + 4, upper_knob_pins);
	//checkSwitch(grp, ind);

	
	
	//increment channel
	ind = (ind+1) & 0x0F;
	if(!ind)
	{
		grp = indexIncrement(grp, 1, 5);
	}
	
	if(!(ticks % 700))
	{
		l_ind = (l_ind + 1) & 0x0F;
		
		//LogTextMessage("%u led\n", l_ind);
	}
	
	//more generic control
	/* for(uint32_t i = 0; i < 4; ++i)
	{
		write_pin(led_pins[i], 1);
			
	}
	
	//set the channel for the next read
	for(int32_t i = 0; i < 4; ++i)
	{
		write_pin(chan_pins[i], (ind >> i) & 1);
		
	}
	
	
	uint8_t on[4];
	for(uint32_t i = 0; i < 4; ++i)
	{
		on[i] =  (LED[i] >> ind) & 1;
		if(i == blinkGrp && ind == blinkInd)
		{
			on[i] = (ticks >> 10) & 1;
		}
		write_pin(led_pins[i], on[i]);
			
	} */
	
	//set the channel for the next read
	palWriteGroup(GPIOA, 0x0F, 4, ind);
	
	
	uint8_t on[4];
	for(uint32_t i = 0; i < 4; ++i)
	{
		on[i] =  (LED[i] >> ind) & 1;
		if(i == blinkGrp && ind == blinkInd)
		{
			on[i] = (ticks >> 10) & 1;
		}			
	}

	//set the LEDS
	palWritePad(GPIOC, 5, on[0]);
	palWritePad(GPIOA, 0, on[1]);
	palWritePad(GPIOA, 1, on[2]);
	palWritePad(GPIOA, 2, on[3]);


	

}

void __attribute__(( noinline )) checkSwitch(uint8_t group, uint8_t ind, uint8_t sig)
{
	static uint32_t timer[5] = {0};
	static uint16_t held[5];
	
	uint8_t prev = (MX[group] >> ind) & 1;
	

	if(sig != prev)
	{
		MX[group] ^= (1 << ind);
		if(!sig) 
		{
			timer[group] = ticks;
			held[group] &= ~(1 << ind);
		}
		else if(!((held[group] >> ind) & 1)) 
		{
			addToInputQueue(group, ind, 1, 1);
		}
	}
	
	else if(!sig && !((held[group] >> ind) & 1) && ticks-timer[group] > HOLD_TIME)
	{
		held[group] |= (1 << ind);
		addToInputQueue(group, ind, 1, 0);
	}
}

// void __attribute__(( noinline )) checkSwitch(uint8_t group, uint8_t ind)
// {
	// static uint32_t timer[5] = {0};
	// static uint16_t held[5];
	
	// uint8_t sig = read_pin(mx_pins[group]);
	// uint8_t prev = (MX[group] >> ind) & 1;
	

	// if(sig != prev)
	// {
		// MX[group] ^= (1 << ind);
		// if(!sig) 
		// {
			// timer[group] = ticks;
			// held[group] &= ~(1 << ind);
		// }
		// else if(!((held[group] >> ind) & 1)) 
		// {
			// addToInputQueue(group, ind, 1, 1);
		// }
	// }
	
	// else if(!sig && !((held[group] >> ind) & 1) && ticks-timer[group] > HOLD_TIME)
	// {
		// held[group] |= (1 << ind);
		// addToInputQueue(group, ind, 1, 0);
	// }
// }


void  __attribute__(( noinline )) addToInputQueue(uint8_t group, uint8_t ind, int32_t val, uint8_t isQuick)
{
	static uint8_t writeInd = 0;
	if(group == 4 && ind > 7) ++group;
	//LogTextMessage("g %u, ind %u, val %u, isQ %u", group, ind, val, isQuick);
	if(inputQueue[writeInd][2] && (inputQueue[writeInd][0] != group || inputQueue[writeInd][1] != ind)) LogTextMessage("s");
	inputQueue[writeInd][0] = group;
	inputQueue[writeInd][1] = ind;
	inputQueue[writeInd][2] += val;
	inputQueue[writeInd][3] = isQuick;	
	++writeInd &= 0x03;
}

uint8_t  __attribute__(( noinline )) oscFromGrpInd(uint8_t group, uint8_t ind)
{
	return ((group << 4) + ind) >> 3;
}
	
void  __attribute__(( noinline )) toggleSelected(uint8_t osc)
{
	
	for(uint8_t tOsc = 0; tOsc < OSC_CNT; tOsc++)
	{
		if(tOsc == osc) SETBIT(tOsc, bitOsc);//toggles[tOsc] |= (1 << bitOsc)//maskOsc;
		else CLEARBIT(tOsc, bitOsc);//toggles[tOsc] &= ~maskOsc;

	}
	//LogTextMessage("s");
	CLEARBIT(MAINTOG, bitMainLVL);
	oscInd = osc;
}

	
void  __attribute__(( noinline )) updateLEDs()
{
	uint8_t tLed[2];
	for(uint8_t osc = 0; osc < OSC_CNT; osc++)
	{
		uint8_t isSecond = osc & 1;
		tLed[isSecond] = 0;
		uint8_t pos;
		
		if(SHIFTMASK(MAINTOG, bitDrum))
		{
			int8_t *e = &arpeggio[osc].E[(drumPage << 3)];
			for(pos= 0; pos < 8; pos++)
			{
				tLed[isSecond] |= *(e + pos)? (1 << pos) : 0;
			}				
		}
		else
		{
			
			//get main toggle statuses
			
			for(pos= 0; pos < 8; pos++)
			{
				uint8_t isOn = SHIFTMASK(osc, big_group[pos][0]);
				
				//leds that depend on other settings as well
				switch(big_group[pos][0])
				{
					case bitMain: isOn &= ~(SHIFTMASK(MAINTOG, bitSolo) && osc != oscInd); break;
					case bitFEnv: isOn &= SHIFTMASK(osc, bitFilt); break;
				}
				tLed[isSecond] |= (isOn << pos);
			}
		}
		//update the LED variables (every two osc)
		if(isSecond) LED[osc >> 1] = ~(tLed[0] | (tLed[1] << 8));
	}
	
	//get the other button group's toggles
	LED[3] = 0;
	for(uint8_t ind = 0; ind < 16; ind++)
	{
		LED[3] |= LEDfromGroup(other_groups[ind][0], ind, other_groups[ind][1]);
	}
	LED[3] = ~LED[3];
			
}

uint16_t __attribute__(( noinline )) LEDfromGroup(int8_t osc, uint8_t ind, uint8_t tog)
{
	if(osc != -1 && tog != -1)
	{
		if(osc == E_OSC) osc = oscInd;//? oscInd: MAINTOG;
		return (SHIFTMASK(osc, tog) << ind);
	}
	return 0;
}
	
void __attribute__(( noinline )) copyOsc(uint8_t destOsc, uint8_t srcOsc, uint8_t bit)	
{
	//dont bother copying itself			
	if(destOsc == srcOsc) return;
	
	//copy entire oscillator
	if(bit == bitOsc)
	{
		//copy all settings
		for(uint8_t i = 0; i < settingsCnt; ++i)
		{
			uint8_t sz = ptrSingleSizes[i];
			void *addSrc = varPtrs[i] + sz * srcOsc;
			void *addDst = varPtrs[i] + sz * destOsc;
			memcpy(addDst, addSrc, sz);
		}
		
		//copy wave table reference
		curWavFile[destOsc] = curWavFile[srcOsc];
		
		//handle weirdness of copying poly to mono and visa versa
		if(destOsc >= POLY_CNT) CLEARBIT(destOsc, bitPoly);
		else if(srcOsc >= POLY_CNT) SETBIT(destOsc, bitPoly);
		
		//if osc poly settings match, copy envelope shit
		if(SHIFTMASK(destOsc, bitPoly) == SHIFTMASK(srcOsc, bitPoly))
		{
			uint8_t childTo = firstChild[destOsc];
			uint8_t childFrom = firstChild[srcOsc];
			uint8_t children = (destOsc < POLY_CNT && srcOsc < POLY_CNT)? NOTES_CNT: 1;
			
			susOn[destOsc] = susOn[srcOsc];
			monoPitch[destOsc] = monoPitch[srcOsc];
			monoVel[destOsc] = monoVel[srcOsc];
			
			for(uint8_t i = settingsCnt; i < copyStop; ++i)
			{
				uint8_t sz = ptrSingleSizes[i];
				void *addSrc = varPtrs[i] + sz * childFrom;
				void *addDst = varPtrs[i] + sz * childTo;
				
				memcpy(addDst, addSrc, sz * children);			
			}
		}
	}
	//copy specific objects
	else
	{
		uint32_t extraBits = (1 << bit);
		switch(bit)
		{
			case bitHarms:
				harmParams[destOsc] = harmParams[srcOsc];
				extraBits |= (1 << bitHarms); 
				break;
			case bitAEnv: 
				amp_env_knobs[destOsc] = amp_env_knobs[srcOsc]; 
				break;
			case bitPEnv: 
				pit_env_knobs[destOsc] = pit_env_knobs[srcOsc]; 
				break;
			case bitFilt:
				filt_knobs[destOsc] = filt_knobs[srcOsc];
				extraBits |= (1 << bitFTrack); 
				break;
			case bitFEnv:
				filt_env_knobs[destOsc] = filt_env_knobs[srcOsc];
				extraBits |= (1 << bitFECut);
				break;
			case bitArp:
				arpeggio[destOsc] = arpeggio[srcOsc];
				extraBits |= (1 << bitArpFilt) | (1 << bitArpSkip) | (1 << bitArpTrig);
				break;
			case bitMain:
				panLeft[destOsc] = panLeft[srcOsc];	
				break;
		}
		
		//clear affected bits
		toggles[destOsc] &= ~extraBits;
		//copy the relevant bits
		toggles[destOsc] |= (extraBits & toggles[srcOsc]);
	}	
	if(destOsc < srcOsc) initPatch(destOsc, srcOsc);
	else initPatch(srcOsc, destOsc);
}	

void __attribute__(( noinline )) routeMod(uint8_t destOsc, uint8_t bit, uint16_t sourceBit)	
{
	
	uint8_t ind = -1;
	uint16_t src = OSC_SRC;
	switch(sourceBit)
	{
		case bitAEnv: src = AENV_SRC; break;
		case bitPEnv: src = PENV_SRC; break;
		case bitFEnv: src = FENV_SRC; break;
		case bitArp: src = ARP_SRC; break;
	}

	
	switch(bit)
	{
		case bitAEnv: ind = AMP_MOD; break;
		case bitPEnv: ind = PIT_MOD; break;
		case bitFilt: ind = CUT_MOD; break;
		case bitFEnv: ind = RES_MOD; break;
		case bitArp: ind = ARPTIME_MOD; break;
		case bitMain: ind = GATE_MOD; break;
		//case bitMain: ind = PAN_MOD; break;
	}
	if(ind != -1)
	{
		mod_src[destOsc][ind] = src + 1 + oscInd * TOTAL_MOD_SRC;
		SETBIT(destOsc, bitMod);
		updateSingleMod(ind, destOsc, mod_src[destOsc][ind]);
	}	
}

uint8_t __attribute__(( noinline ))  finishRecording()
{	
	uint32_t recState = 0; 
	if(SHIFTMASK(MAINTOG, bitRecArp)) recState = 1;
	else if(SHIFTMASK(MAINTOG, bitRecEnv)) recState = 2;

	if(!recState) return 0;
	
	CLEARBIT(MAINTOG, bitRecArp);
	CLEARBIT(MAINTOG, bitRecEnv);

	//add in last time step
	incArpRecTime();
			
	if(recState == 1)
	{
		SETBIT(oscInd, bitArp);
		LCD_update[OBJ6] =1 ;
		if(recNotes)
		{
			ARP_KNOBS *curArp = &arpeggio[oscInd];
			SETBIT(oscInd, bitArp);
			LCD_update[OBJ6] = 1; 
			curArp->steps = recNotes;
			
			//normalize volume
			if(recVel)
			{
				uint8_t adder = 127 - recLoudest;
				for(uint8_t i = 0; i < recNotes; ++i) curArp->V[i] += adder;
			}
			
			float avgStep = 0;
		
			if(!recRhythm) 
			{
				avgStep = (float)(recNotes)/(float)(recFullTime);
				CLEARBIT(oscInd, bitArpSkip);
			}
			else 
			{
				SETBIT(oscInd, bitArpSkip);//curArp->trigAtk |= SKIP_MASK;
				uint16_t halfBase = recShortest >> 1;
				uint8_t totalSteps = 0;
				for(uint8_t i = 0; i < recNotes; ++i)
				{
					uint8_t steps = 1;
					uint16_t time = recShortest;
					int16_t actTime = recTimes[i];
					while(actTime - time > halfBase)
					{
						steps++;
						time += recShortest;
					}
					stepsPer[i] = steps;
					totalSteps += steps;
					if(totalSteps > MAXARP) 
					{
						//LogTextMessage("notes %u, total %u", recNotes, totalSteps);
						totalSteps -= steps;
						recNotes = i;
						break;
					}
				}
				//LogTextMessage("notes %u, total %u", recNotes, totalSteps);
				
				avgStep = (float)(totalSteps)/(float)(recFullTime);
				curArp->steps = totalSteps;
				//LogTextMessage("notes %u shortest %u half %u steps %u avg %f", recNotes, recShortest, halfBase, totalSteps, avgStep);
				
				uint8_t newInd = totalSteps; 
				
				for(int8_t oldInd = recNotes - 1; oldInd >= 0; oldInd--)
				{
					newInd -= stepsPer[oldInd];
					uint8_t clearSize = stepsPer[oldInd] - 1;
					if(clearSize)
					{	
						uint8_t clearInd = newInd + 1;
						memset(&curArp->P[clearInd], curArp->P[oldInd], clearSize);
						memset(&curArp->V[clearInd], 127, clearSize);
						memset(&curArp->E[clearInd], 0, clearSize);
					}
					curArp->P[newInd] = curArp->P[oldInd];
					curArp->V[newInd] = curArp->V[oldInd];	
					curArp->E[newInd] = curArp->E[oldInd];
				} 
			}
			
			updateArpTime(oscInd, avgStep * 180000.0);
			resetArpPages(oscInd, oscInd);
		}

	}
	/* else
	{
		updateLCDelems(OBJ3, OBJ6);
		//memset(&LCD_update[OBJ3], 1, 4);
		
		PIT_ENV_KNOBS *curEnv;
		if(screenInd == PITENV)
		{
			SETBIT(oscInd, bitPEnv);
			curEnv = &pit_env_knobs[oscInd];
		}
		else
		{
			SETBIT(oscInd, bitFEnv);
			curEnv = &filt_env_knobs[oscInd];
		}
		
		if(recNotes)
		{
			uint8_t free = recNotes -1;
			if(free > 3) free = 3;
			int8_t cent = curEnv->pitch[free];
			for(uint8_t stg = 0; stg < recNotes; stg++)
			{
				curEnv->pitch[stg]  = (curEnv->pitch[stg]-cent)<<PITCH_COARSE;
				for(uint8_t ind = 0; ind < 128; ind++)
				{
					if(recTimes[stg] < TIME[ind]) 
					{
						uint8_t actStg = stg;
						if(stg < free) curEnv->time[stg] = ind;
						else actStg = stg-free + FREE_STAGES;
						if(screenInd == FILTENV) curEnv->glide[actStg] = ind;
						break;
					}
				}
			}
			if(recNotes < FREE_STAGES + 2) 
			{
				curEnv->pitch[FREE_STAGES + 1] = 0;
				curEnv->glide[FREE_STAGES + 1] = curEnv->glide[FREE_STAGES];
			}
				
			for(uint8_t stg = free; stg < FREE_STAGES; stg++)
			{
				curEnv->time[stg] = 0;
			}
		}
	} */ 
	updateLEDs();
	return 1;
}

/* void __attribute__(( noinline )) equalizeAmp(uint8_t osc)
{
	if(SHIFTMASK(osc, bitAEnv))
	{
		for(uint8_t child = firstChild[osc]; child < firstChild[osc] + childCnt(osc); child++)
		{
			amp_env[child].val = 0x7FFFFFFF;
		}
	}
} */

int32_t __attribute__(( noinline )) unit_dir(int32_t inc)
{
	int32_t uInc = 0;
	if(inc > 0)
	{
		uInc = 1;
	}
	else if(inc < 0)
	{
		uInc = -1;
	}
	return uInc;
}

int32_t __attribute__(( noinline )) ensure_not_self_ratio(int32_t inc)
{
	int32_t ret = 0;
	if(pit_ratio[oscInd].src == oscInd)
	{
		inc = unit_dir(inc);
		pit_ratio[oscInd].src = indexIncrement(pit_ratio[oscInd].src, inc, 6);
		ret = 1;
		
	}
	LCD_update[OBJ2] = 1;
	return ret;
}

void __attribute__(( noinline )) updateUINT8val(uint8_t *val, int8_t inc, uint8_t isToggle, uint8_t LCD, uint32_t full)
{
	uint32_t t = *val;
	if(isToggle)
	{
		if(t == 0) t = (full ? 255 : 127);
		else t = 0;	
	}
	else
	{
		if(full) t = __USAT(t + inc, 8);
		else t = __USAT(t + inc, 7);
	}
	*val = t;
	LCD_update[LCD] = 1;//*val;
}

void handleKnobs()
{
	#if !LOADTABLES
	
	static uint8_t readInd = 0;
	static int32_t next_loop = 0;
	static int32_t next_inc = 0;
	static int32_t next_dir = 0;
	
	if(next_loop && !main_clock1)
	{
		int32_t act = next_loop;
		//main_clock = MAIN_FADE;
		next_loop = 0;
		switch(act)
		{
			//patch reset
			case EX_PATRNDCLR: resetPatch(); break;
			//filter track
			case EX_FTRACK: 
				TOGGLEBIT(oscInd, bitFTrack);
				if(SHIFTMASK(oscInd, bitFTrack)) filt_knobs[oscInd].FRQ = 0;
				else filt_knobs[oscInd].FRQ = (A4 + MIDI_KEY_0)<<PITCH_COARSE;
				LCD_update[OBJ2] = 1;
				LCD_update[OBJ3] = 1;
				break;
			//patch load
			case EX_PATCHLD:
				incrementFileIndex(PATCH, next_inc, next_dir);
				FIL_update[MAINTOG] = 1;//loadFile(fType, oscInd);
				updateLCDelems(OBJ1, OBJ6);
				break;
			//osc copy
			case EX_COPY:
				copyOsc(oscInd, next_inc, bitOsc);
				break;
		}
		updateLEDs();
	}

	if(inputQueue[readInd][2])
	{
		GRAPH_update = 1;
		//LogTextMessage("h%u",inputQueue[readInd][0]);
		uint8_t inputGrp = inputQueue[readInd][0];
		uint8_t inputInd = inputQueue[readInd][1];
		int32_t inc = inputQueue[readInd][2];
		inputQueue[readInd][2] = 0;
		
		
		int8_t done = 0;
		uint8_t osc = oscInd;
		int8_t scrn = -1;
		int8_t tog = -1;
		int8_t extra = 0;
		uint8_t wasCopy = SHIFTMASK(MAINTOG, bitCopy);
		uint8_t wasRoute = SHIFTMASK(MAINTOG, bitRoute);
		//CLEARBIT(MAINTOG, bitCopy);
		
		//if recording, stop and ignore the input
		if(finishRecording()) 
		{
			LCD_update[OBJ6] = 1;
		}
		
		//buttons
		else if(inputGrp < 5)
		{
			//main block
			if(inputGrp < 3)
			{
				uint8_t row = oscFromGrpInd(inputGrp, inputInd);
				if(SHIFTMASK(MAINTOG, bitDrum))
				{
					//toggle hits
					uint16_t eInd = (drumPage << 3) + (inputInd & 7); 
					arpeggio[row].E[eInd] = (arpeggio[row].E[eInd])? 0 : recEnv;
					
					tog = -1;
					scrn = -1;
					extra = -1;
				}
				else
				{			
					uint8_t col = (inputInd & 7);
					osc = row;
					tog = big_group[col][0];
					scrn = big_group[col][1];
					extra = big_group[col][2];
					
					//copy if applicable and cancel any other actions
					if(wasCopy)
					{
						if(tog == bitOsc)
						{
							next_loop = EX_COPY;
							main_clock1 = MAIN_FADE;
							main_clock2 = MAIN_FADE << 2;
							next_inc = oscInd;
						}
						else copyOsc(osc, oscInd, tog);
						
						tog = -1;
						extra = 0;
					}
					else if(wasRoute)
					{
						if(osc == oscInd)
						{
							blinkInd = inputInd;
							blinkGrp = inputGrp;
							routeTog = tog;
							wasRoute = 0;
						}
						else routeMod(osc, tog, routeTog);
						tog = -1;
						scrn = -1;
						extra = 0;
						osc = oscInd;
					}
					else if(tog == bitMain && SHIFTMASK(MAINTOG, bitSolo) && osc != oscInd) 
					{
						SETBIT(osc, bitMain);
						tog = -1;
						extra = 0;
					}
				}
			}
				
			//remaining group (varied)
			else
			{
				int32_t t_ind = inputInd + ((inputGrp - 3) << 4);
				osc = other_groups[t_ind][0];
				tog = other_groups[t_ind][1];
				scrn = other_groups[t_ind][2];
				extra = other_groups[t_ind][3];
			}
			
			if(osc == E_OSC) osc = oscInd;
	
			//tell non poly's to fuck off
			if(tog == bitPoly && osc >= POLY_CNT) 
			{
				tog = -1;
				extra = 0;
			}
			//tell filter track to fuck off too (needs to wait for volume reduction)
			if(tog == bitFTrack) tog = -1;
			
			int32_t scrnOsc = osc;
			
			//toggle stuff
			if(tog != -1 && inputQueue[readInd][3])
			{
				TOGGLEBIT(osc, tog); 	
				
				//default route from main of selected
				if(tog == bitRoute)
				{
					blinkGrp = oscInd >> 1;
					blinkInd = (oscInd & 1)? 15: 7;
					routeTog = bitOsc;
				}
				
				//don't change screens or osc if it's a toggle off
				if(!SHIFTMASK(osc, tog))
				{
					scrn = -1;
					scrnOsc = oscInd;
				}
			}
			
			//do any actions
			if(extra)
			{
				uint8_t isTog = inputQueue[readInd][3];
				switch(extra)
				{
					
					//case EX_WAVE: updateOscTypes(waveOsc, &waveCnt, noiseOsc, &noiseCnt, bitWave); break;
					//case EX_FILT: updateOscTypes(filtOsc, &filtCnt, nonFiltOsc, &nonFiltCnt, bitFilt); break;
					case EX_PIT_RATIO:
						pit_ratio_update |= (1 << oscInd);
						break;
					case EX_SYNC: 
						if(!isTog)
						{
							for(uint8_t tOsc = 0; tOsc < OSC_CHILD_CNT; tOsc++)
							{
								arp_env[tOsc].stage = MAXARP-1;
								arp_env[tOsc].clock = 65534;
							}
							
						}
						else if(SHIFTMASK(MAINTOG, tog))
						{
							updateArpTime(oscInd, arpeggio[oscInd].BPM);
						}
						break;
	
					case EX_PATRNDCLR: 
						if(!isTog) 
						{
							//do the randomizing
						}
						else
						{
							main_clock1 = MAIN_FADE;
							main_clock2 = MAIN_FADE << 2;
							next_loop = EX_PATRNDCLR;
							//resetPatch();
						}
						break;
					case EX_FAV1: 
					case EX_FAV2:
					case EX_FAV3:
					case EX_FAV4:
					case EX_FAV5: 
						favInd = (extra - EX_PATRNDCLR) + ((isTog)? 0: 5); 
						favSave = wasCopy; 
						favAction(favInd, favSave);
						if(!favSave)
						{
							//queue files load
							FIL_update[MAINTOG] = 1; //loadFile(PATCH, 0);
						}
						
						break;
					case EX_TRIG_ON:
						if(isTog) onEvent(oscInd, oscInd, ALL_SLOTS, 1);
						else onEvent(0, OSC_CNT-1, ALL_SLOTS, 1);
						break;
					case EX_TRIG_OFF:
						if(isTog) offEvent(oscInd, oscInd, ALL_SLOTS, 1);
						else offEvent(0, OSC_CNT-1, ALL_SLOTS, 1);
						break;
						
					case EX_POLY: if(isTog) togglePolyMono(oscInd, oscInd); break;
					case EX_HOLD1: offEvent(oscInd, oscInd, ALL_SLOTS, 0); break;
					case EX_HOLD_ALL: offEvent(0, OSC_CNT -1, ALL_SLOTS, 0); break;
					case EX_ARPNOTREC: scrn = (isTog)? ARPEGNOTES: ARPREC; break;
					case EX_PATSVLD: scrn = (isTog)? PATCHSV: PATCHLD; break;
					case EX_HARM: HARM_update[osc] = -1; break;
					case EX_FTRACK: 
						main_clock1 = MAIN_FADE;
						main_clock2 = MAIN_FADE << 2; 
						next_loop = EX_FTRACK;
						//if(SHIFTMASK(oscInd, bitFTrack)) filt_knobs[oscInd].FRQ = 0;
						//else filt_knobs[oscInd].FRQ = (A4 + MIDI_KEY_0)<<PITCH_COARSE;
						break;
					case EX_DRUM: 
						for(int32_t osc = 0; osc < OSC_CNT; ++osc)
						{
							SETBIT(osc, bitArpSkip);
							
							//CLEARBIT(osc, bitArpTrig);
						}
						drumPage = arp_page[oscInd] >> 1;
						break;
					

					//case EX_AMP_SET: if(inputQueue[readInd][3]) equalizeAmp(osc); break;
				}
			}
			
			//update the active oscillator
			//if(osc != oscInd && osc != MAINTOG) toggleSelected(osc);
			if(scrnOsc != MAINTOG) toggleSelected(scrnOsc);
			
			//update the screen
			if(scrn != -1) screenInd = scrn;
			
			//make sure copy and route are properly cleared after any action (except toggling them on)
			if(wasCopy) CLEARBIT(MAINTOG, bitCopy);
			if(wasRoute)
			{
				CLEARBIT(MAINTOG, bitRoute);
				blinkInd = -1;
			}			
			
			updateLCDelems(SCRN, OBJ6);//memset(&LCD_update[0], 1,  LCDelems);//
			//update leds
			updateLEDs();
			
			
		}

		
		//vol knobs
		else if((inputInd & 7) > 5)
		{
			isMainLVL = (inputInd & 1)? 0: 1;
			if(inputInd & 8) inc = -127;
			{
				uint8_t *lvl = (isMainLVL)? &main_gain: & osc_gain[oscInd];
				*lvl = __USAT(*lvl + inc, 7); 
			}
			LCD_update[LVL] = 1;
		}
		
		//screen knobs
		else
		{
			//LogTextMessage("k %u %u %d", inputGrp, inputInd, inc);
			switch(screenInd)
			{
				case WAVETBL:
					if(!SHIFTMASK(oscInd, bitWave))
					{
						SETBIT(oscInd, bitWave);
						//updateOscTypes(waveOsc, &waveCnt, noiseOsc, &noiseCnt, bitWave);
						updateLEDs();
					}
				case PATCHLD:
				{
					uint8_t fType = (screenInd == WAVETBL)? WAVE: PATCH;
					next_dir = !(inputInd & 1);
					next_inc = unit_dir(inc);
					if(fType == PATCH) 
					{
						main_clock1 = MAIN_FADE;
						main_clock2 = MAIN_FADE << 2;
						next_loop = EX_PATCHLD;
					}
					else
					{
						incrementFileIndex(fType, next_inc, next_dir);
						FIL_update[oscInd] = 1;//loadFile(fType, oscInd);
						updateLCDelems(OBJ1, OBJ6);
					}
				}
				break;
					

				
				case AMPENV:
				{
					uint8_t ind = knobPos(KNOB1, inputInd);//-1;
					updateUINT8val(&amp_env_knobs[oscInd].rate[0] + ind, inc, inputInd >= KNOB_BUT1, ind + OBJ1);

				}
				break;
				
				case PITCH:
				{
					PIT_KNOBS *curPits = &pit_knobs[oscInd];
					int32_t p_chg = 0;
					switch(inputInd)
					{
						//legato
						case KNOB1:
						case KNOB_BUT1:	
							TOGGLEBIT(osc, bitLgto);
							LCD_update[OBJ1] = 1;
							break;
							
						//edit pitch
						case KNOB3:
							inc = inc<<PITCH_FINE_RES;
						case KNOB4:
							inc = inc<<PITCH_FINE;
							curPits->pitch = __USAT(curPits->pitch + inc, PITCH_SAT);
							LCD_update[OBJ3] = 1;
							++p_chg;
							break;
						
						//zero coarse pitch
						case KNOB_BUT3:
							curPits->pitch = (MIDI_KEY_0<<PITCH_COARSE) + (curPits->pitch & PITCH_MASK);
							LCD_update[OBJ3] = 1;
							++p_chg;
							break;
						
						//zero fine pitch
						case KNOB_BUT4:
							curPits->pitch = curPits->pitch & ~PITCH_MASK;
							LCD_update[OBJ3] = 1;
							++p_chg;
							break;
						
						
						
						//edit pit/vel glide
						case KNOB6:
						case KNOB_BUT6:
							done = 1;
						case KNOB5:
						case KNOB_BUT5:
							updateUINT8val(&(curPits->pit_glide) + done, inc, inputInd >= KNOB_BUT1, OBJ5 + done);
							break;	
					}
					if(p_chg)
					{
						CLEARBIT(oscInd, bitPitRatio);
						pit_ratio_update = 0xFF;
						updateLEDs();
					}
					
				}
				break;
				
				case PITRATIO:
				{
					PIT_RATIO_KNOBS *cur = &pit_ratio[oscInd];
					switch(inputInd)
					{
						//src
						case KNOB1:
						case KNOB_BUT1:
						case KNOB2:
						case KNOB_BUT2:
							cur->src = indexIncrement(cur->src, inc, 6);
							ensure_not_self_ratio(inc);
							break;
						
						//dst value
						case KNOB3:
						case KNOB_BUT3:
							updateUINT8val(&cur->dst_val, inc, inputInd > KNOB8, OBJ3, 1);
							break;
						
						//src value
						case KNOB4:
						case KNOB_BUT4:
							updateUINT8val(&cur->src_val, inc, inputInd > KNOB8, OBJ4, 1);
							break;
						
						//offset
						case KNOB5:
							cur->offset = __SSAT(cur->offset + inc, 7);
							LCD_update[OBJ5] = 1;
							break;	
							
						case KNOB_BUT5:
							cur->offset = cur->offset != 0? 0 : 63;
							LCD_update[OBJ5] = 1;
							break;
						
						//mod shortcut
						case KNOB_BUT6:
							addToInputQueue(3, 14, 1, 1); //rt mod button
							addToInputQueue(cur->src / 2, 3 + ((cur->src & 1) ? 8 : 0), 1, 1); //pit env button for src
							break;
					}
					pit_ratio_update |= (1 << oscInd);
				}
				break;
				
				case PITENV: done = 1;
				case FILTENV:
				{
					PIT_ENV_KNOBS *curEnv = (done)? &pit_env_knobs[oscInd] : &filt_env_knobs[oscInd]; 
					switch(inputInd)
					{
						//look at different envelope point
						case KNOB_BUT1: 
						case KNOB1:
							envInd = indexIncrement(envInd, unit_dir(inc), FREE_STAGES + 2);
							//curLCD = OBJ1;
							updateLCDelems(OBJ1, OBJ6);
							break;
						
						//type/bypass
						case KNOB_BUT2:
						case KNOB2:
							if(!done) TOGGLEBIT(oscInd, bitFECut); LCD_update[OBJ2] = 1; break;
							
						//pitch
						case KNOB3:
							inc = inc<<PITCH_FINE_RES;
						case KNOB4:
							inc = inc<<PITCH_FINE;
							curEnv->pitch[envInd] = __SSAT(curEnv->pitch[envInd] + inc, 29);
							LCD_update[OBJ3] = 1;
							break;
						
						//zero coarse pitch
						case KNOB_BUT3: curEnv->pitch[envInd] = curEnv->pitch[envInd] & PITCH_MASK; LCD_update[OBJ3] = 1; break;
							
						/* //record
						case KNOB_BUT4: 
							CLEARBIT(oscInd, (done)? bitPEnv: bitFEnv);
							SETBIT(MAINTOG, bitRecEnv);
							recNotes = 0;
							LCD_update[OBJ4] = 1;
							
							break; */
						
						//time
						case KNOB6:
						case KNOB_BUT6:
							if(envInd < FREE_STAGES) 
							{
								updateUINT8val(&curEnv->time[envInd], inc, inputInd >= KNOB_BUT1, OBJ6);
								//curLCD = OBJ6;
								
								LCD_update[OBJ6] = 1;
								break;
							}
							
						//edit glide
						case KNOB5:
						case KNOB_BUT5: updateUINT8val(&curEnv->glide[envInd], inc, inputInd >= KNOB_BUT1, OBJ5); break;

						
					}
				}
				break;	
					
				
				
				case ARPEGSETUP:
				{
					ARP_KNOBS *curArp = &arpeggio[oscInd];
					
					switch(inputInd)
					{
						//beats
						case KNOB_BUT1:
						case KNOB1:
							if(inputInd > 7) curArp->steps = (curArp->steps == 1)? 4: 1;
							else curArp->steps = indexIncrement(curArp->steps-1, inc, MAXARP)+ 1;
							resetArpPages(oscInd, oscInd);
							//curLCD = OBJ1;
							
							LCD_update[OBJ1] = 1;
							break;
						
						case KNOB2:
						case KNOB_BUT2: TOGGLEBIT(oscInd, bitArpFilt); LCD_update[OBJ2] = 1; break;
						
						//BPM inc size
						case KNOB_BUT3:
						
							++indBPM &= 3;
		
							LCD_update[OBJ1] = 1;
							break;
						
						//BPM
						case KNOB3: updateArpTime(oscInd, curArp->BPM + inc * incsBPM[indBPM]); LCD_update[OBJ3] = 1; break;
						//case KNOB3: updateArpTime(oscInd, curArp->BPM + inc * BPM_inc); LCD_update[OBJ3] = 1; break;
							
						//glide rate
						case KNOB4:
						case KNOB_BUT4: updateUINT8val(&curArp->G, inc, inputInd >= KNOB_BUT1, OBJ4); break;
						
						//loop/trig
						case KNOB5:
						case KNOB_BUT5: TOGGLEBIT(osc, bitArpTrig); LCD_update[OBJ5] = 1; break;
						
						//skip/all
						case KNOB6:
						case KNOB_BUT6: TOGGLEBIT(osc, bitArpSkip); LCD_update[OBJ6] = 1; break;						
					}
				}
				break;
	
						
				case ARPEGNOTES:
				{
					ARP_KNOBS *curArp = &arpeggio[oscInd];
					uint8_t posInd = 0;
					switch(inputInd)
					{
						//page index
						case KNOB1:
						case KNOB_BUT1:
							arp_page[oscInd] = indexIncrement(arp_page[oscInd], unit_dir(inc), arp_pages[oscInd]);
							
							if(SHIFTMASK(MAINTOG, bitDrum))
							{
								drumPage = arp_page[oscInd] >> 1;
								//LogTextMessage("%d", drumPage);
								updateLEDs();
							}
							//curLCD = OBJ1;
							LCD_update[OBJ1] = 1;
							updateLCDelems(OBJ3, OBJ6);
							break;
						
						//display toggle
						case KNOB2:
						case KNOB_BUT2:
						{
							uint8_t tTog = indexIncrement(arpToggle[0], unit_dir(inc), (SHIFTMASK(oscInd, bitArpFilt))? 2: 3);
							memset(&arpToggle, tTog, sizeof(arpToggle));
							updateLCDelems(OBJ3, OBJ6);
						}
						break;
							
						//steps
						default:
							posInd = knobPos(KNOB3, inputInd);//(inputInd > KNOB6)? inputInd - KNOB_BUT3 : inputInd - KNOB3;
							uint8_t stepInd = posInd + (arp_page[oscInd] << 2);
							if(stepInd <= curArp->steps)
							{
								if(inputInd > KNOB6) arpToggle[posInd] = indexIncrement(arpToggle[posInd], inc, (SHIFTMASK(oscInd, bitArpFilt))? 2: 3);
								else if(arpToggle[posInd] == PIT_TOG) curArp->P[stepInd] = __SSAT(curArp->P[stepInd] + inc, 8);
								else if(arpToggle[posInd] == ENV_TOG) curArp->E[stepInd] = (curArp->E[stepInd] + inc) & 0x07;
								else curArp->V[stepInd] = __USAT(curArp->V[stepInd] + inc, 7);
								LCD_update[OBJ3 + posInd] = 1;
							}
							break;						
					}
				}
				break;
				
				case ARPREC:
					switch(inputInd)
					{
						case KNOB1:
						case KNOB_BUT1:
							recRhythm = (recRhythm == 1)? 0: 1;
							LCD_update[OBJ1] = 1;
							break;
						
						case KNOB2:
						case KNOB_BUT2:
							recVel = (recVel == 1)? 0: 1;
							LCD_update[OBJ2] = 1;
							break;	

						case KNOB6: break;
						case KNOB_BUT6: 
						{
							CLEARBIT(oscInd, bitArp);
							SETBIT(MAINTOG, bitRecArp);
							recNotes = 0;
							//LogTextMessage("r");
							LCD_update[OBJ6] = 1;
						}
						break;
						
						case KNOB_BUT3:
							recEnv = (recEnv == 0)? 7: 0;
							done = 1;
						case KNOB3:
							if(!done) recEnv = (recEnv + inc) & 0x07;
							LCD_update[OBJ3] = 1;
							break;
		
					}	
					break;
				
				case FILTER:
				{
					FILT_KNOBS *curFilt = &filt_knobs[oscInd];
					
					switch(inputInd)
					{	
						
						
						//type
						case KNOB1:
						case KNOB_BUT1:
						
							curFilt->TYPE = indexIncrement(curFilt->TYPE, unit_dir(inc), 3);
							//curLCD = 2;
							LCD_update[OBJ1] = 1;
							break;
						
						//track keys
						case KNOB2:
						case KNOB_BUT2:
							main_clock1 = MAIN_FADE;
							main_clock2 = MAIN_FADE << 2; 
							next_loop = EX_FTRACK;
							break;
						
						//edit pitch
						case KNOB3:
							inc = inc<<PITCH_FINE_RES;
						case KNOB4:
							inc = inc<<PITCH_FINE;
							if(SHIFTMASK(oscInd, bitFTrack)) curFilt->FRQ = __SSAT(curFilt->FRQ + inc, PITCH_SAT);
							else curFilt->FRQ = __USAT(curFilt->FRQ + inc, PITCH_SAT);
							LCD_update[OBJ3] = 1;
							break;
						
						//zero coarse pitch
						case KNOB_BUT3:
							if(SHIFTMASK(oscInd, bitFTrack)) curFilt->FRQ = (curFilt->FRQ & PITCH_MASK);
							else
							{
								if((curFilt->FRQ>>PITCH_COARSE) == A4 + MIDI_KEY_0) 
									curFilt->FRQ = LFO_FREQ<<PITCH_COARSE;
								else
									curFilt->FRQ = (A4 + MIDI_KEY_0)<<PITCH_COARSE;
							}
							LCD_update[OBJ3] = 1;
							break;
						
						//zero fine pitch
						case KNOB_BUT4:
							curFilt->FRQ = curFilt->FRQ & ~PITCH_MASK;
							LCD_update[OBJ3] = 1;
							break;
							
						//filter res
						case KNOB5:
						case KNOB6:
						case KNOB_BUT5:
						case KNOB_BUT6:
							updateUINT8val(&curFilt->RES, inc, inputInd >= KNOB_BUT1, OBJ5);
							break;	
					}
				}
				break;	

					
				case PATCHSV:
					switch(inputInd)
					{
						//change dir
						case KNOB1:
						case KNOB_BUT1:
						case NEXT_PREV:
							saveDirInd = indexIncrement(saveDirInd, unit_dir(inc) , browseCnt[PATCH].dirs);
							LCD_update[OBJ1] = 1;	
							LCD_update[OBJ2] = 1;	
						break;
						
						case KNOB_BUT2:
							strcpy(saveName, saveCopyStr[2]);
							saveNameInd = 0;
							LCD_update[OBJ5] = 1;
							break;
							
						//edit current letter
						case KNOB3:
						case KNOB5:
						{
							if(saveName[saveNameInd] == ' ')
								saveName[saveNameInd] = lastLetter;
							else
							{
								uint8_t lwr = 'A';
								uint8_t upr = 'Z';
								
								if(saveName[saveNameInd] < '0') {upr = ')'; lwr = '!';}
								else if(saveName[saveNameInd] < 'A') {upr = '9'; lwr = '0';}
								
								saveName[saveNameInd] += inc;
								if(saveName[saveNameInd] > upr) saveName[saveNameInd] = upr;
								else if(saveName[saveNameInd] < lwr) saveName[saveNameInd] = lwr;
								
								if(saveName[saveNameInd] > '9') lastLetter = saveName[saveNameInd];
							}
							LCD_update[OBJ5] = 1;
						}
						break;
						
						//edit current letter type (upper/lower/number)
						case KNOB_BUT3:
						case KNOB_BUT5:
							if(saveName[saveNameInd] >= 'A') saveName[saveNameInd] = ' ';
							else if(saveName[saveNameInd] == ' ') saveName[saveNameInd] = '0';
							else if(saveName[saveNameInd] >= '0') saveName[saveNameInd] = '!';
							else saveName[saveNameInd] = 'A';
							
							LCD_update[OBJ5] = 1;
							break;
						
						//change letter index
						case KNOB4:
						case KNOB6:
						
							
							saveNameInd = indexIncrement(saveNameInd, unit_dir(inc), MAXFNAMELEN-1);
							LCD_update[OBJ5] = 1;
							break;
							
						//save file
						case KNOB_BUT4:
						case KNOB_BUT6:
							if(savePatch() > 0) {isSaved = 1; LCD_update[OBJ5] = 1;}
							//knob_incs[inputInd] = 0;
							break;
					}
				break;
				
				case MIDIINS:
				{
					uint8_t done = 0;
					MIDI_PARAMS *curKnobs = &midi_knobs[oscInd];
					switch(inputInd)
					{
						//midi channel toggle
						case KNOB_BUT1:
						case KNOB1:
							if(inputInd > KNOB6) curKnobs->chan = (curKnobs->chan > 0)? 0: 1;
							else curKnobs->chan = indexIncrement(curKnobs->chan, inc, 17);
							//initOscMidi(oscInd, oscInd);
							LCD_update[OBJ1] = 1;
							break;			
						
						//high key
						case KNOB2:
						case KNOB_BUT2: 
						case KNOB4: 
						case KNOB_BUT4:
						{
							uint8_t type = (inputInd > KNOB6)? 0 : inc;
							uint8_t isLow = (inputInd & 7) - KNOB2;
							uint8_t *high = &curKnobs->keyMax;
							uint8_t *low = &curKnobs->keyMin;

							if(!type)
							{
								if(isLow) *low = A0;
								else *high = C8;
							}
							else
							{
								if(isLow) *low = bounded(*low, inc, A0, C8);
								else *high = bounded(*high, inc, A0, C8);
								
								if(*high < *low)
								{
									if(isLow) *high = *low;
									else *low = *high; 
								}
							}
							//initOscMidi(oscInd, oscInd);
							LCD_update[OBJ2] = 1;
							LCD_update[OBJ4] = 1;
						}
						break;
							
							
						//vel type
						/* case KNOB3:
						case KNOB_BUT3:
							if(SHIFTMASK(oscInd, bitWind))
							{
								CLEARBIT(oscInd, bitWind);
								updateLEDs();
							}
							else TOGGLEBIT(oscInd, bitKeyVel);
							LCD_update[OBJ3] = 1;
							break; */
							
						case KNOB5:
						case KNOB_BUT5:
							TOGGLEBIT(osc, bitLgto);
							LCD_update[OBJ5] = 1;
							break;

					}
				}
				break;
				
				case MIDICCS:
				{
					uint8_t ind = -1;
					if(inputInd >= KNOB7 && inputInd <= KNOB6) ind = inputInd - KNOB1;
					else if(inputInd >= KNOB_BUT1 && inputInd <= KNOB_BUT6) ind = inputInd - KNOB_BUT1;
					if(ind != -1) updateUINT8val(&midi_knobs[oscInd].CC_nums[0] + ind, inc, inputInd >= KNOB_BUT1, ind + OBJ1);
				}	
				break;
					
				case MODA:
				{
					uint8_t ind = knobPos(KNOB1, inputInd);
					/* if((inputInd - KNOB1) & 1)
					if(inputInd >= KNOB1 && inputInd <= KNOB4) ind = inputInd - KNOB1;
					else if(inputInd >= KNOB_BUT1 && inputInd <= KNOB_BUT4) ind = inputInd - KNOB_BUT1;
					if(ind != -1) */
					{
						if(inputInd > KNOB6) mod_src[oscInd][ind] = (mod_src[oscInd][ind] == 0)? 1 : 0;
						else mod_src[oscInd][ind] = indexIncrement(mod_src[oscInd][ind], inc, TOTAL_MODS);
						updateSingleMod(ind, oscInd, mod_src[oscInd][ind]);
						LCD_update[ind + OBJ1] = 1;
					}
				}		
				break;
				
				case OUTS:
				{
					int32_t knob = knobPos(KNOB1, inputInd);
					if(knob < 2)
					{
						if(inputInd > KNOB6)
						{
							panLeft[oscInd] = 64;//((inputInd - KNOB1) & 1)? ((panLeft[oscInd] == 0)? 127 : (panLeft[oscInd] > 64)? 64: 0;) : 
																		//((panLeft[oscInd] == 127)? 0 : (panLeft[oscInd] < 64)? 64: 127);
						}
						else panLeft[oscInd] = __USAT(panLeft[oscInd] - inc, 7);
						updateLCDelems(OBJ1, OBJ2);//memset(&LCD_update[OBJ1], 1, 2);
					}
					//else
					//{
					//	const uint32_t sh = 5;
					//	if(!(knob & 1) || knob > 3)
					//	{
					//		updateUINT8val(&delay_left_knobs[oscInd], inc, inputInd > KNOB6, OBJ3, 1); 
					//		delay_read_left[oscInd] = delay_write - (delay_left_knobs[oscInd] << sh) - 1;
					//	}
					//	if((knob & 1) || knob > 3)
					//	{
					//		updateUINT8val(&delay_right_knobs[oscInd], inc, inputInd > KNOB6, OBJ4, 1); 
					//		delay_read_right[oscInd] = delay_write - (delay_right_knobs[oscInd] << sh) - 1;
					//	}
					//}
				}
				break;
				
				
				
				case NOTES:
				{
					uint8_t stepInd = 0;
					if(oscInd >= POLY_CNT) notesPage = 0;
					switch(inputInd)
					{
						//page index
						case KNOB1:
						case KNOB_BUT1:
						if(oscInd < POLY_CNT)
							{
								notesPage = (notesPage)? 0: 1;
								//curLCD = OBJ1;
								updateLCDelems(OBJ1, OBJ6);//memset(&LCD_update[OBJ1], 1, 6);//		
							}								
							break;
						
						//pitch/vel toggle
						case KNOB2:
						case KNOB_BUT2:
							notesTog = (notesTog)? 0: 1;
							updateLCDelems(OBJ2, OBJ6);
							break;
						
						//steps
						default:
							
							stepInd = (notesPage << 2) + inputInd - ((inputInd > KNOB6)? KNOB_BUT3: KNOB3);
							if(stepInd < childCnt(oscInd))
							{
								uint8_t child = firstChild[oscInd] + stepInd;
								//LogTextMessage("h");
								LCD_update[(stepInd & 3) + OBJ3] = 1;
								if(inputInd > KNOB6)
								{
									if(!notesTog) pitchShift = (pitchShift == PITCH_COARSE)? PITCH_FINE: PITCH_COARSE;
									else vel[child] = -monoVel[oscInd];
								}
								else
								{
									
									if(!notesTog)
									{
										int32_t offPit = pit_knobs[oscInd].pitch + monoPitch[oscInd];
										note[child] = __USAT(offPit + note[child] + (inc<<pitchShift), 29) - offPit;
									}
									else vel[child] = __USAT(vel[child] + monoVel[oscInd] + inc, 7) - monoVel[oscInd];
								}
							}
							break;

							
					}					
				}
				break;
				
				case HARMONIC:
				{
					uint8_t ind = knobPos(KNOB1, inputInd);//-1;
					updateUINT8val(&harmParams[oscInd].gainFund + ind, inc, inputInd >= KNOB_BUT1, ind + OBJ1);
					if(ind == 1  && !*(&harmParams[oscInd].gainFund + ind))
					{
						*(&harmParams[oscInd].gainFund + ind) = 1;
					}
					HARM_update[oscInd] = -1;
				}
				break;
				
				case PHASE:
				{
					uint8_t pos = knobPos(KNOB1, inputInd);
					if(pos < 4)
					{
						pos = (pos & 1)? 1 : 0;
						uint8_t *p = &phase_knobs[oscInd].before_harm + pos;
						*p = !(*p);
						LCD_update[OBJ1 + pos] = 1;
					}
					else
					{
						pos = (pos & 1)? 1 : 0;
						if(inputInd >= KNOB_BUT1)
						{
							*(&phase_knobs[oscInd].before_phase + pos) = 127;
							LCD_update[OBJ3 + pos] = 1;
						}
						else 
						{
							updateUINT8val(&phase_knobs[oscInd].before_phase + pos, inc, 0, pos + OBJ3, 1);
						}
					}
					HARM_update[oscInd] = -1;
				}
				break;

				
				
			}
		}
	}
	++readInd &= 0x03;
	
	#endif
}
	

void __attribute__(( noinline )) updatePitRatio()
{
	static int32_t uInd = 0;
	if((pit_ratio_update >> uInd) & 1)
	{
		pit_ratio_update &= ~(1 << uInd);
		PIT_RATIO_KNOBS *cur = &pit_ratio[uInd];
		if(SHIFTMASK(uInd, bitPitRatio))
		{
			float ratio = float(cur->dst_val + 1) / float(cur->src_val + 1);
			uint32_t goalPhase = float(getPhaseInc(pit_knobs[cur->src].pitch)) * ratio;
			pit_knobs[uInd].pitch = getPitch(goalPhase) + (cur->offset << PITCH_FINE);
		}
	}
	uInd = indexIncrement(uInd, 1, 6);
}
		
void __attribute__(( noinline )) updateSingleMod(uint8_t modType, uint8_t destParent, uint8_t sourceIndex)
{
	uint8_t children = childCnt(destParent);//(destParent < POLY_CNT)? NOTES_CNT : 1;
	uint8_t firstDest = firstChild[destParent];
	
	int32_t *src = NULL;
	
	//simple mods
	switch(sourceIndex)
	{
		case MOD_NONE: src = (modType == GATE_MOD)? &maxMod : &zeroMod; break;
		case MOD_MAIN_OUT: src = &lastMain; break;
		case MOD_AUDIO_L: 
		case MOD_AUDIO_R:
		case MOD_AUDIO_MX: 
			src = &lastAudio[MOD_AUDIO_MX - sourceIndex]; break;
	}
	if(src)
	{
		for(uint8_t j = 0; j < children; j++)
		{
			modSrc[modType][firstDest + j] = src;
		}
	}
	//osc/env mods
	else
	{
		uint8_t eInd = (--sourceIndex) % TOTAL_MOD_SRC;
		uint8_t oInd = (sourceIndex) / TOTAL_MOD_SRC;
		uint8_t srcInc = (oInd < POLY_CNT)? 1: 0;
		uint8_t sourceChild = firstChild[oInd];
		for(uint8_t child = 0; child < children; child++)
		{
			if(eInd < 4) modSrc[modType][firstDest + child] = &kCCs[eInd][oInd];
			else if(eInd == OSC_SRC) modSrc[modType][firstDest + child] = (srcInc && children == 1) ? &lastPolyCombo[oInd] : &lastSignal[sourceChild];
			else if(eInd == AENV_SRC) modSrc[modType][firstDest + child] = (int32_t *)&amp_env[sourceChild].val;
			else if(eInd == PENV_SRC) modSrc[modType][firstDest + child] = &pit_env[sourceChild].val;
			else if(eInd == FENV_SRC) modSrc[modType][firstDest + child] = &filt_env[sourceChild].val; 
			else modSrc[modType][firstDest + child] = &arp_env[sourceChild].val;
			sourceChild += srcInc;
		}
	}
	/* //no mod
	if(!sourceIndex)
	{	
		for(uint8_t j = 0; j < children; j++)
		{
			modSrc[modType][firstDest + j] = &zeroMod;
		}
		
	}
	
	//main source
	else if(sourceIndex == TOTAL_MODS)
	{
		for(uint8_t j = 0; j < children; j++)
		{
			modSrc[modType][firstDest + j] = &lastMain;
		}
	}
	
	//CC source
	else if(sourceIndex >= OSC_CNT)
	{
		uint8_t CC_ind = sourceIndex - OSC_CNT;
		for(uint8_t j = 0; j < children; j++)
		{
			modSrc[modType][firstDest + j] = &kCCs[CC_ind][destParent];
		}
	}
	
	
	//mono source
	else if(sourceIndex >= POLY_CNT)
	{
		uint8_t sourceChild = firstChild[sourceIndex];
		for(uint8_t j = 0; j < children; j++)
		{
			modSrc[modType][firstDest + j] = &lastSignal[sourceChild];
		}
	}
	
	//poly source to mono dest
	else if(destParent >= POLY_CNT)
	{	
		modSrc[modType][firstDest] = &lastPolyCombo[sourceIndex];
	}
		
	//poly source to poly dest
	else
	{
		uint8_t firstSource = firstChild[sourceIndex];
		for(uint8_t j = 0; j < children; j++)
		{
			modSrc[modType][firstDest + j] = &lastSignal[firstSource + j];
		}
	} */
			
}

void updateAllMod(uint8_t first, uint8_t last)
{
	for(uint8_t mod = 0; mod < MOD_CNT; mod++)
	{
		for(uint8_t osc = first; osc <= last; osc++)
		{
			updateSingleMod(mod, osc, mod_src[osc][mod]);
		}
	}
}

void __attribute__((noinline)) updateArpTime(uint8_t osc, float newBPM)
{
	//LogTextMessage("o %u bpm %f", osc, newBPM);
	if(newBPM > 9999) arpeggio[osc].BPM = 9999;
	else if(newBPM < 6) arpeggio[osc].BPM = 6;
	else arpeggio[osc].BPM = newBPM;
	arpeggio[osc].T = (uint16_t)((float)(180000) / arpeggio[osc].BPM);
	
	
	if(SHIFTMASK(MAINTOG, bitArpSync))
	{
		uint8_t sz = sizeof(arpeggio[osc].BPM) + sizeof(arpeggio[osc].T);
		//LogTextMessage("%u sz", sz);
		for(uint8_t dstOsc = 0; dstOsc < OSC_CNT; ++dstOsc)
		{
			if(dstOsc != osc) memcpy(&arpeggio[dstOsc].T, &arpeggio[osc].T, sz);
		}
	}
	//LCD_update[OBJ3] = 1;
}


void __attribute__((noinline)) resetArpPages(uint8_t firstOsc, uint8_t lastOsc)
{
	for(uint8_t osc = firstOsc; osc <= lastOsc; osc++)
	{
		arp_pages[osc] = ((arpeggio[osc].steps) >> 2) + (((arpeggio[osc].steps) & 0x03)? 1 : 0);
		arp_page[osc] = 0;
	}
}


uint8_t __attribute__(( noinline )) knobPos(uint8_t zeroKnob, uint8_t knobID)
{	
	uint8_t ind = knobID - zeroKnob;
	if(knobID > KNOB6) ind -= 8;
	return ind;
}

uint8_t __attribute__(( noinline )) bounded(uint8_t val, int8_t inc, uint8_t min, uint8_t max)
{
	int16_t num = val + inc;
	if(num > max) return max;
	else if(num < min) return min;
	else return num;
	
}

#endif 