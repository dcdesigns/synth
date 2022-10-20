
#include <string.h>
#include <string>
#include "pitchTables.h"
#include "synthVariables.h"
#include "helperFunctions.h"

const float floatMults[9] = {10000,1000,100,10,1,.1,.01,.001,.0001};


__attribute__((always_inline)) int32_t ___SMMUL(int32_t op1, int32_t op2)
{
	int32_t result;

	__asm__ __volatile__("smmul %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2));
	return(result);
}

__attribute__((always_inline)) int32_t ___SMMLA(int32_t op1, int32_t op2, int32_t op3)
{
	int32_t result;

	__asm__ __volatile__("smmla %0, %1, %2, %3" : "=r" (result) : "r" (op1), "r" (op2), "r" (op3));
	return(result);
}

__attribute__((always_inline)) int32_t ___SMMLS(int32_t op1, int32_t op2, int32_t op3)
{
	int32_t result;

	__asm__ __volatile__("smmls %0, %1, %2, %3" : "=r" (result) : "r" (op1), "r" (op2), "r" (op3));
	return(result);
}

__attribute__((always_inline)) float q27_to_float(int32_t op1) {
	float fop1 = *(float*)(&op1);
	__asm__ __volatile__("VCVT.F32.S32 %0, %0, 27" : "+w" (fop1));
	return(fop1);
}

__attribute__((always_inline)) float q31_to_float(int32_t op1) {
	float fop1 = *(float*)(&op1);
	__asm__ __volatile__("VCVT.F32.S32 %0, %0, 31" : "+w" (fop1));
	return(fop1);
}

int32_t signed_rand()
{
	return (((int32_t)rand()) - (INT_MAX >> 1)) << 1;
}
uint32_t __USAT(int32_t val, int bits)
{
	uint32_t comp = (1 << bits) - 1;
	return val < 0 ? 0 : val > comp ? comp : val;
}

uint32_t __USAT_add(uint32_t a, uint32_t b)
{
	uint32_t ret = a + b;
	if (ret < a || ret < b)
	{
		ret = 0xFFFFFFFF;
	}
	return ret;
}

int32_t __USAT_add_signed(int32_t a, int32_t b, uint32_t saturate_bit)
{
	int32_t ret;
	//both negative
	if (a < 0 && b < 0)
	{
		ret = 0;
	}
	//one negative
	else if (a < 0 || b < 0)
	{
		ret = a + b;
		if (ret < 0) ret = 0;
	}
	//both positive or zero
	else
	{
		uint32_t sum = (uint32_t)a + (uint32_t)b;
		if (sum > MAX_INT32) ret = MAX_INT32;
		else ret = sum;
	}
	
	return __USAT(ret, saturate_bit);
}

uint32_t __USAT_add_unsigned_signed(uint32_t a, int32_t b)
{
	if (b < 0)
	{
		if (-b > a) return 0;
		return a - b;
	}
	return __USAT_add(a, (uint32_t)b);
}

int32_t __SSAT(int32_t val, uint32_t bits)
{
	int32_t posMax, negMin;
	uint32_t i;

	posMax = 1;
	for (i = 0; i < (bits - 1); i++)
	{
		posMax = posMax * 2;
	}

	if (val > 0)
	{
		posMax = (posMax - 1);

		if (val > posMax)
		{
			val = posMax;
		}
	}
	else
	{
		negMin = -posMax;

		if (val < negMin)
		{
			val = negMin;
		}
	}
	return (val);
}

int getOscTblInd(int32_t val, int32_t& osc, int32_t& table)
{
	if (val >= MAIN_FIL) return 0;
	osc = val / TABLE_CNT;
	table = val % TABLE_CNT;
	return 1;
}

void queueOscTbl(uint32_t& queue, int32_t osc, int32_t table)
{
	if (table > -1) queue |= (1 << (osc * TABLE_CNT + table));
	else queue |= (0xF << (osc * TABLE_CNT));
}

void memcpy_safe(uint8_t* dst, uint8_t* src, int32_t num_bytes)
{
	const int32_t MAX_BYTES = 256;

	while (num_bytes > 0)
	{
		int32_t amt = num_bytes > MAX_BYTES ? MAX_BYTES : num_bytes;
		memcpy(dst, src, amt);
		num_bytes -= amt;
		dst += amt;
		src += amt;
	}
}

//int32_t __SSAT(int32_t val, int bits)
//{
//	int32_t comp = (1 << bits) - 1;
//	return val < 0 ? (val < -comp ? -comp : val) : (val > comp ? comp : val);
//}

int32_t unit_dir(int32_t inc)
{
	return inc > 0 ? 1 : inc < 0 ? -1 : 0;
}

int32_t ensure_not_self_ratio(int32_t inc)
{
	int32_t ret = 0;
	if (pit_ratio[oscInd].src == oscInd)
	{
		inc = unit_dir(inc);
		pit_ratio[oscInd].src = indexIncrement(pit_ratio[oscInd].src, inc, 6);
		ret = 1;

	}
	LCD_update[OBJ2] = 1;
	return ret;
}

uint8_t __attribute__( ( noinline ) )  indexIncrement(uint32_t cur, int32_t inc, uint32_t cnt)
{
	
	int32_t tInd = cur + inc;
	int32_t max = cnt -1;
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
	
	
	for(uint8_t i = firstDigit; i <= lastDigit; ++i)
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
	int32_t had_val = 0;
	for(uint8_t i = firstDigit; i < lastDigit; ++i)
	{
		if(!had_val && i < 5 && !digits[i]) temp[0] = ' ';
		else
		{
			had_val = 1;
			if(i == 5) strcat(str, ".");
			temp[0] = digits[i] + '0';	
		}
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
	for(uint8_t i = 0; i < cnt; ++i) SETBIT(osc, bits[i]);
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

int32_t perTableTog(int32_t toggle)
{
	return (toggle == bitHarms || toggle == bitWave || toggle == bitPhase);
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

void toggle_POLY16()
{
	int32_t on = SHIFTMASK(MAINTOG, bitPoly16);
	rt_printf("poly 16: %d\n", on);
	int32_t POLY2_LIMIT = NOTES_CNT << 1;
	for (int32_t child = NOTES_CNT; child < POLY2_LIMIT; ++child)
	{
		parents[child] = on ? 0 : 1;
	}
	if (on)
	{
		childCnt[0] = POLY2_LIMIT;
		childCnt[1] = 0;
		untilChild[0] = POLY2_LIMIT;
		untilChild[1] = 0;
	}
	else
	{
		childCnt[0] = NOTES_CNT;
		childCnt[1] = NOTES_CNT;
		untilChild[0] = NOTES_CNT;
		untilChild[1] = POLY2_LIMIT;
	}
}

int32_t note_slot_limit(int32_t osc)
{
	return (osc < POLY_CNT) ? childCnt[osc] : NOTES_CNT;
}