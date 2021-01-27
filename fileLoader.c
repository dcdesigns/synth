#ifndef FILELOADER_C
#define FILELOADER_C


#include "./settings.h"
#include "./helperfunctions.c"
#include "./synthControls.h"

browseObj filBrowser[FILTYPES]; 
const char filError[6] = "file?";


void initFILES(uint8_t fType)
{	
	char tPath[MAXDPATHLEN];
	char basePath[MAXDPATHLEN];
	char curDir[7];
	char browserLab;
	browseCnt[fType].files = 0;
	browseCnt[fType].dirs = 0;
	
	//get the base folder based on the file type, set file index pointers to the first file
	if(fType == WAVE)
	{
		strcpy(curDir, WAVE_FOLDER);
		browseCnt[fType].dirs = 1;
		strcpy(dirs[fType][0].name, "AUDIO");
		dirs[fType][0].numFiles = 10;
		dirs[fType][0].insertAfter = &files[fType][9];
		
		const char auds[10][7] = {"IN MX", "IN L", "IN R", "POLY 1", "POLY 2", "MONO 1", "MONO 2", "MONO 3", "MONO 4", "MAIN"};

		for(uint32_t i = 0; i < 10; ++i)
		{
			filsList *cur = &files[fType][i];
			++browseCnt[fType].files;
			strcpy(cur->name, auds[i]);
			cur->filInd = i + 1;
			cur->dirInd = 0;
			if(i > 0) cur->prev = &files[fType][i-1];
			cur->next = &files[fType][i+1];
		}
		
		/* for(uint8_t curOsc = 0; curOsc < OSC_CNT; curOsc++) 
			curWavFile[curOsc] = &files[fType][0]; */
		
	}
	else if(fType == PATCH)
	{
		strcpy(curDir, PATCH_FOLDER);
		curPatchFile = &files[fType][0];
	}
	else
	{
		LogTextMessage("%s", filError);
		return;
	}
	
	//make a path to the proper starting folder (root + file type)
	makeTempPath((char*)ROOT_FOLDER, (char*)curDir, basePath);

	//map the entire directory
	scanDir(basePath, fType);
	
	//point the first and last files at each other to make a full loop
	if(browseCnt[fType].files > 0)
	{
		uint8_t tInd = browseCnt[fType].files -1;
		files[fType][tInd].next = &files[fType][0];
		files[fType][0].prev = &files[fType][tInd];
	}
}

void __attribute__(( noinline )) scanDir(char *path, uint8_t fType)
{
	uint8_t done;
	static uint8_t filesInDir;
	static uint8_t dirInd;
	char tStr[MAXDPATHLEN];
	DIR dir;
	
	//open the path
	if(f_opendir(&dir, path) != FR_OK) return;	
	
	//get the current directory index
	dirInd = browseCnt[fType].dirs;
	
	//increment the directory count
	browseCnt[fType].dirs++;

	//fill in the directory's initial data (path, name)
	strcpy(dirs[fType][dirInd].path, path);
	splitDirFromPath(dirs[fType][dirInd].name, path);
	dirs[fType][dirInd].insertAfter = NULL;
	filesInDir = 0;
	
	//loop through and add all non-directory files
	done = 0;
	while(!done)	
	{
		//read the next file
		if(f_readdir(&dir, &filBrowser[fType].curObj) != FR_OK) return; 
		
		//end of directory: go to next step
		else if(filBrowser[fType].curObj.fname[0] == 0) done = 1; 
	
		//valid file: add it to the list
		else if(filBrowser[fType].curObj.fattrib != AM_DIR && 
			strncmp(&filBrowser[fType].curObj.fname[0], FILES_LIST, strlen(FILES_LIST)) != 0 && 
			strncmp(&filBrowser[fType].curObj.fname[0], DIRS_LIST, strlen(DIRS_LIST)) != 0) 		
		{
			//get the current file index
			uint8_t fileInd = browseCnt[fType].files + filesInDir;
			
			//increment the number of files in the directory
			filesInDir++;
			
			//copy the filename and make sure it's null terminated
			strncpy(files[fType][fileInd].name, filBrowser[fType].curObj.fname, MAXFNAMELEN);	
			files[fType][fileInd].name[sizeof(files[fType][fileInd].name)-1] = '\0';
			
			//set the file's directory index
			files[fType][fileInd].dirInd = dirInd;
			
			//set the file's index within the directory
			files[fType][fileInd].filInd = filesInDir;
			
			//set the new file as the insertAfter pointer
			dirs[fType][dirInd].insertAfter = &files[fType][fileInd];
			
			//if it's not the first file, make pointer connections with the previous
			if(fileInd > 0)
			{
				files[fType][fileInd - 1].next = &files[fType][fileInd];
				files[fType][fileInd].prev = &files[fType][fileInd - 1];
			}			
		}
	}
	
	//set the number of files in the current directory
	dirs[fType][dirInd].numFiles = filesInDir;
	
	//increment the number of files overall
	browseCnt[fType].files += filesInDir;
		
	//reset the directory 
	if(f_opendir(&dir, path) != FR_OK) return;
	done = 0;
	
	//loop through again to search sub-directories
	while(!done)	
	{
		//read the next file
		if(f_readdir(&dir, &filBrowser[fType].curObj) != FR_OK) return;		

		//end of dir: exit
		if(filBrowser[fType].curObj.fname[0] == 0) done = 1;	
		
		//valid subdirectory
		else if(filBrowser[fType].curObj.fname[0] != '.' && filBrowser[fType].curObj.fattrib == AM_DIR)	
		{
			//get the new directory's path
			makeTempPath(path, filBrowser[fType].curObj.fname, tStr);
			
			//scan the new directory
			scanDir(tStr, fType);	
		}
	}
} 


void __attribute__(( noinline )) incrementFileIndex(uint8_t fType, int8_t moveAmt, uint8_t findDir)
{
	uint8_t fileInd;
	filsList *startFile;
	filsList *curFile;
	uint8_t steps;
	
	
	//make sure there's anything useful to do	
	if(browseCnt[fType].files == 0 ||browseCnt[fType].dirs == 0 || (browseCnt[fType].dirs < 2 && findDir)) return;
	
	//set the starting file index depending on the file type

	if(fType == WAVE) startFile = curWavFile[oscInd];
	else if(fType == PATCH) startFile = curPatchFile;
/* 	if(fType == WAVE) startFile = curWavFile[oscInd];
	else if(fType == PATCH) startFile = curPatchFile;
	else
	{
		//LogTextMessage("file type? %d", fType);
		return;
	} */
	
	curFile = startFile;
	steps = (moveAmt < 0)? -moveAmt: moveAmt;
	//LogTextMessage("%d", moveAmt);
	if(steps > 0)
	{
		do
		{
			steps--;
			curFile = (moveAmt > 0)? curFile->next : curFile->prev;			
		
		//loop until the steps are gone or you've found the first of a different directory or you've looped all the way around (unlikely but a nice safeguard)
		}while((findDir || steps > 0) && curFile != startFile && !((*curFile).filInd == 1 && (*curFile).dirInd != (*startFile).dirInd));
	}
	
	//set the starting file index depending on the file type
	
	if(fType == WAVE) curWavFile[oscInd] = curFile;
	else if(fType == PATCH) curPatchFile = curFile;
	else
	{
		//LogTextMessage("file type? %d", fType);
		return;
	}
}

void setFileIndexFromName(uint8_t fType, uint8_t osc, char *fName)
{
	char name[MAXFNAMELEN];
	filsList *curFile;
    
	//make sure there's useful data
	if(browseCnt[fType].files == 0 ||browseCnt[fType].dirs == 0) return;
	
	curFile = &files[fType][0];
	
	//find the target file/directory indexes........................................................................................................
	for(uint8_t i = 0; i < browseCnt[fType].files; ++i)
	{			
		if(strcmp(curFile->name, fName) == 0)
		{
			if(fType == WAVE)curWavFile[osc] = curFile;
			else if(fType == PATCH) 
			{
				//LogTextMessage("f");
				curPatchFile = curFile;
			}
			else
			{				
				//LogTextMessage("what file? %d", fType);
			}
			return;
		}
		curFile = curFile->next;
	}
	LogTextMessage("%s", filError);
	curFile = &files[fType][0]; 
}


/* 
void __attribute__(( noinline )) setCurFile(filsList *curFile, uint8_t fType)
{
	if(fType == WAVE) curFile = curWavFile[osc];
	else if(fType == PATCH) curFile = curPatchFile;
} */


uint8_t savePatch()
{
	UINT bytesRead;
	char path[MAXDPATHLEN + MAXFNAMELEN];
	char reviseName[MAXFNAMELEN];
	uint8_t fileInd;
	
	//get the save name (ignore trailing spaces)
	for(uint8_t i = MAXFNAMELEN-2; i >= 0; i--)
	{
		if(saveName[i] != ' ')
		{
			strncpy(reviseName, saveName, i+1);
			reviseName[i + 1] = '\0';
			break;
		}
		else if(i == 0) return 0;
	}
	
	//variable for the index of the new file (just to make code cleaner)
	fileInd = browseCnt[PATCH].files;
	browseCnt[PATCH].files++;
	dirs[PATCH][saveDirInd].numFiles++;
	strcpy(files[PATCH][fileInd].name, reviseName);
	files[PATCH][fileInd].dirInd = saveDirInd;
	files[PATCH][fileInd].filInd = dirs[PATCH][saveDirInd].numFiles;
	
	//get the path to the new file
	makeTempPath(dirs[PATCH][saveDirInd].path, reviseName, path);
	
	//if it's the first ever file, set up pointers (to itself)
	if(fileInd == 0)
	{
		files[PATCH][fileInd].next = &files[PATCH][fileInd];
		files[PATCH][fileInd].prev = &files[PATCH][fileInd];
	}
	
	//otherwise insert it within the linked list
	else
	{
		filsList *nextFile;
		filsList *prevFile;
		
		//if there were already files in the directory, add it after the last one
		if(dirs[PATCH][saveDirInd].insertAfter != NULL)
		{
			nextFile = dirs[PATCH][saveDirInd].insertAfter->next;
		}
		
		//otherwise we have to look for the insertion point from the beginning of the list
		else
		{
			uint8_t foundDir = 0;
			nextFile = &files[PATCH][0];
			filsList *firstFile = nextFile;
			
			//increment until we find either a higher directory or any directory after the save directory (in case we looped back to the beginning)
			while((foundDir == 0 || nextFile->dirInd == saveDirInd) && nextFile->dirInd <= saveDirInd)
			{
				//look at the next file
				nextFile = nextFile->next;
				
				//if we've looped around without finding the directory or any higher, put it at the end
				if(nextFile == firstFile)
					break;
				
				//if we find the directory, set a flag
				if(nextFile->dirInd == saveDirInd)
					foundDir = 1;
			}
		}
		
		//get the previous file
		prevFile = nextFile->prev;
		
		//reroute connections with the previous file
		prevFile->next = &files[PATCH][fileInd];
		files[PATCH][fileInd].prev = prevFile;
		
		//reroute connections with the next file
		nextFile->prev = &files[PATCH][fileInd];
		files[PATCH][fileInd].next = nextFile;
	}
	
	//set the new file as the updated insertion point
	dirs[PATCH][saveDirInd].insertAfter = &files[PATCH][fileInd];
	
	
	//create the file
	if(f_open(&filBrowser[PATCH].curFile, path, FA_CREATE_ALWAYS | FA_READ | FA_WRITE) != FR_OK) return 0;
	
	if(!writeReadPatch(1)) curPatchFile = &files[PATCH][fileInd];
	
	f_close(&filBrowser[PATCH].curFile);
	
	//set the new file as the current loaded patch index
	
	
	return 1;
}

#if LOADTABLES

uint32_t save_data_arrays()
{
	LogTextMessage("s");
	char path[50];
	char file[] = "DATA_DO_NOT_DELETE";
	FIL *data_file;
	UINT bytesRead;

	const void *ps[] = {data_phase_width_incs, data_PHASEINCS, data_ATTACK_K, data_SEEK, data_SEEK_S_RATE, data_GAIN, data_VELGAIN, data_TIME, data_screens, data_SCREENS, data_noteLabels};
	uint16_t sz[] = {256 * 2 * 4, 257 * 4, 128 * 4, 128 * 4, 128 * 4, 128 * 4, 128 * 4, 128 * 2, SCREEN_CNT * 4 * 21, SCREEN_CNT * 9, 169 * 5}; 
	makeTempPath((char*)ROOT_FOLDER, file, path);
	
	//create the file
	if(f_open(data_file, path, FA_CREATE_ALWAYS | FA_READ | FA_WRITE) != FR_OK) return 0;
	
	for(int32_t i = 0; i < 11; ++i)
	{
		void *p = (void *)ps[i];
		uint16_t left = sz[i];
		uint16_t write;
		
		while(left)
		{
			write = left;
			if(write > 256) write = 256;
			
			if(f_write(data_file, p, write, &bytesRead) != FR_OK) return 0;
			p += write;
			left -= write;
		}
	}
	
	f_close(data_file);
	LogTextMessage("e");
	return 1;
	 
	//const char noteLabels[169][5];
	//const char screens[18][4][21];
 
}

#endif

/* uint32_t append_data_arrays()
{
	LogTextMessage("sa");
	char path[50];
	char file[] = "DATA_DO_NOT_DELETE";
	FIL *data_file;
	UINT bytesRead;
	
	const void *ps[] = {data_screens, data_SCREENS, data_noteLabels};
	uint16_t sz[] = {SCREEN_CNT * 4 * 21, SCREEN_CNT * 9, 169 * 5}; 
	makeTempPath((char*)ROOT_FOLDER, file, path);
	
	//create the file
	if(f_open(data_file, path, FA_OPEN_EXISTING | FA_WRITE) != FR_OK) return 0;
	if(f_lseek(data_file, 5892) != FR_OK) return 0;
	
	for(int32_t i = 0; i < 3; ++i)
	{
		void *p = (void *)ps[i];
		uint16_t left = sz[i];
		uint16_t write;
		
		while(left)
		{
			write = left;
			if(write > 256) write = 256;
			
			if(f_write(data_file, p, write, &bytesRead) != FR_OK) return 0;
			p += write;
			left -= write;
		}
	}
	
	f_close(data_file);
	LogTextMessage("ea");
	return 1;
	 
	//const char noteLabels[169][5];
	//const char screens[18][4][21];
 
} */

uint32_t read_data_arrays()
{
	//LogTextMessage("s");
	char path[50];
	char file[] = "DATA_DO_NOT_DELETE";
	FIL *data_file;
	UINT bytesRead;
	
	const void *ps[] = {phase_width_incs, PHASEINCS, ATTACK_K, SEEK, SEEK_S_RATE, GAIN, VELGAIN, TIME, screens, SCREENS, noteLabels};
	uint16_t sz[] = {256 * 2 * 4, 257 * 4, 128 * 4, 128 * 4, 128 * 4, 128 * 4, 128 * 4, 128 * 2, SCREEN_CNT * 4 * 21, SCREEN_CNT * 9, 169 * 5}; 
	makeTempPath((char*)ROOT_FOLDER, file, path);
	
	//create the file
	if(f_open(data_file, path, FA_READ) != FR_OK) return 0;
	
	for(int32_t i = 0; i < 11; ++i)
	{
		void *p = (void *)ps[i];
		uint16_t left = sz[i];
		uint16_t read;
		
		while(left)
		{
			read = left;
			if(read > 256) read = 256;
			
			if(f_read(data_file, p, read, &bytesRead) != FR_OK) return 0;
			p += read;
			left -= read;
		}
	}
	
	f_close(data_file);
	//LogTextMessage("e");
	return 1;
	 
	//const char noteLabels[169][5];
	//const char screens[18][4][21];
 
}


//helpers
void  __attribute__(( noinline )) makeTempPath(char *basePath, char *newChunk, char *outputStr)
{
	if(strcmp(basePath, outputStr) != 0) strcpy(outputStr, basePath);
	if(strcmp(outputStr, "") != 0) strcat(outputStr, "/");
	strcat(outputStr, newChunk);
}

void __attribute__(( noinline )) splitDirFromPath(char *dir, char *path)
{
	strcpy(dir, "");
	uint8_t endInd = strlen(path)-1;
	for(uint8_t ind = endInd; ind >= 0; ind--)
	{
		if(path[ind] == '/')
		{
			uint8_t length = endInd-ind;
			if(length >= MAXFNAMELEN-1) length = MAXFNAMELEN-1;
			strncpy(dir, path + (ind+1), length+1); 
			break;
		}
	}
}




void  __attribute__(( noinline )) favAction(uint8_t ind, uint8_t writeIt)
{
	//LogTextMessage("h");

	char path[MAXDPATHLEN];
	char patch[MAXFNAMELEN];
	UINT bytesRead;
	uint8_t mode = (writeIt)? (FA_WRITE | FA_OPEN_ALWAYS) : (FA_READ | FA_OPEN_EXISTING);
	uint8_t res;
	makeTempPath((char*)ROOT_FOLDER, (char*)FAVS_FILE, path);
	//LogTextMessage("p %s", path);
	res = f_open(&filBrowser[PATCH].curFile, path, mode);
	if(!res) 
	{
		res = f_lseek(&filBrowser[PATCH].curFile, MAXFNAMELEN * ind);
		if(writeIt) res =  f_write(&filBrowser[PATCH].curFile, curPatchFile->name, MAXFNAMELEN, &bytesRead);
		else res = f_read(&filBrowser[PATCH].curFile, &patch, MAXFNAMELEN, &bytesRead);
	}
	f_close(&filBrowser[PATCH].curFile);
	
	if(!res && !writeIt)
	{
		//LogTextMessage("%s", patch);
		setFileIndexFromName(PATCH, 0, patch);
		FIL_update[MAINTOG] = 1;
	}

}


uint8_t __attribute__(( noinline )) patchReadWrite(void *var, uint16_t size, uint8_t isWrite)
{
	UINT bytes;
	if(isWrite) return f_write(&filBrowser[PATCH].curFile, var, size, &bytes);
	else return f_read(&filBrowser[PATCH].curFile, var, size, &bytes);
}
	

	
uint8_t __attribute__(( noinline )) writeReadPatch(uint8_t isWrite)
{
	
	UINT bytes;

	for(uint8_t i = 0; i < ptrCnt; ++i)
	{
		uint16_t rem = ptrSizes[i];
		char *pos = (char *)varPtrs[i];
		while(rem)
		{
			uint8_t amt = (rem < 200)? rem: 200;
			patchReadWrite(pos, amt, isWrite);
			/* if(isWrite)  f_write(&filBrowser[PATCH].curFile, pos, amt, &bytes);
			else  f_read(&filBrowser[PATCH].curFile, pos, amt, &bytes); */
			pos += amt;
			rem -= amt;
		}
	}
	
	//bigger/weirder osc parent stuff
	for(uint8_t i = 0; i < OSC_CNT; ++i)
	{
		char wavFil[MAXFNAMELEN];
		//patchReadWrite(&arpeggio[i], sizeof(arpeggio[i]), isWrite);//) return 1;
		if(isWrite) strcpy(wavFil, curWavFile[i]->name);
		patchReadWrite(wavFil, MAXFNAMELEN, isWrite);//) return 1;		
		if(!isWrite) setFileIndexFromName(WAVE, i, wavFil);	
	}

}


void __attribute__(( noinline )) checkFileQueue()
{
	static uint8_t curFIL = 0;

	if(FIL_update[curFIL] > 0)
	{
		//LogTextMessage("f %u", curFIL);
		FIL_update[curFIL] = 0;
		 
		char path[MAXDPATHLEN + MAXFNAMELEN];
		filsList *curFile;
		UINT bytesRead;
		uint8_t fType;
		uint8_t skip_it = 0;

		if(curFIL < OSC_CNT)
		{
			fType = WAVE;
			curFile = curWavFile[curFIL];
			HARM_update[curFIL] = -1;
			toggles[curFIL] &= ~(((1 << 5) - 1) << bitAudio);

			if(curFile->dirInd == 0)
			{	
				toggles[curFIL] |= ((((curFile->filInd - 1) << 1) | 1) << bitAudio);
				skip_it = 1;
			}
		}
		else
		{
			fType = PATCH;
			curFile = curPatchFile;
		}
		
		//make sure there's useful data
		if(!skip_it && browseCnt[fType].files)
		{
			//make a path to the file
			makeTempPath(dirs[fType][curFile->dirInd].path, curFile->name, path);													
			//open the file
			if(f_open(&filBrowser[fType].curFile, path, FA_READ | FA_OPEN_EXISTING) != FR_OK) return;
		
			//read the file
			if(fType == WAVE)
			{			
				uint16_t bytesLeft = WAVE_RES * 4;
				void *p = wavArray[curFIL];
				
				if(filBrowser[fType].curFile.fsize < bytesLeft) //if the file is smaller than expected, only read in the length of the file
					bytesLeft = filBrowser[fType].curFile.fsize;

				while (bytesLeft > 0) 
				{
					if(f_read(&filBrowser[fType].curFile, p, 256, &bytesRead) != FR_OK) break; //load the file into the array piece by piece
					else if (bytesRead < 256) break;
					bytesLeft -= bytesRead;
					p += bytesRead;
				} 		
				
			}
			else if(fType == PATCH)
			{
				uint32_t midiThru = SHIFTMASK(MAINTOG, bitMidiThru);
				writeReadPatch(0);
				/* for(uint8_t i = 0; i < OSC_CNT; ++i)
				{
					setFileIndexFromName(WAVE, i, curWavFile[i]->name);	
				} */
				//memset(&FIL_update[0], 1, OSC_CNT);
				if(midiThru) SETBIT(MAINTOG, bitMidiThru);
				initPatch(0, OSC_CNT-1);
				strcpy(saveName, curPatchFile->name);
				saveNameInd = strlen(curPatchFile->name)-1;	
			}
				
			f_close(&filBrowser[fType].curFile);
		} 
	}
	curFIL = indexIncrement(curFIL, 1, OSC_CNT + 1);
}

#endif