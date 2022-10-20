#ifndef FILELOADER_H
#define FILELOADER_H


#include <dirent.h>
#include "synthStructs.h"

//data structure for a file browser
struct browseObj {
	struct dirent *curObj;
	int curFile;
};



int initFILES(uint8_t fType);
void scanDir(char *path, uint8_t browserInd);
void incrementFileIndex(uint8_t fType, int8_t moveAmt, uint8_t findDir, filsList **ptr = NULL);
void setFileIndexFromName(uint8_t fType, filsList **ptr, const char *fName);
//void loadFile(uint8_t fType, uint8_t osc);
uint8_t savePatch();
//void __attribute__(( noinline )) setCurFile(filsList *curFile, uint8_t ftype);
void makeTempPath(char *basePath, char *newChunk, char *outputStr);
void incrementSaveDir(int8_t moveAmt);
void setFileIndCntForLCD(uint8_t ind, uint8_t cnt, char *str);
void splitDirFromPath(char *dir, char *path);
void  __attribute__(( noinline )) favAction(uint8_t ind, uint8_t writeIt);
uint32_t  patchReadWrite(void *var, uint16_t size, uint8_t isWrite);
uint8_t writeReadPatch(uint8_t isWrite);
void readWaveFile(filsList* to_read, int8_t* data_ptr);
void __attribute__(( noinline )) checkFileQueue();
uint32_t save_data_arrays();
uint32_t read_data_arrays();
uint32_t append_data_arrays();

#endif