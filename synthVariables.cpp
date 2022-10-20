#include <stdint.h>
#include "settings.h"
#include "synthControls.h"
#include "synthStructs.h"
#include "synthVariables.h"
#include "modules.h"



const float incsBPM[5] = { .1, 1, 10, 100, 1000 };
const uint8_t posBPM[5] = { 5, 3, 2, 1, 0 };
const uint8_t charL[3] = { 'A', '0', '!' };
const uint8_t charH[3] = { 'Z', '9', ')' };

//synth main out
uint8_t main_gain;
int32_t main_mask;
uint32_t main_clock1;
uint32_t main_clock2;

uint8_t untilChild[OSC_CNT] = { NOTES_CNT, 2 * NOTES_CNT, 2 * NOTES_CNT + 1, 2 * NOTES_CNT + 2, 2 * NOTES_CNT + 3, 2 * NOTES_CNT + 4 };
uint8_t childCnt[OSC_CNT] = { NOTES_CNT, NOTES_CNT, 1, 1, 1, 1 };

const uint32_t TABLE_BAND_PARTIAL_MAX[TABLE_BAND_CNT] = { 231, 115, 57, 28, 14, 7, 3, 1 };
const uint32_t TABLE_RES[TABLE_BAND_CNT] = { 1024, 512, 256, 128, 64, 32, 16, 8 };
const uint32_t TABLE_BAND_OFFSET[TABLE_BAND_CNT] = { 0, 1024, 1536, 1792, 1920, 1984, 2016, 2032 };
const uint32_t TABLE_BAND_COARSE_SHIFT[TABLE_BAND_CNT] = { 22, 23, 24, 25, 26, 27, 28, 29 };
const uint32_t TABLE_BAND_FINE_SHIFT[TABLE_BAND_CNT] = { 9, 8, 7, 6, 5, 4, 3, 2 };
const uint32_t TABLE_BAND_FINE_MASK[TABLE_BAND_CNT] = { 0x003FFFFF, 0x007FFFFF, 0x00FFFFFF, 0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF, 0x0FFFFFFF, 0x1FFFFFFF };
const uint32_t TABLE_BAND_LAST_MASK[TABLE_BAND_CNT] = { 1023, 511, 255, 127, 63, 31, 15, 7 };

//objects for every osc parent
int32_t wavArray[OSC_CNT][TABLE_CNT][TABLE_FULL_SIZE];
int32_t harmArray[OSC_CNT][TABLE_CNT][TABLE_FULL_SIZE];

struct HARMONICS harmParams[OSC_CNT][TABLE_CNT];
uint8_t osc_gain[OSC_CNT];
uint8_t mod_src[OSC_CNT][MOD_CNT];
struct PIT_KNOBS pit_knobs[OSC_CNT];
struct AMP_ENV_KNOBS amp_env_knobs[OSC_CNT];
struct DUAL_ENV_KNOBS env1_knobs[OSC_CNT];
struct DUAL_ENV_KNOBS env2_knobs[OSC_CNT];
struct PIT_RATIO_KNOBS pit_ratio[OSC_CNT];
struct FILT_KNOBS filt_knobs[OSC_CNT];
struct ARP_KNOBS arpeggio[OSC_CNT];
struct MIDI_PARAMS midi_knobs[OSC_CNT];
int8_t panLeft[OSC_CNT];
struct PHASE_KNOBS phase_knobs[OSC_CNT][TABLE_CNT];
int32_t *curWave[OSC_CNT][TABLE_CNT];
uint32_t tableRatios[OSC_CNT][2] = { 0 };
int32_t lcdRand[129];
//uint8_t style[OSC_CNT];

uint32_t toggles[OSC_CNT + 1];
uint8_t arp_pages[OSC_CNT];
uint8_t arp_page[OSC_CNT];
uint8_t table_page;
int32_t phaseArray[TABLE_FULL_SIZE];
filsList* phaseFile[OSC_CNT][TABLE_CNT];
char lastPhaseName[MAXFNAMELEN];


//cc values (updated on cc input, don't need to be saved)
//int32_t wind_gain[OSC_CNT];
uint8_t susOn[OSC_CNT];
int32_t CC_vals[CC_CNT][OSC_CNT];
int32_t kCCs[CC_CNT][OSC_CNT];


//objects for every osc child
struct AMP_ENV amp_env[OSC_CHILD_CNT];
struct PIT_ENV pit_env[OSC_CHILD_CNT];
struct PIT_ENV filt_env[OSC_CHILD_CNT];
struct ARP_ENV arp_env[OSC_CHILD_CNT];
int32_t note[OSC_CHILD_CNT];
int8_t vel[OSC_CHILD_CNT];
int32_t monoPitch[OSC_CNT];
uint8_t monoVel[OSC_CNT];
int32_t *modSrc[MOD_CNT][OSC_CHILD_CNT];
int32_t kNote[OSC_CHILD_CNT];



//arrays for matching children and parents



//mod sources
//int32_t maxNegMod;
int32_t zeroMod;
int32_t maxMod;
//uint8_t maxModByte;
int32_t lastSignal[OSC_CHILD_CNT];
int32_t lastPolyCombo[POLY_CNT];
int32_t lastMain; //most recent main out sample
int32_t lastAudio[3]; //most recent audio in samples


//int32_t delay_lefts[65536][OSC_CNT] __attribute__ ((section (".sdram")));
//int32_t delay_rights[OSC_CNT][65536] __attribute__ ((section (".sdram")));
//uint16_t delay_write;
//uint16_t delay_read_left[OSC_CNT];
//uint16_t delay_read_right[OSC_CNT];
//uint8_t delay_left_knobs[OSC_CNT];
//uint8_t delay_right_knobs[OSC_CNT];


//event queues
uint8_t midiEvents[2][MIDI_QUEUE_SIZE][3]; 	//buffer for incoming midi notes
//KNOB_TRACKER knobs[ENCODERS];	//keeps track of knob/button positions
//uint8_t scrn_switches[ENCODERS];
int8_t inputQueue[4][4] = { 0 };
uint8_t KNOB_A[8];
uint32_t KNOB_TICKS[8];
int8_t KNOB_DIR[8];
uint16_t KNOB_S;
uint16_t LED[4];

//int8_t knob_incs[3 * ENCODERS];	//buffer for knob/button events
uint8_t LCD_update[LCDelems + 1];	//buffer for lcd updates
uint32_t GRAPH_update;
uint32_t FIL_update;
uint32_t HARM_update;
uint32_t pit_ratio_update;
//uint8_t curKnob; //which knob event to check
//uint8_t curNote; //which note event to check
//uint8_t curCC; //which cc event to check
//uint8_t curLCD; //which LCD element to check
uint8_t scanInd; //which knob/button address to check
uint32_t ticks; //global time (used to calculate knob acceleration)
//uint8_t showUpdate;
int16_t blinkInd;
int16_t blinkGrp;
uint16_t routeTog;
uint16_t drumPage;
float joyVal[4];


//file browser lists
struct filsList files[FILTYPES][MAXFILES];
struct dirsList dirs[FILTYPES][MAXFILES];

//stores the number of used indexes in the above two arrays
struct browseCnts browseCnt[FILTYPES];

//pointers to files lists to allow browsing
struct filsList *curWavFile[OSC_CNT][TABLE_CNT];
struct filsList *curPatchFile;

//patch save variables
char saveName[MAXFNAMELEN];
uint8_t saveNameInd;
uint8_t saveDirInd;
char lastLetter;
uint8_t isSaved;

//variables for screen/osc selecting
//uint8_t scrnInd;
uint8_t screenInd; //current screen
uint8_t oscInd; //current osc
uint8_t pitchShift; //for toggling between coarse/fine increments
uint8_t envInd; //current screen for pitch envelopes
uint8_t notesPage;
uint8_t notesTog;
uint8_t spreadIsVel;

uint8_t isMainLVL;
uint8_t favInd;
uint8_t favSave;
// float BPM_inc;
// uint8_t BPM_pos;
uint8_t arpToggle[4];
uint8_t recording;
uint8_t recNotes; 
uint8_t recCent;
uint8_t recEnv;
uint8_t recVel;
uint8_t recRhythm;
uint16_t recTimes[MAXARP];
uint8_t stepsPer[MAXARP];
uint8_t recLoudest;
uint32_t recShortest;
uint32_t recFullTime;
uint8_t indBPM;
uint8_t indChar;
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






const void* varPtrs[] = {
	toggles, osc_gain, panLeft, midi_knobs,
	pit_knobs, amp_env_knobs, env1_knobs, env2_knobs,
	filt_knobs, mod_src, arpeggio, harmParams,
	phase_knobs, pit_ratio, tableRatios,
	amp_env, pit_env, filt_env, arp_env,
	vel, note, monoPitch, monoVel
};


const uint16_t ptrSizes[] = {
	sizeof(toggles), sizeof(osc_gain), sizeof(panLeft), sizeof(midi_knobs),
	sizeof(pit_knobs), sizeof(amp_env_knobs), sizeof(env1_knobs), sizeof(env2_knobs),
	sizeof(filt_knobs), sizeof(mod_src), sizeof(arpeggio), sizeof(harmParams),
	sizeof(phase_knobs), sizeof(pit_ratio), sizeof(tableRatios),
	sizeof(amp_env), sizeof(pit_env), sizeof(filt_env), sizeof(arp_env), 
	sizeof(vel), sizeof(note), sizeof(monoPitch), sizeof(monoVel)
	
};

const uint16_t ptrSingleSizes[] = {
	sizeof(toggles[0]), sizeof(osc_gain[0]), sizeof(panLeft[0]), sizeof(midi_knobs[0]),
	sizeof(pit_knobs[0]), sizeof(amp_env_knobs[0]), sizeof(env1_knobs[0]),  sizeof(env2_knobs[0]),
	sizeof(filt_knobs[0]), sizeof(mod_src[0]), sizeof(arpeggio[0]), sizeof(harmParams[0]),
	sizeof(phase_knobs[0]), sizeof(pit_ratio[0]), sizeof(tableRatios[0]),
	sizeof(amp_env[0]), sizeof(pit_env[0]), sizeof(filt_env[0]), sizeof(arp_env[0]), 
	sizeof(vel[0]), sizeof(note[0]), sizeof(monoPitch[0]), sizeof(monoVel[0])
	
};

const uint8_t resetCnt = 19;
const uint8_t settingsCnt = 15;
const uint8_t copyStop = 21;
const uint8_t resetVals[] = {
	0,0,64,0,
	0,50,0,0,
	0,0,0,0,
	0,0,0,
	0,0,0,0
};
	