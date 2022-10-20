#include <Bela.h>
#include <string.h>
#include "settings.h"
#include "synthStructs.h"
#include "synthVariables.h"
#include "helperFunctions.h"
#include "pitchTables.h"
#include "midiHandler.h"
#include "modules.h"

NOTE_SLOT notes[OSC_CNT][NOTES_CNT];
uint16_t onInd[OSC_CNT];
uint16_t offInd[OSC_CNT];
uint8_t lastActive[POLY_CNT];


void initOscMidi(uint8_t first, uint8_t last)
{
	//memset(wind_gain, 0, sizeof(wind_gain));
	memset(CC_vals, 0, sizeof(CC_vals));
	memset(susOn, 0, sizeof(susOn));
	
	for(uint8_t osc = first; osc <= last; osc++)
	{
		CC_vals[SUS_EVENT][osc] = (- 64)<<(FULL_SAT_SHIFT + 1);
		CC_vals[WND_EVENT][osc] = (- 64)<<(FULL_SAT_SHIFT + 1);
		
		onInd[osc] = RELEASE_OFFSET;
		offInd[osc] = 0;
		for(uint8_t note = 0; note < NOTES_CNT; note++)
		{
			notes[osc][note].priority = offInd[osc]++;
		}

		
	}
}

void  togglePolyMono(uint8_t startOsc, uint8_t endOsc)
{
	for(uint8_t osc = startOsc; osc <= endOsc; osc++)
	{
		uint8_t first = firstChild[osc];
		uint8_t children = childCnt[osc];
		
		//mono mode
		if(osc >= POLY_CNT || !SHIFTMASK(osc, bitPoly))
		{
			CLEARBIT(osc, bitPoly);
			
			uint8_t checkClear = 0;
			//check if we should 0 non active notes
			if(SHIFTMASK(osc, bitAEnv) && SHIFTMASK(osc, bitEnvs)) checkClear = 1;
			
			//find lowest note and highest vel
			uint8_t highV = vel[first];
			for(uint8_t child = 1; child < children; child++)
			{
				if(vel[first + child] > highV) highV = vel[first + child];
			}
			//LogTextMessage("%u", highV);
			monoPitch[osc] = lastActive[osc]<<PITCH_COARSE;
			monoVel[osc] = highV;
			for(uint8_t child = first; child < first + children; child++)
			{
				note[child] -= monoPitch[osc];
				if(checkClear && amp_env[child].stage > 2) vel[first + child] = -127;
				else vel[child] -= highV;// __SSAT(vel[first + child] - highV, 8);
				//LogTextMessage("%d, %d", note[first + child] + monoPitch[osc], vel[first + child] + monoVel[osc]);
			}
		}
		
		//poly mode
		else
		{
			for(uint8_t child = 0; child < children; child++)
			{
				note[first + child] += monoPitch[osc];
				vel[first + child] = __SSAT(vel[first + child] + monoVel[osc], 8);
				//vel[first + child] += monoVel[osc];
				//LogTextMessage("%d, %d", note[first + child], vel[first + child]);
			}
			monoPitch[osc] = 0;
			monoVel[osc] = 0;
		}
		
	}
}

void  onEvent(uint8_t startOsc, uint8_t endOsc, uint8_t noteSlot, uint8_t force)
{
	if(screenInd == NOTES) memset(&LCD_update[OBJ3], 1, 4);//updateLCDelems(OBJ3, OBJ6);
	for(uint8_t osc = startOsc; osc <= endOsc; osc++)
	{
		//apply notes if it wasn't a forced trigger and notes are tracked
		if(!force && SHIFTMASK(osc, bitNotes))
		{
			//if(osc == 1) LogTextMessage("o");
			//LogTextMessage("on start");
			//apply poly note
			if(SHIFTMASK(osc, bitPoly))
			{
				uint8_t child = firstChild[osc] + noteSlot;
					
				//if the child was dead, glide from the most recent note
				if(amp_env[child].val < 2000) kNote[child] = notes[osc][lastActive[osc]].pitch<<PITCH_COARSE;
				lastActive[osc] = notes[osc][noteSlot].pitch;	
				
				//apply the new note
				note[child] = notes[osc][noteSlot].pitch<<PITCH_COARSE;
				vel[child] = notes[osc][noteSlot].vel;
				//LogTextMessage("poly note");
			}
			
			//apply mono note
			else 
			{
				monoPitch[osc] = notes[osc][noteSlot].pitch<<PITCH_COARSE;
				monoVel[osc] = notes[osc][noteSlot].vel;
				//LogTextMessage("poly note");
			}
		}
				
		//apply envs
		if(force || SHIFTMASK(osc, bitEnvs))
		{
			uint8_t first = firstChild[osc];
			uint8_t children = childCnt[osc];
			if (!force && SHIFTMASK(osc, bitPoly) && children)
			{
				children = 1;
				first += noteSlot;
			}
			
			//uint8_t stage = (SHIFTMASK(osc, bitWind))? 1: 0;
			if(force || (!SHIFTMASK(osc, bitLgto) || amp_env[first].stage > 2))
			{
				
				for(uint8_t child = first; child < first + children; child++)
				{
					//LogTextMessage("trig on %u", first);
					//LogTextMessage("trig on osc %d slot %d val %u stage %d", osc, first, amp_env[child].val, amp_env[child].stage);
					amp_env[child].stage = 0;
					//amp_env[child].val = 0;
					pit_env[child].stage = 0;
					//pit_env[child].val = 0;
					pit_env[child].clock = 0;
					filt_env[child].stage = 0;
					//filt_env[child].val = 0;
					filt_env[child].clock = 0;	
					
					if(SHIFTMASK(osc, bitArpTrig))
					{
						arp_env[child].stage = MAXARP-1;
						arp_env[child].clock = 65534;
						/* arp_env[child].stage = 0;
						arp_env[child].atk_stage = 0;
						arp_env[child].clock = 0; */
					}
				}
			}
		}
	}
}
			

void  offEvent(uint8_t startOsc, uint8_t endOsc, uint8_t noteSlot, uint8_t force)
{
	if(screenInd == NOTES) memset(&LCD_update[OBJ3], 1, 4);//updateLCDelems(OBJ3, OBJ6);
	for(uint8_t osc = startOsc; osc <= endOsc; osc++)
	{
		if(force || (!SHIFTMASK(MAINTOG, bitHoldAll) && !SHIFTMASK(osc, bitHold) && !susOn[osc]))
		{
			
			uint8_t monoBest = 0;
			int32_t limit = note_slot_limit(osc);
			//mono note fallback
			if(!force && !SHIFTMASK(osc, bitPoly))
			{
				//LogTextMessage("off start");
				//find best note slot
				
				for(uint8_t i = 1; i < limit; ++i)
				{
					if(notes[osc][i].priority > notes[osc][monoBest].priority) monoBest = i;
				}
				if(notes[osc][monoBest].priority < offInd[osc]) monoBest = limit;
				
				//apply note slot if valid and notes are tracked
				if(monoBest < limit && SHIFTMASK(osc, bitNotes))
				{
					//LogTextMessage("best %u, %u", monoBest, notes[osc][monoBest].pitch);
					monoPitch[osc] = notes[osc][monoBest].pitch<<PITCH_COARSE;
					monoVel[osc] = notes[osc][monoBest].vel;
					//LogTextMessage("mon %u", monoPitch[osc]);// fallback");
				}
			}
			else monoBest = limit;
			
			//kill envelopes
			if(force || SHIFTMASK(osc, bitEnvs))
			{
				uint8_t first = firstChild[osc];
				uint8_t firstSlot;
				uint8_t lastSlot;
				if(force || noteSlot == ALL_SLOTS || !SHIFTMASK(osc, bitPoly))
				{
					firstSlot = 0;
					lastSlot = firstSlot + childCnt[osc];
				}
				else
				{
					firstSlot = noteSlot;
					lastSlot = noteSlot + 1;
				}
				
				for(uint8_t slot = firstSlot; slot < lastSlot; slot++)
				{
					if(force || (monoBest == limit && notes[osc][slot].priority < offInd[osc]))
					{
						uint32_t child = first + slot;
						//LogTextMessage("trig off %u", first+child);
						amp_env[child].stage = 3;
						pit_env[child].stage = FREE_STAGES + 1;
						filt_env[child].stage = FREE_STAGES + 1;
						
						if(force)
						{
							//wind_gain[osc] = 0;
							susOn[osc] = 0;
							notes[osc][slot].priority = offInd[osc]++;
						}
					}
				}
			}
			if (force)
			{
				initOscMidi(0, OSC_CNT - 1);
			}
		}
	}
}

	
void  addToNotesQueue(uint8_t status, uint8_t data1, uint8_t data2)
{
	static uint8_t writeNoteInd = 0;
	static uint8_t writeCCInd = 0;
	uint8_t type;
	uint8_t *ind;
	
	uint8_t upr = status & 0xF0;
	if(upr == 240 || upr == 192) return; 
	else if(upr == NOTEON || upr == NOTEOFF) {ind = &writeNoteInd; type = 0;}
	else { ind = &writeCCInd; type = 1;}

	if(midiEvents[type][*ind][0] != DEAD_MIDI_EVENT)
		rt_printf("m- %u: %u %u", upr, data1);
	midiEvents[type][*ind][0] = status;
	midiEvents[type][*ind][1] = data1;//__USAT(data1 + MIDI_OFF, 7);
	midiEvents[type][*ind][2] = data2;
	*ind = indexIncrement(*ind, 1, MIDI_QUEUE_SIZE);
}


void handleNotes()
{
	static uint8_t curNote = 0;
	while(midiEvents[0][curNote][0] != DEAD_MIDI_EVENT)
	{
		uint8_t type = midiEvents[0][curNote][0] & 0xF0;
		uint8_t chan = 1 + (midiEvents[0][curNote][0] & 0x0F);
		uint8_t curPitch = midiEvents[0][curNote][1];
		uint8_t curVel = midiEvents[0][curNote][2];
		//rt_printf("note event %u %u %u channel %u\n", type, curPitch, curVel, chan);
		
		//key on.................................................................................................
		if(type == NOTEON && curVel)
		{
			if(SHIFTMASK(MAINTOG, bitRecArp) && recNotes < MAXARP)
			{
				ARP_KNOBS *curArp = &arpeggio[oscInd];
				incArpRecTime();
				if(!recNotes) 
				{
					recCent = curPitch;	
					recLoudest = 0;
				}
				
				if(!recVel) curArp->V[recNotes] = 127;
				else
				{
					curArp->V[recNotes] = curVel;
					if(curVel > recLoudest) recLoudest = curVel;
				}
				curArp->P[recNotes] = curPitch - recCent;
				curArp->E[recNotes] = recEnv;
				recNotes++;
				LCD_update[OBJ6] = 1;
				//curLCD = OBJ6;
			}
			/* else if(SHIFTMASK(MAINTOG, bitRecEnv) && recNotes < FREE_STAGES + 2)
			{
				DUAL_ENV_KNOBS *curEnv = (screenInd == DUALENV1)? &pit_env_knobs[oscInd]: &filt_env_knobs[oscInd];
				curEnv->pitch[recNotes] = curPitch;
				incArpRecTime();
				recNotes++;
				LCD_update[OBJ4] = 1;
			} */
					

			
			for(uint8_t osc = 0; osc < OSC_CNT; osc++)
			{
				MIDI_PARAMS *curMidi = &midi_knobs[osc];
				uint8_t actVel = (SHIFTMASK(osc, bitKeyVel))? curVel : 127;
				int32_t limit = note_slot_limit(osc);
				//trigger the event for all osc that use this channel and key range
				if((!curMidi->chan || curMidi->chan == chan) && (curMidi->keyMax >= curPitch && curMidi->keyMin <= curPitch) && limit)
				{
					uint8_t lowestInd = 0;
					
					//find oldest slot
					for(uint8_t i = 1; i < limit; ++i)
					{
						if(notes[osc][i].priority < notes[osc][lowestInd].priority) lowestInd = i;
					}

					//update the slot
					notes[osc][lowestInd].priority = onInd[osc]++;
					notes[osc][lowestInd].pitch = curPitch;
					notes[osc][lowestInd].vel = actVel;
					//if (osc == 0) rt_printf("on slot %u vel %u\n", lowestInd, actVel);
					onEvent(osc, osc, lowestInd, 0);		
				}
			}
		}
		
		//key off.................................................................................................
		else
		{			
			
			for(uint8_t osc = 0; osc < OSC_CNT; osc++)
			{
				MIDI_PARAMS *curMidi = &midi_knobs[osc];
				if ((!curMidi->chan || curMidi->chan == chan) && (curPitch <= curMidi->keyMax && curPitch >= curMidi->keyMin))
				{
					int8_t bestMatch = -1;
					uint32_t limit = note_slot_limit(osc);

					//find newest matching note
					for (uint8_t i = 0; i < limit; ++i)
					{
						if (notes[osc][i].pitch == curPitch && (bestMatch == -1 || notes[osc][i].priority > notes[osc][bestMatch].priority)) bestMatch = i;
					}

					//only do stuff if the note was found and it's still active
					if (bestMatch > -1 && notes[osc][bestMatch].priority > offInd[osc])
					{

						notes[osc][bestMatch].priority = offInd[osc]++;
						//if (osc == 0) rt_printf("off slot %u\n", bestMatch);
						offEvent(osc, osc, bestMatch, 0);
					}
				}
			}		
		}
		midiEvents[0][curNote][0] = DEAD_MIDI_EVENT;
		curNote = indexIncrement(curNote, 1, MIDI_QUEUE_SIZE);

	}

	
}


void handleCCs()
{
	static uint8_t curCC = 0;
	while(midiEvents[1][curCC][0] != DEAD_MIDI_EVENT)
	{
		 
		
 		uint8_t chan = midiEvents[1][curCC][0] & 0x0F;
		uint8_t cmd = midiEvents[1][curCC][0] & 0xF0;
		int32_t num = midiEvents[1][curCC][1];
		uint8_t val = midiEvents[1][curCC][2];
		
		//rt_printf("CC event %u %u %u channel %u", cmd, num, val, chan);
		//int32_t gain = ___SMMUL((midiEvents[1][curCC].val - 64)<<PITCH_COARSE, GAIN[CC_gains[modType]])<<1;
		

		for(uint8_t osc = 0; osc < OSC_CNT; osc++)
		{
			MIDI_PARAMS *curMidi = &midi_knobs[osc];
			if(!curMidi->chan || curMidi->chan == chan)
			{
				int8_t modType = -1;
				//pitch bend
				if(cmd == PB_CMD) modType = PB_EVENT;

				//all other cc's
				else if(cmd == CC_CMD)
				{
					//wind
					if(num == midi_knobs[osc].CC_nums[WND_EVENT])
					{
						CC_vals[WND_EVENT][osc] = VELGAIN[val];
						//if(osc == 0) LogTextMessage("cc %u %d %d", cmd, num, val);
						//LogTextMessage("w %u", val);
					}
					
					//mod wheel
					else if(num == midi_knobs[osc].CC_nums[MW_EVENT])
					{
						modType = MW_EVENT;
					}
					
					//sustain
					else if(num == SUS_CC)
					{
						modType = SUS_EVENT;
						uint8_t oldSus = susOn[osc];
						susOn[osc] = (val > SUS_THRESH)? 1: 0;
						
						//sus dropped below threshold, kill unheld notes
						if(oldSus && !susOn[osc]) offEvent(osc, osc, ALL_SLOTS, 0);
					}
				}
				
				//update the generic mod value
				if(modType != -1)
				{
					CC_vals[modType][osc] = (val - 64)<<(FULL_SAT_SHIFT + 1);
					//if(osc == 0) LogTextMessage("cc %u %d %d", cmd, num, CC_vals[modType][osc]);
				}
				//___SMMUL((val - 64)<<(FULL_SAT_SHIFT + 1), GAIN[midi_knobs[osc].CC_levs[modType]])<<1;
			} 
		} 							
		midiEvents[1][curCC][0] = DEAD_MIDI_EVENT;
		curCC = indexIncrement(curCC, 1, MIDI_QUEUE_SIZE);
	}
} 



