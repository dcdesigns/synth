#ifndef MIDIHANDLER_H
#define MIDIHANDLER_H


void initOscMidi(uint8_t firstOsc, uint8_t lastOsc);
//void  killEnvelopes(uint8_t first, uint8_t children);
//void  startEnvelopes(uint8_t first, uint8_t children, uint8_t doArp);
void  onEvent(uint8_t startOsc, uint8_t endOsc, uint8_t noteSlot, uint8_t force);
void  offEvent(uint8_t startOsc, uint8_t endOsc, uint8_t noteSlot, uint8_t force);
void addToNotesQueue(uint8_t status, uint8_t data1, uint8_t data2);
//void addToCCQueue(uint8_t status, uint8_t data1, uint8_t data2);
void handleNotes();
void handleCCs();
void  togglePolyMono(uint8_t startOsc, uint8_t endOsc);
//void applySpread(uint8_t osc, uint8_t firstChild);


//void addToGoals(uint32_t *obj, uint32_t target, uint8_t theMask, uint8_t isS);

#endif 