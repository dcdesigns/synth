#ifndef LCDLIB_H
#define LCDLIB_H

#include <Bela.h>
#include <I2c.h>

//constants
#define mainAddr 0x3C 
#define graphAddr 0x3D
#define I2C_PATH "/dev/i2c-1"

#define maxBytes 128
#define MAIN_COLS 128
#define MAIN_ROWS 64
#define GRAPH_COLS 128
#define GRAPH_SECTORS (128 >> 1)
#define GRAPH_ROWS 128

#define MICRODELAY 1

#define COL_OFFSET (0)
#define CTRL_DATA (0x40)
#define CTRL_CMD (0x00)
#define SET_CONTRAST 0x81
#define SET_NORM_INV (0xa6 | 0)
#define SET_DISP 0xae
#define SET_SCAN_DIR 0xc0
#define SET_SEG_REMAP 0xa0
#define LOW_COLUMN_ADDRESS 0x00
#define HIGH_COLUMN_ADDRESS 0x10
#define SET_PAGE_ADDRESS 0xB0
#define SET_START_LINE (0x40 | 0)
#define SET_OFFSET_A 0xD3
#define SET_OFFSET_B 0x00
#define SET_RATION 0xA8
#define SET_NORMAL (0xA6 | 0)
#define SET_SCAN 0xC8
#define SET_CONTRAST_A 0x81
#define SET_CONTRAST_B 0xFF
#define SET_REMAP (0xA0 | 1)
#define SET_FULL_ON 0xA4

#define SPACE_COLS (3)
#define MAX_SPACE_COLS (8)


#define SCAN_DIR 0xc0
#define RE_MAP 0xa0

#define flip_scan 0x08
#define flip_map 0x01


#define SSD1327_BLACK 0x0
#define SSD1327_WHITE 0xF

#define SSD1327_I2C_ADDRESS 0x3D

#define SSD1305_SETBRIGHTNESS 0x82

#define SSD1327_SETCOLUMN 0x15

#define SSD1327_SETROW 0x75

#define SSD1327_SETCONTRAST 0x81

#define SSD1305_SETLUT 0x91

#define SSD1327_SEGREMAP 0xA0
#define SSD1327_SETSTARTLINE 0xA1
#define SSD1327_SETDISPLAYOFFSET 0xA2
#define SSD1327_NORMALDISPLAY 0xA4
#define SSD1327_DISPLAYALLON 0xA5
#define SSD1327_DISPLAYALLOFF 0xA6
#define SSD1327_INVERTDISPLAY 0xA7
#define SSD1327_SETMULTIPLEX 0xA8
#define SSD1327_REGULATOR 0xAB
#define SSD1327_DISPLAYOFF 0xAE
#define SSD1327_DISPLAYON 0xAF

#define SSD1327_PHASELEN 0xB1
#define SSD1327_DCLK 0xB3
#define SSD1327_PRECHARGE2 0xB6
#define SSD1327_GRAYTABLE 0xB8
#define SSD1327_PRECHARGE 0xBC
#define SSD1327_SETVCOM 0xBE

#define SSD1327_FUNCSELB 0xD5

#define SSD1327_CMDLOCK 0xFD


//structs
struct LCDelem //struct to hold positioning/length of each element
{		
	uint8_t row;
	uint8_t col;
	uint8_t len;
};

extern uint64_t mainBuff[MAIN_COLS];

//user-friendly function declarations
int initLCD(); //set everything up, write the titles to the screen
void writeStr(const char* str, uint8_t row, uint8_t col, uint8_t len, uint8_t align_right = 0);
void setCursorPosition(uint8_t row, uint8_t col);
void toggleBlinkCursor(uint8_t on);										
void toggleDisplay(uint8_t on);											
void clearDisplay();
void updateLCDelems(uint8_t first, uint8_t last);
void checkWriteElem();
void updateGraphic();
void redrawMain();
void drawMainPortion(uint8_t s_row, uint8_t e_row, uint8_t s_col, uint8_t e_col);
void writeMainStr(const char* str, int32_t row, int32_t startCol, int32_t endCol, int32_t chrRow, int32_t chrCol, int32_t chrLen, int32_t space_cols = SPACE_COLS, int32_t align_right = 0);
void redrawGraphic();
void drawGraphicPortion(uint8_t s_row, uint8_t e_row, uint8_t s_col, uint8_t e_col);
void drawGraphicLine(uint8_t clear, uint8_t overwrite, uint8_t gray_val, int8_t row, int8_t col, int full_screen);

//lower-level function declarations
int setAddress(int address);
int sendBytes(int addr, const void *buf, size_t count);
int sendCmd(int addr, uint8_t byte);
int sendMainCmd(uint8_t byte);
int sendMainBytes(const void *buf, size_t count);
int sendGraphicCmd(uint8_t byte);
int sendGraphBytes(const void *buf, size_t count);

void setGrayBuffVals(uint8_t val);
void setGraphicPos(uint8_t s_row, uint8_t s_sect, uint8_t e_row = GRAPH_ROWS - 1, uint8_t e_sect = GRAPH_SECTORS - 1);

void pitchStr(char *str, int32_t pitch, uint8_t isNum, uint8_t center, int32_t note);
void pitchNumtoStr(int16_t num, char *str);
void strCatCat(const char *cat1, const char *cat2, char *str);
void arpEnvStr(uint8_t env, char *str);
void intToStr(const char *before, int16_t num, uint8_t digitsOut, int16_t center, uint8_t leadingSign, char *str);
void writeBasicInt(int16_t num, uint8_t digits, uint8_t leadingSign, uint8_t row, uint8_t col);
void writeNumSpaceStr(uint8_t num, char *str, uint8_t row, uint8_t col, uint8_t len);



#endif 