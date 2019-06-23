#ifndef SYNTHCONTROLS_H
#define SYNTHCONTROLS_H

void __attribute__(( noinline )) initPatch(uint8_t first, uint8_t last);
//void __attribute__(( noinline )) resetPatch();
//void __attribute__(( noinline )) updateOscTypes(uint8_t *arrA, uint8_t *cntA, uint8_t *arrB, uint8_t *cntB, uint8_t testArr);
//void __attribute__(( noinline )) updateFilterCnt();
void initSynthStuff();
void __attribute__(( noinline )) checkHarmQueue();
void __attribute__(( noinline )) scanInputs();
void  __attribute__(( noinline )) checkSwitch(uint16_t *state, uint8_t ind, uint8_t sig, uint8_t group);
void  __attribute__(( noinline )) addToInputQueue(uint8_t group, uint8_t ind, int32_t val, uint8_t isQuick);
uint8_t  __attribute__(( noinline )) oscFromGrpInd(uint8_t group, uint8_t ind);
void  __attribute__(( noinline )) updateLEDs();
uint16_t LEDfromGroup(int8_t osc, uint8_t ind, uint8_t tog);
//void __attribute__(( noinline )) equalizeAmp(uint8_t osc);
uint8_t finishRecording();
void  __attribute__(( noinline )) toggleSelected(uint8_t osc);
void __attribute__(( noinline )) copyOsc(uint8_t osc, uint8_t bit);	
void __attribute__(( noinline )) routeMod(uint8_t destOsc, uint8_t bit, uint16_t sourceBit);	
//void __attribute__(( noinline )) copyToggle(uint8_t mask, uint8_t osc);
//void togglePitchType();
//void __attribute__(( noinline )) updateSetGain(uint8_t firstOsc, uint8_t lastOsc);
//void  __attribute__(( noinline )) incrementGain(uint8_t gainInd, int8_t inc, uint8_t spreadInd);
void __attribute__(( noinline )) updateUINT8val(uint8_t *val, int8_t inc, uint8_t isToggle, uint8_t LCD);
void handleKnobs();
void __attribute__(( noinline )) updateSingleMod(uint8_t modType, uint8_t destParent, uint8_t sourceIndex);
void updateAllMod(uint8_t first, uint8_t last);
void __attribute__((noinline)) updateArpTime(uint8_t osc, float newBPM);
void __attribute__((noinline)) resetArpPages(uint8_t firstOsc, uint8_t lastOsc);
//void __attribute__(( noinline )) styleEdit(int8_t inc, uint8_t lcd);
uint8_t __attribute__(( noinline )) knobPos(uint8_t zeroKnob, uint8_t knobID);
uint8_t __attribute__(( noinline )) bounded(uint8_t val, int8_t inc, uint8_t min, uint8_t max);
void __attribute__(( noinline )) spreadNotes(uint8_t osc, uint8_t zeroNote);
int32_t __attribute__(( noinline )) randVal(int32_t max, uint8_t isPos);

#include "synthControls.c"

#endif 