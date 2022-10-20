

#include <string.h>
#include "settings.h"
#include "synthControls.h"
#include "synthVariables.h"
#include "helperFunctions.h"
#include "fileLoader.h"
#include "pitchTables.h"
#include "lcdLib.h"
#include "modules.h"
#include "midiHandler.h"


void  addToInputQueue(uint8_t group, uint8_t ind, int32_t val, uint8_t isQuick)
{
	if (group == 4 && ind > 7) ++group;
	//rt_printf("input grp %d ind %d, val %d, isQuick %d\n", group, ind, val, isQuick);
	static uint8_t writeInd = 0;
	inputQueue[writeInd][0] = group;
	inputQueue[writeInd][1] = ind;
	inputQueue[writeInd][2] += val;
	inputQueue[writeInd][3] = isQuick;
	++writeInd &= 0x03;
}


int32_t checkSwitch(int group, int ind, int signal)
{
	static uint32_t timer[5] = { 0 };
	static uint16_t held[5] = { 0 };
	static uint16_t was[5] = { 0 };
	static uint32_t initialized[5] = { 0 };
	
	int ret = 0;

	if (!((initialized[group] >> ind) & 1))
	{
		initialized[group] |= (1 << ind);
		was[group] |= (1 << ind);
	}
	int prev = (was[group] >> ind) & 1;
	if (prev != signal)
	{
		was[group] ^= (1 << ind);

		if (!signal)
		{
			timer[group] = ticks;
			held[group] &= ~(1 << ind);
		}
		else if (!((held[group] >> ind) & 1))
		{
			addToInputQueue(group, ind, 1, 1);
			ret = 1;
		}
	}
	else if (!signal && !((held[group] >> ind) & 1) && ticks - timer[group] > HOLD_TIME)
	{
		held[group] |= (1 << ind);
		addToInputQueue(group, ind, 1, 0);
		ret = 1;
	}
	return ret;
}


int32_t check_knob_turns(uint32_t knobInd, uint32_t a, uint32_t b)
{
	static uint8_t cnt[8] = { 0 };
	static uint8_t A_seq[8] = { 0 };
	static uint8_t B_seq[8] = { 0 };
	static uint32_t initialized = 0;
	static int32_t last_dir[8] = { 0 };
	int32_t ret = 0;
	static int32_t t_knob_state[8] = { 0 };
	static int32_t t_elapsed[8] = { 0 };
	static int32_t elapsed[8] = { 0 };

	if (!((initialized >> knobInd) & 1))
	{
		initialized |= (1 << knobInd);
		A_seq[knobInd] = a;
		B_seq[knobInd] = b;
		KNOB_TICKS[knobInd] = ticks;
	}

	uint32_t A_chg = a != (A_seq[knobInd] & 1);
	uint32_t B_chg = b != (B_seq[knobInd] & 1);

	//check for knob turns
	if (A_chg || B_chg)
	{
		A_seq[knobInd] <<= 1;
		A_seq[knobInd] |= a;
		B_seq[knobInd] <<= 1;
		B_seq[knobInd] |= b;

		t_elapsed[knobInd] = ticks - KNOB_TICKS[knobInd];
		int32_t dir = ((B_chg && b != a) || (A_chg && a == b)) ? -1 : 1;
		if (t_elapsed[knobInd] > 2000)
		{
			dir = 0;
			cnt[knobInd] = 0;
		}
		else if (dir != last_dir[knobInd] && t_elapsed[knobInd] < 16)
		{
			dir = last_dir[knobInd];
		}
		int32_t mult = 1;
		if (t_elapsed[knobInd] < 100) mult <<= 1;
		if (t_elapsed[knobInd] < 50) mult <<= 1;
		if (t_elapsed[knobInd] < 20) mult <<= 1;
		
		
		t_knob_state[knobInd] += dir * mult;
		elapsed[knobInd] += t_elapsed[knobInd];
		KNOB_TICKS[knobInd] = ticks;
		last_dir[knobInd] = dir;
		++cnt[knobInd];



		if (A_chg && a && cnt[knobInd] >= 3)
		{
			mult = (t_knob_state[knobInd] > 1) ? 1 : (t_knob_state[knobInd] < 0) ? -1 : 0;

			if (elapsed[knobInd] < 48) mult <<= 1;
			if (elapsed[knobInd] < 100) mult <<= 1;
			if (elapsed[knobInd] < 200) mult <<= 1;

			addToInputQueue(KNOB_GRP, knobInd, mult, 0);
			KNOB_TICKS[knobInd] = ticks;
			last_dir[knobInd] = dir;
			elapsed[knobInd] = 0;
			t_knob_state[knobInd] = 0;
			cnt[knobInd] = 0;

			ret = 1;
		}

	}
	return ret;
}


void scanInputs(BelaContext* context)
{
	static uint8_t ind = 0;
	const float onLevel = 3.3 / 5.0;
	const int32_t offset = context->analogFrames - 2;
	const int32_t d_offset = context->digitalFrames - 2;
	const float thresh = 0.2;
	check_knob_turns((ind - 1) & 0x03, analogRead(context, offset, 0) > thresh, analogRead(context, offset, 1) > thresh);
	check_knob_turns(((ind - 1) & 0x03) + 4, analogRead(context, offset, 2) > thresh, analogRead(context, offset, 3) > thresh);

	for (int i = 0; i < 4; ++i)
	{
		joyVal[i] = 0.0f;
		for (int j = 0; j < context->analogFrames; ++j)
		{
			joyVal[i] += analogRead(context, j, i + 4);
		}
		joyVal[i] /= float(context->analogFrames);
	}

	for (int i = 0; i < 5; ++i)
	{
		checkSwitch(i, (ind - 1) & 0xF, digitalRead(context, d_offset, i));
	}

	//increment channel
	ind = (ind + 1) & 0x0F;


	//set the channel for the next read
	//set address pins
	for (int channel = 4; channel < 8; channel++)
	{
		analogWrite(context, 0, channel, float((ind >> (channel - 4)) & 1) * onLevel);
	}


	//set led pins
	for (int channel = 0; channel < 4; ++channel)
	{
		int on = (LED[channel] >> ind) & 1;
		if (channel == blinkGrp && ind == blinkInd)
		{
			on = (ticks >> 10) & 1;
		}
		analogWrite(context, 0, channel, onLevel);
		analogWrite(context, 2, channel, onLevel * float(on));
	}
}





//runs once at startup
int initSynthStuff(BelaContext* context, void* userData)
{	

	#if LOADTABLES
	save_data_arrays();
	#endif
	read_data_arrays();
	//get the pointers to the settings variables
	/*void *ptrs[] = {
		toggles, osc_gain, panLeft, midi_knobs,
		pit_knobs, amp_env_knobs, pit_env_knobs, filt_env_knobs, 
		filt_knobs, mod_src, arpeggio, harmParams,
		phase_knobs, pit_ratio,
		amp_env, pit_env, filt_env, arp_env, 
		vel, note, monoPitch, monoVel 
	};
	

	memcpy(varPtrs, ptrs, sizeof(ptrs));*/

	for (int i = 0; i < 5; ++i)
	{
		pinMode(context, 0, i, INPUT); //set input
	}

	//printf("analog frames %d\n", context->analogFrames);
	// Check if analog channels are enabled
	if (context->analogFrames == 0 || context->analogFrames > context->audioFrames) {
		rt_printf("Error: this example needs analog enabled, with 4 or 8 channels\n");
		return -1;
	}

	for (unsigned int channel = 0; channel < 4; channel++)
	{
		analogWrite(context, 0, channel, 0);
	}

	
	//clear the note and cc events arrays
	memset(&midiEvents, DEAD_MIDI_EVENT, sizeof(midiEvents));

	//initialize buttons/knobs
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
	return 0;
	
}	


	
//runs when a patch is loaded/reset/randomized	
void initPatch(uint8_t first, uint8_t last)
{
	resetArpPages(first, last);
	updateAllMod(first, last);

	FIL_update = ~0;
	FIL_update &= ~(1 << MAIN_FIL);

	toggleSelected(oscInd);
	updateLCDelems(OBJ1, OBJ6);
	updateLEDs();

	memset(lastPhaseName, 0, MAXFNAMELEN);
};

//uint8_t onTogs[3] = {bitArpTrig, bitFTrack, bitFECut};


//clear patch to default settings
void resetPatch()
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
	
	uint8_t resetTogs[11] = {bitOsc, bitMain, bitNotes, bitEnvs, bitAEnv, bitWave, bitWave + 1, bitWave + 2, bitWave + 3, bitPoly, bitKeyVel};
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
		for (int32_t j = 0; j < TABLE_CNT; ++j)
		{
			setFileIndexFromName(WAVE, &curWavFile[i][j], def_wave[j]);
			setFileIndexFromName(WAVE, &phaseFile[i][j], def_wave[0]);
		}

		env1_knobs[i].send[0] = e_TBLX;
		env1_knobs[i].send[1] = e_TBLY;
		env2_knobs[i].send[0] = e_FCUT;
		env2_knobs[i].send[1] = e_FRES;

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
		
		for (int32_t j = 0; j < TABLE_CNT; ++j)
		{
			harmParams[i][j].gainFund = 127;
			harmParams[i][j].gainFirst = 90;
			harmParams[i][j].gainLast = 20;
			harmParams[i][j].first = 2;
			harmParams[i][j].cnt = 1;
			harmParams[i][j].step = 1;

			phase_knobs[i][j].before_harm = 1;
			phase_knobs[i][j].gain = 32;
			phase_knobs[i][j].phase = 0;
			phase_knobs[i][j].partial = 0;
		}
		
		
		
		
		filt_knobs[i].FRQ = (A4 + MIDI_KEY_0)<<PITCH_COARSE;

		
	}

	
	osc_gain[0] = 50;
	amp_env_knobs[0].rate[0] = 2;
	pit_knobs[0].vel_glide = 0;

	if(midiThru) SETBIT(MAINTOG, bitMidiThru);
	
	initPatch(0, OSC_CNT-1);
	initOscMidi(0, OSC_CNT - 1);
	
}



void phaseWidth(int32_t osc, int32_t tbl, int32_t *in_arr, int32_t *out_arr)
{
	PHASE_KNOBS* phase = &phase_knobs[osc][tbl];

	if (strncmp(phaseFile[osc][tbl]->name, lastPhaseName, MAXFNAMELEN - 1) != 0)
	{
		memcpy(lastPhaseName, phaseFile[osc][tbl]->name, MAXFNAMELEN);
		readWaveFile(phaseFile[osc][tbl], (int8_t*)phaseArray);
		usleep(SLEEP_MICROS);;
		//rt_printf("read file: '%s'\n", phaseFile[osc][tbl]->name);
	}
	
	const float MAX_F = float(int32_t(0x7FFFFFFF));
	const float MAX_PHASE = float(uint32_t(0xFFFFFFFF));
	const float mult = float(phase->gain) / float(32.0) / MAX_F;
	for (int32_t band = 0; band < TABLE_BAND_CNT; ++band)
	{
		uint32_t offset = TABLE_BAND_OFFSET[band];
		const uint32_t incPhase = 0xFFFFFFFF / TABLE_RES[band];
		const uint32_t incWaveX = phase->partial + 1;
		uint32_t indWaveX = float(phase->phase) / float(255.0) * float(TABLE_RES[band]);
		float curPhase = 0;
		for (uint16_t i = 0; i < TABLE_RES[band]; ++i)
		{
			curPhase += float(incPhase) * float(1 + float(phaseArray[indWaveX + offset]) * mult);
			while (curPhase > MAX_PHASE) curPhase -= MAX_PHASE;
			while (curPhase < 0) curPhase += MAX_PHASE;
			uint32_t tPhase = (uint32_t)curPhase;
			uint32_t wavInd = (tPhase >> TABLE_BAND_COARSE_SHIFT[band]) & TABLE_BAND_LAST_MASK[band];
			uint32_t off = ((tPhase)&TABLE_BAND_FINE_MASK[band]) << TABLE_BAND_FINE_SHIFT[band];
			int32_t amp1 = *(in_arr + offset + wavInd);
			int32_t amp2 = *(in_arr + offset + ((wavInd + 1) & TABLE_BAND_LAST_MASK[band]));
			int32_t sig = ___SMMUL(amp1, INT_MAX - off);
			sig = ___SMMLA(amp2, off, sig) << 1;
			*(out_arr + i + offset) = sig;

			indWaveX = (indWaveX + incWaveX) & TABLE_BAND_LAST_MASK[band];
			
		}
		if(!band) usleep(SLEEP_MICROS);;
	}
}
		
	
	

	
void checkHarmQueue()
{
	static uint8_t curInd = 0;
	
	if(HARM_update & (1 << curInd))
	{
		int32_t curOsc, curTbl;
		getOscTblInd(curInd, curOsc, curTbl);
		HARMONICS* curHarm = &harmParams[curOsc][curTbl];
		PHASE_KNOBS* cur_phase = &phase_knobs[curOsc][curTbl];
		
		static uint8_t curPart = 1;
		static int32_t tArr[TABLE_FULL_SIZE] __attribute__((section(".sdram")));
		static int32_t base_arr[TABLE_FULL_SIZE];
		uint32_t gain;
		
		//mark count as completed
		HARM_update &= ~(1 << curInd);

		////apply before phasing to base wave
		if(SHIFTMASK(curOsc, bitPhase + curTbl) && cur_phase->before_harm)
		{
			phaseWidth(curOsc, curTbl, wavArray[curOsc][curTbl], tArr);
			usleep(SLEEP_MICROS);;
		}
		////otherwise copy base wave
		else
		{
			memcpy_safe((uint8_t *)tArr, (uint8_t*)(wavArray[curOsc][curTbl]), sizeof(tArr));
		}
		
		//add harmonics
		if(SHIFTMASK(curOsc, bitHarms + curTbl))
		{
			memcpy(base_arr, tArr, sizeof(base_arr));
			
			for (int32_t band = 0; band < TABLE_BAND_CNT; ++band)
			{
				uint32_t offset = TABLE_BAND_OFFSET[band];
				uint32_t res = TABLE_RES[band];
				uint32_t part_limit = TABLE_BAND_PARTIAL_MAX[band];
				uint32_t ind_mask = TABLE_BAND_LAST_MASK[band];
				gain = GAIN[curHarm->gainFund];
				for (uint32_t i = 0; i < res; ++i)
				{
					tArr[i + offset] = ___SMMUL(base_arr[i + offset], gain);//<<1;	
				}

				curPart = curHarm->first;
				//*curCnt += 1;

				//add in remaining partials
				for (uint32_t p = 0; p < curHarm->cnt && curPart <= part_limit; p++)
				{
					if (curHarm->cnt < 2)
					{
						gain = GAIN[curHarm->gainFirst];
					}
					else
					{
						gain = GAIN[int8_t((curHarm->gainLast - curHarm->gainFirst) * float(p) / float((curHarm->cnt) - 1)) + curHarm->gainFirst];
					}

					uint32_t j = 0;

					//add in the current partial
					for (uint32_t i = 0; i < res; ++i)
					{
						tArr[i + offset] = __SSAT(tArr[i + offset] + (___SMMUL(base_arr[j + offset], gain)), 31);
						j = (j + curPart) & ind_mask;
					}

					curPart += curHarm->step;
				}
			}
			for (uint32_t i = 0; i < TABLE_FULL_SIZE; ++i)
			{
				tArr[i] <<= 1;
			}
			
		}
		
		if(SHIFTMASK(curOsc, bitPhase + curTbl) && !cur_phase->before_harm)
		{
			usleep(SLEEP_MICROS);;
			phaseWidth(curOsc, curTbl, tArr, tArr);
			usleep(SLEEP_MICROS);;
		}
		memcpy(harmArray[curOsc][curTbl], tArr, sizeof(tArr));
		
		GRAPH_update = 1;

	}
	else
	{
		curInd = indexIncrement(curInd, 1, OSC_CNT * TABLE_CNT);
	}
}

	




uint8_t  oscFromGrpInd(uint8_t group, uint8_t ind)
{
	return ((group << 4) + ind) >> 3;
}
	
void  toggleSelected(uint8_t osc)
{
	
	for(uint8_t tOsc = 0; tOsc < OSC_CNT; tOsc++)
	{
		if(tOsc == osc) SETBIT(tOsc, bitOsc);//toggles[tOsc] |= (1 << bitOsc)//maskOsc;
		else CLEARBIT(tOsc, bitOsc);//toggles[tOsc] &= ~maskOsc;

	}
	//rt_printf("osc %d\n", osc);
	CLEARBIT(MAINTOG, bitMainLVL);
	oscInd = osc;
}

	
void  updateLEDs()
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
				int32_t tog = big_group[pos][0];
				if (perTableTog(tog)) tog += table_page;
				uint8_t isOn = SHIFTMASK(osc, tog);
				
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

uint16_t LEDfromGroup(int8_t osc, uint8_t ind, int8_t tog)
{
	if(osc != -1 && tog != -1)
	{
		if(osc == E_OSC) osc = oscInd;//? oscInd: MAINTOG;
		if (perTableTog(tog)) tog += table_page;
		return (SHIFTMASK(osc, tog) << ind);
	}
	return 0;
}
	
void copyOsc(uint8_t destOsc, uint8_t srcOsc, uint8_t bit)	
{
	//dont bother copying itself			
	if(destOsc == srcOsc) return;
	
	//copy entire oscillator
	if(bit == bitOsc)
	{
		//copy all settings
		for(uint8_t i = 0; i < settingsCnt; ++i)
		{
			uint32_t sz = ptrSingleSizes[i];
			uint8_t *addSrc = ((uint8_t*)(varPtrs[i])) + sz * srcOsc;
			uint8_t *addDst = ((uint8_t*)(varPtrs[i])) + sz * destOsc;
			memcpy_safe(addDst, addSrc, sz);
		}
		
		//copy wave table reference
		for (int32_t tableInd = 0; tableInd < TABLE_CNT; ++tableInd)
		{
			curWavFile[destOsc][tableInd] = curWavFile[srcOsc][tableInd];
			phaseFile[destOsc][tableInd] = phaseFile[srcOsc][tableInd];
		}
		
		//handle weirdness of copying poly to mono and visa versa
		if(destOsc >= POLY_CNT) CLEARBIT(destOsc, bitPoly);
		else if(srcOsc >= POLY_CNT) SETBIT(destOsc, bitPoly);
		
		//if osc poly settings match, copy envelope shit
		if(SHIFTMASK(destOsc, bitPoly) == SHIFTMASK(srcOsc, bitPoly))
		{
			uint32_t childTo = firstChild[destOsc];
			uint32_t childFrom = firstChild[srcOsc];
			uint32_t children = (childCnt[destOsc] == childCnt[srcOsc]) ? childCnt[destOsc] : 1;
			
			susOn[destOsc] = susOn[srcOsc];
			monoPitch[destOsc] = monoPitch[srcOsc];
			monoVel[destOsc] = monoVel[srcOsc];
			
			for(uint8_t i = settingsCnt; i < copyStop; ++i)
			{
				uint32_t sz = ptrSingleSizes[i];
				uint8_t* addSrc = ((uint8_t*)(varPtrs[i])) + sz * childFrom;
				uint8_t* addDst = ((uint8_t*)(varPtrs[i])) + sz * childTo;
				
				memcpy_safe(addDst, addSrc, sz * children);			
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
				memcpy(harmParams[destOsc], harmParams[srcOsc], sizeof(harmParams[srcOsc]));
				extraBits |= (0xF << bitHarms); 
				break;
			case bitAEnv: 
				amp_env_knobs[destOsc] = amp_env_knobs[srcOsc]; 
				break;
			case bitPEnv: 
				env1_knobs[destOsc] = env1_knobs[srcOsc];
				break;
			case bitFilt:
				filt_knobs[destOsc] = filt_knobs[srcOsc];
				extraBits |= (1 << bitFTrack); 
				break;
			case bitFEnv:
				env2_knobs[destOsc] = env2_knobs[srcOsc];
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

void routeMod(uint8_t destOsc, uint8_t bit, uint16_t sourceBit)	
{
	
	int8_t ind = -1;
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
		case bitHarms: ind = TBL_X_MOD; break;
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

uint8_t  finishRecording()
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
		
		DUAL_ENV_KNOBS *curEnv;
		if(screenInd == DUALENV1)
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
						if(screenInd == DUALENV2) curEnv->glide[actStg] = ind;
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

/* void equalizeAmp(uint8_t osc)
{
	if(SHIFTMASK(osc, bitAEnv))
	{
		for(uint8_t child = firstChild[osc]; child < firstChild[osc] + childCnt[osc]; child++)
		{
			amp_env[child].val = 0x7FFFFFFF;
		}
	}
} */




void updateUINT8val(uint8_t *val, int8_t inc, uint8_t isToggle, uint8_t LCD, uint32_t full)
{
	int32_t t = *val;
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
				FIL_update |= (1 << MAIN_FIL);//loadFile(fType, oscInd);
				updateLCDelems(OBJ1, OBJ6);
				break;
			//osc copy
			case EX_COPY:
				//rt_printf("doing the copy\n");
				copyOsc(oscInd, next_inc, bitOsc);
				break;
			case EX_POLY16:
				toggle_POLY16(); 
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
		int32_t is_quick = inputQueue[readInd][3];
		inputQueue[readInd][2] = 0;
		
		
		int8_t done = 0;
		uint8_t osc = oscInd;
		int8_t scrn = -1;
		int8_t tog = -1;
		int8_t extra = 0;
		uint8_t wasCopy = SHIFTMASK(MAINTOG, bitCopy);
		uint8_t wasRoute = SHIFTMASK(MAINTOG, bitRoute);

		//rt_printf("was copy %u\n", wasCopy);
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
						//rt_printf("copying osc %d tog %d\n", osc, tog);
						if (tog == bitOsc)
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
			
			if (osc < MAINTOG)
			{
				//tell non poly's to fuck off
				if (tog == bitPoly && osc >= POLY_CNT)
				{
					tog = -1;
					extra = 0;
				}
				//modify toggle for per-table toggles
				if (perTableTog(tog))
				{
					tog += table_page;
				}
				//tell filter track to fuck off too (needs to wait for volume reduction)
				if (tog == bitFTrack) tog = -1;
			}
			
			
			int32_t scrnOsc = osc;
			
			//toggle stuff
			if(tog != -1 && inputQueue[readInd][3])
			{
				TOGGLEBIT(osc, tog); 	

				//printf("osc %d, tog %d, copy %d\n", osc, tog, SHIFTMASK(MAINTOG, bitCopy));
				
				//default route from main of selected
				if(osc == MAINTOG && tog == bitRoute)
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
						favInd = extra - EX_FAV1;
						if (screenInd == DUALENV2 || screenInd == DUALENV1)
						{
							if ((envInd == 0 || envInd == 3) && envInd == favInd)
							{
								envInd = 4;
							}
							else
							{
								envInd = favInd;
							}
						}
						else
						{
							table_page = favInd;
						}
						updateLCDelems(OBJ1, OBJ6);
						//favInd = (extra - EX_PATRNDCLR) + ((isTog)? 0: 5); 
						//favSave = wasCopy; 
						//favAction(favInd, favSave);
						//if(!favSave)
						//{
						//	//queue files load
						//	FIL_update[MAINTOG] = 1; //loadFile(PATCH, 0);
						//}
						
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
					case EX_HARM: queueOscTbl(HARM_update, osc); GRAPH_update = 0; break;
					case EX_POLY16: 
						main_clock1 = MAIN_FADE;
						main_clock2 = MAIN_FADE << 2;
						next_loop = EX_POLY16;
						break;
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
			if (scrn != -1)
			{
				
				screenInd = scrn;
				//rt_printf("screen %d\n", screenInd);
			}

			//make sure copy and route are properly cleared after any action (except toggling them on)
			if (wasCopy)
			{
				//rt_printf("clearing copy\n");
				CLEARBIT(MAINTOG, bitCopy);
			}
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
			int32_t isEnv2 = 1;
			//LogTextMessage("k %u %u %d", inputGrp, inputInd, inc);
			switch(screenInd)
			{
				case WAVETBL:
					
					if (inputInd == KNOB_BUT1)
					{
						table_page = 0;
						updateLCDelems(OBJ1, OBJ6);
						break;
					}
					else if (inputInd == KNOB1)
					{
						table_page = indexIncrement(table_page, unit_dir(inc), TABLE_CNT);
						updateLCDelems(OBJ1, OBJ6);
						break;
					}
					else if (inputInd == KNOB2 || inputInd == KNOB_BUT2)
					{
						screenInd = TBLPOS;
						updateLCDelems(SCRN, OBJ6);
						break;
					}
					if (!SHIFTMASK(oscInd, bitWave + table_page))
					{
						SETBIT(oscInd, bitWave + table_page);
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
						queueOscTbl(FIL_update, oscInd);//loadFile(fType, oscInd);
						updateLCDelems(OBJ1, OBJ6);
					}
				}
				break;
					
				case TBLPOS:
				{
					int8_t ind = knobPos(KNOB1, inputInd);//-1;
					if (ind < 2)
					{
						if (inputInd >= KNOB_BUT1) tableRatios[oscInd][ind] = tableRatios[oscInd][ind] ? 0 : TBL_MAX;
						else tableRatios[oscInd][ind] = __USAT(tableRatios[oscInd][ind] + (inc * TBL_INC), TBL_SAT);
					}
					else
					{
						ind -= 2;
						int32_t x = inc * ((ind & 0x1) ? 1 : -1);
						int32_t y = inc * ((ind & 0x2) ? 1 : -1);
						//rt_printf("x %d y %d\n", x, y);

						if (inputInd >= KNOB_BUT1)
						{
							x = x > 0 ? TBL_MAX : 0;
							y = y > 0 ? TBL_MAX : 0;
							//rt_printf("x %d y %d rx %d ry %d\n", x, y, tableRatios[oscInd][0], tableRatios[oscInd][1]);
							if (tableRatios[oscInd][0] == x && tableRatios[oscInd][1] == y)
							{
								tableRatios[oscInd][0] = TBL_MAX;
								tableRatios[oscInd][1] = TBL_MAX;
							}
							else
							{
								tableRatios[oscInd][0] = x;
								tableRatios[oscInd][1] = y;
							}
						}
						else
						{
							tableRatios[oscInd][0] = __USAT(tableRatios[oscInd][0] + (x * TBL_INC), TBL_SAT);
							tableRatios[oscInd][1] = __USAT(tableRatios[oscInd][1] + (y * TBL_INC), TBL_SAT);
						}
					}
					/*rt_printf("goal x 0x%08x y 0x%08x max 0x%08x thirty 0x%08x\n", goalx, goaly, TBL_MAX, thirty);
					rt_printf("0 goal 0x%08x\n", ___SMMUL(TBL_MAX  - goalx, TBL_MAX - goaly) << 1);
					rt_printf("1 goal 0x%08x\n", ___SMMUL(goalx, TBL_MAX - goaly) << 1);
					rt_printf("2 goal 0x%08x\n", ___SMMUL(TBL_MAX - goalx, goaly) << 1);
					rt_printf("3 goal 0x%08x\n", ___SMMUL(goalx, goaly) << 1);*/
					updateLCDelems(OBJ1, OBJ3);
				}
				break;
				
				case AMPENV:
				{
					//handleAmpEnvChange(inputInd, inc);
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
				
				case DUALENV1: isEnv2 = 0;
				case DUALENV2:
				{
					DUAL_ENV_KNOBS* curEnv = !isEnv2 ? &env1_knobs[oscInd] : &env2_knobs[oscInd];
					int g_ind = knobPos(KNOB1, inputInd) >= 2;
					switch(inputInd)
					{
						//pitch
						case KNOB1:
						case KNOB3:
							inc = inc << pitchShift;
							curEnv->goal[envInd][g_ind] = __SSAT(curEnv->goal[envInd][g_ind] + inc, 29);
							LCD_update[OBJ3 + 2 * g_ind] = 1;
							break;
						
						//coarse/fine toggle
						case KNOB_BUT1:
						case KNOB_BUT3:
							if (is_quick)
							{
								pitchShift = (pitchShift == PITCH_COARSE) ? PITCH_FINE : PITCH_COARSE;
							}
							else
							{
								curEnv->goal[envInd][g_ind] = 0;
								LCD_update[OBJ3 + 2 * g_ind] = 1;
								break;
							}
							break;
						
						//send
						case KNOB2:
						case KNOB_BUT2:
						case KNOB4:
						case KNOB_BUT4:
							int used;
							do
							{
								used = 0;
								curEnv->send[g_ind] = (ENV_ITEM)indexIncrement(curEnv->send[g_ind], unit_dir(inc), e_ENV_MAX);
								for (int32_t j = 0; j < 2; ++j)
								{
									DUAL_ENV_KNOBS* checkKnobs = screenInd == DUALENV1 ? &env1_knobs[oscInd] : &env2_knobs[oscInd];
									for (int32_t k = 0; k < 2; ++k)
									{
										if (j == isEnv2 && k == g_ind) continue;
										if (curEnv->send[g_ind] == checkKnobs->send[k] && checkKnobs->send[k] != e_NONE)
										{
											used = 1;
											break;
										}
									}
								}
							} while (used);
							
							//curLCD = OBJ1;
							LCD_update[OBJ4 + 2 * g_ind] = 1;
							break;
						
							
						/* //record
						case KNOB_BUT4: 
							CLEARBIT(oscInd, (done)? bitPEnv: bitFEnv);
							SETBIT(MAINTOG, bitRecEnv);
							recNotes = 0;
							LCD_update[OBJ4] = 1;
							
							break; */
						//edit glide
						case KNOB5:
						case KNOB_BUT5: updateUINT8val(&curEnv->glide[envInd], inc, inputInd >= KNOB_BUT1, OBJ1); 
							break;

						//time
						case KNOB6:
						case KNOB_BUT6:
							if(envInd < FREE_STAGES) 
							{
								updateUINT8val(&curEnv->time[envInd], inc, inputInd >= KNOB_BUT1, OBJ6);
								//curLCD = OBJ6;
								
								LCD_update[OBJ1] = 1;
								break;
							}
							
						

						
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
						
							indBPM = indexIncrement(indBPM, 1, 5);
							LCD_update[OBJ3] = 1;
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
							if(saveName[saveNameInd] >= 'A' && saveName[saveNameInd] <= 'Z') saveName[saveNameInd] = 'a';
							else if (saveName[saveNameInd] >= 'a' && saveName[saveNameInd] <= 'z') saveName[saveNameInd] = ' ';
							else if(saveName[saveNameInd] == ' ') saveName[saveNameInd] = '0';
							else if(saveName[saveNameInd] >= '0' && saveName[saveNameInd] <= '9') saveName[saveNameInd] = '!';
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
					int ind = -1;
					if(inputInd >= KNOB1 && inputInd <= KNOB6) ind = inputInd - KNOB1;
					else if(inputInd >= KNOB_BUT1 && inputInd <= KNOB_BUT6) ind = inputInd - KNOB_BUT1;
					if(ind != -1) updateUINT8val(&midi_knobs[oscInd].CC_nums[0] + ind, inc, inputInd >= KNOB_BUT1, ind + OBJ1);
				}	
				break;
				
				case MODA:
				{
					int8_t ind = knobPos(KNOB1, inputInd);
					int32_t revised_ind = ind + (table_page > 0 ? 6 : 0);
					//rt_printf("cont i %d rev i %d\n", ind, revised_ind);
					/* if((inputInd - KNOB1) & 1)
					if(inputInd >= KNOB1 && inputInd <= KNOB4) ind = inputInd - KNOB1;
					else if(inputInd >= KNOB_BUT1 && inputInd <= KNOB_BUT4) ind = inputInd - KNOB_BUT1;
					if(ind != -1) */
					if(revised_ind < 8)
					{
						if(inputInd > KNOB6) mod_src[oscInd][revised_ind] = (mod_src[oscInd][revised_ind] == 0)? 1 : 0;
						else mod_src[oscInd][revised_ind] = indexIncrement(mod_src[oscInd][revised_ind], inc, TOTAL_MODS);
						updateSingleMod(revised_ind, oscInd, mod_src[oscInd][revised_ind]);
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
							if(stepInd < childCnt[oscInd])
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
					updateUINT8val(&harmParams[oscInd][table_page].gainFund + ind, inc, inputInd >= KNOB_BUT1, ind + OBJ1);
					if(ind == 1  && !*(&harmParams[oscInd][table_page].gainFund + ind))
					{
						*(&harmParams[oscInd][table_page].gainFund + ind) = 1;
					}
					queueOscTbl(HARM_update, oscInd, table_page);
					GRAPH_update = 0;
				}
				break;
				
				case PHASE:
				{
					PHASE_KNOBS *phase = &phase_knobs[oscInd][table_page];
					int32_t next_dir = 0;
					int32_t next_inc = unit_dir(inc);
					switch (inputInd)
					{
						//page index
						case KNOB1:
						case KNOB_BUT1:
							next_dir = 1;
						case KNOB2:
						case KNOB_BUT2:
							incrementFileIndex(WAVE, next_inc, next_dir, &phaseFile[oscInd][table_page]);
							updateLCDelems(OBJ1, OBJ2);
							break;
						case KNOB3:
						case KNOB_BUT3:
							updateUINT8val(&(phase->gain), inc, inputInd > KNOB8, OBJ3, 1);
							break;
						case KNOB4:
							phase->phase += 2 * inc;
							LCD_update[OBJ4] = 1;
							break;
						case KNOB_BUT4:
							phase->phase = 0;
							LCD_update[OBJ4] = 1;
							break;
						case KNOB5:
						case KNOB_BUT5:
							updateUINT8val(&(phase->partial), inc, inputInd > KNOB8, OBJ5, 0);
							break;
						case KNOB6:
						case KNOB_BUT6:
							phase->before_harm = !phase->before_harm;
							LCD_update[OBJ6] = 1;
							break;
					}
					queueOscTbl(HARM_update, oscInd, table_page);
					GRAPH_update = 0;
				}
				break;

				
				
			}
		}
	}
	++readInd &= 0x03;
	
}
	

void updatePitRatio()
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
		
void updateSingleMod(uint8_t modType, uint8_t destParent, uint8_t sourceIndex)
{
	uint8_t children = childCnt[destParent];//(destParent < POLY_CNT)? NOTES_CNT : 1;
	uint8_t firstDest = firstChild[destParent];
	
	int32_t *src = NULL;
	
	//simple mods
	switch(sourceIndex)
	{
		case MOD_NONE: src = (modType == GATE_MOD)? &maxMod : &zeroMod; break;
		case MOD_MAIN_OUT: src = &lastMain; break;
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
			else if(eInd == PENV_SRC) modSrc[modType][firstDest + child] = &pit_env[sourceChild].val[0];
			else if(eInd == FENV_SRC) modSrc[modType][firstDest + child] = &filt_env[sourceChild].val[0]; 
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

void updateArpTime(uint8_t osc, float newBPM)
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
			if(dstOsc != osc) memcpy_safe((uint8_t *)&arpeggio[dstOsc].T, (uint8_t*)&arpeggio[osc].T, sz);
		}
	}
	//LCD_update[OBJ3] = 1;
}


void resetArpPages(uint8_t firstOsc, uint8_t lastOsc)
{
	for(uint8_t osc = firstOsc; osc <= lastOsc; osc++)
	{
		arp_pages[osc] = ((arpeggio[osc].steps) >> 2) + (((arpeggio[osc].steps) & 0x03)? 1 : 0);
		arp_page[osc] = 0;
	}
}


uint8_t knobPos(uint8_t zeroKnob, uint8_t knobID)
{	
	uint8_t ind = knobID - zeroKnob;
	if(knobID > KNOB6) ind -= 8;
	return ind;
}

uint8_t bounded(uint8_t val, int8_t inc, uint8_t min, uint8_t max)
{
	int16_t num = val + inc;
	if(num > max) return max;
	else if(num < min) return min;
	else return num;
	
}
