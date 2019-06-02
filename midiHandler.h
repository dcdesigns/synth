#ifndef MIDIHANDLER_H
#define MIDIHANDLER_H


void __attribute__(( noinline )) initOscMidi(uint8_t firstOsc, uint8_t lastOsc);
//void  __attribute__(( noinline )) killEnvelopes(uint8_t first, uint8_t children);
//void  __attribute__(( noinline )) startEnvelopes(uint8_t first, uint8_t children, uint8_t doArp);
void  __attribute__(( noinline )) onEvent(uint8_t startOsc, uint8_t endOsc, uint8_t noteSlot, uint8_t force);
void  __attribute__(( noinline )) offEvent(uint8_t startOsc, uint8_t endOsc, uint8_t noteSlot, uint8_t force);
void addToNotesQueue(uint8_t status, uint8_t data1, uint8_t data2);
//void addToCCQueue(uint8_t status, uint8_t data1, uint8_t data2);
void handleNotes();
void handleCCs();
void __attribute__(( noinline )) applySpread(uint8_t osc, uint8_t firstChild);


//void addToGoals(uint32_t *obj, uint32_t target, uint8_t theMask, uint8_t isS);


#include "./midiHandler.c"

#endif 