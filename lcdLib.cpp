#include <Bela.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include "fileLoader.h"
#include "settings.h"
#include "synthVariables.h"
#include "lcdLib.h"
#include "pitchTables.h"
#include "helperFunctions.h"
#include "modules.h"

static int i2C_file;
uint64_t mainBuff[MAIN_COLS];
uint8_t graphBuff[GRAPH_ROWS][GRAPH_SECTORS];
static uint8_t i2cWritePtr[maxBytes + 1];									
static uint8_t *readByte;
static int last_addr;
int8_t vals[2][12000] = {0};



const uint8_t FIRST_ROW = 2;
const uint8_t ROW_LINES = 11;
const uint8_t SECOND_ROW = FIRST_ROW + ROW_LINES;
const uint8_t THIRD_ROW = SECOND_ROW + ROW_LINES;
const uint8_t FOURTH_ROW = THIRD_ROW + ROW_LINES;
const uint8_t FIFTH_ROW = FOURTH_ROW + ROW_LINES;
const uint8_t SIXTH_ROW = FIFTH_ROW + ROW_LINES;

#define ACT_COL(text_col) text_col * 6

const int32_t ASC_MIN = 33;
const int32_t ASC_MAX = 126;
const int32_t CHAR_W = 6;
const int32_t CHAR_H = 8;
const uint8_t spaces[CHAR_W + 1] = { 0 };
const uint8_t ascii_map[94][CHAR_W + 1] = {
		{5, 0x00, 0x00, 0x5f, 0x00, 0x00, 0x00},
	{5, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00},
	{5, 0x14, 0x7f, 0x14, 0x7f, 0x14, 0x00},
	{5, 0x6e, 0x4a, 0xff, 0x4a, 0x7a, 0x00},
	{5, 0x23, 0x13, 0x08, 0x64, 0x62, 0x00},
	{5, 0x36, 0x49, 0x55, 0x22, 0x50, 0x00},
	{5, 0x00, 0x01, 0x06, 0x00, 0x00, 0x00},
	{5, 0x00, 0x00, 0x3e, 0x41, 0x00, 0x00},
	{5, 0x00, 0x41, 0x3e, 0x00, 0x00, 0x00},
	{5, 0x14, 0x08, 0x3e, 0x08, 0x14, 0x00},
	{5, 0x08, 0x08, 0x3e, 0x08, 0x08, 0x00},
	{5, 0x00, 0x80, 0x60, 0x00, 0x00, 0x00},
	{5, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00},
	{5, 0x00, 0x60, 0x60, 0x00, 0x00, 0x00},
	{5, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00},
	{5, 0x3e, 0x51, 0x49, 0x45, 0x3e, 0x00},
	{5, 0x00, 0x42, 0x7f, 0x40, 0x00, 0x00},
	{5, 0x00, 0x61, 0x51, 0x49, 0x46, 0x00},
	{5, 0x00, 0x41, 0x49, 0x49, 0x36, 0x00},
	{5, 0x18, 0x14, 0x12, 0x7f, 0x10, 0x00},
	{5, 0x27, 0x45, 0x45, 0x45, 0x39, 0x00},
	{5, 0x3c, 0x4a, 0x49, 0x49, 0x30, 0x00},
	{5, 0x01, 0x01, 0x71, 0x09, 0x07, 0x00},
	{5, 0x36, 0x49, 0x49, 0x49, 0x36, 0x00},
	{5, 0x06, 0x49, 0x49, 0x29, 0x1e, 0x00},
	{5, 0x00, 0x6c, 0x6c, 0x00, 0x00, 0x00},
	{5, 0x00, 0x8c, 0x6c, 0x00, 0x00, 0x00},
	{5, 0x1c, 0x22, 0x41, 0x00, 0x00, 0x00},
	{5, 0x00, 0x14, 0x14, 0x14, 0x14, 0x00},
	{5, 0x41, 0x22, 0x1c, 0x00, 0x00, 0x00},
	{5, 0x06, 0x01, 0x51, 0x09, 0x06, 0x00},
	{5, 0x32, 0x49, 0x79, 0x41, 0x3e, 0x00},
	{5, 0x7e, 0x09, 0x09, 0x09, 0x7e, 0x00},
	{5, 0x7f, 0x49, 0x49, 0x4e, 0x30, 0x00},
	{5, 0x3e, 0x41, 0x41, 0x41, 0x22, 0x00},
	{5, 0x7f, 0x41, 0x41, 0x42, 0x3c, 0x00},
	{5, 0x7f, 0x49, 0x49, 0x41, 0x00, 0x00},
	{5, 0x7f, 0x09, 0x09, 0x01, 0x00, 0x00},
	{5, 0x3e, 0x41, 0x49, 0x49, 0x78, 0x00},
	{5, 0x7f, 0x08, 0x08, 0x08, 0x7f, 0x00},
	{5, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00},
	{5, 0x30, 0x41, 0x41, 0x41, 0x3f, 0x00},
	{5, 0x7f, 0x08, 0x14, 0x22, 0x41, 0x00},
	{5, 0x7f, 0x40, 0x40, 0x40, 0x00, 0x00},
	{5, 0x7f, 0x04, 0x08, 0x04, 0x7f, 0x00},
	{5, 0x7f, 0x04, 0x08, 0x10, 0x7f, 0x00},
	{5, 0x3e, 0x41, 0x41, 0x41, 0x3e, 0x00},
	{5, 0x7f, 0x09, 0x09, 0x09, 0x06, 0x00},
	{5, 0x3e, 0x41, 0x51, 0x21, 0x5e, 0x00},
	{5, 0x7f, 0x09, 0x19, 0x29, 0x46, 0x00},
	{5, 0x46, 0x49, 0x49, 0x49, 0x30, 0x00},
	{5, 0x01, 0x01, 0x7f, 0x01, 0x01, 0x00},
	{5, 0x3f, 0x40, 0x40, 0x40, 0x3f, 0x00},
	{5, 0x1f, 0x20, 0x40, 0x20, 0x1f, 0x00},
	{5, 0x7f, 0x20, 0x10, 0x20, 0x7f, 0x00},
	{5, 0x63, 0x14, 0x08, 0x14, 0x63, 0x00},
	{5, 0x07, 0x08, 0x70, 0x08, 0x07, 0x00},
	{5, 0x61, 0x51, 0x49, 0x45, 0x43, 0x00},
	{5, 0x00, 0x00, 0x7f, 0x41, 0x00, 0x00},
	{5, 0x02, 0x04, 0x08, 0x10, 0x20, 0x00},
	{5, 0x00, 0x41, 0x7f, 0x00, 0x00, 0x00},
	{5, 0x04, 0x02, 0x01, 0x02, 0x04, 0x00},
	{5, 0x00, 0x40, 0x40, 0x40, 0x40, 0x00},
	{5, 0x00, 0x01, 0x02, 0x04, 0x00, 0x00},
	{5, 0x00, 0x70, 0x54, 0x54, 0x7c, 0x00},
	{5, 0x7f, 0x44, 0x44, 0x38, 0x00, 0x00},
	{5, 0x38, 0x44, 0x44, 0x44, 0x00, 0x00},
	{5, 0x00, 0x38, 0x44, 0x44, 0x7f, 0x00},
	{5, 0x38, 0x54, 0x54, 0x58, 0x00, 0x00},
	{5, 0x00, 0x08, 0x7e, 0x09, 0x02, 0x00},
	{5, 0x98, 0xa4, 0xa4, 0x78, 0x00, 0x00},
	{5, 0x7f, 0x04, 0x04, 0x78, 0x00, 0x00},
	{5, 0x00, 0x00, 0x7a, 0x00, 0x00, 0x00},
	{5, 0x20, 0x40, 0x40, 0x3a, 0x00, 0x00},
	{5, 0x7e, 0x10, 0x28, 0x44, 0x00, 0x00},
	{5, 0x00, 0x00, 0x7f, 0x40, 0x00, 0x00},
	{5, 0x78, 0x0c, 0x18, 0x0c, 0x78, 0x00},
	{5, 0x78, 0x04, 0x04, 0x78, 0x00, 0x00},
	{5, 0x38, 0x44, 0x44, 0x38, 0x00, 0x00},
	{5, 0xfc, 0x24, 0x24, 0x18, 0x00, 0x00},
	{5, 0x18, 0x24, 0x24, 0xfc, 0x80, 0x00},
	{5, 0x00, 0x7c, 0x08, 0x04, 0x00, 0x00},
	{5, 0x00, 0x48, 0x54, 0x54, 0x24, 0x00},
	{5, 0x00, 0x08, 0x7e, 0x48, 0x00, 0x00},
	{5, 0x3c, 0x40, 0x40, 0x7c, 0x00, 0x00},
	{5, 0x1c, 0x20, 0x40, 0x20, 0x1c, 0x00},
	{5, 0x7c, 0x20, 0x10, 0x20, 0x7c, 0x00},
	{5, 0x44, 0x28, 0x10, 0x28, 0x44, 0x00},
	{5, 0x00, 0x9c, 0xa0, 0xa0, 0x7c, 0x00},
	{5, 0x00, 0x64, 0x54, 0x4c, 0x00, 0x00},
	{5, 0x00, 0x08, 0x36, 0x41, 0x00, 0x00},
	{5, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00},
	{5, 0x00, 0x41, 0x36, 0x08, 0x00, 0x00},
	{5, 0x08, 0x04, 0x08, 0x10, 0x08, 0x00}
};



//global variables
//LCDelem LCD_layout[ENCODERS + 3];


//higher level functions
int initLCD()
{	
	usleep(1000000);
	last_addr = 0xFF;
	i2C_file = -1;

	// open I2C device as a file
	//rt_printf("setting path %s\n", I2C_PATH);
	if ((i2C_file = open(I2C_PATH, O_RDWR)) < 0)
	{
		rt_printf("Failed to open %s I2C Bus\n", I2C_PATH);
		return(1);
	}
	
	// Init sequence for 128x64 OLED module
	static const uint8_t initMain[] = {
		CTRL_CMD,
		SCAN_DIR | flip_scan,
		RE_MAP | flip_map,
		SET_DISP | 1,
		SET_CONTRAST, 8
	};
	sendMainBytes(initMain, sizeof(initMain));

	/*i2cWritePtr[0] = CTRL_CMD;
	sendGraphBytes(i2cWritePtr, 1);*/

	// Init sequence for 128x128 OLED module
	static const uint8_t init_128x128[] = {	
		CTRL_CMD,
		SSD1327_DISPLAYOFF, // 0xAE
		SSD1327_SETCONTRAST, 0x80,    // 0x81, 0x80
		SSD1327_SEGREMAP, 0x53, //0x40, //0x51, // remap memory, odd even columns, com flip and column swap
		SSD1327_SETSTARTLINE, 0x00, // 0xA1, 0x00
		SSD1327_SETDISPLAYOFFSET, 0x00, // 0xA2, 0x00
		SSD1327_DISPLAYALLOFF, 
		SSD1327_SETMULTIPLEX, 0x7F, // 0xA8, 0x7F (1/64)
		SSD1327_PHASELEN, 0x11, // 0xB1, 0x11
		SSD1327_DCLK, 0x00, // 0xb3, 0x00 (100hz)
		SSD1327_REGULATOR, 0x01, // 0xAB, 0x01
		SSD1327_PRECHARGE2, 0x04, // 0xB6, 0x04
		SSD1327_SETVCOM, 0x0F, // 0xBE, 0x0F
		SSD1327_PRECHARGE, 0x08, // 0xBC, 0x08
		SSD1327_FUNCSELB, 0x62, // 0xD5, 0x62
		SSD1327_CMDLOCK, 0x12, // 0xFD, 0x12
		SSD1327_NORMALDISPLAY, SSD1327_DISPLAYON
	};
	
	sendGraphBytes(init_128x128, sizeof(init_128x128));

	// usleep(1000000);
	// i2cWritePtr[0] = CTRL_CMD;
	// i2cWritePtr[1] = SSD1327_DISPLAYALLON; // 0xaf
	// i2cWritePtr[2] = SSD1327_SETCONTRAST;
	// i2cWritePtr[3] = 0x7F;
	// i2cWritePtr[4] = SSD1327_DISPLAYON;
	// sendBytes(graphAddr, i2cWritePtr, 5);
	
	//memset(mainBuff, 0, sizeof(mainBuff));
	//redrawMain();
	//writeMainStr("1.(This i$, #a Secret--Ruling)", FIRST_ROW, ACT_COL(0));
	//writeMainStr("2.Quiz {Riddlers} unite!", SECOND_ROW, ACT_COL(1));
	//writeMainStr("3.The \"Quick\" [Brown] Fox?", THIRD_ROW, ACT_COL(2));
	//writeMainStr("4.Jumps Over; the Lazy Dog B&B.", FOURTH_ROW, ACT_COL(3));
	//writeMainStr("5.Numbers: 0123456789%", FIFTH_ROW, ACT_COL(4));
	//writeMainStr("6.Symbols: ~!@#$%^&*()_+", SIXTH_ROW, ACT_COL(2));
	///*int border = 1;
	//int printed = 0;
	//for(int32_t i = 0; i < MAIN_COLS; ++i)
	//{
	//	if (i < border || i >= MAIN_COLS - border)
	//	{
	//		mainBuff[i] = ~mainBuff[i];
	//	}
	//	else
	//	{
	//		for (int j = 0; j < border; ++j)
	//		{
	//			mainBuff[i] |= ((unsigned long long)1 << j);
	//			mainBuff[i] |= ((unsigned long long)1 << (63 - j));
	//		}
	//		if (!printed)
	//		{
	//			for (int j = 0; j < 64; ++j)
	//			{
	//				rt_printf("%u", (mainBuff[i] & ((unsigned long long)1 << j))? 1 : 0);
	//			}
	//			printf("\n");
	//			printed = 1;
	//		}
	//		
	//	}
	//}*/

	//updateLCDelems(0, LCDelems);
	//for (int32_t i = 0; i < LCDelems; ++i)
	//{
	//	checkWriteElem();
	//}
	memset(mainBuff, 0, sizeof(mainBuff));
	redrawMain();
	//rt_printf("clearing...\n");
	redrawGraphic();
	updateLCDelems(0, LCDelems);
	/*memset(graphBuff, 0, sizeof(graphBuff));
	for (int32_t i = 0; i < GRAPH_ROWS; ++i)
	{
		for (int32_t j = 0; j < GRAPH_SECTORS; ++j)
		{
			graphBuff[i][j] = 0x4 | (0x4 << 4);
		}
	}*/
	//const int x_speed = 11;
	//const int y_speed = 3;
	//int x = 0, y = 13, sz = 60, x_inc = x_speed, y_inc = y_speed;
	//for (int i = 0; i < 5; ++i)
	//{
	//	if (x + sz > GRAPH_ROWS)
	//	{
	//		x = GRAPH_ROWS - sz;
	//		x_inc = -x_speed;
	//	}
	//	else if (x < 0)
	//	{
	//		x = 0;
	//		x_inc = x_speed;
	//	}
	//	if (y + sz > GRAPH_COLS)
	//	{
	//		y = GRAPH_COLS - sz;
	//		y_inc = -y_speed;
	//	}
	//	else if (y < 0)
	//	{
	//		y = 0;
	//		y_inc = y_speed;
	//	}
	//	int y_sect = y >> 1;
	//	int y_sz = sz >> 1;
	//	memset(graphBuff, 0, sizeof(graphBuff));
	//	for (int i = 0; i < sz; ++i)
	//	{
	//		for (int j = 0; j < (y_sz >> 1); ++j)
	//		{
	//			graphBuff[x + i][y_sect + j] = 4 | (4 << 4);
	//		}
	//	}
	//	redrawGraphic(16, 1);
	//	x += x_inc;
	//	y += y_inc;
	//}
	//for (int i = 0; i < GRAPH_ROWS; i += 2)
	//{
	//	drawGraphicLine(0, 0, 4, -1, i, -1);
	//	//usleep(100000);
	//	
	//	//drawGraphicPortion(0, 127, i, i);
	//	//usleep(100000);
	//}
	//redrawGraphic(0, 1);
	//for (int i = 0; i < GRAPH_ROWS; i += 2)
	//{
	//	drawGraphicLine(0, 0, 4, -1, i, 0);

	//	//drawGraphicPortion(0, 127, i, i);
	//	//
	//}
	//redrawGraphic(4, 1);

	/*
	
	
	
	drawGraphicPortion(0, 127, 30, 60);
	usleep(2000000);*/
	/*drawGraphicPortion(0, 127, 0, 127);
	usleep(2000000);*/
	//redrawGraphic(0, 1);
	
	/*drawGraphicLine(0, 1, 9, 120, 120);
	drawGraphicLine(0, 1, 11, 50, 50);
	drawGraphicLine(0, 1, 14, 60, 60);*/
	/*for (int i = 0; i < 16; ++i)
	{
		drawGraphicLine(0, 0, i, 8 * i, 8 * i);
	}
	*/
	
	//GRAPH_update = 1;	
	return 0;
	
}

int setAddress(int address)
{
	if(last_addr != address)
	{
		//rt_printf("changing address to 0x%02X\n", address);
		last_addr = address;
		if (ioctl(i2C_file, I2C_SLAVE, last_addr) < 0)
		{
			rt_printf("I2C_SLAVE address %#x failed...", last_addr);
			return(2);
		}
	}
	return 0;
}

int sendBytes(int addr, const void *buf, size_t count)
{
	
	if(i2C_file == -1) return -1;
	if(setAddress(addr)) return -1;
	//rt_printf("sending %d bytes to 0x%02X\n", count, addr);
	return write(i2C_file, buf, count);
}

int sendCmd(int addr, uint8_t byte)
{
	i2cWritePtr[0] = CTRL_CMD;
	i2cWritePtr[1] = byte;
	return sendBytes(addr, (void *)i2cWritePtr, 2);
}

int sendMainCmd(uint8_t byte)
{
	return sendCmd(mainAddr, byte);
}
int sendMainBytes(const void *buf, size_t count)
{
	return sendBytes(mainAddr, buf, count);
}

int sendGraphicCmd(uint8_t byte)
{
	return sendCmd(graphAddr, byte);
}
int sendGraphBytes(const void *buf, size_t count)
{
	//rt_printf("sending %d graph bytes\n", count);
	return sendBytes(graphAddr, buf, count);
}

void setGrayBuffVals(uint8_t val)
{
	for (int32_t i = 0; i < GRAPH_ROWS; ++i)
	{
		for (int32_t j = 0; j < GRAPH_SECTORS; ++j)
		{
			//if (i % 4 == 0 && j % 2 == 0)
			//if(i == (j << 1))
			graphBuff[i][j] = val | (val << 4);
		}
	}
}

void setGraphicPos(uint8_t s_row, uint8_t s_sect, uint8_t e_row, uint8_t e_sect)
{
	i2cWritePtr[0] = CTRL_CMD;
	i2cWritePtr[1] = SSD1327_SETCOLUMN;
	i2cWritePtr[2] = s_sect;
	i2cWritePtr[3] = e_sect;

	i2cWritePtr[4] = SSD1327_SETROW;
	i2cWritePtr[5] = s_row;
	i2cWritePtr[6] = e_row;
	sendBytes(graphAddr, i2cWritePtr, 7);
	
	//rt_printf("graphic pos row %d-%d, col sect %d-%d\n", s_row, e_row, s_sect, e_sect);
}

void setGraphicInds(uint8_t x_ind, uint8_t y_ind, uint8_t row_cnt, uint8_t sect_cnt)
{
	uint8_t row = x_ind * row_cnt;
	uint8_t sect = y_ind * sect_cnt;
	i2cWritePtr[0] = CTRL_CMD;
	i2cWritePtr[1] = SSD1327_SETCOLUMN;
	i2cWritePtr[2] = sect;
	i2cWritePtr[3] = sect + sect_cnt - 1;

	i2cWritePtr[4] = SSD1327_SETROW;
	i2cWritePtr[5] = row;
	i2cWritePtr[6] = row + row_cnt - 1;
	sendBytes(graphAddr, i2cWritePtr, 7);

	//rt_printf("graphic pos x: %d, y: %d\n", x_ind, y_ind);
}


void setUpperGraphicPixel(uint8_t overwrite, uint8_t* buff, uint8_t gray_val)
{
	uint8_t exists = ((*buff & 0xF0) >> 4);
	if (overwrite || !exists || gray_val < exists)
	{
		*buff &= 0x0F;
		*buff |= (gray_val << 4);
	}
}

void setLowerGraphicPixel(uint8_t overwrite, uint8_t* buff, uint8_t gray_val)
{
	uint8_t exists = (*buff & 0x0F);
	if (overwrite || !exists || gray_val < exists)
	{
		*buff &= 0xF0;
		*buff |= gray_val;
	}
}

void setGraphicPixel(int is_upper, uint8_t* buff, uint8_t gray_val)
{
	if(is_upper) setUpperGraphicPixel(1, buff, gray_val);
	else setLowerGraphicPixel(1, buff, gray_val);
}

void drawGraphicPortion(uint8_t s_row, uint8_t e_row, uint8_t s_col, uint8_t e_col)
{
	if (s_row > GRAPH_ROWS - 1 || s_col > GRAPH_COLS - 1) return;
	if (e_row > GRAPH_ROWS - 1) e_row = GRAPH_ROWS - 1;
	if (e_col > GRAPH_COLS - 1) e_col = GRAPH_COLS - 1;

	uint8_t s_sect = s_col >> 1;
	uint8_t e_sect = e_col >> 1;
	uint8_t to_print;	

	setGraphicPos(s_row, s_sect, e_row, e_sect);
	i2cWritePtr[0] = CTRL_DATA;
	to_print = 1;
	
	for (int32_t i = s_row; i <= e_row; ++i)
	{	
		for (int32_t j = s_sect; j <= e_sect; ++j)
		{
			i2cWritePtr[to_print++] = graphBuff[i][j];
			if (to_print >= maxBytes)
			{
				sendGraphBytes(i2cWritePtr, to_print);
				to_print = 1;
			}
		}
	}
	if (to_print > 1)
	{
		sendGraphBytes(i2cWritePtr, to_print);
	}
}



void drawGraphicLine(uint8_t clear, uint8_t overwrite, uint8_t gray_val, int8_t row, int8_t col, int full_screen)
{
	gray_val &= 0x0F;
	uint8_t col_sect = col >> 1;
	//rt_printf("line row %d col sect %d\n", row, col_sect);
	if (clear)
	{
		//memset(graphBuff, 0, sizeof(graphBuff));
		redrawGraphic();
		//sendGraphicCmd(SSD1327_DISPLAYALLOFF);
	}
	

	if (row > -1)
	{
		for (int i = 0; i < GRAPH_SECTORS; ++i)
		{
			setLowerGraphicPixel(overwrite, &graphBuff[row][i], gray_val);
			setUpperGraphicPixel(overwrite, &graphBuff[row][i], gray_val);
		}
		if(!full_screen) drawGraphicPortion(row, row, 0, GRAPH_COLS - 1);
	}
	

	if (col > -1)
	{
		for (int i = 0; i < GRAPH_ROWS; ++i)
		{
			if (col & 1) setUpperGraphicPixel(overwrite, &graphBuff[i][col_sect], gray_val);
			else setLowerGraphicPixel(overwrite, &graphBuff[i][col_sect], gray_val);
		}
		if (!full_screen) drawGraphicPortion(0, GRAPH_ROWS - 1, col, col);
	}
	if (full_screen) redrawGraphic();
}

void redrawGraphic()
{
	//if(gray_val < 16) setGrayBuffVals(gray_val);
	
	/*if (!blocks)
	{
		setGraphicPos(0, 0, GRAPH_ROWS - 1, GRAPH_SECTORS - 1);
		i2cWritePtr[0] = CTRL_DATA;
		for (int32_t i = 0; i < GRAPH_ROWS; ++i)
		{
			for (int32_t j = 0; j < GRAPH_SECTORS; ++j)
			{
				i2cWritePtr[j + 1] = graphBuff[i][j];
			}
			sendGraphBytes(i2cWritePtr, GRAPH_SECTORS + 1);
		}
	}
	else*/
	{
		static const int divisions = 8;
		static const int rows = GRAPH_ROWS / divisions;
		static const int sectors = GRAPH_SECTORS / divisions;
		static const int blockBytes = GRAPH_ROWS * GRAPH_SECTORS / (divisions * divisions);
		static uint8_t blocks[divisions][divisions][blockBytes + 1];
		static int init = 1;
		int drawn = 0;
		const int maxDraw = 2;
		if (init)
		{
			init = 0;
			setGraphicPos(0, 0, GRAPH_ROWS - 1, GRAPH_SECTORS - 1);
			memset(blocks, 0, sizeof(blocks));
			for (int y = 0; y < divisions; ++y)
			{
				for (int x = 0; x < divisions; ++x)
				{
					blocks[x][y][0] = CTRL_DATA;
					setGraphicInds(x, y, rows, sectors);
					sendGraphBytes(blocks[x][y], blockBytes + 1);
				}

			}
			return;
		}

		for (int y = 0; y < divisions; ++y)
		{
			for (int x = 0; x < divisions; ++x)
			{
				int s_row = x * rows, e_row = s_row + rows;
				int s_sect = y * sectors, e_sect = s_sect + sectors;
				int ind = 1;
				int needs_draw = 0;
				for (int r = s_row; r < e_row; ++r)
				{
					for (int s = s_sect; s < e_sect; ++s)
					{
						if (graphBuff[r][s] != blocks[x][y][ind])
						{
							blocks[x][y][ind] = graphBuff[r][s];
							needs_draw = 1;
						}
						++ind;
					}
				}
				if (needs_draw)
				{
					setGraphicInds(x, y, rows, sectors);
					sendGraphBytes(blocks[x][y], blockBytes + 1);
					if (++drawn >= maxDraw)
					{
						drawn = 0;
						//usleep(SLEEP_MICROS);
					}
				}
				else
				{
					//rt_printf("skipping %d, %d\n", x, y);
				}
			}
		}
	}
}



void redrawMain()
{
	drawMainPortion(0, MAIN_ROWS, 0, MAIN_COLS);
}

void drawMainPortion(uint8_t s_row, uint8_t e_row, uint8_t s_col, uint8_t e_col)
{
	const uint8_t start_page = (s_row >> 3);
	uint8_t end_page = (e_row >> 3);
	if (end_page > 7) end_page = 7;
	if (e_col > MAIN_COLS) e_col = MAIN_COLS;

	//rt_printf("page %d end %d, srow %d, erow %d, sCol %d eCol %d\n", start_page, end_page, s_row, e_row, s_col, e_col);

	for (uint8_t page = start_page; page <= end_page; page++)
	{
		i2cWritePtr[0] = CTRL_CMD;
		i2cWritePtr[1] = LOW_COLUMN_ADDRESS | (s_col & 0x0F);
		i2cWritePtr[2] = HIGH_COLUMN_ADDRESS | ((s_col & 0xF0) >> 4);
		i2cWritePtr[3] = SET_PAGE_ADDRESS | page;
		sendBytes(mainAddr, i2cWritePtr, 4);
		i2cWritePtr[0] = CTRL_DATA;

		uint8_t rem = e_col - s_col;
		uint8_t ind = s_col;

		while (rem)
		{
			uint8_t amt = (rem < maxBytes) ? rem : maxBytes;

			for (uint8_t i = 0; i < amt; ++i)
			{
				//i2cWritePtr[i + 1] = *(((uint8_t *)&(buff[i + ind])) + page);
				i2cWritePtr[i + 1] = *(((uint8_t*)&(mainBuff[i + ind])) + page);
			}
			sendBytes(mainAddr, (void*)i2cWritePtr, amt + 1);
			rem -= amt;
			ind += amt;
		}
	}
}

void writeMainStr(const char* str, int32_t row, int32_t startCol, int32_t endCol, int32_t chrRow, int32_t chrCol, int32_t chrLen, int32_t space_cols, int32_t align_right)
{
	int32_t target_start = -1, target_end = -1;

	
	if (screenInd == PATCHSV && chrRow == 3 && chrCol == 6)
	{
		target_start = target_end = saveNameInd;
	}
	else if (screenInd == ARPEGSETUP && chrRow == 2 && chrCol == 4)
	{
		target_start = target_end = posBPM[indBPM];
	}
	else if ((screenInd == WAVETBL && (chrRow == 2 || chrRow == 3)) || (screenInd == PHASE && (chrRow == 1 || chrRow == 2)))
	{
		target_start = 0;
		target_end = 23;
	}


	const int32_t start_page = (row >> 3);
	const int32_t page_cnt = 1 + (row & 0x7) ? 1 : 0;
	int32_t col = startCol;
	int full_width = CHAR_W;

	const int32_t len = strlen(str);
	if (endCol > MAIN_COLS) endCol = MAIN_COLS;
	uint64_t shifter, temp, mask = (unsigned long long)0x1FF << (row);
	mask = ~mask;
	int32_t sInd = 0;
	int32_t has_space = 0;
	int32_t to_offset = 0;

	if (align_right)
	{		
		int32_t total_used = 0;
		for (int32_t i = 0; i < strlen(str); ++i)
		{
			//past end of string or invalid char: fill with space
			if (int(str[i]) < ASC_MIN || int(str[i]) > ASC_MAX)
			{
				total_used += full_width; // space_cols;
				has_space = 1;
			}
			else
			{
				total_used += full_width; // ascii_map[int(str[i]) - ASC_MIN][0] + 1;
			}
		}
		//rt_printf("%s %d empty cols\n", str, endCol - col);
		to_offset = endCol - col - total_used;
	}



	uint8_t* ptr;
	int32_t ptrCols = 0;
	while (col < endCol)
	{
		//ready for a new letter
		if (ptrCols < 1)
		{
			while (sInd < len && str[sInd] == '@')
			{
				col += full_width;// CHAR_W + 1;
				++sInd;
			}
			int val = int(str[sInd]);
			//past end of string or invalid char: fill with space
			if (sInd >= len || val < ASC_MIN || val > ASC_MAX)
			{
				ptr = (uint8_t*)spaces;
				ptrCols = full_width;// space_cols;
				has_space = 0;
			}
			
			else
			{
				ptr = (uint8_t*)&ascii_map[val - ASC_MIN][1];
				ptrCols = full_width;// *(ptr++) + 1;
			}
			++sInd;
		}
		if (to_offset && !has_space)
		{
			for (int32_t i = 0; i < to_offset; ++i)
			{
				mainBuff[col++] &= mask;
			}
			to_offset = 0;
		}
		shifter = *(ptr++);
		if (target_start > -1 && (sInd - 1 >= target_start && sInd - 1 <= target_end)) shifter |= 0x100;
		else
		{
			temp = 0x100;
			temp = ~temp;
			shifter &= temp;
		}

		shifter <<= row;
		mainBuff[col] &= mask;
		mainBuff[col] |= shifter;
		++col;
		--ptrCols;
	}
	drawMainPortion(uint8_t(row), uint8_t(row + CHAR_H), uint8_t(startCol), uint8_t(endCol));
}

void drawWaveForm(int8_t (&graphic)[MAIN_COLS + 1][2], uint32_t lower_row, uint32_t &cleared, int32_t minCol = 0, int32_t maxCol = GRAPH_COLS)
{
	uint8_t next = graphic[minCol][0];
	uint8_t cur;

	if (!cleared)
	{
		memset(graphBuff, 0, sizeof(graphBuff));
		cleared = 1;
	}
	uint8_t gray_val = 4;
	for (uint8_t i = minCol; i < maxCol; ++i)
	{
		cur = next;
		next = graphic[i + 1][0];

		if (cur <= next)
		{
			graphic[i][1] = next;
		}
		else
		{
			graphic[i][0] = next;
			graphic[i][1] = cur;
		}
		int s_row = lower_row - graphic[i][1];
		int e_row = lower_row - graphic[i][0];

		for (int j = s_row; j <= e_row; ++j)
		{
			setGraphicPixel(i & 1, &graphBuff[j][i >> 1], gray_val);
		}
	}
}

 void updateGraphic()
 {
	 int32_t *wavPtr = NULL;
	 int8_t graphic[MAIN_COLS + 1][2] = {0};
	 uint8_t suslen = 10;
	 GRAPH_update = 0;
	 uint32_t strt = ticks;
	 uint8_t vertA = 200;
	 uint8_t vertB = 200;
	 uint32_t cleared = 0;
	 int32_t drawWaveCombo = 0;

	 if(screenInd == AMPENV)
	 {
		 uint8_t *ptr = &amp_env_knobs[oscInd].rate[0];
		 int8_t sh = 6;
		 uint32_t cycles = 0;
		 uint16_t majCycle[7];
		 uint8_t majPos[7];
		 float curCycle = 0;
		 uint8_t curPos = 0;
		 uint8_t majInd = 0;
		 uint8_t curVal = 0;
		 uint32_t val = 0;
		
		 //attack
		 vals[0][0] = val;
		 majCycle[0] = 0;
		 uint32_t att = (ATTACK_K[*ptr] < MAX_INT32>>sh)? ATTACK_K[*ptr]<<sh: MAX_INT32;
		 while(val < MAX_INT32)
		 {
			 cycles++;
			 val += att;
			 if(val > MAX_INT32) val = MAX_INT32;
			 vals[0][cycles] = val>>graphShift;
		 }
		 majCycle[1] = cycles;
		
		 //remaining portions
		 for(uint8_t i = 0; i < 3; ++i)
		 {
			 int32_t goal;
			 int32_t rate = SEEK[*(ptr + 1 + i)];
			 if(i < 2) goal = GAIN[*(ptr + 4 + i)];
			 else goal = 0;
			 int32_t diff = goal-val;
			 rate = (rate > (MAX_INT32 >>sh))? MAX_INT32 : rate <<sh;
			 do
			 {
				 val += (___SMMUL(diff, rate))<<1;
				
				 cycles++;
				 diff = goal-val;
				 vals[0][cycles] = (val > MAX_INT32)? 0: val>>graphShift;

			 } while(diff > 40000 || diff < -40000);
			 majCycle[2+i] = cycles;
		 }
		
		 float perCol = (float)cycles/(MAIN_COLS-suslen);
		 if(perCol < 1)
		 {
			 perCol = 1;
			 suslen = MAIN_COLS - cycles;
		 }
		
		 //insert sus line and add an extra slot (since we count one extra cycle)
		 majCycle[5] = majCycle[4];
		 majCycle[6] = majCycle[4];
		 majCycle[4] = suslen;
		
		
		 //get the rounded versions of major points (to make sure everything gets visible)
		 for(uint8_t i = 0; i < 7; ++i)
		 {
			 majPos[i] = float(majCycle[i])/perCol;
		 }
		
		 //get the interpolated graph points
		 for(uint8_t i = 0; i < MAIN_COLS+1; ++i)
		 {
			 //if we're on the sus line, don't increment the cycle and hold the same value
			 if(majInd == 4)
			 {
				 if(!(--suslen)) majInd++;
			 }
			 else
			 {
				 uint16_t ind = curCycle;
				
				 //if we're at a major point, ignore the actual cycle and use the major cycle
				 if(curPos >= majPos[majInd])
				 {
					 ind = majCycle[majInd];
					 majInd++;
				 }
				 curVal =vals[0][ind];
				 curCycle += perCol;
				 ++curPos;
			 }
			 graphic[i][0] = curVal;
		 }
	 }
	 else if(screenInd == DUALENV1 || screenInd == DUALENV2)
	 {

		 DUAL_ENV_KNOBS*ptr = screenInd == DUALENV1 ? &env1_knobs[oscInd] : &env2_knobs[oscInd];
		 int8_t sh = 5;
		 uint32_t cycles = 0;
		 uint16_t majCycle[7];
		 uint8_t majPos[7];
		 float curCycle = 0;
		 uint8_t curPos = 0;
		 int8_t curVal = 0;
		 int32_t val[2] = { ptr->goal[FREE_STAGES + 1][0], ptr->goal[FREE_STAGES + 1][1] };
		 int32_t goal[2];
		 int32_t diff[2];
		 uint8_t majInd = 1;
		 uint8_t susMaj;
		 uint8_t max = 1;
		 float scl;
		 majCycle[0] = 0;
		
		 for(uint8_t i = 0; i < FREE_STAGES + 2; ++i)
		 {
			 uint16_t limit;
			 if(i < FREE_STAGES)
			 {
				 limit = TIME[ptr->time[i]] >> sh;
				 if(ptr->time[i] && limit < 1) limit = 1;
			 }
			 else limit = 0x7FFF;
			 int32_t rate = SEEK[ptr->glide[i]];
			 for (int32_t j = 0; j < 2; ++j)
			 {
				 goal[j] = ptr->goal[i][j];
				 diff[j] = goal[j] - val[j];
			 }
					
			 rate = (rate > (MAX_INT32 >>sh))? MAX_INT32 : rate <<sh;
			 uint16_t curCyc = 0;
			 int32_t not_close = 0;
			 if(limit)
			 {
				 do
				 {
					 not_close = 0;
					 cycles++;
					 
					 for (int32_t j = 0; j < 2; ++j)
					 {
						 val[j] += (___SMMUL(diff[j], rate)) << 1;
						 diff[j] = goal[j] - val[j];
						 vals[j][cycles] = (val[j] >> 21);
						 int8_t abs = val[j] >> 21;
						 if (abs < 0) abs = -abs;
						 if (abs > max) max = abs;
						 not_close |= diff[j] > 40000 || diff[j] < -40000;
					 }
					 
					 curCyc++;

				 } while((i<FREE_STAGES && curCyc < limit) || (i>=FREE_STAGES && not_close));
				 majCycle[majInd++] = cycles;
				 if(i == FREE_STAGES) susMaj = majInd++;
			 }
		 }
		 scl = (float)31.5/max;
		 if(scl > 4) scl = 4;
		 //LogTextMessage("cyc %u %u %f" , cycles, max, scl);
		 majCycle[majInd] = majCycle[majInd-1];
		 vals[0][0] = vals[0][cycles];
		 vals[1][0] = vals[1][cycles];
		 float perCol = (float)cycles/(MAIN_COLS-suslen);
		 if(perCol < 1)
		 {
			 perCol = 1;
			 suslen = MAIN_COLS - cycles;
		 }
		 majCycle[susMaj] = suslen;

		 //get the rounded versions of major points (to make sure everything gets visible)
		 for(uint8_t i = 0; i < majInd + 1; ++i)
		 {
			 majPos[i] = float(majCycle[i])/perCol;
		 }
		

		 for (int32_t j = 1; j >= 0; --j)
		 {
			 uint8_t now_sus = suslen;
			 float nowCycle = curCycle;
			 majInd = 0;
			 uint8_t nowPos = curPos;

			 //get the interpolated graph points
			 for (uint8_t i = 0; i < MAIN_COLS + 1; ++i)
			 {
				 //if we're on the sus line, don't increment the cycle and hold the same value
				 if (majInd == susMaj)
				 {
					 if (!(--now_sus)) majInd++;
				 }
				 else
				 {
					 uint16_t ind = nowCycle;

					 //if we're at a major point, ignore the actual cycle and use the major cycle
					 if (nowPos >= majPos[majInd])
					 {
						 ind = majCycle[majInd];
						 majInd++;
					 }
					 curVal = vals[j][ind];
					 nowCycle += perCol;
					 ++nowPos;
				 }
				 //LogTextMessage("%d", curVal);
				 graphic[i][0] = curVal * scl + 32;
			 }
			 if (j) drawWaveForm(graphic, 127, cleared);
		 }
	 }
	 /* else if(screenInd == ARPEGSETUP || screenInd == ARPEGNOTES)
	 {

		 ARP_KNOBS *ptr = &arpeggio[oscInd];
		 int8_t sh = 5;
		 uint32_t cycles = 0;
		 uint16_t majCycle[65];
		 uint8_t majPos[65];
		 float curCycle = 0;
		 uint8_t curPos = 0;
		 int8_t curVal = 0;
		 int32_t val = 0;
		 uint8_t majInd = 1;
		 uint8_t susMaj;
		 uint8_t max = 1;
		 float scl;
		 majCycle[0] = 0;
		 uint16_t limit = ptr->T >> sh;
		 //LogTextMessage("h %d %d", 0x7ffffff, 0x7fffffff>>1);
		 int32_t rate = SEEK[ptr->G];
		 rate = (rate > (MAX_INT32 >>sh))? MAX_INT32 : rate <<sh;
		 uint8_t isVel = 1;
		 for(uint8_t i = 0; i < 4; ++i)
		 {
			 if(arpToggle[i] != VEL_TOG)
			 {
				 isVel = 0;
				 break;
			 }
		 }
		
		 for(uint8_t i = 0; i < ptr->steps; ++i)
		 {
			 //int32_t goal = ptr->P[i]<<PITCH_COARSE; 
			 int32_t goal = (isVel)? ((int32_t)VELGAIN[ptr->V[i]]-(0x7fffffff>>1))>>3 : ptr->P[i]<<PITCH_COARSE; 
			 //LogTextMessage("g %d", goal);
			 int32_t diff = goal-val;	
			 uint16_t curCyc = 0;

			 do
			 {
				 val += (___SMMUL(diff, rate))<<1;
				 cycles++;
				 diff = goal-val;
				 vals[cycles] = (val>>21);
				 int8_t abs = val>>21;
				 if(abs < 0) abs = -abs;
				 if(abs > max) max = abs;
				 curCyc++;

			 } while(curCyc < limit);
			 majCycle[majInd++] = cycles;
		 }
		 scl = (float)31.5/max;
		 if(scl > 4) scl = 4;
		 //LogTextMessage("cyc %u %u %f" , cycles, max, scl);
		 majCycle[majInd] = majCycle[majInd-1];
		 vals[0] = 0;
		 float perCol = (float)cycles/(MAIN_COLS);

		 //get the rounded versions of major points (to make sure everything gets visible)
		 for(uint8_t i = 0; i < majInd + 1; ++i)
		 {
			 majPos[i] = float(majCycle[i])/perCol;
		 }
		
		 majInd = 0;
		
		 //get the interpolated graph points
		 for(uint8_t i = 0; i < MAIN_COLS+1; ++i)
		 {
			 uint16_t ind = curCycle;
				
			 //if we're at a major point, ignore the actual cycle and use the major cycle
			 if(curPos >= majPos[majInd])
			 {
				 ind = majCycle[majInd];
				 majInd++;
			 }
			 curVal =vals[ind];
			 curCycle += perCol;
			 ++curPos;

			 //LogTextMessage("%d", curVal);
			 graphic[i][0] = curVal*scl + 32;
		 }
	 } */
	 else if (screenInd == WAVETBL || screenInd == TBLPOS)
	 {
		 drawWaveCombo = 1;
		 const int gap = 10;
		 int cnt = (GRAPH_COLS - gap)/ 2;
		 float inc = float(TABLE_RES[0]) / float(cnt);
		
		 for (int y = 0; y < 2; ++y)
		 {
			 for (int x = 0; x < 2; ++x)
			 {
				 int offset =  (x ? cnt + gap : 0);
				 int ind = 2 * y + x;
				 float pos = 0;
				 if (SHIFTMASK(oscInd, bitWave + ind))
				 {
					 ////wavPtr = (SHIFTMASK(oscInd, bitHarms))? &harmArray[oscInd][0]: &wavArray[oscInd][0];
					 wavPtr = &harmArray[oscInd][ind][0];
					 for (uint16_t i = 0; i < cnt; ++i)
					 {
						 graphic[i + offset][0] = ((harmArray[oscInd][ind][int(pos)]) >> 27) + 16;
						 pos += inc;
					 }
				 }
				 else
				 {

					 for (uint16_t i = 0; i < cnt; ++i)
					 {
						 graphic[i + offset][0] = (lcdRand[i] >> 27) + 16;
					 }
				 }
				 graphic[offset + cnt][0] = graphic[offset][0];
				 drawWaveForm(graphic, 95 + 32 * y, cleared, offset, offset + cnt);
				 
			 }
		 }
		 int xPix = float(tableRatios[oscInd][0] >> TBL_SHIFT) / 127.0f * (GRAPH_COLS - cnt) + cnt / 2 - 2;
		 int yPix = float(tableRatios[oscInd][1] >> TBL_SHIFT) / 127.0f * GRAPH_ROWS / 4 + GRAPH_ROWS / 2 + GRAPH_ROWS / 8 - 2;
		 uint8_t val = 0xFF;
		 for (int i = 0; i < 4; ++i)
		 {
			 for (int j = 0; j < 4; ++j)
			 {
				 int col = i + xPix;
				 int row = j + yPix;
				 setGraphicPixel(col & 1, &graphBuff[row][col >> 1], 4);
			 }
		 }
		 
	 }
	 else if(screenInd == HARMONIC)
	 {
		drawWaveCombo = 1;
		if(SHIFTMASK(oscInd, bitWave + table_page))
		 {		 
			 ////wavPtr = (SHIFTMASK(oscInd, bitHarms))? &harmArray[oscInd][0]: &wavArray[oscInd][0];
			 wavPtr = &harmArray[oscInd][table_page][0];

			 uint32_t inc = TABLE_RES[0] / MAIN_COLS;
			 for(uint16_t i = 0; i < MAIN_COLS + 1; ++i)
			 {
				 graphic[i][0] = (*(wavPtr) >> 26) + 32; 
				 wavPtr += inc;
			 }
			 drawWaveForm(graphic, 127, cleared);
		 }
		 else
		 {

			 for (uint16_t i = 0; i < MAIN_COLS + 1; ++i)
			 {
				 graphic[i][0] = (lcdRand[i] >> 26) + 32;
			 }
		 }
	 }
	 else if (screenInd == PHASE)
	 {
		 if (SHIFTMASK(oscInd, bitWave + table_page))
		 {
			 PHASE_KNOBS* phase = &phase_knobs[oscInd][table_page];
			 if (strncmp(phaseFile[oscInd][table_page]->name, lastPhaseName, MAXFNAMELEN - 1) != 0)
			 {
				 memcpy(lastPhaseName, phaseFile[oscInd][table_page]->name, MAXFNAMELEN);
				 readWaveFile(phaseFile[oscInd][table_page], (int8_t*)phaseArray);
				 usleep(SLEEP_MICROS);;
				 //rt_printf("read file: '%s'\n", phaseFile[oscInd][table_page]->name);
			 }

			 uint32_t incWaveX = TABLE_RES[0] / MAIN_COLS * (phase->partial + 1);
			 uint32_t indWaveX = float(phase->phase) / float(255.0) * float(TABLE_RES[0]);
			 float pre_mult = float(phase->gain) / float(255.0);

			 for (uint16_t i = 0; i < MAIN_COLS + 1; ++i)
			 {
				 indWaveX = (indWaveX + incWaveX) & TABLE_BAND_LAST_MASK[0];
				 graphic[i][0] = (int32_t(pre_mult * float(phaseArray[indWaveX])) >> 26) + 32;
			 }
			 drawWaveForm(graphic, 127, cleared);

			 incWaveX = TABLE_RES[0] / MAIN_COLS;
			 indWaveX = 0;//float(phase->phase) / float(255.0) * float(TABLE_RES[0]);
			 for (uint16_t i = 0; i < MAIN_COLS + 1; ++i)
			 {
				 //curPhase += float(incPhase) * float(1 + float(phaseArray[indWaveX]) * mult);
				 indWaveX = (indWaveX + incWaveX) & TABLE_BAND_LAST_MASK[0];
				 graphic[i][0] = (harmArray[oscInd][table_page][indWaveX] >> 26) + 32;
				// uint32_t wavInd = uint32_t(curPhase) >> TABLE_BAND_COARSE_SHIFT[0];
				 //graphic[i][0] = (harmArray[oscInd][table_page][wavInd] >> 26) + 32;
			 }
		 }
		 else
		 {

			 for (uint16_t i = 0; i < MAIN_COLS + 1; ++i)
			 {
				 graphic[i][0] = (lcdRand[i] >> 26) + 32;
			 }
		 }
	 }
	 if (screenInd == TBLPOS || screenInd == PATCHLD || drawWaveCombo)
	 {
		int32_t x = tableRatios[oscInd][0];
		int32_t y = tableRatios[oscInd][1];
		int32_t tblPct[4];

		tblPct[0] = ___SMMUL(TBL_MAX - x, TBL_MAX - y) << TBL_RENDER_SHIFT;
		tblPct[1] = ___SMMUL(x, TBL_MAX - y) << TBL_RENDER_SHIFT;
		tblPct[2] = ___SMMUL(TBL_MAX - x, y) << TBL_RENDER_SHIFT;
		tblPct[3] = ___SMMUL(x, y) << TBL_RENDER_SHIFT;

		uint32_t inc = TABLE_RES[0] / MAIN_COLS;
		uint32_t mask = TABLE_BAND_LAST_MASK[0];
		uint32_t pos = 0;
		for (uint16_t i = 0; i < MAIN_COLS + 1; ++i)
		{
			int32_t sig = 0;
			
			for (int32_t j = 0; j < TABLE_CNT; ++j)
			{
				sig += ___SMMUL(SHIFTMASK(oscInd, bitWave + j) ? (harmArray[oscInd][j][pos]) : lcdRand[i], tblPct[j]) << 1;
			}
			graphic[i][0] = (sig >> 26) + 32;
			pos = (pos + inc) & mask;
		}
	 }

	 drawWaveForm(graphic, 64, cleared);
	 redrawGraphic();
	
 }

// void updateGraphic()
// {
	// sendGraphicByte(SET_DISP | 1);
	
	
	// int32_t *wavPtr = NULL;
	// int8_t wave[MAIN_COLS] = {0};
	
	// GRAPH_update = 0;
	
	// if(SHIFTMASK(oscInd, bitWave))
	// {
		// wavPtr = (SHIFTMASK(oscInd, bitHarms))? &harmArray[oscInd][0]: &wavArray[oscInd][0];
	// }
	
	// int32_t max = 0;
	// for(uint16_t i = 0; i < MAIN_COLS; ++i)
	// {
		// wave[i] = ((*(wavPtr + 4 * i)) >> 26) + 32;
	// }
	// uint8_t cnt = 0;
	// for(uint8_t page = 0; page < 8; page++)
	// {
		// sendGraphicByte(LOW_COLUMN_ADDRESS |);
		// sendGraphicByte(HIGH_COLUMN_ADDRESS | 0);
		// sendGraphicByte(SET_PAGE_ADDRESS | page);
		
		// uint8_t upr = (8-page) * 8 - 1;
		// uint8_t rem = MAIN_COLS;
		// uint8_t ind = 0;
		
		// while(rem)
		// {
			// uint8_t amt = (rem < maxBytes)? rem: maxBytes;
			
			// memset(&i2cWritePtr[1], 0x0, amt);
			// i2cWritePtr[0] = 0x40;
			// if(wavPtr != NULL)
			// {
				// for(uint8_t i = 0; i < amt; ++i)
				// {
					// int8_t dist = upr - wave[i + ind];
					// if(dist > -1 && dist < 8)
					// {
						// cnt++;
						// i2cWritePtr[i + 1] |= (1 << dist);
						// LogTextMessage("page %u, ind %u, %d, %u", page, i + ind, wave[i + ind], cnt);
					// }
				// }
			// }
			
			// i2cMasterTransmit(&I2CD1, 0x3c, i2cWritePtr, amt + 1, readByte, 0);
			// rem -= amt;
			// ind += amt;
		// }
		  
	// }
	// LogTextMessage("max %d", max);
// }





 void writeStr(const char* str, uint8_t row, uint8_t col, uint8_t len, uint8_t align_right)
 {	
	 writeMainStr(str, FIRST_ROW + ROW_LINES * row, col * CHAR_W, (col + len) * CHAR_W, row, col, len, SPACE_COLS, align_right);
 }

	

 void  setCursorPosition(uint8_t row, uint8_t col)
 {
	 //uint8_t rowAddrs[] = {0x00, 0x40, 0x14, 0x54};
	 //if (row > ROWS-1) 
		// row = ROWS - 1;

	 //if(col > COLS-1)
		// col = COLS-1;

	 //sendByte(0x80 | (rowAddrs[row] + col), Cmd);
 }

// void toggleBackLight(uint8_t on)
// {
	// backlightStatus = (on > 0) ? Bl : 0;
	// sendByte(0, Cmd);
// }

// void toggleDisplay(uint8_t on)
// {
	// displayStatus = (on > 0) ? 0x04 : 0;
	// sendByte(0x08 | displayStatus, Cmd);
// }

// void clearDisplay()
// {
	// sendByte(0x01, Cmd);
// }

 void  toggleBlinkCursor(uint8_t on)
 {
	 //uint8_t tByte = (on > 0) ? (0x01) : 0;
	 //sendByte(0x08 | displayStatus | tByte, Cmd);
 }

 void  updateLCDelems(uint8_t first, uint8_t last)
 {
	 if (last > LCDelems - 1) last = LCDelems - 1;

	 for(uint8_t i = first; i <= last; ++i)
		 LCD_update[i] = 1;
 }

 void setObjPos(int screenInd, int8_t* objRow, int8_t* objCol, int8_t* objLen)
 {
	 for (int i = 0; i < 3; ++i)
	 {
		 int offset = 2 * i;
		 int8_t* tRow = objRow + offset;
		 int8_t* tCol = objCol + offset;
		 int8_t* tLen = objLen + offset;
		 int sInd = 0;
		 int col = 0;
		 int fullLen = strlen(screens[screenInd][i]);
		 while (sInd < fullLen)
		 {
			 if (screens[screenInd][i][sInd] == '@')
			 {
				 *tRow = i + 1;
				 *tCol = col;
				 *tLen = 0;
				 while (sInd < fullLen && screens[screenInd][i][sInd] == '@')
				 {
					 *tLen += 1;
					 ++sInd;
					 col += CHAR_W + 1;
				 }
				 ++tRow;
				 ++tCol;
				 ++tLen;
			 }
			 if (sInd < fullLen)
			 {
				 col += ascii_map[int(screens[screenInd][i][sInd]) - ASC_MIN][0] + 1;
			 }
			 ++sInd;
		 }
	 }
 }

 void checkWriteElem()
 {
		
	 char tempStr[23] = { 0 };
	 static uint8_t curLCD = 0;

	 static int8_t objRow[6];
	 static int8_t objCol[6];
	 static int8_t objLen[6];
	
	
	 uint8_t isEnv2 = 1;
	 uint8_t simple = LCD_update[curLCD];
	 if(LCD_update[curLCD] > 0)
	 {
		// rt_printf("lcd obj %d\n", curLCD);
		 LCD_update[curLCD] = 0;
		 //GRAPH_update = 1;
		 //osc number
		
		 
		 if(curLCD == OSC)
		 { 
			 //rt_printf("osc %d\n", oscInd);
			 //oscStr(oscInd, tempStr);
			 writeStr((char*)oscStr[oscInd], 0, 0, 5, 0);

		 }
		 //volume/level
		 else if(curLCD == LVL)
		 {
			 intToStr((char *)lvlStr[isMainLVL],(isMainLVL)? (int16_t)main_gain: (int16_t)(osc_gain[oscInd]), 3, 0, 0, tempStr);
			 writeStr(tempStr, 0, 5, 7, 1);
		 }
		
		 //screen label
		 /*else if (screenInd == AMPENV)
		 {
			 handleAmpEnvScreen(curLCD);
		 }*/
		 else if (curLCD == SCRN)
		 {
			 toggleBlinkCursor(screenInd == ARPEGSETUP || screenInd == PATCHSV);
			 //rt_printf("screen %s\n", SCREENS[screenInd]);
			 writeStr((char*)SCREENS[screenInd], 0, 12, 11, 1);
			 //setObjPos(screenInd, objRow, objCol, objLen);
			 /*for (int i = 0; i < 3; ++i)
			 {
				 rt_printf("%d %d %d, %d %d %d\n", objRow[2 * i], objCol[2 * i], objLen[2 * i], objRow[2 * i + 1], objCol[2 * i + 1], objLen[2 * i + 1]);
			 }*/
			 for(uint8_t i = 0; i < 4; ++i) writeStr((char*)screens[screenInd][i], i+1, 0, 23, 0);
			 writeStr("", 5, 0, 23, 0);
			 updateLCDelems(OBJ1, OBJ6);
		 } 
		

		
		 //object labels
		 /* else if(simple > 1)
		 {
			 uint8_t ind = curLCD-OBJ1;
			 LogTextMessage("obj %u val %u %u %u %u", ind, simple, rcl[screenInd][ind][0], rcl[screenInd][ind][1], rcl[screenInd][ind][2]);
			 writeBasicInt((int16_t)simple,rcl[screenInd][ind][2], 0, rcl[screenInd][ind][0],rcl[screenInd][ind][1]);
		 } */
			else
			{
			
				switch(screenInd)
				{
					case WAVETBL:
					{
						//"01234567890123456789"
						//"@@@@@@@@@ @@@@@@@@@ ",
						//"@@@@@@@@@>@@@@@@@@@<",
						//"@@@@@@@@@ @@@@@@@@@ "
						

						if (curLCD == OBJ1)
						{
							char temp[30] = { 0 };
							intToStr("PAGE ", table_page, 1, 0, 0, temp);
							writeStr(temp, 1, 0, 22, 0);
						}
						else if (curLCD == OBJ3)
						{
							if (browseCnt[WAVE].files)
							{
								filsList* fil = curWavFile[oscInd][table_page];
								uint8_t dir = fil->dirInd;
								for (int32_t line = 0; line < 3; ++line)
								{
									filsList* tFil;
									uint8_t tDir = dir;
									int32_t tInc = 0;
									if (line == 0)
									{
										tFil = fil->prev;
										tInc = -1;
									}
									else if (line == 1)
									{
										tFil = fil;
										tInc = 0;
									}
									else
									{
										tFil = fil->next;
										tInc = 1;
									}
									if (tInc)
									{
										do
										{
											tDir = indexIncrement(tDir, tInc, browseCnt[WAVE].dirs);
										} while (dirs[WAVE][tDir].numFiles == 0 && tDir != dir);
									}
									writeNumSpaceStr(tDir + 1, dirs[WAVE][tDir].name, line + 2, 0, 10);
									writeNumSpaceStr(tFil->filInd, tFil->name, line + 2, 10, 10);
								}
							}
						}
						
					}
					break;
					case TBLPOS:
					{
						//"01234567890123456789"
						//"TBLX:@@@   TBLY:@@@ ",
						//"@@@@@@:@@@ @@@@@@:@@@",
						//"@@@@@@:@@@ @@@@@@:@@@",

						uint32_t x = tableRatios[oscInd][0], y = tableRatios[oscInd][1];

						if (curLCD == OBJ1)
						{
							writeBasicInt((int16_t)(tableRatios[oscInd][0] >> TBL_SHIFT), 3, 0, 1, 5);
						}
						else if (curLCD == OBJ2)
						{
							writeBasicInt((int16_t)(tableRatios[oscInd][1] >> TBL_SHIFT), 3, 0, 1, 16);
						}
						else if (curLCD == OBJ3)
						{
							writeStr(curWavFile[oscInd][0]->name, 2, 0, 6, 1);
							writeBasicInt(___SMMUL(TBL_MAX - x, TBL_MAX - y) >> (TBL_LCD_SHIFT), 3, 0, 2, 7);

							writeStr(curWavFile[oscInd][1]->name, 2, 11, 6, 1);
							writeBasicInt(___SMMUL(x, TBL_MAX - y) >> (TBL_LCD_SHIFT), 3, 0, 2, 18);

							writeStr(curWavFile[oscInd][2]->name, 3, 0, 6, 1);
							writeBasicInt(___SMMUL(TBL_MAX - x, y) >> (TBL_LCD_SHIFT), 3, 0, 3, 7);

							writeStr(curWavFile[oscInd][3]->name, 3, 11, 6, 1);
							writeBasicInt(___SMMUL(x, y) >> (TBL_LCD_SHIFT), 3, 0, 3, 18);
						}
					}
					break;

					case PATCHLD:
					{	
						//"01234567890123456789"
						//"@@@@@@@@@ @@@@@@@@@ ",
						//"@@@@@@@@@>@@@@@@@@@<",
						//"@@@@@@@@@ @@@@@@@@@ "
						filsList *fil;
						uint8_t fType;
						fType = PATCH;
						fil = curPatchFile;
					 
						if(browseCnt[fType].files)
						{
							uint8_t line = ((curLCD-OBJ1)>>1);
						  
							if((curLCD-OBJ1) & 1)
							{
								if(line == 0) fil = fil->prev;
								else if(line == 2) fil = fil->next;
								writeNumSpaceStr(fil->filInd, fil->name, line + 1, 10, 9);
							}
							else 
							{
								uint8_t dir = fil->dirInd;
								do
								{
									dir = indexIncrement(dir, line-1, browseCnt[fType].dirs);
								} while(dirs[fType][dir].numFiles == 0 && line != 1 && dir != fil->dirInd);
							
								writeNumSpaceStr(dir + 1, dirs[fType][dir].name, line + 1, 0, 9);
							}
						}
					} 
					break;
				
				 case AMPENV:
				 {
					 /*const char LAYOUT[4][21] = 
					 {
						 " TIME: A:@@@  D:@@@ ",
						 "       S:@@@  R:@@@ ",
						 " TRGT: D:@@@  S:@@@ "
					 }*/

					 //const char ampLetter[6][3] = { "A:", "D:", "S:", "R:" , "D:", "S:" };
					 ////handleAmpEnvScreen(curLCD);
					 //uint8_t ind = curLCD - OBJ1;
					 //char str[7] = { 0 };
					 //intToStr((char*)ampLetter[ind], (int16_t)*((&amp_env_knobs[oscInd].rate[0]) + ind), 3, 0, 0, str);
					 ////rt_printf("str %s\n", str);
					 //writeStr(str, (ind >> 1) + 1, (ind & 0x01) ? 16 : 9, 5);
					 uint8_t ind = curLCD - OBJ1;
					 writeBasicInt((int16_t) * ((&amp_env_knobs[oscInd].rate[0]) + ind), 3, 0, (ind >> 1) + 1, (ind & 0x01) ? 16 : 9);
				 }
				 break;
				
				 case PITCH:
				 {
					 // "01234567890123456789"
					 // "LEGATO:@@@          ",
					 // "TRANSPOSE: @@@@@@@  ",
					 // "GLIDE: P:@@@  V:@@@ "
		
		
					 PIT_KNOBS *curPits = &pit_knobs[oscInd];
					 uint8_t arrInd = curLCD - OBJ1;
					 if(arrInd > 0) arrInd--;
					
					 switch(curLCD)
					 {
						 case OBJ1: writeStr((char*)yesNoStr[SHIFTMASK(oscInd, bitLgto)], 1, 7, 3, 1); break;
						
						 case OBJ3:								
							 pitchStr(tempStr, curPits->pitch + ((oscInd < POLY_CNT)? 0: note[firstChild[oscInd]]), (oscInd < POLY_CNT) || SHIFTMASK(oscInd, bitNotes), MIDI_KEY_0, monoPitch[oscInd]);
							 writeStr(tempStr, 2, 11, 7, 1);
							 break;
							
						 case OBJ5: writeBasicInt((int16_t)curPits->pit_glide,3, 0, 3, 9); break;	
						 case OBJ6: writeBasicInt((int16_t)curPits->vel_glide,3, 0, 3, 16); break;
						
						
					 }
				 }
				 break;
				
				 case PITRATIO:
				 {
					 // "01234567890123456789"
					 // "(SELF)      @@@@    ",
					 // " @@@   TO   @@@     ",
					 // "FINE:@@@    *PITMOD "
		
		
					 PIT_RATIO_KNOBS *cur = &pit_ratio[oscInd];				
					 switch(curLCD)
					 {
						 //case OBJ1: writeStr(1, 9, 4, (char *)oscStr[oscInd]); break;
						 //case OBJ2: writeStr(1, 12, 4, (char *)oscStr[oscInd]); break;	
						 case OBJ2: 
						 {
							 if(ensure_not_self_ratio(1))
							 {
								 pit_ratio_update |= (1 << oscInd);	
							 }
							 LCD_update[OBJ2] = 0;
							 writeStr((char*)oscStr[cur->src], 1, 12, 4, 1); break;
						 }						
						 case OBJ3: writeBasicInt((int16_t)cur->dst_val + 1,3, 0, 2, 1); break;	
						 case OBJ4: writeBasicInt((int16_t)cur->src_val + 1,3, 0, 2, 12); break;
						 case OBJ5: writeBasicInt((int16_t)cur->offset, 2, 1, 3, 5); break;
					 }
				 }
				 break;
					
				 case DUALENV1: isEnv2 = 0;
				 case DUALENV2:
				 {
					 //"01234567890123456789"
					 //"STAGE:@@@ @@@@@@@@@@",
					 //"GOAL:@@@@@@@ @@@@@@@",
					 //"GLIDE:@@@ @@@@@@@@@@"
					 DUAL_ENV_KNOBS*curEnv = !isEnv2? &env1_knobs[oscInd] : &env2_knobs[oscInd];
					switch(curLCD)
					{
						case OBJ1: 
							writeStr((char*)stageStr[envInd], 1, 6, 3, 0); 

							writeBasicInt((int16_t)(curEnv->glide[envInd]), 3, 0, 4, 6);

							if (envInd < FREE_STAGES)
							{
							strcpy(tempStr, timeStr);
							float time = (float)TIME[curEnv->time[envInd]] / (float)3000;
							uint8_t leading = 0;
							if (time >= 100) leading++;
							if (time >= 10) leading++;
							if (time >= 1) leading++;

							floatToStr(time, leading, 3 - leading, 0, tempStr);
							strcat(tempStr, "s");
							}
							writeStr(tempStr, 4, 10, 10, 1);
						break;

	
						
					/*case OBJ2:
					{
					if(isEnv2) strCatCat((char *)typeStr, (char *)envStr[(SHIFTMASK(oscInd, bitFECut))? 1: 2], tempStr);
					writeStr(tempStr, 1, 10, 10, 1);
					}
					break;*/
					/* case OBJ4:
					if(SHIFTMASK(MAINTOG, bitRecEnv))
					{
					//LogTextMessage("p");
					intToStr((char *)startStopStr[1],(int16_t)(recNotes), 1, 0, 0, tempStr);
					}
					else strcpy(tempStr, recStr);
					writeStr(2, 13, 7, tempStr);
					break; */
						
							
					case OBJ3: 
						pitchStr(tempStr, curEnv->goal[envInd][0], 1, 0, 0);
						//rt_printf("obj %d str '%s'\n", curLCD - 2, tempStr);
						writeStr(tempStr, 2, 7, 7, 1);
						break;
						
					case OBJ4:
						writeStr(ENV_ITEM_STR[curEnv->send[0]], 2, 16, 4, 0);
						break;
						 
					case OBJ5:
						pitchStr(tempStr, curEnv->goal[envInd][1], 1, 0, 0);
						//rt_printf("obj %d str '%s'\n", curLCD - 2, tempStr);
						writeStr(tempStr, 3, 7, 7, 1);
						break;

					case OBJ6:
						writeStr(ENV_ITEM_STR[curEnv->send[1]], 3, 16, 4, 0);
						break;
							
						 
							
					}		
				 }
				 break;	
					
				
				 case ARPEGSETUP:
				 {					
					 //"01234567890123456789"
					 //"STEPS:@@  TYPE:@@@@@",
					 //"SPM:@@@@@@ GLIDE:@@@",
					 //"STYLE:@@@@-@@@@     "
					
					 ARP_KNOBS *curArp = &arpeggio[oscInd];
					 switch(curLCD)
					 {
						 case OBJ1: writeBasicInt((uint16_t)(curArp->steps), 2, 0, 1, 6); break;
						 case OBJ2: writeStr((char*)envStr[SHIFTMASK(oscInd, bitArpFilt)], 1, 15, 5, 1); break;
							
						 case OBJ3:
							 floatToStr((curArp->BPM),4, 1, 0, tempStr);
							 writeStr(tempStr, 2, 4, 6, 1);
							 break;
						
						 case OBJ4: writeBasicInt((uint16_t)(curArp->G), 3, 0, 2, 17); break;
						
						 case OBJ5: writeStr((char*)loopStr[(SHIFTMASK(oscInd, bitArpTrig)) ? 0 : 1], 3, 6, 4, 1); break;
						 case OBJ6: writeStr((char*)loopStr[(SHIFTMASK(oscInd, bitArpSkip)) ? 2 : 3], 3, 11, 4, 1); break;
	
					 }
				 }
				 break;
				
				 case ARPEGNOTES:
				 {
					 //"01234567890123456789"
					 //"POS:@@@ *PIT/ENF/VEL",
					 //"@@@@@@@@   @@@@@@@@ ",
					 //"@@@@@@@@   @@@@@@@@ "
					
					 ARP_KNOBS *curArp = &arpeggio[oscInd];
					
					 uint8_t baseInd = arp_page[oscInd] << 2;
					 if(curLCD == OBJ1) writeBasicInt((uint16_t)(baseInd + 1), 2, 0, 1, 4);
					 else if(curLCD > OBJ2)
					 {
						 uint8_t posInd = curLCD - OBJ3;
						 baseInd += posInd;

						 if(baseInd < curArp->steps)
						 {
							 intToStr("",(int16_t)(baseInd + 1), 2, 0, 0, tempStr);
							
							 strcat(tempStr, arpNoteLeader[arpToggle[posInd]]);
							 if(arpToggle[posInd] == PIT_TOG)
							 {
								 if(SHIFTMASK(oscInd, bitArpSkip) && !curArp->E[baseInd]) strcat(tempStr, ":SKP");
								 else intToStr("",(int16_t)(curArp->P[baseInd]), 3, 0, 1, tempStr);
							 }
							 else if(arpToggle[posInd] == ENV_TOG)
							 {
								 arpEnvStr(curArp->E[baseInd], tempStr);
							 }
							 else
							 {
								 if(SHIFTMASK(oscInd, bitArpSkip) && !curArp->E[baseInd]) strcat(tempStr, "SKP");
								 else intToStr("",(int16_t)(curArp->V[baseInd]), 3, 0, 0, tempStr);
							 }							
						 }
						 writeStr(tempStr, (posInd >> 1) + 2, (posInd & 0x01)? 11: 0, 8, 1);
					 }
				 }
				 break;
				
				 case ARPREC:
				 {
					 // "01234567890123456789"
					 // "RYTHM:@@@ VEL:@@@@@@",
					 // "ENV:@@@             ",
					 // "           @@@@@@@@ "
					 switch(curLCD)
					 {
						 case OBJ1: writeStr((char*)yesNoStr[(recRhythm) ? 1 : 0], 1, 6, 3, 1); break;
						 case OBJ2: writeStr((char*)velStr[recVel], 1, 14, 6, 1); break;
						
						 case OBJ3: 
							 arpEnvStr(recEnv, tempStr);
							 writeStr(tempStr, 2, 4, 3, 1);
							 break;
							
						 case OBJ6:
							 strcpy(tempStr, startStopStr[SHIFTMASK(MAINTOG, bitRecArp)]);
							 if(SHIFTMASK(MAINTOG, bitRecArp)) intToStr("",(int16_t)(recNotes), 2, 0, 0, tempStr);
							 writeStr(tempStr, 3, 11, 8, 1);
							 break;
					 }
				 }
				 break;

				 case FILTER:
				 {
					 FILT_KNOBS *curFilt = &filt_knobs[oscInd];
					 switch(curLCD)
					 {
							
						 case OBJ1: writeStr((char*)filtStr[curFilt->TYPE], 1, 5, 5, 1); break;
						 case OBJ2: writeStr((char*)yesNoStr[SHIFTMASK(oscInd, bitFTrack)], 1, 17, 3, 1); break;

						 case OBJ3:
							 pitchStr(tempStr, curFilt->FRQ, SHIFTMASK(oscInd, bitFTrack), 0, 0);
							 writeStr(tempStr, 2, 7, 7, 1);
						 break;
						
						 case OBJ5: writeBasicInt((int16_t)(curFilt->RES),3, 0, 3, 4); break;
					 }
				 }
				 break;
					

					
				 case PATCHSV:
					
					 //"DIR: @@@@@@@@@  *CLR",
					 //"EDIT *aA1       MOVE",
					 //"NAME: @@@@@@   *SAVE"
					 switch(curLCD)
					 {
						 case OBJ1: writeNumSpaceStr(saveDirInd + 1, dirs[PATCH][saveDirInd].name, 1, 5, 9); break;
						 case OBJ5:
							 if(isSaved > 0) strcpy(tempStr, saveCopyStr[0]);
							 else strcpy(tempStr, saveName);
							
							 writeStr(tempStr, 3, 6, 6, 0);
							 isSaved = 0;
							 break;
					 }
					
					 break;
				
				   case MIDIINS:
				 {
					 // "01234567890123456789"
					 // " MIDICH:@@@ HKEY:@@@",
					 // " VEL:@@@@@@ LKEY:@@@",
					 // " LEGATO:@@@         "
					
					 MIDI_PARAMS *curMidi = &midi_knobs[oscInd];
					 switch(curLCD)
					 {
						 case OBJ1:
							 if(!curMidi->chan) strcat(tempStr, "ALL");
							 else intToStr("",(int16_t)(curMidi->chan), 2, 0, 0, tempStr);
							 writeStr(tempStr, 1, 8, 3, 1);
							 break;
							
						 case OBJ2:
							 pitchNumtoStr(curMidi->keyMax + MIDI_KEY_0, tempStr);
							 writeStr(tempStr, 1, 17, 3, 1);
							 break;
						
						 case OBJ4:
							 pitchNumtoStr(curMidi->keyMin + MIDI_KEY_0, tempStr);
							 writeStr(tempStr, 2, 17, 3, 1);
							 break;
							
						 //case OBJ3: writeStr(2, 5, 6, (char *)velStr[(SHIFTMASK(oscInd, bitWind))? 2: SHIFTMASK(oscInd, bitKeyVel)]); break;
						 //case OBJ3: writeStr((char *)velStr[curMidi->velType]); break;
						
						 case OBJ5: writeStr((char*)yesNoStr[SHIFTMASK(oscInd, bitLgto)], 3, 8, 3, 1); break;
					 }
				 }
				 break; 
					
				 case MIDICCS:
				 {
					 // "01234567890123456789"
					 // "CC#:WIND:@@@ MDW:@@@",
					 // "RNG:WIND:@@@ MDW:@@@",
					 // "    PBND:@@@ SUS:@@@"
					 //rt_printf("midi cc obj %d\n", curLCD);
					 uint8_t ind = curLCD-OBJ1;
					 writeBasicInt((int16_t)(*(&(midi_knobs[oscInd].CC_nums[0]) + ind)), 3, 0, (ind >> 1) + 1, (ind & 0x01)? 17: 9);
				 }
				 break;
				
				 case MODA:
				 {
					/*{
						"PIT:@@@@@ FCUT:@@@@@",
						"AMP:@@@@@ TBLX:@@@@@",
						"GAT:@@@@@ TBLY:@@@@@"
					},
					{
						"FRS:@@@@@ ARPT:@@@@@",
						"                    ",
						"                    "
					},*/
					 // "01234567890123456789"
					 // "PIT:@@@@  F.CUT:@@@@",
					 // "AMP:@@@@  F.RES:@@@@",
					 // "                    "
					
					 //if(curLCD < OBJ5)
					 {						
						char temp[30] = { 0 };
						intToStr("PAGE ", table_page, 1, 0, 0, temp);
						writeStr(temp, 1, 0, 22, 0);

						 int8_t ind = curLCD-OBJ1;
						 int32_t revised_ind = ind + (table_page > 0 ? 6 : 0);
						 //rt_printf("i %d rev i %d\n", ind, revised_ind);
						 if (revised_ind < 8)
						 {
							 uint8_t srcInd = mod_src[oscInd][revised_ind];
							 strcpy(tempStr, MOD_DESTS[revised_ind]);
							 strcat(tempStr, ":");

							 uint8_t eInd = 5;
							 if (srcInd == 0) eInd = 0;
							 else if (srcInd == MOD_MAIN_OUT) eInd = 1;

							 if (eInd < 5) strcat(tempStr, modStrA[eInd]);
							 else
							 {
								 eInd = (--srcInd) % TOTAL_MOD_SRC;
								 uint8_t oInd = (srcInd) / TOTAL_MOD_SRC;
								 //LogTextMessage("%u, %u", oInd, eInd);

								 strCatCat((char*)modStrO[oInd], (char*)modStrB[eInd], tempStr);
								 //strcpy(tempStr, modStrB[eInd]);
								 //tempStr[3] = oInd + '1';//(srcInd, tempStr);
							 }
						 }
						 writeStr(tempStr, (ind >> 1) + 2, (ind & 0x01) ? 11 : 0, 10, 0);
					 }
				 }
				 break;
				
				 case OUTS:
					 // "01234567890123456789"
					 // " PAN: L@@@ R@@@     ",
					 //"                    ",
					 //"                    "
					 switch(curLCD)
					 {
						 case OBJ1: writeBasicInt((int16_t)panLeft[oscInd], 3, 0, 1, 7); break;
						 case OBJ2: writeBasicInt((int16_t)(127 - panLeft[oscInd]), 3, 0, 1, 12); break;
						 //case OBJ3: writeBasicInt((int16_t)(delay_left_knobs[oscInd]), 3, 0, 2, 7); break;
						 //case OBJ4: writeBasicInt((int16_t)(delay_right_knobs[oscInd]), 3, 0, 2, 12); break;
					 }
					 break;
				 
				 case NOTES:
				 {
					 //"01234567890123456789"
					 //"NOTES:@@@   EDIT:@@@",
					 //" @@@@@@@@@ @@@@@@@@@",
					 //" @@@@@@@@@ @@@@@@@@@"
					
					 if(oscInd >= POLY_CNT) notesPage = 0;
					 if(curLCD == OBJ1)
					 {
						 intToStr("",(int16_t)((notesPage << 2) + 1), 1, 0, 0, tempStr);
						 if(oscInd < POLY_CNT) intToStr("-",(int16_t)(notesPage << 2) + 4, 1, 0, 0, tempStr);
						 writeStr(tempStr, 1, 6, 3, 1);
					 }
					 else if(curLCD == OBJ2) writeStr((char*)notesStr[notesTog], 1, 17, 3, 1);
						
					 else if(curLCD > OBJ2)
					 {
						 uint8_t scrnInd = curLCD-OBJ3;
						 uint8_t stepInd = (notesPage << 2) + scrnInd;
						
						 if(stepInd < childCnt[oscInd])
						 {
							
							 uint8_t child = firstChild[oscInd] + stepInd;
							 if(!notesTog)
							 {								
								 //LogTextMessage("l %d %u", note[child], child);
								 //num: monopoly track, str: everything else
								 pitchStr(tempStr, note[child], oscInd < POLY_CNT && !SHIFTMASK(oscInd, bitPoly) && SHIFTMASK(oscInd, bitNotes), 0, monoPitch[oscInd] + pit_knobs[oscInd].pitch);
							 //pitchStr(tempStr, note[child], 0, 0, monoPitch[oscInd] + pit_knobs[oscInd].pitch);
								 //intToStr("P",(int16_t)(stepInd + 1), 1, 0, 0, tempStr);
								 //pitchStr(tempStr, notes[spread[oscInd].pit[stepInd], 1, 0, 0);
							 }
							 else
							 {								
								 intToStr("V:",(int16_t)__USAT(vel[child] + monoVel[oscInd], 7), 3, 0, 0, tempStr);
							 }
							
						 }
						 writeStr(tempStr, (scrnInd >> 1) + 2, (scrnInd & 0x01)? 11: 1, 9, 1);
					 }
				 }
				 break;
				
				  case FAVS:
				 {
					 //"01234567890123456789"
					 //"FAV @@: @@@@@@      ",
					 //"@@@@@@@@@@@@@@@@@@@@",
					 //"@@@@@@@@@@@@@@@@@@@@"
					
					 switch(curLCD)
					 {
						 case OBJ1: writeBasicInt(favInd, 2, 0, 1, 4); break;
						 case OBJ2: writeStr(saveName, 1,8,6, 1); break;
						 case OBJ3: 
							 if(favSave) strcpy(tempStr, favStr[0]);
							 writeStr(tempStr, 2,0,20, 1);
							 break;
						 case OBJ5:
							 if(favSave) strcpy(tempStr, favStr[1]);
							 writeStr(tempStr, 3,0,20, 1);
							 break;
					 }
					
				 }
				 break; 
				
				 case HARMONIC:
				 {
					 //"G.FUND @@@ PRTL1 @@@",
					 //"G.PTL1 @@@ STEP  @@@",
					 //"G.LAST @@@ COUNT @@@"
					
					 // struct HARMONICS {
						 // uint8_t first;
						 // uint8_t step;
						 // uint8_t cnt;
						 // uint8_t gainFirst;
						 // uint8_t gainLast;
					 // };
					 uint8_t ind = curLCD - OBJ1;
					 writeBasicInt((int16_t)*((&harmParams[oscInd][table_page].gainFund) + ind),3, 0, (ind >> 1) + 1, (ind & 0x01)? 17: 7);
					
 /* 					uint8_t rcs[6][3] = {{1,6,0},{0,0,0},{2,5,1},{2,17,2}, {3,6,3},{3,17,4}};
					 uint8_t *rc = &rcs[curLCD-OBJ1][0];
					 if(*rc > 0)
					 {
						 writeBasicInt(*(&(harmParams[oscInd].first) + *(rc + 2)), 3, 0, *rc, *(rc + 1)); 
					 } */
				 }
				 break;
				
				 case PHASE:
				 {
					 //"PRE-HARM  POST-HARM ",
					 //" TOG:@@@    TOG:@@@ ",
					 //"@@@@@@     @@@@@@   "
					 PHASE_KNOBS* phase = &phase_knobs[oscInd][table_page];
					 uint8_t ind = curLCD - OBJ1;
					if (curLCD == OBJ1 || curLCD == OBJ2)
					{
						if (browseCnt[WAVE].files)
						{
							filsList* fil = phaseFile[oscInd][table_page];
							uint8_t dir = fil->dirInd;
							for (int32_t line = 0; line < 3; ++line)
							{
								filsList* tFil;
								uint8_t tDir = dir;
								int32_t tInc = 0;
								if (line == 0)
								{
									tFil = fil->prev;
									tInc = -1;
								}
								else if (line == 1)
								{
									tFil = fil;
									tInc = 0;
								}
								else
								{
									tFil = fil->next;
									tInc = 1;
								}
								if (tInc)
								{
									do
									{
										tDir = indexIncrement(tDir, tInc, browseCnt[WAVE].dirs);
									} while (dirs[WAVE][tDir].numFiles == 0 && tDir != dir);
								}
								writeNumSpaceStr(tDir + 1, dirs[WAVE][tDir].name, line + 1, 0, 10);
								writeNumSpaceStr(tFil->filInd, tFil->name, line + 1, 10, 10);
							}
						}
					}
					else if (curLCD == OBJ3)
					{
						writeBasicInt(phase->gain, 3, 0, 4, 5);
					}
					else if (curLCD == OBJ4)
					{
						writeBasicInt(phase->phase / 2, 3, 0, 4, 15);
					}
					else if (curLCD == OBJ5)
					{
						writeStr("FRQ: ", 5, 0, 5, 0);
						writeBasicInt(phase->partial + 1, 3, 0, 5, 5);
					}
					else
					{
						if (phase->before_harm) strcpy(tempStr, "BEFORE HARM.");
						else strcpy(tempStr, "AFTER HARM.");
						writeStr(tempStr, 5, 9, 11, 0);
					}
				 }
				 break;
							
				
			 }
		 }
		 if(screenInd == PATCHSV) setCursorPosition(3, 6 + saveNameInd);
		 else if(screenInd == ARPEGSETUP) setCursorPosition(2, 4 + posBPM[indBPM]);
		 //else if(screenInd == ARPEGSETUP) setCursorPosition(2, 4 + BPM_pos);
	 }	
	 curLCD = indexIncrement(curLCD, 1, LCDelems);
	 static int32_t lastJoy[4] = { 0, 0, 0, 0 };
	 //if (!curLCD)
	 //{
		// for (int32_t i = 0; i < 4; ++i)
		// {
		//	 int32_t nowJoy = float(256) * joyVal[i];
		//	 if ((lastJoy[i] >> 1) != (nowJoy >> 1))
		//	 {
		//		 //memset(tempStr, 0, sizeof(tempStr));
		//		 lastJoy[i] = nowJoy;
		//		 writeBasicInt(nowJoy, 3, 0, 5, i * 6);
		//		// floatToStr(joyVal[i], 1, 3, 0, tempStr);
		//		// writeStr(tempStr, 5, 6 * i, 5, 0);
		//	 }
		// }
	 //}
	 if (!curLCD && GRAPH_update)
	 {
		 usleep(SLEEP_MICROS);
		 updateGraphic();
	 }

 }


// void  __attribute__(( noinline )) send4Bits(uint8_t data)
// {
	// //static uint8_t i2cWritePtr[1] __attribute__ ((section (".sram2")));	//the right type of memory for i2c write buffer										
	// //static uint8_t *readByte;
	
	// i2cWritePtr[0] = data | backlightStatus ;
	// i2cMasterTransmit(&I2CD1, address, i2cWritePtr, 1, readByte, 0);	
// }

// void  __attribute__(( noinline )) clearFlag(uint8_t data)
// {
	// //pulse high
	// send4Bits(data | En);
	// //chThdSleepMicroseconds(MICRODELAY);
	
	// //pulse low
	// send4Bits(data & ~En);
	// //chThdSleepMicroseconds(MICRODELAY);
// }

// void  __attribute__(( noinline )) send4BitsAndClear(uint8_t data)
// {
	// send4Bits(data);
	// clearFlag(data);
	// //chThdSleepMicroseconds(MICRODELAY);
// }

// void __attribute__(( noinline ))  sendByte(uint8_t data, uint8_t mode)
// {
	// send4BitsAndClear((data&0xF0)|mode);
	// send4BitsAndClear(((data<<4)&0xF0)|mode);
// }

 void pitchStr(char *str, int32_t pitch, uint8_t isNum, uint8_t center, int32_t note)
 {
	 if(isNum) intToStr("",(int16_t)((pitch>>PITCH_COARSE)), 3, center, 1, str);
	 else pitchNumtoStr((pitch + note)>>PITCH_COARSE, str);
	 intToStr("",(int16_t)((pitch>>PITCH_FINE)&0x3F), 2, 0, 1, str);
 }

 void pitchNumtoStr(int16_t num, char *str)
 {
	
	 static const uint8_t zone1Lim = MIDI_KEY_0 + A0 -1;
	 static const uint8_t zone2Lim = MIDI_KEY_0 + C8;
	
	 if(num > zone1Lim && num <= zone2Lim)
	 {
		 uint8_t octaves = -1 + '0';
		 uint8_t rem = num;
		 uint8_t sharp = 0;
		 char tStr[4];
		 tStr[3] = '\0';
		 rem -= MIDI_KEY_0;
		 while(rem >= 12) 
		 {
			 rem-= 12;
			 octaves++;
		 }
		
		 //char tStr[4];
		 strcpy(tStr, nts[rem]);
		 tStr[1] = octaves;
		 strcat(str, tStr);
		
	 }
	 else
	 {
		
		 if(num > zone1Lim) num = num - (zone2Lim + 1) + SET2;
		 //if(num > maxNoteLabelInd) num = maxNoteLabelInd;
		 strcat(str, noteLabels[num]);
		 if(num < noteLabelTypes[0]) strcat(str, units[0]);
		 else if(num < noteLabelTypes[1]) strcat(str, units[1]);
		 else strcat(str, units[2]);
	 }
		
 }



 void strCatCat(const char *cat1, const char *cat2, char *str)
 {
	 strcat(str, cat1);
	 strcat(str, cat2);
 }




 void arpEnvStr(uint8_t env, char *str)
 {
	 if(env & AMP_MASK) strcat(str, "A");
	 else strcat(str, "-");
	 if(env & PIT_MASK) strcat(str, "P");
	 else strcat(str, "-");
	 if(env & FILT_MASK) strcat(str, "F");
	 else strcat(str, "-");
 }

 void writeBasicInt(int16_t num, uint8_t digits, uint8_t leadingSign, uint8_t row, uint8_t col)
 {
	 char str[5];
	 strcpy(str, "");
	 intToStr("", num, digits, 0, leadingSign, str);
	 writeStr(str, row, col, digits + leadingSign, 1);
 }

 void intToStr(const char *before, int16_t num, uint8_t digitsOut, int16_t center, uint8_t leadingSign, char *str)
 {
	 strcat(str, before);
	 int16_t tNum = num - center;
	 uint8_t digits[4] = {0,0,0,0};
	 char temp[2];
	 temp[1] = '\0';

	 if(leadingSign)
	 {
		 temp[0] = (tNum >= 0)? '+' : '-';
		 strcat(str, temp);
	 }
	
	
	 if(tNum < 0) 
	 {
		 if(leadingSign) tNum = -tNum;
		 else tNum = 0;
	 }
	
	 uint16_t vals[3] = {1000,100,10};
	 for(uint8_t i = 0; i < 3; ++i)
	 {

		 while(tNum >= vals[i])
		 {
			 tNum -= vals[i];
			 digits[i]++;
		 }
	 }

	 digits[3] = tNum;
	 int32_t some = 0;
	 //LogTextMessage("%u %u %u %s %d", digits[0], digits[1], digits[2], str, tNum);
	 for(uint8_t i = 0; i < 4; ++i)
	 {
		 if(digitsOut >= 4-i)
		 {
			 if(i < 3 && !some && !digits[i])
			 {
				 if(leadingSign)
				 {
					 temp[0] = str[strlen(str) -1];
					 str[strlen(str) -1] = ' ';
				 }
				 else temp[0] = ' ';
			 }
			 else
			 {
				 some = 1;
				 temp[0] = digits[i] + '0';
			 }
			 strcat(str, temp);
			 //
		 }
	 }
	 //LogTextMessage("string %s", str);
	

 }

 void writeNumSpaceStr(uint8_t num, char *str, uint8_t row, uint8_t col, uint8_t len)
 {
	 char tStr[21];
	 strcpy(tStr, "");
	 intToStr("",(int16_t)num, 2, 0, 0, tStr);
	 strCatCat(" ", str, tStr);
	 //LogTextMessage("row %u", row);
	 writeStr(tStr, row, col, len, 0);
 }





	