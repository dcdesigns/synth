#ifndef SYNTH_VARIABLES_H
#define SYNTH_VARIABLES_H

#include "settings.h"
#include "synthStructs.h"

//globals


extern const float incsBPM[5];
extern const uint8_t posBPM[5];
extern const uint8_t charL[3];
extern const uint8_t charH[3];

//synth main out
extern uint8_t main_gain;
extern int32_t main_mask;
extern uint32_t main_clock1;
extern uint32_t main_clock2;

extern uint8_t untilChild[OSC_CNT];
extern uint8_t childCnt[OSC_CNT];

#define TABLE_BAND_CNT 8
#define TABLE_FULL_SIZE 2040
extern const uint32_t TABLE_BAND_PARTIAL_MAX[];
extern const uint32_t TABLE_RES[];
extern const uint32_t TABLE_BAND_OFFSET[];
extern const uint32_t TABLE_BAND_COARSE_SHIFT[];
extern const uint32_t TABLE_BAND_FINE_SHIFT[];
extern const uint32_t TABLE_BAND_FINE_MASK[];
extern const uint32_t TABLE_BAND_LAST_MASK[];

//objects for every osc parent
extern int32_t wavArray[OSC_CNT][TABLE_CNT][TABLE_FULL_SIZE];
extern int32_t harmArray[OSC_CNT][TABLE_CNT][TABLE_FULL_SIZE];
extern struct HARMONICS harmParams[OSC_CNT][TABLE_CNT];
extern uint8_t osc_gain[OSC_CNT];
extern uint8_t mod_src[OSC_CNT][MOD_CNT];
extern struct PIT_KNOBS pit_knobs[OSC_CNT];
extern struct AMP_ENV_KNOBS amp_env_knobs[OSC_CNT];
extern struct DUAL_ENV_KNOBS env1_knobs[OSC_CNT];
extern struct DUAL_ENV_KNOBS env2_knobs[OSC_CNT];
extern struct PIT_RATIO_KNOBS pit_ratio[OSC_CNT];
extern struct FILT_KNOBS filt_knobs[OSC_CNT];
extern struct ARP_KNOBS arpeggio[OSC_CNT];
extern struct MIDI_PARAMS midi_knobs[OSC_CNT];
extern int8_t panLeft[OSC_CNT];
extern struct PHASE_KNOBS phase_knobs[OSC_CNT][TABLE_CNT];
extern int32_t *curWave[OSC_CNT][TABLE_CNT];
extern uint32_t tableRatios[OSC_CNT][2];
extern int32_t lcdRand[129];

//uint8_t style[OSC_CNT];

extern uint32_t toggles[OSC_CNT + 1];
extern uint8_t arp_pages[OSC_CNT];
extern uint8_t arp_page[OSC_CNT];
extern uint8_t table_page;
extern int32_t phaseArray[TABLE_FULL_SIZE];
extern filsList* phaseFile[OSC_CNT][TABLE_CNT];
extern char lastPhaseName[MAXFNAMELEN];

//cc values (updated on cc input, don't need to be saved)
//extern int32_t wind_gain[OSC_CNT];
extern uint8_t susOn[OSC_CNT];
extern int32_t CC_vals[CC_CNT][OSC_CNT];
extern int32_t kCCs[CC_CNT][OSC_CNT];


//objects for every osc child
extern struct AMP_ENV amp_env[OSC_CHILD_CNT];
extern struct PIT_ENV pit_env[OSC_CHILD_CNT];
extern struct PIT_ENV filt_env[OSC_CHILD_CNT];
extern struct ARP_ENV arp_env[OSC_CHILD_CNT];
extern int32_t note[OSC_CHILD_CNT];
extern int8_t vel[OSC_CHILD_CNT];
extern int32_t monoPitch[OSC_CNT];
extern uint8_t monoVel[OSC_CNT];
extern int32_t *modSrc[MOD_CNT][OSC_CHILD_CNT];
extern int32_t kNote[OSC_CHILD_CNT];



//arrays for matching children and parents



//mod sources
//extern int32_t maxNegMod;
extern int32_t zeroMod;
extern int32_t maxMod;
//extern uint8_t maxModByte;
extern int32_t lastSignal[OSC_CHILD_CNT];
extern int32_t lastPolyCombo[POLY_CNT];
extern int32_t lastMain; //most recent main out sample
extern int32_t lastAudio[3]; //most recent audio in samples


//int32_t delay_lefts[65536][OSC_CNT] __attribute__ ((section (".sdram")));
//int32_t delay_rights[OSC_CNT][65536] __attribute__ ((section (".sdram")));
//uint16_t delay_write;
//uint16_t delay_read_left[OSC_CNT];
//uint16_t delay_read_right[OSC_CNT];
//uint8_t delay_left_knobs[OSC_CNT];
//uint8_t delay_right_knobs[OSC_CNT];


//event queues
extern uint8_t midiEvents[2][MIDI_QUEUE_SIZE][3]; 	//buffer for incoming midi notes
//KNOB_TRACKER knobs[ENCODERS];	//keeps track of knob/button positions
//uint8_t scrn_switches[ENCODERS];
extern int8_t inputQueue[4][4];
extern uint8_t KNOB_A[8];
extern uint32_t KNOB_TICKS[8];
extern int8_t KNOB_DIR[8];
extern uint16_t KNOB_S;
extern uint16_t LED[4];

//int8_t knob_incs[3 * ENCODERS];	//buffer for knob/button events
extern uint8_t LCD_update[LCDelems + 1]; //buffer for lcd updates
extern uint32_t GRAPH_update;
extern uint32_t FIL_update;
extern uint32_t HARM_update;
extern uint32_t pit_ratio_update;
//uint8_t curKnob; //which knob event to check
//uint8_t curNote; //which note event to check
//uint8_t curCC; //which cc event to check
//uint8_t curLCD; //which LCD element to check
extern uint8_t scanInd; //which knob/button address to check
extern uint32_t ticks; //global time (used to calculate knob acceleration)
//uint8_t showUpdate;
extern int16_t blinkInd;
extern int16_t blinkGrp;
extern uint16_t routeTog;
extern uint16_t drumPage;
extern float joyVal[4];


//file browser lists
extern struct filsList files[FILTYPES][MAXFILES];
extern struct dirsList dirs[FILTYPES][MAXFILES];

//stores the number of used indexes in the above two arrays
extern struct browseCnts browseCnt[FILTYPES];

//pointers to files lists to allow browsing
extern struct filsList *curWavFile[OSC_CNT][TABLE_CNT];
extern struct filsList *curPatchFile;

//patch save variables
extern char saveName[MAXFNAMELEN];
extern uint8_t saveNameInd;
extern uint8_t saveDirInd;
extern char lastLetter;
extern uint8_t isSaved;

//variables for screen/osc selecting
//uint8_t scrnInd;
extern uint8_t screenInd; //current screen
extern uint8_t oscInd; //current osc
extern uint8_t pitchShift; //for toggling between coarse/fine increments
extern uint8_t envInd; //current screen for pitch envelopes
extern uint8_t notesPage;
extern uint8_t notesTog;
extern uint8_t spreadIsVel;

extern uint8_t isMainLVL;
extern uint8_t favInd;
extern uint8_t favSave;
// float BPM_inc;
// uint8_t BPM_pos;
extern uint8_t arpToggle[4];
extern uint8_t recording;
extern uint8_t recNotes; 
extern uint8_t recCent;
extern uint8_t recEnv;
extern uint8_t recVel;
extern uint8_t recRhythm;
extern uint16_t recTimes[MAXARP];
extern uint8_t stepsPer[MAXARP];
extern uint8_t recLoudest;
extern uint32_t recShortest;
extern uint32_t recFullTime;
extern uint8_t indBPM;
extern uint8_t indChar;
//uint8_t copy[3];
//uint8_t copyArmed;

//uint32_t poop[poopSize];
//arrays for storing which oscillators are noise and which are wave table (rather than doing if/then on the spot)
/* uint8_t noiseOsc[OSC_CHILD_CNT];
uint8_t waveOsc[OSC_CHILD_CNT];
uint8_t noiseCnt;
uint8_t waveCnt;

uint8_t windOsc[OSC_CHILD_CNT];
uint8_t keyOsc[OSC_CHILD_CNT];
uint8_t keyCnt;
uint8_t windCnt;

uint8_t filtOsc[OSC_CHILD_CNT];
uint8_t nonFiltOsc[OSC_CHILD_CNT];
uint8_t filtCnt;
uint8_t nonFiltCnt; */




extern const void *varPtrs[ptrCnt]; 


extern const uint16_t ptrSizes[];
extern const uint16_t ptrSingleSizes[];

extern const uint8_t resetCnt;
extern const uint8_t settingsCnt;
extern const uint8_t copyStop;
extern const uint8_t resetVals[];

#endif //SYNTH_VARIABLES_H