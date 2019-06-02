#ifndef SETTINGS_H
#define SETTINGS_H

#define MAX_INT32 0x7FFFFFFF
//midi settings
#define MIDI_QUEUE_SIZE 8
#define DEAD_MIDI_EVENT 255
#define LFO_FREQ 110
#define MIDI_KEY_0 119
#define A0 21
#define A4 69
#define C8 108
#define NOTEON 144
#define NOTEOFF 128
#define RELEASE_OFFSET 1000
#define DRONE_VEL 100
#define ALL_SLOTS 9

//number of voices/polyphony
#define NOTES_CNT 8
#define POLY_CNT 2
#define MONO_CNT 4
//static const uint8_t NOISE_OSC = (POLY_CNT + 20);
static const uint8_t OSC_CNT =( POLY_CNT + MONO_CNT);
static const uint8_t MAINTOG = OSC_CNT;
static const uint8_t E_OSC = MAINTOG + 1;
static const uint8_t POLY_CHILD_CNT = (POLY_CNT * NOTES_CNT);
static const uint8_t OSC_CHILD_CNT = (POLY_CHILD_CNT + MONO_CNT);
#define MAXARP 64

//pitch settings
#define PITCH_COARSE 21
#define PITCH_FINE_RES 6
static const uint8_t PITCH_FINE = (PITCH_COARSE - PITCH_FINE_RES);
static const uint8_t PITCH_SAT = (PITCH_COARSE + 8);
const uint32_t PITCH_MASK = ((1<<PITCH_COARSE)-1);
static const uint8_t PITCH_INTERP = (31 - PITCH_COARSE);
const int32_t TRACK_OFFSET = (179<<PITCH_COARSE);
#define graphShift 25
//wav resolution
#define WAVE_RES 512
static const uint16_t WAVE_RES_MASK = WAVE_RES - 1;
#define WAVE_BYTES 4
#define WAVE_SHIFT 23
static const uint32_t WAVE_MASK = (1<<WAVE_SHIFT)-1;
static const uint8_t WAVE_INTERP = 31 - WAVE_SHIFT;
static const uint16_t WAVE_READ_INC = (WAVE_RES * WAVE_BYTES) >> 4;
static const uint8_t WAVE_IND_INC = WAVE_READ_INC >> 2;
static const uint8_t WAVE_REPS = WAVE_RES/WAVE_IND_INC;


//string settings for files
static const uint8_t MAXFNAMELEN =(6 + 1);
#define MAXDPATHLEN 50
#define ROOT_FOLDER "/POLYSYN"
#define WAVE_FOLDER "WAVE"
#define VOICE_FOLDER "VOICE"
#define PATCH_FOLDER "PATCH"
#define FAVS_FILE "FAVS"
#define FILES_LIST "FILS-"
#define DIRS_LIST "DIRS-"
#define MAXFILES 256 //if you ever increase this, indexes need to be switched to uint16_t
#define MAXDIRS 50

//file browser indexes
#define FILTYPES 2
#define WAVE 0
#define PATCH 1



//knob object indexes
#define ENCODERS 8 
static const uint8_t INPUTS = (ENCODERS * 3);

#define KNOB_GRP 4
#define KNOB1 0
#define KNOB2 1
#define KNOB3 2
#define KNOB4 3
#define KNOB5 4
#define KNOB6 5
#define KNOB7 6
#define KNOB8 7

#define KNOB_BUT1 8
#define KNOB_BUT2 9
#define KNOB_BUT3 10
#define KNOB_BUT4 11
#define KNOB_BUT5 12
#define KNOB_BUT6 13
#define KNOB_BUT7 14
#define KNOB_BUT8 15
#define NEXT_PREV 18

//LCD screen elements
#define SCRN 0
#define OSC 1
#define LVL 2
#define OBJ1 3
#define OBJ2 4
#define OBJ3 5
#define OBJ4 6
#define OBJ5 7
#define OBJ6 8
static const uint8_t LCDelems = OBJ6 + 1;


//screen indexes
#define WAVETBL 0
#define AMPENV 1
#define PITCH 2
#define PITENV 3
#define FILTER 4
#define FILTENV 5
#define ARPEGSETUP 6
#define ARPEGNOTES 7
#define PATCHLD 8
#define PATCHSV 9
#define MIDIINS 10
#define MIDICCS 11
#define OUTS 12
#define MODA 13
#define NOTES 14
#define ARPREC 15
#define FAVS 16
#define HARMONIC 17
#define SCREEN_CNT 18

//status bits
#define bitSolo 0
#define bitCopy 1
#define bitHoldAll 2
#define bitMainLVL 3
#define bitSaved 4
#define bitRecArp 5
#define bitRecEnv 6
//#define bitRecFEnv 7
#define bitRecRyth 8
#define bitRecVel 9 
#define bitRoute 10
#define bitArpSync 11

//button/knob settings
#define INPUTS_CNT 16
#define bitOsc 0 	
#define bitWave 1 	
#define bitAEnv 2 	
#define bitPEnv 3 
#define bitFilt 4 	
#define bitFTrack 5	 
#define bitFEnv 6 	
#define bitFECut 7 
#define bitArp 8 	
#define bitArpFilt 9
#define bitArpTrig 10 
#define bitArpSkip 11 	
//#define bitArpPitch 12 
#define bitMod 13 		
#define bitMain 14 		
#define bitNotes 15 
#define bitEnvs 16 	
#define bitPoly 17 	
#define bitLgto 18 
#define bitHold 19	
//#define bitWind 20
#define bitKeyVel 21
#define bitHarms 22
	
	
#define EX_WAVE 1
#define EX_FILT 2
#define EX_SYNC 3
#define EX_PATRNDCLR 4
#define EX_FAV1 5
#define EX_FAV2 6
#define EX_FAV3 7
#define EX_FAV4 8
#define EX_FAV5 9
#define EX_TRIG_ON 10
#define EX_TRIG_OFF 11
#define EX_POLY 12
#define EX_HOLD1 13
#define EX_HOLD_ALL 14
#define EX_ARPNOTREC 15
#define EX_PATSVLD 16


static const float incsBPM[4] = {.1, 1, 10, 100};
static const uint8_t posBPM[4] = {5, 3, 2, 1};
static const uint8_t charL[3] = {'A', '0', '!'};
static const uint8_t charH[3] = {'Z', '9', ')'};


//#define EX_AMP_SET 14

#define HOLD_TIME 300
	
static const int8_t BIG_GROUP[7][3] = {
	{bitOsc, -1, 0},
	{bitAEnv, AMPENV, 0},
	{bitPEnv, PITENV, 0},
	{bitFilt, FILTER, EX_FILT},
	{bitFEnv, FILTENV, 0},
	{bitArp, ARPEGSETUP, 0},
	{bitMain, OUTS, 0}
};

static const int8_t LAST_GROUP_COL[6][4] = {
	{MAINTOG, bitRoute, -1, 0},
	{MAINTOG, bitCopy, -1, 0},
	{E_OSC, -1, FAVS, EX_FAV1},
	{E_OSC, -1, FAVS, EX_FAV2},
	{E_OSC, -1, FAVS, EX_FAV3},
	{E_OSC, -1, -1, EX_ARPNOTREC},
};


static const int8_t OTHER_GROUP[16][4] = {
																											{E_OSC, -1, FAVS, EX_FAV4}, 			{E_OSC, -1, -1, EX_PATSVLD},
																											{E_OSC, -1, FAVS, EX_FAV5}, 			{E_OSC, -1, -1, EX_PATRNDCLR},
																											{MAINTOG, bitArpSync, -1, EX_SYNC},				{E_OSC, -1, -1, EX_TRIG_ON},
	{E_OSC, bitWave, WAVETBL, EX_WAVE}, {E_OSC, bitPoly, PITCH, EX_POLY}, 	{E_OSC, bitNotes, NOTES, 0}, 	{E_OSC, bitHold, -1, EX_HOLD1}, 		{E_OSC, bitHarms, HARMONIC, 0},
	{E_OSC, bitMod, MODA, 0}, 			{E_OSC, bitKeyVel, MIDICCS, 0}, 		{E_OSC, bitEnvs, MIDIINS, 0}, 	{MAINTOG, bitHoldAll, -1, EX_HOLD_ALL}, {MAINTOG, bitSolo, -1, 0},
};
	




//toggle stuff
#define ALL_BITS 0xFFFFFFFF
#define FULL_SAT_SHIFT 24

//pit/filt env settings
#define ENV_BYPASS 2
#define FREE_STAGES 3

//built-in smoothers
#define P_RATE 40
#define G_RATE 40

//arp masks
#define AMP_MASK 0x04
#define PIT_MASK 0x02
#define FILT_MASK 0x01
#define TRIG_MASK 0x02
#define SKIP_MASK 0x01
#define PIT_TOG 0
#define ENV_TOG 1
#define VEL_TOG 2

#define ARP_REC 1
#define PIT_REC 2
#define FILT_REC 3

//vel types
#define NO_VEL 0
#define KEY_VEL 1
#define WIND_VEL 2

#define TRACK_ALL 0
#define TRACK_ENV 1

#define POLY_MASK 0x02
#define LEG_MASK 0x01

//filter types
#define LOWPASS 0
#define BANDPASS 1
#define HIGHPASS 2

//modulation destinations
#define CUR_MOD_CNT 4
#define MOD_CNT 13
#define PIT_MOD 0
#define CUT_MOD 1
#define AMP_MOD 2
#define RES_MOD 3
#define GATE_MOD 4
#define ARPTIME_MOD 5
#define PAN_MOD 6
#define ATK_MOD 7
#define DEC_MOD 8
#define SUS_MOD 9
#define REL_MOD 10
#define PGLIDE_MOD 11
#define VGLIDE_MOD 12
#define ARPGLIDE_MOD 13



//predefined cc codes
#define CC_CMD 176
#define PB_CMD 224
#define SUS_CC 64
#define SUS_THRESH 20

//cc types (modulation sources)
#define CC_CNT 4
#define WND_EVENT 0
#define MW_EVENT 1
#define PB_EVENT 2
#define SUS_EVENT 3
static const uint32_t TOTAL_MOD_SRC =  CC_CNT + 5;
//static const uint8_t TOTAL_MODS = (OSC_CNT + CC_CNT + 2);
static const uint32_t TOTAL_MODS = (OSC_CNT * TOTAL_MOD_SRC + 2);
#define MOD_NONE 0
#define MOD_MAIN_OUT TOTAL_MODS -1

#define COPY_ALL 0
#define COPY_PIT 1
#define COPY_FILT 2
#define COPY_ARP 3

//solo/mute settings
#define SOLO 1
#define MUTE 2


#include "./synthVariables.c"

#endif 