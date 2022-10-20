
#include <Bela.h>
#include "fileLoader.h"
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include "settings.h"
#include "synthStructs.h"
#include "helperFunctions.h"
#include "synthVariables.h"
#include "pitchTables.h"
#include "synthControls.h"


struct browseObj filBrowser[FILTYPES]; 
const char filError[6] = "file?";


int initFILES(uint8_t fType)
{	
	char basePath[MAXDPATHLEN];
	char curDir[7];
	browseCnt[fType].files = 0;
	browseCnt[fType].dirs = 0;
	
	//get the base folder based on the file type, set file index pointers to the first file
	if(fType == WAVE)
	{
		strcpy(curDir, WAVE_FOLDER);
		/*browseCnt[fType].dirs = 1;
		strcpy(dirs[fType][0].name, "AUDIO");
		dirs[fType][0].numFiles = 10;
		dirs[fType][0].insertAfter = &files[fType][9];
		
		const char auds[10][7] = {"IN MX", "IN L", "IN R", "POLY 1", "POLY 2", "MONO 1", "MONO 2", "MONO 3", "MONO 4", "MAIN"};

		for(uint32_t i = 0; i < 10; ++i)
		{
			struct filsList *cur = &files[fType][i];
			++browseCnt[fType].files;
			strcpy(cur->name, auds[i]);
			cur->filInd = i + 1;
			cur->dirInd = 0;
			if(i > 0) cur->prev = &files[fType][i-1];
			cur->next = &files[fType][i+1];
		}*/
		
		/*for(uint8_t curOsc = 0; curOsc < OSC_CNT; curOsc++) 
			curWavFile[curOsc] = &files[fType][0];*/
		
	}
	else if(fType == PATCH)
	{
		strcpy(curDir, PATCH_FOLDER);
		curPatchFile = &files[fType][0];
	}
	else
	{
		//rt_printf("ftype %u %s\n", fType, filError);
		return 0;
	}
	
	//make a path to the proper starting folder (root + file type)
	makeTempPath((char*)ROOT_FOLDER, curDir, basePath);

	//printf("scanning %s\n", basePath);

	//map the entire directory
	scanDir(basePath, fType);
	
	//point the first and last files at each other to make a full loop
	if(browseCnt[fType].files > 0)
	{
		uint8_t tInd = browseCnt[fType].files -1;
		files[fType][tInd].next = &files[fType][0];
		files[fType][0].prev = &files[fType][tInd];
	}
	return 1;
}

void scanDir(char *path, uint8_t fType)
{
	static uint8_t filesInDir;
	static uint8_t dirInd;
	char tStr[MAXDPATHLEN];
	DIR* dir;
	
	//open the path
	if(!(dir = opendir(path))) return;	
	
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
	while((filBrowser[fType].curObj = readdir(dir)))
	{
		//valid file: add it to the list
		if(filBrowser[fType].curObj->d_type != DT_DIR &&
			strncmp(filBrowser[fType].curObj->d_name, FILES_LIST, strlen(FILES_LIST)) != 0 && 
			strncmp(filBrowser[fType].curObj->d_name, DIRS_LIST, strlen(DIRS_LIST)) != 0) 		
		{
			//get the current file index
			uint8_t fileInd = browseCnt[fType].files + filesInDir;
			
			//increment the number of files in the directory
			filesInDir++;
			
			//copy the filename and make sure it's null terminated
			strncpy(files[fType][fileInd].name, filBrowser[fType].curObj->d_name, MAXFNAMELEN);	
			files[fType][fileInd].name[strlen(filBrowser[fType].curObj->d_name)] = '\0';
			
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
	closedir(dir);
	if (!(dir = opendir(path))) return;

	
	//loop through again to search sub-directories
	while ((filBrowser[fType].curObj = readdir(dir)))
	{		
		//valid subdirectory
		if(filBrowser[fType].curObj->d_name[0] != '.' && filBrowser[fType].curObj->d_type == DT_DIR)
		{
			//get the new directory's path
			makeTempPath(path, filBrowser[fType].curObj->d_name, tStr);
			
			//scan the new directory
			scanDir(tStr, fType);	
		}
	}
	closedir(dir);
} 


void incrementFileIndex(uint8_t fType, int8_t moveAmt, uint8_t findDir, filsList **ptr)
{
	struct filsList *startFile;
	struct filsList *curFile;
	uint8_t steps;
	
	if (ptr == NULL)
	{
		if (fType == WAVE) ptr = &curWavFile[oscInd][table_page];
		else if (fType == PATCH) ptr = &curPatchFile;
	}
	startFile = *ptr;
	
	//make sure there's anything useful to do	
	if(browseCnt[fType].files == 0 ||browseCnt[fType].dirs == 0 || (browseCnt[fType].dirs < 2 && findDir)) return;
	

	curFile = startFile;
	steps = (moveAmt < 0)? -moveAmt: moveAmt;
	//rt_printf("%d", moveAmt);
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
	
	*ptr = curFile;
}

void setFileIndexFromName(uint8_t fType, filsList **ptr, const char *fName)
{
	struct filsList *curFile;
    
	//make sure there's useful data
	if(browseCnt[fType].files == 0 ||browseCnt[fType].dirs == 0) return;
	
	curFile = &files[fType][0];
	if (fType == WAVE) *ptr = curFile;

	//find the target file/directory indexes........................................................................................................
	for(uint8_t i = 0; i < browseCnt[fType].files; ++i)
	{			
		if(strcmp(curFile->name, fName) == 0)
		{
			*ptr = curFile;
			return;
		}
		curFile = curFile->next;
	}
	rt_printf("couldn't find file %s\n", fName);
}


/* 
void setCurFile(filsList *curFile, uint8_t fType)
{
	if(fType == WAVE) curFile = curWavFile[osc];
	else if(fType == PATCH) curFile = curPatchFile;
} */


uint8_t savePatch()
{
	char path[MAXDPATHLEN + MAXFNAMELEN];
	char reviseName[MAXFNAMELEN];
	uint8_t fileInd;
	
	//get the save name (ignore trailing spaces)
	for(int32_t i = MAXFNAMELEN-2; i >= 0; i--)
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
		struct filsList *nextFile;
		struct filsList *prevFile;
		
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
			struct filsList *firstFile = nextFile;
			
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
	if((filBrowser[PATCH].curFile = open(path, O_CREAT | O_RDWR)) < 0) return 0;
	
	if(!writeReadPatch(1)) curPatchFile = &files[PATCH][fileInd];
	
	close(filBrowser[PATCH].curFile);
	
	//set the new file as the current loaded patch index
	
	
	return 1;
}




#if LOADTABLES

uint32_t save_data_arrays()
{
	//rt_printf("s");
	char path[50];
	char file[] = "DATA_DO_NOT_DELETE";
	int DATA_file;
	int bytesWritten;

	const void *DATA_SRC[] = {DATA_phase_width_incs, DATA_PHASEINCS, DATA_ATTACK_K, DATA_SEEK, DATA_SEEK_S_RATE, DATA_GAIN, DATA_VELGAIN, DATA_TIME, DATA_screens, DATA_SCREENS, DATA_noteLabels, BIG_GROUP, OTHER_GROUPS, DATA_parents, DATA_firstChild, 
		DATA_chan_pins, DATA_mx_pins, DATA_led_pins, DATA_lower_knob_pins, DATA_upper_knob_pins, 
		DATA_saveCopyStr, DATA_oscStr, DATA_lvlStr, DATA_arpNoteLeader, DATA_yesNoStr, DATA_onOffStr, DATA_startStopStr, DATA_copyWhat, DATA_envStr, DATA_filtStr, DATA_stageStr, DATA_notesStr, DATA_trackStr, DATA_velStr, DATA_loopStr, DATA_modStrA, DATA_modStrB, DATA_modStrO, DATA_recStr, DATA_timeStr, DATA_typeStr, DATA_favStr, DATA_nts, DATA_units};
	
	makeTempPath((char*)ROOT_FOLDER, file, path);
	
	//create the file
	if ((DATA_file = open(path, O_CREAT | O_RDWR)) < 0) return 0;
	
	for(int32_t i = 0; i < DATA_CNT; ++i)
	{
		uint8_t *p = (uint8_t *)DATA_SRC[i];
		int left = DATA_SZ[i];
		int to_write;
		
		while(left)
		{
			to_write = left > 256 ? 256 : left;
			if ((bytesWritten = write(DATA_file, p, to_write)) < to_write) return 0;
			p += bytesWritten;
			left -= bytesWritten;
		}
	}
	
	close(DATA_file);
	//rt_printf("sv");
	return 1;
	 
	//const char noteLabels[169][5];
	//const char screens[18][4][21];
 
}

#endif


uint32_t read_data_arrays()
{
	//rt_printf("s");
	char path[50];
	char file[] = "DATA_DO_NOT_DELETE";
	int DATA_file;
	int bytesRead;
	
	const void *DATA_DST[] = {phase_width_incs, PHASEINCS, ATTACK_K, SEEK, SEEK_S_RATE, GAIN, VELGAIN, TIME, screens, SCREENS, noteLabels, big_group, other_groups, parents, firstChild, 
		chan_pins, mx_pins, led_pins, lower_knob_pins, upper_knob_pins, 
		saveCopyStr, oscStr, lvlStr, arpNoteLeader, yesNoStr, onOffStr, startStopStr, copyWhat, envStr, filtStr, stageStr, notesStr, trackStr, velStr, loopStr, modStrA, modStrB, modStrO, recStr, timeStr, typeStr, favStr, nts, units};
	
	
	makeTempPath((char*)ROOT_FOLDER, file, path);
	
	//create the file
	if((DATA_file = open(path, O_RDONLY)) < 0) return 0;
	
	for(int32_t i = 0; i < DATA_CNT; ++i)
	{
		int8_t *p = (int8_t *)DATA_DST[i];
		uint16_t left = DATA_SZ[i];
		uint16_t to_read;
		
		while(left)
		{
			to_read = left;
			if(to_read > 256) to_read = 256;
			
			bytesRead = read(DATA_file, p, to_read);
			p += to_read;
			left -= to_read;
		}
		
	}
	
	close(DATA_file);
	
	
	//rt_printf("e");
	return 1;
	 
	//const char noteLabels[169][5];
	//const char screens[18][4][21];
 
}


//helpers
void  makeTempPath(char *basePath, char *newChunk, char *outputStr)
{
	strcpy(outputStr, basePath);
	if(strlen(outputStr)) strcat(outputStr, "/");
	strcat(outputStr, newChunk);
}

void splitDirFromPath(char *dir, char *path)
{
	strcpy(dir, "");
	int32_t endInd = strlen(path)-1;
	for(int32_t ind = endInd; ind >= 0; ind--)
	{
		if(path[ind] == '/')
		{
			int32_t length = endInd-ind;
			if(length >= MAXFNAMELEN-1) length = MAXFNAMELEN-1;
			strncpy(dir, path + (ind+1), length+1); 
			break;
		}
	}
}




void  favAction(uint8_t ind, uint8_t writeIt)
{
	//rt_printf("h");
	int bytesRead;
	char path[MAXDPATHLEN];
	char patch[MAXFNAMELEN];
	uint8_t mode = (writeIt)? (O_WRONLY | O_CREAT) : (O_RDONLY);
	makeTempPath((char*)ROOT_FOLDER, (char*)FAVS_FILE, path);
	//rt_printf("p %s", path);

	if((filBrowser[PATCH].curFile = open(path, mode)))
	{
		lseek(filBrowser[PATCH].curFile, MAXFNAMELEN * ind, SEEK_CUR);
		if(writeIt) bytesRead = write(filBrowser[PATCH].curFile, curPatchFile->name, MAXFNAMELEN);
		else bytesRead = read(filBrowser[PATCH].curFile, &patch, MAXFNAMELEN);
	}
	close(filBrowser[PATCH].curFile);
	
	if(!writeIt)
	{
		//rt_printf("%s", patch);
		setFileIndexFromName(PATCH, &curPatchFile, patch);
		FIL_update |= (1 << MAIN_FIL);
	}

}


uint32_t patchReadWrite(void *var, uint16_t size, uint8_t isWrite)
{
	if(isWrite) return write(filBrowser[PATCH].curFile, var, size);
	else return read(filBrowser[PATCH].curFile, var, size);
}
	

	
uint8_t writeReadPatch(uint8_t isWrite)
{
	uint32_t bytes;
	for(uint8_t i = 0; i < ptrCnt; ++i)
	{
		uint16_t rem = ptrSizes[i];
		char *pos = (char *)varPtrs[i];
		while(rem)
		{
			uint32_t amt = (rem < 200)? rem: 200;
			bytes = patchReadWrite(pos, amt, isWrite);
			if (bytes != amt)
			{
				rt_printf("patch read ptr %u expected %u bytes, read %u bytes\n", i, amt, bytes);
			}
			/* if(isWrite)  f_write(&filBrowser[PATCH].curFile, pos, amt, &bytes);
			else  f_read(&filBrowser[PATCH].curFile, pos, amt, &bytes); */
			pos += amt;
			rem -= amt;
		}
	}
	
	//bigger/weirder osc parent stuff
	for(int32_t i = 0; i < OSC_CNT; ++i)
	{
		for (int32_t j = 0; j < TABLE_CNT; ++j)
		{
			char wavFil[MAXFNAMELEN];
			char phaseFil[MAXFNAMELEN];
			//patchReadWrite(&arpeggio[i], sizeof(arpeggio[i]), isWrite);//) return 1;
			if (isWrite)
			{
				strcpy(wavFil, curWavFile[i][j]->name);
				strcpy(phaseFil, phaseFile[i][j]->name);
			}
			bytes = patchReadWrite(wavFil, MAXFNAMELEN, isWrite);
			bytes += patchReadWrite(phaseFil, MAXFNAMELEN, isWrite);//) return 1;	
			if (bytes < 2 * MAXFNAMELEN)
			{
				rt_printf("patch files expected %u bytes, read %u bytes\n", 2 * MAXFNAMELEN, bytes);
			}
			if (!isWrite)
			{
				setFileIndexFromName(WAVE, &curWavFile[i][j], wavFil);
				setFileIndexFromName(WAVE, &phaseFile[i][j], phaseFil);
			}
		}
	}
	return 0;
}

void readWaveFile(filsList* to_read, int8_t* data_ptr)
{
	char path[MAXDPATHLEN + MAXFNAMELEN];
	
	//make a path to the file
	makeTempPath(dirs[WAVE][to_read->dirInd].path, to_read->name, path);

	//printf("loading file %s\n", path);

	//open the file
	if ((filBrowser[WAVE].curFile = open(path, O_RDONLY)) < 0) return;

	//read the file
	int bytesLeft = TABLE_FULL_SIZE * 4;
	int bytesRead = 0;
	int fsize = 0;
	int8_t* p = data_ptr;

	off_t currentPos = lseek(filBrowser[WAVE].curFile, (size_t)0, SEEK_CUR);
	fsize = (int)lseek(filBrowser[WAVE].curFile, (size_t)0, SEEK_END);
	lseek(filBrowser[WAVE].curFile, currentPos, SEEK_SET);

	//printf("file size %d (expected %d)\n", fsize, bytesLeft);

	if (fsize < bytesLeft) //if the file is smaller than expected, only read in the length of the file
		bytesLeft = fsize;

	while (bytesLeft > 0)
	{
		bytesRead = read(filBrowser[WAVE].curFile, p, bytesLeft);
		bytesLeft -= bytesRead;
		p += bytesRead;
	}

	close(filBrowser[WAVE].curFile);
}


void checkFileQueue()
{
	static uint8_t curFIL = 0;

	if(FIL_update & (1 << curFIL))
	{
		//rt_printf("f %u", curFIL);
		FIL_update &= ~(1 << curFIL);
		 
		char path[MAXDPATHLEN + MAXFNAMELEN];
		struct filsList *curFile;
		int bytesRead = 0;
		uint8_t fType;
		uint8_t skip_it = 0;
		int curOsc = -1;
		int curTbl = -1;

		if(getOscTblInd(curFIL, curOsc, curTbl))
		{
			//printf("loading osc %d table %d\n", curOsc, curTbl);
			fType = WAVE;
			curFile = curWavFile[curOsc][curTbl];
			
			//toggles[curOsc] &= ~(((1 << 5) - 1) << bitAudio);

			//printf("getting dir ind %d\n", curFile->dirInd);

			/*if(curFile->dirInd == 0)
			{	
				toggles[curOsc] |= ((((curFile->filInd - 1) << 1) | 1) << bitAudio);
				skip_it = 1;
			}*/
		}
		else
		{
			//printf("loading patch\n");
			fType = PATCH;
			curFile = curPatchFile;
		}

		
		
		//make sure there's useful data
		if(!skip_it && browseCnt[fType].files)
		{
			//printf("getting path %d\n", curOsc);
			
			//make a path to the file
			makeTempPath(dirs[fType][curFile->dirInd].path, curFile->name, path);													
			
			//printf("loading file %s\n", path);
			
			//open the file
			if((filBrowser[fType].curFile = open(path, O_RDONLY)) < 0) return;
		
			//read the file
			if(fType == WAVE)
			{			
				int bytesLeft = TABLE_FULL_SIZE * 4;
				int fsize = 0;
				int8_t* p = (int8_t*)wavArray[curOsc][curTbl];

				off_t currentPos = lseek(filBrowser[fType].curFile, (size_t)0, SEEK_CUR);
				fsize = (int)lseek(filBrowser[fType].curFile, (size_t)0, SEEK_END);
				lseek(filBrowser[fType].curFile, currentPos, SEEK_SET);

				//printf("file size %d (expected %d)\n", fsize, bytesLeft);

				if (fsize < bytesLeft) //if the file is smaller than expected, only read in the length of the file
					bytesLeft = fsize;

				while (bytesLeft > 0)
				{
					bytesRead = read(filBrowser[fType].curFile, p, bytesLeft);
					bytesLeft -= bytesRead;
					p += bytesRead;
				}

				//printf("success\n");
				queueOscTbl(HARM_update, curOsc, curTbl);
				//int bytesLeft = WAVE_RES * 4;
				//int fsize = 0;
				//int8_t *p = (int8_t *)wavArray[curOsc][curTbl];

				//off_t currentPos = lseek(filBrowser[fType].curFile, (size_t)0, SEEK_CUR);
				//fsize = (int)lseek(filBrowser[fType].curFile, (size_t)0, SEEK_END);
				//lseek(filBrowser[fType].curFile, currentPos, SEEK_SET);
				//
				////printf("file size %d (expected %d)\n", fsize, bytesLeft);

				//if(fsize < bytesLeft) //if the file is smaller than expected, only read in the length of the file
				//	bytesLeft = fsize;

				//while (bytesLeft > 0) 
				//{
				//	bytesRead = read(filBrowser[fType].curFile, p, bytesLeft);
				//	bytesLeft -= bytesRead;
				//	p += bytesRead;
				//} 	

				////printf("success\n");
				//queueOscTbl(HARM_update, curOsc, curTbl);
				
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
				
			close(filBrowser[fType].curFile);
		} 
	}
	curFIL = indexIncrement(curFIL, 1, OSC_CNT * TABLE_CNT + 1);
}
