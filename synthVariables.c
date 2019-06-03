#ifndef SYNTH_VARIABLES_C
#define SYNTH_VARIABLES_C

#include "./synthStructs.h"

//synth main out
uint8_t main_gain;
int32_t main_mask;
uint32_t main_clock;


//objects for every osc parent
int32_t wavArray[OSC_CNT][WAVE_RES];
int32_t harmArray[OSC_CNT][WAVE_RES];
HARMONICS harmParams[OSC_CNT];
uint8_t osc_gain[OSC_CNT];
uint8_t mod_src[OSC_CNT][MOD_CNT];
PIT_KNOBS pit_knobs[OSC_CNT];
AMP_ENV_KNOBS amp_env_knobs[OSC_CNT];
PIT_ENV_KNOBS pit_env_knobs[OSC_CNT];
PIT_ENV_KNOBS filt_env_knobs[OSC_CNT];
FILT_KNOBS filt_knobs[OSC_CNT];
ARP_KNOBS arpeggio[OSC_CNT];
MIDI_PARAMS midi_knobs[OSC_CNT];
int8_t panLeft[OSC_CNT];
int32_t *curWave[OSC_CNT];
//uint8_t style[OSC_CNT];

uint32_t toggles[OSC_CNT + 1];
uint8_t arp_pages[OSC_CNT];
uint8_t arp_page[OSC_CNT];


//cc values (updated on cc input, don't need to be saved)
//int32_t wind_gain[OSC_CNT];
uint8_t susOn[OSC_CNT];
int32_t CC_vals[CC_CNT][OSC_CNT];
int32_t kCCs[CC_CNT][OSC_CNT];


//objects for every osc child
AMP_ENV amp_env[OSC_CHILD_CNT];
PIT_ENV pit_env[OSC_CHILD_CNT];
PIT_ENV filt_env[OSC_CHILD_CNT];
ARP_ENV arp_env[OSC_CHILD_CNT];
int32_t note[OSC_CHILD_CNT];
int8_t vel[OSC_CHILD_CNT];
int32_t monoPitch[OSC_CNT];
uint8_t monoVel[OSC_CNT];
int32_t *modSrc[MOD_CNT][OSC_CHILD_CNT];
int32_t kNote[OSC_CHILD_CNT];



//arrays for matching children and parents
const uint8_t parents[OSC_CHILD_CNT] = {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,2,3,4,5};
const uint8_t firstChild[OSC_CNT] = {0,8,16,17,18,19};


//mod sources
//int32_t maxNegMod;
int32_t zeroMod;
int32_t maxMod;
//uint8_t maxModByte;
int32_t lastSignal[OSC_CHILD_CNT];
int32_t lastPolyCombo[POLY_CNT];
int32_t lastMain; //most recent main out sample


//event queues
uint8_t midiEvents[2][MIDI_QUEUE_SIZE][3]; 	//buffer for incoming midi notes
//KNOB_TRACKER knobs[ENCODERS];	//keeps track of knob/button positions
//uint8_t scrn_switches[ENCODERS];
int8_t inputQueue[4][4] = {0};
uint8_t KNOB_A[8];
uint32_t KNOB_TICKS[8];
int8_t KNOB_DIR[8];
uint16_t KNOB_S;
uint16_t MX[4];
uint16_t LED[4];

//int8_t knob_incs[3 * ENCODERS];	//buffer for knob/button events
uint8_t LCD_update[LCDelems + 1];	//buffer for lcd updates
uint8_t GRAPH_update;
uint8_t FIL_update[OSC_CNT + 1];
int8_t HARM_update[OSC_CNT];
//uint8_t curKnob; //which knob event to check
//uint8_t curNote; //which note event to check
//uint8_t curCC; //which cc event to check
//uint8_t curLCD; //which LCD element to check
uint8_t scanInd; //which knob/button address to check
uint32_t ticks; //global time (used to calculate knob acceleration)
//uint8_t showUpdate;



//file browser lists
static filsList files[FILTYPES][MAXFILES] __attribute__ ((section (".sdram")));
static dirsList dirs[FILTYPES][MAXFILES] __attribute__ ((section (".sdram")));

//stores the number of used indexes in the above two arrays
browseCnts browseCnt[FILTYPES];

//pointers to files lists to allow browsing
filsList *curWavFile[OSC_CNT];
filsList *curPatchFile;

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
// static float BPM_inc;
// static uint8_t BPM_pos;
static uint8_t arpToggle[4];
static uint8_t recording;
static uint8_t recNotes; 
static uint8_t recCent;
static uint8_t recEnv;
static uint8_t recVel;
static uint8_t recRhythm;
static uint16_t recTimes[MAXARP];
static uint8_t stepsPer[MAXARP];
static uint8_t recLoudest;
static uint32_t recShortest;
static uint32_t recFullTime;
static uint8_t indBPM;
static uint8_t indChar;
//static uint8_t copy[3];
//static uint8_t copyArmed;

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




const uint8_t ptrCnt = 20;

void *varPtrs[ptrCnt]; 


const uint16_t ptrSizes[] = {
	sizeof(toggles), sizeof(osc_gain), sizeof(panLeft), sizeof(midi_knobs),
	sizeof(pit_knobs), sizeof(amp_env_knobs), sizeof(pit_env_knobs), sizeof(filt_env_knobs), 
	sizeof(filt_knobs), sizeof(mod_src), sizeof(arpeggio), sizeof(harmParams),
	sizeof(amp_env), sizeof(pit_env), sizeof(filt_env), sizeof(arp_env), 
	sizeof(vel), sizeof(note), sizeof(monoPitch), sizeof(monoVel)  
};

const uint8_t ptrSingleSizes[] = {
	sizeof(toggles[0]), sizeof(osc_gain[0]), sizeof(panLeft[0]), sizeof(midi_knobs[0]),
	sizeof(pit_knobs[0]), sizeof(amp_env_knobs[0]), sizeof(pit_env_knobs[0]), sizeof(filt_env_knobs[0]), 
	sizeof(filt_knobs[0]), sizeof(mod_src[0]), sizeof(arpeggio[0]), sizeof(harmParams[0]),
	sizeof(amp_env[0]), sizeof(pit_env[0]), sizeof(filt_env[0]), sizeof(arp_env[0]), 
	sizeof(vel[0]), sizeof(note[0]), sizeof(monoPitch[0]), sizeof(monoVel[0])  
};

const uint8_t resetCnt = 16;
const uint8_t settingsCnt = 12;
const uint8_t copyStop = 18;
const uint8_t resetVals[] = {
	0,0,64,0,
	0,50,0,0,
	0,0,0,0,
	0,0,0,0
};
	

#endif 