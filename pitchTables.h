#ifndef PITCH_TABLES_H
#define PITCH_TABLES_H

#include "settings.h"

#define INT_MAX 0x7FFFFFFF

extern uint32_t phase_width_incs[256][2];
extern uint32_t PHASEINCS[257] __attribute__ ((section (".sram")));
extern uint32_t ATTACK_K[128]; 
extern uint32_t SEEK[128];
extern uint32_t SEEK_S_RATE[128]; 
extern uint32_t GAIN[128]; 
extern uint32_t VELGAIN[128];
extern uint16_t TIME[128];
extern char screens[SCREEN_CNT][4][ROW_CHAR_CNT];
extern char SCREENS[SCREEN_CNT][10];
extern char noteLabels[169][5];
extern int8_t big_group[8][3];
extern int8_t other_groups[24][4];
extern uint8_t parents[OSC_CHILD_CNT];
extern uint8_t firstChild[OSC_CNT];

extern char chan_pins[4][3];
extern char mx_pins[5][3]; 
extern char led_pins[4][3];
extern char lower_knob_pins[2][3];
extern char upper_knob_pins[2][3];

extern char saveCopyStr[3][7];
extern char oscStr[6][5];
extern char lvlStr[3][4];
extern char arpNoteLeader[3][4];
extern char yesNoStr[2][4];
extern char onOffStr[2][4];
extern char startStopStr[2][7];
extern char copyWhat[2][4];
extern char envStr[5][6];
extern char filtStr[3][4];
extern char stageStr[5][4];
extern char notesStr[2][4];
extern char trackStr[2][4];
extern char velStr[3][7];
extern char loopStr[4][6];
extern char modStrA[5][5];
extern char modStrB[9][4];
extern char modStrO[6][3];

extern char recStr[5];
extern char timeStr[6];
extern char typeStr[6];
extern char favStr[2][21];
extern char nts[12][4];
extern char units[3][3];

extern const char ENV_ITEM_STR[(int)e_ENV_MAX][5];


extern const uint8_t noteLabelTypes[3];
extern const uint8_t SET2;
extern const uint8_t maxNoteLabelInd;

extern const uint16_t DATA_SZ[];
extern const int32_t DATA_CNT;
	

//stored data arrays
#if LOADTABLES
extern const char DATA_saveCopyStr[3][7];
extern const char DATA_oscStr[6][5]; 
extern const char DATA_lvlStr[3][4];
extern const char DATA_arpNoteLeader[3][4];
extern const char DATA_yesNoStr[2][4];
extern const char DATA_onOffStr[2][4];
extern const char DATA_startStopStr[2][7];
extern const char DATA_copyWhat[2][4];
extern const char DATA_envStr[5][6];
extern const char DATA_filtStr[3][4];
extern const char DATA_stageStr[5][4];
extern const char DATA_notesStr[2][4];
extern const char DATA_trackStr[2][4];
extern const char DATA_velStr[3][7];
extern const char DATA_loopStr[4][6];
extern const char DATA_modStrA[5][5];
extern const char DATA_modStrB[9][4];
extern const char DATA_modStrO[6][3]; 

extern const char DATA_recStr[5];
extern const char DATA_timeStr[6];
extern const char DATA_typeStr[6];
extern const char DATA_favStr[2][21];
extern const char DATA_nts[12][4];
extern const char DATA_units[3][3];


extern const char DATA_chan_pins[4][3];
extern const char DATA_mx_pins[5][3];
extern const char DATA_led_pins[4][3];
extern const char DATA_lower_knob_pins[2][3];
extern const char DATA_upper_knob_pins[2][3];

extern const uint8_t DATA_parents[OSC_CHILD_CNT];
extern const uint8_t DATA_firstChild[OSC_CNT];
extern const int8_t BIG_GROUP[8][3];
extern const int8_t OTHER_GROUPS[24][4];
extern const uint32_t DATA_phase_width_incs[256][2];
extern const uint32_t DATA_PHASEINCS[257];
extern const uint32_t DATA_ATTACK_K[128];
extern const uint32_t DATA_SEEK[128];
extern const uint32_t DATA_SEEK_S_RATE[128]; 
extern const uint32_t DATA_GAIN[128];
extern const uint32_t DATA_VELGAIN[128];
extern const uint16_t DATA_TIME[128];
extern const char DATA_noteLabels[169][5];
extern const char DATA_screens[SCREEN_CNT][4][ROW_CHAR_CNT];
extern const char DATA_SCREENS[SCREEN_CNT][10];
extern const char MOD_DESTS[MOD_CNT][5];
#endif
uint32_t __attribute__( ( always_inline ) ) getPhaseInc(uint32_t pitch);
uint32_t __attribute__( ( always_inline ) ) getPitch(uint32_t goalPhase);

#endif 
