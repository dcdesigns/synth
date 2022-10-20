#ifndef SYNTHCONTROLS_H
#define SYNTHCONTROLS_H

#include <Bela.h>

void initPatch(uint8_t first, uint8_t last);
//void resetPatch();
//void updateOscTypes(uint8_t *arrA, uint8_t *cntA, uint8_t *arrB, uint8_t *cntB, uint8_t testArr);
//void updateFilterCnt();
int initSynthStuff(BelaContext* context, void* userData);
void checkHarmQueue();
void scanInputs(BelaContext* context);
int checkSwitch(int group, int ind, int sig);
void  addToInputQueue(uint8_t group, uint8_t ind, int32_t val, uint8_t isQuick);
uint8_t  oscFromGrpInd(uint8_t group, uint8_t ind);
void  updateLEDs();
uint16_t LEDfromGroup(int8_t osc, uint8_t ind, int8_t tog);
//void equalizeAmp(uint8_t osc);
uint8_t finishRecording();
void  toggleSelected(uint8_t osc);
void copyOsc(uint8_t osc, uint8_t bit);	
void routeMod(uint8_t destOsc, uint8_t bit, uint16_t sourceBit);	
//void copyToggle(uint8_t mask, uint8_t osc);
//void togglePitchType();
//void updateSetGain(uint8_t firstOsc, uint8_t lastOsc);
//void  incrementGain(uint8_t gainInd, int8_t inc, uint8_t spreadInd);
void updateUINT8val(uint8_t *val, int8_t inc, uint8_t isToggle, uint8_t LCD, uint32_t full = 0);
void handleKnobs();
void updateSingleMod(uint8_t modType, uint8_t destParent, uint8_t sourceIndex);
void updateAllMod(uint8_t first, uint8_t last);
void __attribute__((noinline)) updateArpTime(uint8_t osc, float newBPM);
void __attribute__((noinline)) resetArpPages(uint8_t firstOsc, uint8_t lastOsc);
//void styleEdit(int8_t inc, uint8_t lcd);
uint8_t knobPos(uint8_t zeroKnob, uint8_t knobID);
uint8_t bounded(uint8_t val, int8_t inc, uint8_t min, uint8_t max);
void spreadNotes(uint8_t osc, uint8_t zeroNote);
int32_t randVal(int32_t max, uint8_t isPos);
void updatePitRatio();
int32_t ensure_not_self_ratio(int32_t inc);
void resetArpPages(uint8_t firstOsc, uint8_t lastOsc);
void updateAllMod(uint8_t first, uint8_t last);
void toggleSelected(uint8_t osc);
void updateLEDs();
void resetPatch();


#endif 