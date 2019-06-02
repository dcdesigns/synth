#ifndef HELPERFUNCTIONS_C
#define HELPERFUNCTIONS_C

#include "./pitchTables.c"
#include "./synthVariables.c"

const float floatMults[9] = {10000,1000,100,10,1,.1,.01,.001,.0001};

uint8_t __attribute__( ( noinline ) )  indexIncrement(uint8_t cur, int8_t inc, uint8_t cnt)
{
	
	int8_t tInd = cur + inc;
	int8_t max = cnt -1;
	if(tInd > max)
		return 0;
	else if(tInd < 0)
		return max;
	else
		return tInd;
}

void __attribute__(( noinline )) incArpRecTime()
{
	static uint32_t lastTime;
	static uint8_t shortTimeCnt;
	static uint32_t shortTimeSum;
	if(!recNotes)
	{
		recFullTime = 0;
		recShortest = 50000;
		shortTimeCnt = 1;
		//LogTextMessage("first");
	}
	else
	{
		int16_t time = ticks - lastTime;
		recTimes[recNotes-1] = time;
		//LogTextMessage("time %u, shortest %u", time, recShortest);
		uint8_t addIt = 0;
		
		if((int32_t)(time - recShortest) < 0) 
		{
			
			addIt = 1;
			if(time + (time>>1) < recShortest)
			{
				shortTimeCnt = 0;
				shortTimeSum = 0;
			}
		}
		else if(recShortest + (recShortest>>1) > time) addIt = 1;
		
		if(addIt)
		{
			shortTimeCnt++;
			shortTimeSum+=time;
			recShortest = shortTimeSum/shortTimeCnt;
		}
		
		//LogTextMessage("note %u prevTime %u shorts %u avg %u", recNotes, recTimes[recNotes-1], shortTimeCnt, recShortest);
		recFullTime += time;
	}
	lastTime = ticks;
}

uint8_t childCnt(uint8_t osc)
{
	return (osc < POLY_CNT)? NOTES_CNT: 1;
}




void __attribute__(( noinline )) floatToStr(float num, uint8_t leading, uint8_t decimals, uint8_t leadingSign, char *str)
{
	if(decimals > 3) decimals = 3;
	if(leading > 5) leading = 5;
	float rem = num;
	uint8_t digits[9] = {0};
	char temp[2];
	temp[1] = '\0';
	//float val;
	int8_t firstDigit = 5 - leading;
	uint8_t lastDigit = 5 + decimals;
	
	if(leadingSign > 0)
	{
		temp[0] = (rem >= 0)? '+' : '-';
		strcat(str, temp);
	}
	
	
	if(rem < 0)
		rem = -rem;
	
	
	for(uint8_t i = firstDigit; i <= lastDigit; i++)
	{
		/* switch(i)
		{
			case 0: val = 10000; break;
			case 1: val = 1000; break;
			case 2: val = 100; break;
			case 3: val = 10; break;
			case 4: val = 1; break;
			case 5: val = .1; break;
			case 6: val = .01; break;
			case 7: val = .001; break;
			case 8: val = .0001; break;
		}
		 */
		while(rem >= floatMults[i])
		{
			rem -= floatMults[i];
			digits[i]++;
		}
	}
	
/* 	if(digits[lastDigit] >= 5) 
	{
		digits[lastDigit-1]++;
		if(digits[lastDigit-1] > 9)
		{
			digi */
	
	//LogTextMessage("%u %u %u %s %d", digits[0], digits[1], digits[2], str, rem);
	for(uint8_t i = firstDigit; i < lastDigit; i++)
	{
		if(i == 5) strcat(str, ".");
		temp[0] = digits[i] + '0';
		strcat(str, temp);
		
	}
	//LogTextMessage("float %f, str %s",num, str);
	

}







/* uint32_t __attribute__( ( always_inline ) ) modToUnsigned(int32_t *in)
{
	return ((uint32_t)(*in ^ 0x80000000))>>1;
} */


uint32_t __attribute__(( noinline )) MASK(uint8_t osc, uint8_t bit)
{
	return toggles[osc] & (1 << bit);
}

uint8_t __attribute__(( noinline ))  SHIFTMASK(uint8_t osc, uint8_t bit)
{
	return (toggles[osc] >> bit) & 1;
}

void __attribute__(( noinline ))  SETBIT(uint8_t osc, uint8_t bit)
{
	toggles[osc] |= (1 << bit);
}

void  __attribute__(( noinline )) SETBITS(uint8_t osc, uint8_t bits[], uint8_t cnt)
{
	for(uint8_t i = 0; i < cnt; i++) SETBIT(osc, bits[i]);
}

void __attribute__(( noinline ))  CLEARBIT(uint8_t osc, uint8_t bit)
{
	toggles[osc] &= ~(1 << bit);
}

void __attribute__(( noinline ))  TOGGLEBIT(uint8_t osc, uint8_t bit)
{
	toggles[osc] ^= (1 << bit);
}

void  __attribute__(( noinline )) COPYBIT(uint8_t osc, uint8_t bit)
{
	CLEARBIT(osc, bit);
	toggles[osc] |= MASK(oscInd, bit);
}

/* void  __attribute__(( noinline )) COPYBITS(uint8_t osc, uint8_t bits[])
{
	uint32_t mask = 0;
	for(uint8_t i = 0; i < sizeof(bits); i++)
	{
		mask |= (1 << bits[i]);
	}
	toggles[osc] &= ~mask;
	toggles[osc] |= (mask & toggles[oscInd]);

} */

uint32_t  __attribute__(( noinline )) FULLMASK(uint8_t osc, uint8_t bit)
{
	return (MASK(osc, bit))? ~0: 0;
}
	

/* int32_t BOUNDED_MOD(int32_t base, int32_t mod, int32_t max, int32_t min)
{
	int32_t sum = base + mod;
	if(sum > max || (sum < 0 && base > 0 && mod > 0)) return max;
	else if(sum < min || (sum > 0 && base < 0 && mod < 0)) return min;
	else return sum;
} */

/* int32_t  __attribute__(( noinline )) ARPSPEEDMOD(uint16_t base, int32_t mod)
{
	int32_t sum = -(mod>>18) + base;
	if(sum > UINT16_MAX) return UINT16_MAX;
	else if(sum < 15) return 15;
	else return sum;
}
 */


#endif 