#ifndef SYNTHSTRUCTS_H
#define SYNTHSTRUCTS_H

//parent osc structs

struct AMP_ENV_KNOBS {
	uint8_t rate[4];
	uint8_t goal[2];
};

struct PIT_KNOBS {
	int32_t pitch;
	uint8_t pit_glide;
	uint8_t vel_glide;
};

/* struct PIT_SPREAD {
	int32_t pit[NOTES_CNT];
	uint8_t vel[NOTES_CNT];
}; */

struct PIT_ENV_KNOBS {
	uint8_t time[FREE_STAGES];
	uint8_t glide[FREE_STAGES + 2];
	int32_t pitch[FREE_STAGES + 2];	
};
	
struct FILT_KNOBS {
	int32_t FRQ;
	uint8_t RES;
	uint8_t TYPE;
};

struct ARP_KNOBS {
	uint8_t steps;
	uint8_t G;
	uint16_t T;
	
	int8_t P[MAXARP];
	uint8_t V[MAXARP];
	int8_t E[MAXARP];
	float BPM;	
};

struct MIDI_PARAMS {
	uint8_t chan;
	uint8_t keyMax;
	uint8_t keyMin;
	uint8_t CC_nums[2];
	uint8_t CC_levs[4];
};

//osc child structs
struct AMP_ENV {
	uint32_t stage;
	uint32_t val;
	
	//these are here so they can be set at k rate instead of looking up every s rate
	int32_t goal;
	int32_t rate;
};
	
struct PIT_ENV {
	uint16_t stage;
	uint16_t clock;
	int32_t val;
};

struct ARP_ENV {
	int8_t stage;
	int8_t atk_stage;
	uint16_t clock;
	int32_t val;
	uint32_t vel;
};



struct NOTE_SLOT {
	uint16_t priority;
	uint8_t pitch;
	uint8_t vel;
};

//"G.FUND @@@ PRTL1 @@@",
//"G.PTL1 @@@ STEP  @@@",
//"G.LAST @@@ COUNT @@@"
struct HARMONICS {
	uint8_t gainFund;
	uint8_t first;
	uint8_t gainFirst;
	uint8_t step;
	uint8_t gainLast;
	uint8_t cnt;	
};
	

/* //data structure for knob status
struct KNOB_TRACKER {
	uint8_t A;
	uint8_t B;
	uint8_t S;
	int8_t preTurn;
	uint32_t lastTick;	
}; */

//file save/load/browse structs
struct browseCnts {
	uint8_t files;
	uint8_t dirs;
};

struct filsList {
	char name[MAXFNAMELEN];
	uint8_t dirInd;
	uint8_t filInd;
	struct filsList *next;
	struct filsList *prev;
};

struct dirsList {
	char name[MAXFNAMELEN];
	char path[MAXDPATHLEN];
	uint8_t numFiles;
	struct filsList *insertAfter;
};


	


	

	





#endif 