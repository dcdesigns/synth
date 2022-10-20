/*
 ____  _____ _        _    
| __ )| ____| |      / \   
|  _ \|  _| | |     / _ \  
| |_) | |___| |___ / ___ \ 
|____/|_____|_____/_/   \_\

The platform for ultra-low latency audio and sensor processing

http://bela.io

A project of the Augmented Instruments Laboratory within the
Centre for Digital Music at Queen Mary University of London.
http://www.eecs.qmul.ac.uk/~andrewm

(c) 2016 Augmented Instruments Laboratory: Andrew McPherson,
  Astrid Bin, Liam Donovan, Christian Heinrichs, Robert Jack,
  Giulio Moro, Laurel Pardue, Victor Zappi. All rights reserved.

The Bela software is distributed under the GNU Lesser General Public License
(LGPL 3.0), available here: https://www.gnu.org/licenses/lgpl-3.0.txt
*/


#include <Bela.h>
#include <cmath>
#include <string>
#include "settings.h"
#include "synthVariables.h"
#include "fileLoader.h"
#include "lcdLib.h"
#include "synthControls.h"
#include "pitchTables.h"
#include <libraries/Midi/Midi.h>
#include "SerialMidi.hpp"
#include "helperFunctions.h"
#include "midiHandler.h"
#include <I2c_Codec.h>
#include "axoloti_filters.h"

extern I2c_Codec* gAudioCodec;

Midi midi;
SerialMidi sm;


static int32_t chan = 0;
const int32_t ADDR_CHANNELS = 4;
static int32_t frames = 0;

static uint16_t led[5] = { 0 };
const float onLevel = 3.3 / 5.0;
static int32_t knob_state[8] = { 0 };
static int32_t elapsed[8] = { 0 };
static int32_t longest[8] = { 0 };
static int32_t GRP = -1;
// Set the analog channels to read from
int gSensorInputFrequency = 0;
int gSensorInputAmplitude = 1;
const int32_t ANALOG_OFF = 2;
int sampRate = 0;

uint32_t phase[OSC_CHILD_CNT];
uint32_t phaseInc[OSC_CHILD_CNT];
biquad_coefficients filt_coeff[OSC_CHILD_CNT];
int32_t samps[OSC_CHILD_CNT][4];
int32_t ctrlPitch[OSC_CNT];
uint32_t ctrlGain[OSC_CNT];
int32_t ctrlCut[OSC_CNT];
uint32_t ctrlRes[OSC_CNT];
uint32_t kGainSum[OSC_CHILD_CNT];
int32_t kG[OSC_CHILD_CNT];
uint32_t sGain[OSC_CHILD_CNT];
uint32_t sSEEK[OSC_CHILD_CNT];
uint32_t kPan[OSC_CNT];
int32_t kTableSmooth[OSC_CNT][2];
int32_t kTablePercent[OSC_CHILD_CNT][TABLE_CNT];
int32_t wavSignal[OSC_CHILD_CNT];
//uint32_t kPanSum[OSC_CNT];
uint32_t kMain[OSC_CNT];
uint32_t kMainOut;
uint8_t a_ind = 0;
uint8_t tLoops;
int32_t* envSrc[OSC_CHILD_CNT][int(e_NONE)];


//uint8_t knoiseOsc[OSC_CHILD_CNT];
//uint8_t kwaveOsc[OSC_CHILD_CNT];
//uint8_t knoiseCnt;
//uint8_t kwaveCnt;

//uint8_t kfiltOsc[OSC_CHILD_CNT];
//uint8_t kfiltCnt;

//uint8_t kArpOsc[OSC_CNT];
//uint8_t kArpCnt;

//uint8_t kAEnvOsc[OSC_CNT];
//uint8_t kAEnvCnt;

//uint8_t kNoAmpOsc[OSC_CNT];
//uint8_t kNoAmpCnt;
uint32_t arpFilt[OSC_CNT];
//uint32_t filtTrack[OSC_CNT];
uint32_t fEnvFrq[OSC_CNT];
uint32_t arpOn[OSC_CNT];
//uint32_t pEnvOn[OSC_CNT];
//uint32_t fEnvOn[OSC_CNT];
uint32_t filtOn[OSC_CNT];
uint32_t modMask[OSC_CNT];
//uint32_t gateMask[OSC_CNT];
uint32_t sWaveMask[OSC_CNT][TABLE_CNT];
uint32_t pit_band[OSC_CHILD_CNT];
//uint32_t sFiltMask;

float gFrequency = 440.0;
float gPhase;
float gInverseSampleRate;

AuxiliaryTask printButts, i2cTask, fileTask, sideTask, midiTask, midiInTask, knobTask;
int init_done = 0;

void testFiles(void *)
{
	static int init = 1;
	
	if (init)
	{
		init = 0;
		rt_printf("load wav...\n");
		initFILES(WAVE);
		rt_printf("loaded wav...%d dirs %d files\n", browseCnt[WAVE].dirs, browseCnt[WAVE].files);
		rt_printf("load patch..\n");
		initFILES(PATCH);
		rt_printf("loaded patch...%d dirs %d files\n", browseCnt[PATCH].dirs, browseCnt[PATCH].files);

		//initSynthStuff()

		rt_printf("initing lcd\n");
		initLCD();
		rt_printf("inited lcd\n");
		
		sm.startRead();
		/*FIL_update[0] = 1;
		setFileIndexFromName(WAVE, 0, "TRI");
		checkFileQueue();*/
	}
}


void knobTasks(void*)
{
	static int init = 0;
	static int ind = 0;
	//rt_printf("started side tasks\n");
	while (!gShouldStop)
	{
		handleKnobs();
		usleep(333);
	}
}

void sideTasks(void *)
{
	static int init = 0;
	static int ind = 0;
	//rt_printf("started side tasks\n");
	while (!gShouldStop)
	{
		/*if (!init)
		{
			rt_printf("side task\n");
		}*/
		//scanInputs(ctx);
		

		//memset(audioSrc, NULL, sizeof(audioSrc));
				//memset(isAudio, 0, sizeof(isAudio));

		for (uint8_t osc = 0; osc < OSC_CNT; osc++)
		{
			//uint8_t isOn = (!env)? MASK(osc, bitPEnv): (MASK(osc, bitFilt) && MASK(osc, bitFEnv));
			uint8_t children = childCnt[osc];//(osc < POLY_CNT)? NOTES_CNT: 1;
			uint8_t first = firstChild[osc];
			uint8_t last = first + children;
			uint8_t found[int(e_NONE)] = { 0 };
			
			//now set any that are active
			for (int32_t env = 0; env < 2; ++env)
			{
				if (!env && !MASK(osc, bitPEnv)) continue;
				if (env && !MASK(osc, bitFEnv)) continue;
				for (int32_t sig = 0; sig < 2; ++sig)
				{
					int item = (int)(!env? env1_knobs[osc].send[sig] : env2_knobs[osc].send[sig]);
					if (item < (int)e_NONE)
					{
						found[item] = 1;
						for (uint8_t child = first; child < last; child++)
						{
							envSrc[child][item] = (!env)? &(pit_env[child].val[sig]) : &(filt_env[child].val[sig]);
						}
					}
				}
			}
			//clear any that aren't active
			for (int32_t env = 0; env < int(e_NONE); ++env)
			{
				if(found[env]) continue;
				for (uint8_t child = first; child < last; child++)
				{
					envSrc[child][env] = &zeroMod;
				}
			}
		}

		for (uint8_t parent = 0; parent < OSC_CNT; parent++)
		{
			//pEnvOn[parent] = FULLMASK(parent, bitPEnv);
			//fEnvOn[parent] = FULLMASK(parent, bitFEnv);
			arpOn[parent] = FULLMASK(parent, bitArp);
			arpFilt[parent] = FULLMASK(parent, bitArpFilt);
			filtOn[parent] = FULLMASK(parent, bitFilt);
			fEnvFrq[parent] = FULLMASK(parent, bitFECut);
			for (int32_t tbl = 0; tbl < TABLE_CNT; ++tbl)
			{
				sWaveMask[parent][tbl] = FULLMASK(parent, bitWave + tbl);
			}
			
			
			modMask[parent] = FULLMASK(parent, bitMod);

			uint32_t do_inc = 0;
			uint32_t sourceInd = 40;


			//curWave[parent] = (SHIFTMASK(parent, bitHarms))? &harmArray[parent][0]: &wavArray[parent][0];
			//gateMask[parent] = modMask[parent] & mod_src[GATE_MOD][parent];

		}
		
		if (ind == 0) checkFileQueue();
		else if (ind == 1) checkHarmQueue();
		else if (ind == 2) updatePitRatio();
		else checkWriteElem();

		(++ind &= 0x3);

		if (!init)
		{
			init = 1;
			init_done += 1;
		}
		usleep(333);
	}
	//rt_printf("ended side tasks\n");
}

void midiTasks(void*)
{
	while (!gShouldStop)
	{
		handleNotes();
		handleCCs();
		usleep(SLEEP_MICROS);;
	}

}

void midiInTasks(void*)
{
	sm.readSerialDevice();
}



bool setup(BelaContext *context, void *userData)
{
	gInverseSampleRate = 1.0 / context->audioSampleRate;
	gPhase = 0.0;
	// set up the MIDI device normally
	// the readFrom port should be defined, but it does NOT correspond to the serial port the SerialMidi instance will read from
	midi.readFrom("hw:1,0,0");
	//midi.enableParser(true);
	//midi.setParserCallback(midiCallback);

	// initialize the SerialMidi reader
	// this sets it up to feed into the midi device we just initialized. the arguments `dev` and `bd` are supplied the default values.
	// UART4 (/dev/ttyS4) is the default serial port, and 31250 the default baud rate
	sm.setup(&midi);
	float float_val;
	int32_t int_val;

	for (uint32_t i = 0; i < OSC_CHILD_CNT; ++i)
	{
		phase[i] = rand();
	}
	//for (int32_t i = 0; i < 21; ++i)
	//{
	//	int_val = 1 << i;
	//	float_val = q27_to_float(int_val);
	//	printf("int 0x%08x float %f\n", int_val, float_val);
	//}

	//int_val = 0x7fffffff;
	//float_val = q27_to_float(int_val);
	//printf("int 0x%08x float %f\n", int_val, float_val);
	
	//make sure env pointers gets initialized before any usage
	for (uint8_t child = 0; child < OSC_CHILD_CNT; child++)
	{
		for (int32_t env = 0; env < int(e_NONE); ++env)
		{
			envSrc[child][env] = &zeroMod;
		}
	}

	rt_printf("load wav...\n");
	init_done += initFILES(WAVE);
	rt_printf("loaded wav...%d dirs %d files\n", browseCnt[WAVE].dirs, browseCnt[WAVE].files);
	rt_printf("load patch..\n");
	init_done += initFILES(PATCH);
	rt_printf("loaded patch...%d dirs %d files\n", browseCnt[PATCH].dirs, browseCnt[PATCH].files);

	initSynthStuff(context, userData);
	resetPatch();

	rt_printf("initing lcd\n");
	initLCD();
	rt_printf("inited lcd\n");
	rt_printf("e_NONE %d\n", e_NONE);
	/*rt_printf("max max 0x%08X\n", __USAT_add_signed(MAX_INT32, MAX_INT32, TBL_MAX));
	rt_printf("max 0 0x%08X\n", __USAT_add_signed(MAX_INT32, 0, TBL_MAX));
	rt_printf("0 max 0x%08X\n", __USAT_add_signed(0, MAX_INT32, TBL_MAX));
	rt_printf("neg max 0x%08X\n", __USAT_add_signed(-MAX_INT32, MAX_INT32, TBL_MAX));*/
	//sm.startRead();

	for (int32_t i = 0; i < 129; ++i)
	{
		lcdRand[i] = signed_rand();
	}

	midiInTask = Bela_createAuxiliaryTask(&midiInTasks, 72, "midiInTask");
	midiTask = Bela_createAuxiliaryTask(&midiTasks, 71, "midiTask");
	sideTask = Bela_createAuxiliaryTask(&sideTasks, 1, "sideTask");
	knobTask = Bela_createAuxiliaryTask(&knobTasks, 73, "knobTask");
	init_done += 1;
	for (uint8_t parent = 0; parent < OSC_CNT; parent++)
	{
		for (uint8_t tableInd = 0; tableInd < TABLE_CNT; tableInd++)
		{
			curWave[parent][tableInd] = &harmArray[parent][tableInd][0];
		}
	}
	

	// here's how you can override the defaults:
	// sm.setup (&midi, dev="/some/device", bd=12345);
	// for(int i = 0; i < 5; ++i)
	// {	
		// pinMode(context, 0, i, INPUT); //set input
	// }

	// //printf("analog frames %d\n", context->analogFrames);
	// // Check if analog channels are enabled
	// if(context->analogFrames == 0 || context->analogFrames > context->audioFrames) {
		// rt_printf("Error: this example needs analog enabled, with 4 or 8 channels\n");
		// return false;
	// }
	
	// for(unsigned int channel = 0; channel < ADDR_CHANNELS; channel++) 
	// {
		// analogWrite(context, 0, channel, float((chan >> channel) & 1));
	// }

	//fileTask = Bela_createAuxiliaryTask(&testFiles, 50, "files");
	// printButts = Bela_createAuxiliaryTask(&printButt, 50, "butt");
	// i2cTask = Bela_createAuxiliaryTask(&testI2C, 50, "i2c test");
	//Bela_scheduleAuxiliaryTask(fileTask);
	sampRate = context->audioSampleRate;
	
	return true;
}

void render(BelaContext *context, void *userData)
{
	++ticks;
	/*handleNotes();
	handleCCs();*/
	

	if(ticks < 1000)
	{
		//handleKnobs();
		//checkFileQueue();
		//checkHarmQueue();
		//updatePitRatio();
		//checkWriteElem();
		for (unsigned int n = 0; n < context->audioFrames; n++)
		{
			for (unsigned int channel = 0; channel < context->audioOutChannels && channel < 2; channel++) {
				audioWrite(context, n, channel, 0);
			}
		}
		return;
	}
	else if (init_done < 5)
	{
		for (unsigned int n = 0; n < context->audioFrames; n++)
		{
			for (unsigned int channel = 0; channel < context->audioOutChannels && channel < 2; channel++) {
				audioWrite(context, n, channel, 0);
			}
		}
		//gAudioCodec->setPllD(5277);
		//gAudioCodec->setPllP(7);
		//gAudioCodec->setAudioSamplingRate(48000);
		//rt_printf("sampling rate: %f\n", gAudioCodec->getAudioSamplingRate());
		Bela_scheduleAuxiliaryTask(midiInTask);
		Bela_scheduleAuxiliaryTask(midiTask);
		Bela_scheduleAuxiliaryTask(sideTask);
		Bela_scheduleAuxiliaryTask(knobTask);
		rt_printf("sr an %f dig %f, frames %u\n", context->analogSampleRate, context->digitalSampleRate, context->audioFrames);
		init_done = 5;
		rt_printf("starting\n");
		return;
	}
	
	uint32_t seekR = SEEK[G_RATE];
	uint32_t mainR = SEEK[30];
	if (main_clock1)
	{
		mainR = SEEK[14];
		main_mask = 0;
		--main_clock1;
	}
	else if (main_clock2)
	{
		memset(samps, 0, sizeof(samps));
		mainR = SEEK[14];
		main_mask = 0;
		--main_clock2;
	}
	else
	{
		mainR = SEEK[30];
		main_mask = MAX_INT32;
	}


	//poop[poopSize -1] = ticks;
	//outlet_pit1 = poop[poopSize -1];
	//factor in all gain inputs (except envelope/wind)
	




	{
		uint8_t isSolo = SHIFTMASK(MAINTOG, bitSolo);

		for (uint8_t parent = 0; parent < OSC_CNT; parent++)
		{
			//toggler for solo/mute
			uint32_t outGoal = (MASK(parent, bitMain) && (!isSolo || parent == oscInd)) ? MAX_INT32 : 0;
			kMain[parent] += ___SMMUL(outGoal - kMain[parent], mainR) << 1;


			//smoother for panning
			kPan[parent] += ___SMMUL((panLeft[parent] << (FULL_SAT_SHIFT - 4)) - kPan[parent], seekR) << 1;

			for (int32_t ind = 0; ind < 2; ++ind)
			{
				//int32_t goal = __SSAT(int32_t(tableRatios[parent][ind] >> 1) + int32_t(modMask[parent] & (*(modSrc[ind == 0 ? TBL_X_MOD : TBL_Y_MOD][firstChild[parent]]) >> 1)), 31) << 1;
				//if (goal < 0) goal = 0;
				kTableSmooth[parent][ind] += ___SMMUL(tableRatios[parent][ind] - kTableSmooth[parent][ind], seekR) << 1;
			}
			

			//smoother for vol knobs
			ctrlGain[parent] += ___SMMUL(GAIN[osc_gain[parent]] - ctrlGain[parent], seekR) << 1;

		}

	}
	for (uint32_t child = 0; child < OSC_CHILD_CNT; ++child)
	{
		uint8_t parent = parents[child];
		int32_t tblx = __USAT_add_signed(*(envSrc[child][(int)e_TBLX]), __USAT_add_signed(modMask[parent] & *(modSrc[TBL_X_MOD][child]), kTableSmooth[parent][0], TBL_SAT), TBL_SAT);
		int32_t tbly = __USAT_add_signed(*(envSrc[child][(int)e_TBLY]), __USAT_add_signed(modMask[parent] & *(modSrc[TBL_Y_MOD][child]), kTableSmooth[parent][1], TBL_SAT), TBL_SAT);
		kTablePercent[child][0] = ___SMMUL(TBL_MAX - tblx, TBL_MAX - tbly) << TBL_RENDER_SHIFT;
		kTablePercent[child][1] = ___SMMUL(tblx, TBL_MAX - tbly) << TBL_RENDER_SHIFT;
		kTablePercent[child][2] = ___SMMUL(TBL_MAX - tblx, tbly) << TBL_RENDER_SHIFT;
		kTablePercent[child][3] = ___SMMUL(tblx, tbly) << TBL_RENDER_SHIFT;
	}
	if (main_clock1)
	{
		mainR = SEEK[14];
		main_mask = 0;
		--main_clock1;
	}
	else if (main_clock2)
	{
		memset(samps, 0, sizeof(samps));
		mainR = SEEK[14];
		main_mask = 0;
		--main_clock2;
	}
	else
	{
		mainR = SEEK[30];
		main_mask = MAX_INT32;
	}

	kMainOut += ___SMMUL((int32_t)(GAIN[main_gain] & main_mask) - (int32_t)kMainOut, mainR) << 1;


	//factor in all control-type pitch inputs (knobs) to apply a preset glide
	seekR = SEEK[P_RATE];
	{
		PIT_KNOBS* curPit = &pit_knobs[0];
		for (uint8_t parent = 0; parent < OSC_CNT; parent++)
		{
			ctrlPitch[parent] += ___SMMUL(curPit->pitch - ctrlPitch[parent], seekR) << 1;
			curPit++;
		}

	}



	//factor in all note-type inputs to apply the note glide
	for (uint8_t child = 0; child < OSC_CHILD_CNT; child++)
	{
		uint8_t parent = parents[child];
		kNote[child] += ___SMMUL(note[child] + monoPitch[parent] - kNote[child], SEEK[pit_knobs[parent].pit_glide]) << 1;
		kGainSum[child] = ___SMMUL(VELGAIN[__USAT(vel[child] + monoVel[parent], 7)], ctrlGain[parent]) << 1;
		if (arpOn[parent]) kGainSum[child] = ___SMMUL(kGainSum[child], arp_env[child].vel) << 1;
	}


	for (uint8_t child = 0; child < OSC_CHILD_CNT; child++)
	{
		//kGainSum[child] = __USAT(kGainSum[child]>>1 + (modMask[parents[child]] & *(modSrc[AMP_MOD][child])), 30)<<1;
		//kGainSum[child] = __USAT(kGainSum[child] + (___SMMUL((modMask[parents[child]] & *(modSrc[AMP_MOD][child])), kGainSum[child])<<6), 31);
		kGainSum[child] = __USAT((kGainSum[child] >> 1) + (___SMMUL((modMask[parents[child]] & *(modSrc[AMP_MOD][child])), kGainSum[child]) << 5), 30) << 1;
		//kGainSum[child] = __USAT(kGainSum[child]>>1 + (___SMMUL((modMask[parents[child]] & *(modSrc[AMP_MOD][child])), kGainSum[child])<<5), 30)<<1;
		//if(gateMask[parents[child]]) kGainSum[child] =  ___SMMUL(kGainSum[child], *(modSrc[GATE_MOD][child]))<<1;// : kGainSum[child];
		if (modMask[parents[child]]) kGainSum[child] = ___SMMUL(kGainSum[child], *(modSrc[GATE_MOD][child])) << 1;
	}

	for (uint8_t parent = 0; parent < OSC_CNT; parent++)
	{
		ctrlCut[parent] += ___SMMUL(filt_knobs[parent].FRQ - ctrlCut[parent], seekR) << 1;
		ctrlRes[parent] += ___SMMUL((filt_knobs[parent].RES << FULL_SAT_SHIFT) - ctrlRes[parent], SEEK[G_RATE]) << 1;
	}

	seekR = SEEK[20];
	for (uint8_t cc = 0; cc < CC_CNT; cc++)
	{
		for (uint8_t osc = 0; osc < OSC_CNT; osc++)
		{
			kCCs[cc][osc] += ___SMMUL((___SMMUL(CC_vals[cc][osc], GAIN[midi_knobs[osc].CC_levs[cc]]) << 1) - kCCs[cc][osc], seekR) << 1;
			//kCCs[cc][osc] += ___SMMUL(CC_vals[cc][osc] - kCCs[cc][osc], seekR)<<1;
		}
	}



	//pitch/filter envs
	{
		for (uint8_t env = 0; env < 2; env++)
		{
			DUAL_ENV_KNOBS* curKnobs = !env ? &env1_knobs[0] : &env2_knobs[0];
			PIT_ENV* curEnv = (!env) ? &pit_env[0] : &filt_env[0];
			for (uint8_t osc = 0; osc < OSC_CNT; osc++)
			{
				//uint8_t isOn = (!env)? MASK(osc, bitPEnv): (MASK(osc, bitFilt) && MASK(osc, bitFEnv));
				uint8_t children = childCnt[osc];//(osc < POLY_CNT)? NOTES_CNT: 1;
				for (uint8_t child = 0; child < children; child++)
				{
					//if(isOn)
					{
						uint8_t tStage = curEnv->stage;
						if (tStage < FREE_STAGES && ++curEnv->clock >= TIME[curKnobs->time[tStage]])
						{
							tStage++;
							curEnv->stage++;
							curEnv->clock = 0;
						}
						else
						{
							for (int32_t v = 0; v < 2; ++v)
							{
								curEnv->val[v] += ___SMMUL(((curKnobs->goal[tStage][v] << 1) - curEnv->val[v]) << 1, SEEK[curKnobs->glide[tStage]]);
							}
						}
					}
					//else curEnv->val = 0;
					curEnv++;
				}
				curKnobs++;
			}
		}
	}






	//arpeggiator
	{
		ARP_KNOBS* curKnobs = &arpeggio[0];
		for (uint8_t osc = 0; osc < OSC_CNT; osc++)
		{
			//if(arpOn[osc])
			{
				uint8_t doAll = !SHIFTMASK(osc, bitArpSkip);
				uint8_t first = firstChild[osc];
				uint8_t last = untilChild[osc];
				ARP_ENV* curEnv = &arp_env[first];

				for (uint8_t child = first; child < last; child++)
				{
					uint8_t tStage = curEnv->stage;
					curEnv->val += ___SMMUL(((curKnobs->P[curEnv->atk_stage] << PITCH_COARSE) - curEnv->val) << 1, SEEK[curKnobs->G]);
					curEnv->vel += ___SMMUL((VELGAIN[curKnobs->V[curEnv->atk_stage]] - curEnv->vel), SEEK[curKnobs->G]) << 1;

					//check timing for stage increment trigger
					if (++(curEnv->clock) >= curKnobs->T + (modMask[osc] & (*(modSrc[ARPTIME_MOD][child]) >> 18)))
					{
						tStage = indexIncrement(tStage, 1, curKnobs->steps);
						curEnv->stage = tStage;
						curEnv->clock = 0;
						uint8_t env = curKnobs->E[tStage];

						if (env || doAll)
						{
							curEnv->atk_stage = tStage;
							//curEnv->vel = VELGAIN[curKnobs->V[tStage]];
							if (arpOn[osc] && amp_env[child].stage < 3)
							{
								if (env & AMP_MASK)
								{
									amp_env[child].stage = 0;
									//amp_env[child].val = 0;
								}
								if (env & PIT_MASK)
								{ 
									pit_env[child].stage = 0; 
									//pit_env[child].val = 0;
									pit_env[child].clock = 0; 
								}
								if (env & FILT_MASK)
								{ 
									filt_env[child].stage = 0; 
									//filt_env[child].val = 0;
									filt_env[child].clock = 0; 
								}
							}
						}
					}
					curEnv++;
				}
			}
			curKnobs++;
		}
	}




	//sum pitches, get phase increment and pitch gain
	for (uint8_t child = 0; child < OSC_CHILD_CNT; child++)
	{
		uint8_t parent = parents[child];

		//sum the pitch
		/* int32_t curPitch = __USAT(ctrlPitch[parent] + 	//knob pitch
								kNote[child] +   //note pitch
								(pit_env[child].val & pEnvOn[parent]) + //pitch env
								(arp_env[child].val  & arpOn[parent] & ~arpFilt[parent]) + //arpeggiator
								(modMask[parent] & (*(modSrc[PIT_MOD][child])>>2)), PITCH_SAT); //pitch mod */

		int32_t curPitch = __USAT(ctrlPitch[parent] + 	//knob pitch
			kNote[child] +   //note pitch
			(*(envSrc[child][(int)e_PIT]) >> 1) + //((pit_env[child].val[0] >> 1) & pEnvOn[parent]) + //pitch env
			(arp_env[child].val & arpOn[parent] & ~arpFilt[parent]), PITCH_SAT); //arpeggiator 

		phaseInc[child] = getPhaseInc(curPitch);
		int32_t t_band = ((curPitch >> PITCH_COARSE) - (A0 + 11 + MIDI_KEY_0)) / 12;
		if (t_band < 0) t_band = 0;
		else if (t_band > 7) t_band = 7;
		//t_band = 2;
		if (t_band != pit_band[child])
		{
			//rt_printf("child %d band %d\n", child, t_band);
			pit_band[child] = t_band;
		}


		//if(MASK(parent, bitFilt))
		{
			//sum the cutoff
			uint32_t curCut = ctrlCut[parent] + //knob cut
				(*(envSrc[child][(int)e_FCUT]) >> 1) +  //filter env
				(curPitch & FULLMASK(parent, bitFTrack)) + //pitch tracking
				(arp_env[child].val & arpOn[parent] & arpFilt[parent]) +//arp tracking
				(modMask[parent] & *(modSrc[CUT_MOD][child])); //cutoff mod


			uint32_t filtPhaseInc = getPhaseInc(curCut);

			//if(child == 0) outlet_pit = curCut;	 
			uint32_t curRes = INT_MAX - (__USAT((ctrlRes[parent] >> 1) + 
				(*(envSrc[child][(int)e_FRES]) << 1) + 
				(modMask[parent] & (*(modSrc[RES_MOD][child]))), 30) << 1);
			//+((fEnvOn[parent] & (filt_env[child].val[1] << 2)) >> 1)
			//uint32_t curRes = INT_MAX - __USAT(ctrlRes[parent] + (~fEnvFrq[parent] & (filt_env[child].val<<3)) + (modMask[parent] & (*(modSrc[RES_MOD][child])<<1)), 31);
			if (!filtOn[parent]) biquad_lp_coefs(&filt_coeff[child], 0x77336AE6, INT_MAX);
			else if (filt_knobs[parent].TYPE == LOWPASS) biquad_lp_coefs(&filt_coeff[child], filtPhaseInc, curRes);
			else if (filt_knobs[parent].TYPE == BANDPASS) biquad_bp_coefs(&filt_coeff[child], filtPhaseInc, curRes);
			else biquad_hp_coefs(&filt_coeff[child], filtPhaseInc, curRes);
		}
		//else biquad_lp_coefs(&filt_coeff[child],0x77336AE6,INT_MAX);

	}


	//outlet_pit = preGain[0];
	for (uint8_t child = 0; child < OSC_CHILD_CNT; child++)
	{
		sSEEK[child] = SEEK_S_RATE[pit_knobs[parents[child]].vel_glide];
	}




	for (uint8_t osc = 0; osc < OSC_CNT; osc++)
	{
		uint8_t first = firstChild[osc];
		uint8_t last = untilChild[osc];
		uint8_t amp = SHIFTMASK(osc, bitAEnv);
		//uint8_t wind = SHIFTMASK(osc, bitWind);
		uint8_t poly = SHIFTMASK(osc, bitPoly);
		uint8_t track = SHIFTMASK(osc, bitEnvs);


		//neither wind nor amp env
		if (!amp)
		{
			for (uint8_t child = first; child < last; child++)
			{
				amp_env[child].goal = MAX_INT32;
				amp_env[child].rate = SEEK[10];
			}
		}

		else
		{
			AMP_ENV_KNOBS* curKnobs = &amp_env_knobs[osc];
			AMP_ENV* curEnv = &amp_env[first];

			for (uint8_t child = first; child < last; child++)
			{
				/* if(wind && (!amp || !poly || !track || curEnv->stage < 3))
				{
					curEnv->goal = wind_gain[osc];
					curEnv->rate = SEEK[10];
					//if(!curEnv->stage) curEnv->stage = 1;
				}
				else */
				{
					uint8_t tStage = curEnv->stage;

					//check for completed attack or decay
					if (!tStage && curEnv->val == MAX_INT32) tStage++;
					else if (tStage == 1 && curEnv->val - GAIN[curKnobs->goal[0]] < 40000) tStage++;

					//set the current rate
					curEnv->rate = SEEK[curKnobs->rate[tStage]];

					//set the current goal
					if (!tStage) curEnv->rate = ATTACK_K[curKnobs->rate[0]]; //curEnv->goal = MAX_INT32; 
					else if (tStage < 3) curEnv->goal = GAIN[curKnobs->goal[tStage - 1]];
					else curEnv->goal = 0;

					curEnv->stage = tStage;
				}
				curEnv++;
			}
		}
	}


	{
		AMP_ENV* curEnv = &amp_env[0];
		for (uint8_t child = 0; child < OSC_CHILD_CNT; child++)
		{
			if (!curEnv->stage)
			{
				curEnv->val += curEnv->rate;
				if (curEnv->val > MAX_INT32) curEnv->val = MAX_INT32;
			}
			else curEnv->val += ___SMMUL((curEnv->goal - curEnv->val), curEnv->rate) << 1;
			curEnv++;
		}
	}

	for (uint8_t child = 0; child < OSC_CHILD_CNT; child++)
	{
		kG[child] = ___SMMUL(amp_env[child].val, kGainSum[child]) << 1;
		/* int32_t dif = kG[child] - sGain[child];
		if(dif <= 1 && dif >= -1) sGain[child] = kG[child]; */
	}
	
	
	
	for (unsigned int n = 0; n < context->audioFrames; n++) 
	{
		int32_t outlet[2] = { 0 };
		int32_t noise_val = signed_rand();
		//for (int32_t x = 0; x < 4; ++x)
		{
			//get the waveform/noise value	
			for (uint8_t parent = 0; parent < OSC_CNT; ++parent)
			{
				const uint8_t first = firstChild[parent];
				const uint8_t last = untilChild[parent];
				
				for (uint32_t child = first; child < last; ++child)
				{
					wavSignal[child] = 0;
					phase[child] += phaseInc[child] + (modMask[parent] & (*(modSrc[PIT_MOD][child]) >> 1));
					/*int32_t wavInd = (phase[child]) >> WAVE_SHIFT;
					uint32_t off = ((phase[child]) & WAVE_MASK) << WAVE_INTERP;
					for (uint32_t tableInd = 0; tableInd < TABLE_CNT; ++tableInd)
					{
						int32_t amp1 = curWave[parent][tableInd][wavInd];
						int32_t amp2 = curWave[parent][tableInd][(wavInd + 1) & 0x1FF];
								
						int32_t sig = ___SMMUL(amp1, INT_MAX - off);
						sig = (sWaveMask[parent][tableInd] & (___SMMLA(amp2, off, sig) << 1)) | ((~sWaveMask[parent][tableInd]) & noise_val);
						wavSignal[child] += ___SMMUL(sig, kTablePercent[child][tableInd]) << 1;


					}*/
					int32_t band = pit_band[child];
					int32_t wavInd = ((phase[child] >> TABLE_BAND_COARSE_SHIFT[band]) & TABLE_BAND_LAST_MASK[band]);
					uint32_t off = ((phase[child]) & TABLE_BAND_FINE_MASK[band]) << TABLE_BAND_FINE_SHIFT[band];
					for (uint32_t tableInd = 0; tableInd < TABLE_CNT; ++tableInd)
					{
						int32_t amp1 = curWave[parent][tableInd][(wavInd) + TABLE_BAND_OFFSET[band]];
						int32_t amp2 = curWave[parent][tableInd][((++wavInd) & TABLE_BAND_LAST_MASK[band]) + TABLE_BAND_OFFSET[band]];

						/*int32_t amp1 = SAW_TEST[wavInd];
						int32_t amp2 = SAW_TEST[(wavInd + 1)];*/

						int32_t sig = ___SMMUL(amp1, INT_MAX - off);
						sig = (sWaveMask[parent][tableInd] & (___SMMLA(amp2, off, sig) << 1)) | ((~sWaveMask[parent][tableInd]) & noise_val);
						wavSignal[child] += ___SMMUL(sig, kTablePercent[child][tableInd]) << 1;


					}
				}
			}
		}



		//filters	
		{
			//for (uint8_t child = 0; child < OSC_CHILD_CNT; ++child)
			//{
			//	lastSignal[child] = wavSignal[child] >> 3;
			//}
			
			biquad_coefficients* curCoeff = &filt_coeff[0];
			for (uint8_t child = 0; child < OSC_CHILD_CNT; ++child)
			{
				// sGain[child] += ___SMMUL(kG[child] - sGain[child], sSEEK[child]);
				// wavSignal[child] = ___SMMUL(wavSignal[child], sGain[child]);
				wavSignal[child] >>= 3;
				int32_t* curSamps = &samps[child][0];

				int32_t filtered = ___SMMUL(curCoeff->cxn_0, wavSignal[child]);
				filtered = ___SMMLA(curCoeff->cxn_1, *curSamps, filtered);
				filtered = ___SMMLA(curCoeff->cxn_2, *(curSamps + 1), filtered);
				filtered = ___SMMLS(curCoeff->cyn_1, *(curSamps + 2), filtered);
				filtered = ___SMMLS(curCoeff->cyn_2, *(curSamps + 3), filtered) << 4;

				*(curSamps + 3) = *(curSamps + 2);
				*(curSamps + 2) = filtered;
				*(curSamps + 1) = *curSamps;
				*curSamps = wavSignal[child];

				sGain[child] += ___SMMUL(kG[child] - sGain[child], sSEEK[child]);
				filtered = ___SMMUL(filtered, sGain[child]) << 3;
				lastSignal[child] = filtered;
				++curCoeff;
			}
		}


		//apply main outs
		
		lastMain = 0;

		
		for (uint8_t parent = 0; parent < POLY_CNT; ++parent)
		{
			uint32_t first = firstChild[parent];
			uint32_t last = untilChild[parent];
			int32_t combo = 0;
			for (uint8_t child = first; child < last; ++child)
			{
				combo += lastSignal[child];
			}
			lastPolyCombo[parent] = combo;
			lastMain += combo;
			combo = ___SMMUL(kMain[parent], combo) << 1;


			//int32_t L = ___SMMUL(combo, __USAT((kPan[parent])+ (modMask[parent] & (*(modSrc[PAN_MOD][firstChild[parent]])>>1)),27))<<5;
			int32_t L = ___SMMUL(combo, kPan[parent]) << 5;
			outlet[1] += combo - L;
			outlet[0] += L;

		}

		/*outlet[1] = lastSignal[0];
		outlet[0] = lastSignal[0];*/

		for (uint8_t parent = POLY_CNT; parent < OSC_CNT; ++parent)
		{
			int32_t child = firstChild[parent];
			lastMain += lastSignal[child];
			int32_t sig = ___SMMUL(kMain[parent], lastSignal[child]) << 1;
			//int32_t L = ___SMMUL(sig,  __USAT((kPan[parent])+ (modMask[parent] & (*(modSrc[PAN_MOD][firstChild[parent]])>>1)),27))<<5;
			int32_t L = ___SMMUL(sig, kPan[parent]) << 5;
			outlet[1] += sig - L;
			outlet[0] += L;
		}



		/*lastAudio[1] = AudioInputLeft[a_ind];
		lastAudio[2] = AudioInputRight[a_ind];
		lastAudio[0] = (lastAudio[1]) + (lastAudio[2]);
		++a_ind &= 0xF;*/

		outlet[1] = ___SMMUL(outlet[1], kMainOut) << 1;
		outlet[0] = ___SMMUL(outlet[0], kMainOut) << 1;
		
		
		
		
		
		//float out = 0.8f * sinf(gPhase);
		//gPhase += 2.0f * (float)M_PI * gFrequency * gInverseSampleRate;
		//if (gPhase > M_PI)
		//	gPhase -= 2.0f * (float)M_PI;

		for (unsigned int channel = 0; channel < context->audioOutChannels && channel < 2; channel++) {
			audioWrite(context, n, channel, q27_to_float(outlet[channel]));
		}
	 }
	//
	 scanInputs(context);

	 //Bela_scheduleAuxiliaryTask(midiInTask);
	 /*Bela_scheduleAuxiliaryTask(midiTask);
	 Bela_scheduleAuxiliaryTask(sideTask);*/
	
	

	
}






void cleanup(BelaContext *context, void *userData)
{
	sm.stopRead();
}

















/**
\example analog-input/render.cpp

Connecting potentiometers
-------------------------

This sketch produces a sine tone, the frequency and amplitude of which are 
modulated by data received on the analog input pins. Before looping through each audio 
frame, we declare a value for the `frequency` and `amplitude` of our sine tone; 
we adjust these values by taking in data from analog sensors (for example potentiometers)
with `analogRead()`.

- connect a 10K pot to 3.3V and GND on its 1st and 3rd pins.
- connect the 2nd middle pin of the pot to analogIn 0.
- connect another 10K pot in the same way but with the middle pin connected to analogIn 1.

The important thing to notice is that audio is sampled twice as often as analog 
data. The audio sampling rate is 44.1kHz (44100 frames per second) and the 
analog sampling rate is 22.05kHz (22050 frames per second). Notice that we are 
processing the analog data and updating frequency and amplitude only on every 
second audio sample, since the analog sampling rate is half that of the audio.

````
if(!(n % gAudioFramesPerAnalogFrame)) {
    // Even audio samples: update frequency and amplitude from the analog inputs
    frequency = map(analogRead(context, n/gAudioFramesPerAnalogFrame, gSensorInputFrequency), 0, 1, 100, 1000);
    amplitude = analogRead(context, n/gAudioFramesPerAnalogFrame, gSensorInputAmplitude);
}
````

*/
