#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

__attribute__((always_inline)) int32_t ___SMMUL(int32_t op1, int32_t op2);
__attribute__((always_inline)) int32_t ___SMMLA(int32_t op1, int32_t op2, int32_t op3);
__attribute__((always_inline)) int32_t ___SMMLS(int32_t op1, int32_t op2, int32_t op3);
__attribute__((always_inline)) float q27_to_float(int32_t op1);
__attribute__((always_inline)) float q31_to_float(int32_t op1);
int32_t __SSAT(int32_t val, uint32_t bits);
uint32_t __USAT(int32_t val, int bits);
uint32_t __USAT_add(uint32_t a, uint32_t b);
int32_t __USAT_add_signed(int32_t a, int32_t b, uint32_t saturate_bit);
uint32_t __USAT_add_unsigned_signed(uint32_t a, int32_t b);
int32_t signed_rand();
int getOscTblInd(int32_t val, int32_t& osc, int32_t& table);
void queueOscTbl(uint32_t& queue, int32_t osc, int32_t table = -1);
void memcpy_safe(uint8_t* dst, uint8_t* src, int32_t num_bytes);
int32_t unit_dir(int32_t inc);
int32_t ensure_not_self_ratio(int32_t inc);
uint8_t __attribute__( ( noinline ) )  indexIncrement(uint32_t cur, int32_t inc, uint32_t cnt);
void __attribute__(( noinline )) incArpRecTime();
void __attribute__(( noinline )) floatToStr(float num, uint8_t leading, uint8_t decimals, uint8_t leadingSign, char *str);
uint32_t __attribute__(( noinline )) MASK(uint8_t osc, uint8_t bit);
uint8_t __attribute__(( noinline ))  SHIFTMASK(uint8_t osc, uint8_t bit);
void __attribute__(( noinline ))  SETBIT(uint8_t osc, uint8_t bit);
void  __attribute__(( noinline )) SETBITS(uint8_t osc, uint8_t bits[], uint8_t cnt);
void __attribute__(( noinline ))  CLEARBIT(uint8_t osc, uint8_t bit);
void __attribute__(( noinline ))  TOGGLEBIT(uint8_t osc, uint8_t bit);
void  __attribute__(( noinline )) COPYBIT(uint8_t osc, uint8_t bit);
uint32_t  __attribute__(( noinline )) FULLMASK(uint8_t osc, uint8_t bit);
int32_t perTableTog(int32_t toggle);
void toggle_POLY16();
int32_t note_slot_limit(int32_t osc);

#endif //HELPERFUNCTIONS_H