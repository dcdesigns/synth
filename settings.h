#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>

#define SLEEP_MICROS (100)

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
#define ALL_SLOTS (OSC_CHILD_CNT)

#define LOADTABLES (1)

//number of voices/polyphony
#define NOTES_CNT (8)
#define POLY_CNT (2)
#define MONO_CNT (4)
#define TABLE_CNT (4)
//static const uint8_t NOISE_OSC = (POLY_CNT + 20);
#define OSC_CNT (POLY_CNT + MONO_CNT)
#define MAIN_FIL (OSC_CNT * TABLE_CNT)
#define MAINTOG (OSC_CNT)
#define E_OSC (MAINTOG + 1)
#define POLY_CHILD_CNT (POLY_CNT * NOTES_CNT)
#define OSC_CHILD_CNT (POLY_CHILD_CNT + MONO_CNT)
#define MAXARP (64)

//pitch settings
#define PITCH_COARSE (21)
#define PITCH_FINE_RES (6)
#define PITCH_FINE (PITCH_COARSE - PITCH_FINE_RES)
#define PITCH_SAT (PITCH_COARSE + 8)
#define PITCH_MASK ((1<<PITCH_COARSE)-1)
#define PITCH_INTERP (31 - PITCH_COARSE)
#define TRACK_OFFSET (179<<PITCH_COARSE)
#define graphShift (25)
//wav resolution
#define WAVE_RES (512)
#define WAVE_RES_MASK (WAVE_RES - 1)
#define WAVE_BYTES (4)
#define WAVE_SHIFT (23)
#define WAVE_MASK ((1<<WAVE_SHIFT)-1)
#define WAVE_INTERP (31 - WAVE_SHIFT)
//#define WAVE_READ_INC ((WAVE_RES * WAVE_BYTES) >> 4)
//#define WAVE_IND_INC (WAVE_READ_INC >> 2)
//#define WAVE_REPS (WAVE_RES/WAVE_IND_INC)



#define TBL_MAX (MAX_INT32 >> 2)
#define TBL_INC (TBL_MAX >> 7)
#define TBL_SHIFT (22)
#define TBL_SAT (29)
#define TBL_LCD_SHIFT (TBL_SHIFT - 3)
#define TBL_RENDER_SHIFT (5)


//string settings for files
#define MAXFNAMELEN (6 + 1)
#define MAXDPATHLEN 50
#define ROOT_FOLDER "/mnt/SD/POLYSYN"
#define WAVE_FOLDER "WAVE"
#define VOICE_FOLDER "VOICE"
#define PATCH_FOLDER "PATCH"
#define FAVS_FILE "FAVS"
#define FILES_LIST "FILS-"
#define DIRS_LIST "DIRS-"
#define MAXFILES 256 //if you ever increase this, indexes need to be switched to uint16_t
#define MAXDIRS 50
const char def_wave[4][MAXFNAMELEN] = { "SIN", "TRI", "SQU", "SAW_DN" };

//file browser indexes
#define FILTYPES 2
#define WAVE 0
#define PATCH 1

#define MAIN_FADE 300

//knob object indexes
#define ENCODERS 8 

#define KNOB_GRP 5
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
#define LCDelems (OBJ6 + 1)


//screen indexes
#define WAVETBL 0
#define TBLPOS 1
#define AMPENV 2
#define PITCH 3
#define DUALENV1 4
#define FILTER 5
#define DUALENV2 6
#define ARPEGSETUP 7
#define ARPEGNOTES 8
#define PATCHLD 9
#define PATCHSV 10
#define MIDIINS 11
#define MIDICCS 12
#define OUTS 13
#define MODA 14
#define NOTES 15
#define ARPREC 16
#define FAVS 17
#define HARMONIC 18
#define PHASE 19
#define PITRATIO 20
#define SCREEN_CNT 21
#define ROW_CHAR_CNT (22)

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
#define bitDrum 12
#define bitMidiThru 13

//button/knob settings
#define INPUTS_CNT 16
#define bitOsc 0 	
#define bitWave 1 	
#define bitAEnv 5 	
#define bitPEnv 6 
#define bitFilt 7 	
#define bitFTrack 8	 
#define bitFEnv 9 	
#define bitFECut 10 
#define bitArp 11 	
#define bitArpFilt 12
#define bitArpTrig 13 
#define bitArpSkip 14 	
#define bitMod 15 		
#define bitMain 16 		
#define bitNotes 17 
#define bitEnvs 18 	
#define bitPoly 19 	
#define bitLgto 20 
#define bitHold 21	
#define bitKeyVel 22
#define bitHarms 23
#define bitPhase 27
#define bitPitRatio 28
#define bitPoly16 29
	
	
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
#define EX_DRUM 17
#define EX_FTRACK 18
#define EX_PATCHLD 19
#define EX_HARM 20
#define EX_COPY 21
#define EX_PIT_RATIO 22
#define EX_POLY16 23
#define poopSize 501


enum ENV_ITEM {
	e_TBLX = 0,
	e_TBLY,
	e_PIT,
	e_FCUT,
	e_FRES,
	e_NONE,
	e_ENV_MAX
};

//#define EX_AMP_SET 14

#define HOLD_TIME 300
	
// static const int8_t BIG_GROUP[8][3] = {
	// {bitOsc, -1, 0},
	// {bitHarms, HARMONIC, EX_HARM},
	// {bitAEnv, AMPENV, 0},
	// {bitPEnv, DUALENV1, 0},
	// {bitFilt, FILTER, EX_FILT},
	// {bitFEnv, DUALENV2, 0},
	// {bitArp, ARPEGSETUP, 0},
	// {bitMain, OUTS, 0}
// };

// static const int8_t OTHER_GROUPS[24][4] = {
	// {E_OSC, bitMod, MODA, 0},									//lower mid row		
	// {E_OSC, bitWave, WAVETBL, EX_WAVE}, 			
	// {E_OSC, bitPoly, PITCH, EX_POLY}, 
	// {E_OSC, bitPhase, PHASE, EX_HARM}, 		
	// {E_OSC, bitNotes, NOTES, 0}, 
	// {E_OSC, bitEnvs, MIDIINS, 0}, 
	// {E_OSC, bitKeyVel, MIDICCS, 0}, 
	// {E_OSC, bitHold, -1, EX_HOLD1},	
	
	// {MAINTOG, bitMidiThru, -1, 0}, 								//lower left row
	// {MAINTOG, bitSolo, -1, 0}, 
	// {MAINTOG, bitHoldAll, -1, EX_HOLD_ALL}, 
	// {MAINTOG, bitArpSync, -1, EX_SYNC},
	// {MAINTOG, bitDrum, ARPEGNOTES, EX_DRUM}, 	
	// {E_OSC, -1, -1, EX_ARPNOTREC}, 		
				
	// {MAINTOG, bitRoute, -1, 0}, 	{MAINTOG, bitCopy, -1, 0}, 	//columns
	// {E_OSC, -1, -1, EX_PATSVLD}, 	{E_OSC, -1, -1, EX_PATRNDCLR}, 
	// {E_OSC, -1, FAVS, EX_FAV1},		{E_OSC, -1, FAVS, EX_FAV2},	
	// {E_OSC, -1, FAVS, EX_FAV3}, 	{E_OSC, -1, FAVS, EX_FAV4},		
	// {E_OSC, -1, -1, EX_TRIG_ON}, 	{E_OSC, -1, -1, EX_TRIG_OFF}
// };
	
	
//filter track toggle {E_OSC, bitFTrack, FILTER, EX_FTRACK}, 	

	

#define ptrCnt (23)


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
#define MOD_CNT 16
#define PIT_MOD 0
#define CUT_MOD 1
#define AMP_MOD 2
#define TBL_X_MOD 3
#define GATE_MOD 4
#define TBL_Y_MOD 5
#define RES_MOD 6
#define ARPTIME_MOD 7
#define PAN_MOD 8
#define ATK_MOD 9
#define DEC_MOD 10
#define SUS_MOD 11
#define REL_MOD 12
#define PGLIDE_MOD 13
#define VGLIDE_MOD 14
#define ARPGLIDE_MOD 15





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
#define OSC_SRC 4
#define AENV_SRC 5
#define PENV_SRC 6
#define FENV_SRC 7
#define ARP_SRC 8

#define TOTAL_MOD_SRC (ARP_SRC + 1)
#define TOTAL_MODS (OSC_CNT * TOTAL_MOD_SRC + 2)
#define MOD_NONE 0

#define MOD_MAIN_OUT 	(TOTAL_MODS - 1)

#define COPY_ALL 0
#define COPY_PIT 1
#define COPY_FILT 2
#define COPY_ARP 3

//solo/mute settings
#define SOLO 1
#define MUTE 2




#endif 