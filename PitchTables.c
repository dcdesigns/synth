#ifndef PITCH_TABLES_C
#define PITCH_TABLES_C

#include "./settings.h"

static uint32_t phase_width_incs[256][2] __attribute__ ((section (".sdram")));
static uint32_t PHASEINCS[257] __attribute__ ((section (".sram")));
static uint32_t ATTACK_K[128]; 
static uint32_t SEEK[128];
static uint32_t SEEK_S_RATE[128]; 
static uint32_t GAIN[128] __attribute__ ((section (".sdram"))); 
static uint32_t VELGAIN[128] __attribute__ ((section (".sdram")));
static uint16_t TIME[128] __attribute__ ((section (".sdram")));
static char screens[SCREEN_CNT][4][21] __attribute__ ((section (".sdram")));
static char SCREENS[SCREEN_CNT][9] __attribute__ ((section (".sdram")));
static char noteLabels[169][5] __attribute__ ((section (".sdram")));
static int8_t big_group[8][3] __attribute__ ((section (".sdram")));
static int8_t other_groups[24][4] __attribute__ ((section (".sdram")));
uint8_t parents[OSC_CHILD_CNT];
uint8_t firstChild[OSC_CNT];
char chan_pins[4][3] __attribute__ ((section (".sdram")));
char mx_pins[5][3] __attribute__ ((section (".sdram"))); 
char led_pins[4][3] __attribute__ ((section (".sdram")));
char lower_knob_pins[2][3] __attribute__ ((section (".sdram")));
char upper_knob_pins[2][3] __attribute__ ((section (".sdram")));

char saveCopyStr[3][7] __attribute__ ((section (".sdram")));
char oscStr[6][5] __attribute__ ((section (".sdram")));
char lvlStr[3][4] __attribute__ ((section (".sdram")));
char arpNoteLeader[3][4] __attribute__ ((section (".sdram")));
char yesNoStr[2][4] __attribute__ ((section (".sdram")));
char onOffStr[2][4] __attribute__ ((section (".sdram")));
char startStopStr[2][7] __attribute__ ((section (".sdram")));
char copyWhat[2][4] __attribute__ ((section (".sdram")));
char envStr[5][6] __attribute__ ((section (".sdram")));
char filtStr[3][4] __attribute__ ((section (".sdram")));
char stageStr[5][4] __attribute__ ((section (".sdram")));
char notesStr[2][4] __attribute__ ((section (".sdram")));
char trackStr[2][4] __attribute__ ((section (".sdram")));
char velStr[3][7] __attribute__ ((section (".sdram")));
char loopStr[4][6] __attribute__ ((section (".sdram")));
char modStrA[5][5] __attribute__ ((section (".sdram")));
char modStrB[9][4] __attribute__ ((section (".sdram")));
char modStrO[6][3] __attribute__ ((section (".sdram")));

char recStr[5] __attribute__ ((section (".sdram")));
char timeStr[6] __attribute__ ((section (".sdram")));
char typeStr[6] __attribute__ ((section (".sdram")));
char favStr[2][21] __attribute__ ((section (".sdram")));
char nts[12][4] __attribute__ ((section (".sdram")));
char units[3][3] __attribute__ ((section (".sdram")));


const uint8_t noteLabelTypes[3] = {82, 139, 168};
const uint8_t SET2 = 140;
const uint8_t maxNoteLabelInd = 168;

const uint16_t DATA_SZ[] = {256 * 2 * 4, 257 * 4, 128 * 4, 128 * 4, 128 * 4, 128 * 4, 128 * 4, 128 * 2, SCREEN_CNT * 4 * 21, SCREEN_CNT * 9, 169 * 5, 24, 96, 20, 6, 12, 15, 12, 6, 6, 
	21,30, 12, 12, 8, 8, 14, 8, 30, 12, 20, 8, 8, 21, 24, 25, 36, 18, 5, 6, 6, 42, 48, 9}; 
const int32_t DATA_CNT = 44;
	

//stored data arrays
#if LOADTABLES
const char DATA_saveCopyStr[3][7] = {"SAVED.", "DONE.", "      "};
const char DATA_oscStr[6][5] = {"POL1", "POL2", "MON1", "MON2", "MON3", "MON4"}; 
const char DATA_lvlStr[3][4] = {"LVL", "MST"};
const char DATA_arpNoteLeader[3][4] = {" P", " E:", " V:"};
const char DATA_yesNoStr[2][4] = {"NO", "YES"};
const char DATA_onOffStr[2][4] = {"OFF", "ON"};
const char DATA_startStopStr[2][7] = {"*START", "*STOP:"};
const char DATA_copyWhat[2][4] = {"ALL", "ARP"};
const char DATA_envStr[5][6] = {"PITCH", "F.CUT", "RES"};
const char DATA_filtStr[3][4] = {"LPF", "BPF", "HPF"};
const char DATA_stageStr[5][4] = {"1ST", "2ND", "3RD", "SUS", "REL"};
const char DATA_notesStr[2][4] = {"PIT", "VEL"};
const char DATA_trackStr[2][4] = {"YES", "ENV"};
const char DATA_velStr[3][7] = {"NONE","KEYVEL","WINDCC"};
const char DATA_loopStr[4][6] = {"TRIG-", "LOOP-", "SKIP", "ALL"};
const char DATA_modStrA[5][5] = {"----", "MAIN", "AUDM", "AUDL", "AUDR"};
const char DATA_modStrB[9][4] = {"WND", "MDW", "PBD", "SUS", "OUT", "AMP", "PIT", "FLT", "ARP"};
const char DATA_modStrO[6][3] = {"P1", "P2", "M1", "M2", "M3", "M4"}; 

const char DATA_recStr[5] = "*REC";
const char DATA_timeStr[6] = "TIME:";
const char DATA_typeStr[6] = "TYPE:";
const char DATA_favStr[2][21] = {"MAKE SURE THIS PATCH", "  HAS BEEN SAVED!   "};
const char DATA_nts[12][4] = {"C  ", "C #", "D  ", "D #","E  ","F  ","F #", "G  ","G #", "A  ","A #", "B  "};
const char DATA_units[3][3] = {"s", "Hz", "kH"};


const char DATA_chan_pins[4][3] = {"A4", "A5", "A6", "A7"};
const char DATA_mx_pins[5][3] = {"C0", "C1", "C2", "C3", "C4"};
const char DATA_led_pins[4][3] = {"C5", "A0", "A1", "A2"};
const char DATA_lower_knob_pins[2][3] = {"B6", "B7"};
const char DATA_upper_knob_pins[2][3] = {"B0", "B1"};

const uint8_t DATA_parents[OSC_CHILD_CNT] = {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,2,3,4,5};
const uint8_t DATA_firstChild[OSC_CNT] = {0,8,16,17,18,19};

const int8_t BIG_GROUP[8][3] = {
	{bitOsc, -1, 0},
	{bitHarms, HARMONIC, EX_HARM},
	{bitAEnv, AMPENV, 0},
	{bitPEnv, PITENV, 0},
	{bitFilt, FILTER, EX_FILT},
	{bitFEnv, FILTENV, 0},
	{bitArp, ARPEGSETUP, 0},
	{bitMain, OUTS, 0}
};

const int8_t OTHER_GROUPS[24][4] = {
	{E_OSC, bitMod, MODA, 0},									//lower mid row		
	{E_OSC, bitWave, WAVETBL, EX_WAVE}, 			
	{E_OSC, bitPoly, PITCH, EX_POLY}, 
	{E_OSC, bitPhase, PHASE, EX_HARM}, 		
	{E_OSC, bitNotes, NOTES, 0}, 
	{E_OSC, bitEnvs, MIDIINS, 0}, 
	{E_OSC, bitKeyVel, MIDICCS, 0}, 
	{E_OSC, bitHold, -1, EX_HOLD1},	
	
	{MAINTOG, bitMidiThru, -1, 0}, 								//lower left row
	{MAINTOG, bitSolo, -1, 0}, 
	{E_OSC, bitPitRatio, PITRATIO, EX_PIT_RATIO}, //{MAINTOG, bitHoldAll, -1, EX_HOLD_ALL}, 
	{MAINTOG, bitArpSync, -1, EX_SYNC},
	{MAINTOG, bitDrum, ARPEGNOTES, EX_DRUM}, 	
	{E_OSC, -1, -1, EX_ARPNOTREC}, 		
				
	{MAINTOG, bitRoute, -1, 0}, 	{MAINTOG, bitCopy, -1, 0}, 	//columns
	{E_OSC, -1, -1, EX_PATSVLD}, 	{E_OSC, -1, -1, EX_PATRNDCLR}, 
	{E_OSC, -1, FAVS, EX_FAV1},		{E_OSC, -1, FAVS, EX_FAV2},	
	{E_OSC, -1, FAVS, EX_FAV3}, 	{E_OSC, -1, FAVS, EX_FAV4},		
	{E_OSC, -1, -1, EX_TRIG_ON}, 	{E_OSC, -1, -1, EX_TRIG_OFF}
};

const uint32_t DATA_phase_width_incs[256][2] = {
	{0, 4194304}, {536870912, 4227330}, {357913941, 4244039}, {268435456, 4260880}, 
 	{214748365, 4277856}, {178956971, 4294967}, {153391689, 4312216}, {134217728, 4329604}, 
 	{119304647, 4347133}, {107374182, 4364804}, {97612893, 4382620}, {89478485, 4400581}, 
 	{82595525, 4418691}, {76695845, 4436950}, {71582788, 4455360}, {67108864, 4473924}, 
 	{63161284, 4492644}, {59652324, 4511520}, {56512728, 4530556}, {53687091, 4549753}, 
 	{51130563, 4569114}, {48806447, 4588640}, {46684427, 4608334}, {44739243, 4628198}, 
 	{42949673, 4648233}, {41297762, 4668443}, {39768216, 4688829}, {38347922, 4709394}, 
 	{37025580, 4730140}, {35791394, 4751070}, {34636833, 4772186}, {33554432, 4793490}, 
 	{32537631, 4814986}, {31580642, 4836675}, {30678338, 4858560}, {29826162, 4880645}, 
 	{29020049, 4902931}, {28256364, 4925421}, {27531842, 4948119}, {26843546, 4971027}, 
 	{26188825, 4994148}, {25565282, 5017485}, {24970740, 5041041}, {24403223, 5064820}, 
 	{23860929, 5088824}, {23342214, 5113056}, {22845571, 5137521}, {22369621, 5162220}, 
 	{21913098, 5187159}, {21474836, 5212339}, {21053761, 5237765}, {20648881, 5263440}, 
 	{20259280, 5289369}, {19884108, 5315554}, {19522579, 5341999}, {19173961, 5368709}, 
 	{18837576, 5395688}, {18512790, 5422939}, {18199014, 5450466}, {17895697, 5478275}, 
 	{17602325, 5506368}, {17318417, 5534752}, {17043521, 5563429}, {16777216, 5592405}, 
 	{16519105, 5621685}, {16268816, 5651273}, {16025997, 5681174}, {15790321, 5711393}, 
 	{15561476, 5741935}, {15339169, 5772806}, {15123124, 5804010}, {14913081, 5835553}, 
 	{14708792, 5867442}, {14510025, 5899680}, {14316558, 5932275}, {14128182, 5965232}, 
 	{13944699, 5998558}, {13765921, 6032257}, {13591669, 6066338}, {13421773, 6100806}, 
 	{13256072, 6135668}, {13094412, 6170930}, {12936648, 6206600}, {12782641, 6242685}, 
 	{12632257, 6279192}, {12485370, 6316128}, {12341860, 6353502}, {12201612, 6391320}, 
 	{12064515, 6429592}, {11930465, 6468324}, {11799361, 6507526}, {11671107, 6547206}, 
 	{11545611, 6587373}, {11422785, 6628036}, {11302546, 6669204}, {11184811, 6710886}, 
 	{11069503, 6753093}, {10956549, 6795834}, {10845877, 6839120}, {10737418, 6882960}, 
 	{10631107, 6927367}, {10526881, 6972350}, {10424678, 7017920}, {10324441, 7064091}, 
 	{10226113, 7110873}, {10129640, 7158279}, {10034970, 7206321}, {9942054, 7255012}, 
 	{9850842, 7304366}, {9761289, 7354396}, {9673350, 7405116}, {9586981, 7456540}, 
 	{9502140, 7508684}, {9418788, 7561562}, {9336885, 7615190}, {9256395, 7669584}, 
 	{9177281, 7724761}, {9099507, 7780738}, {9023041, 7837532}, {8947849, 7895160}, 
 	{8873899, 7953643}, {8801162, 8012999}, {8729608, 8073247}, {8659208, 8134408}, 
 	{8589935, 8196502}, {8521761, 8259552}, {8454660, 8323580}, {8388608, 8388608}, 
 	{8323580, 8454660}, {8259552, 8521761}, {8196502, 8589935}, {8134408, 8659208}, 
 	{8073247, 8729608}, {8012999, 8801162}, {7953643, 8873899}, {7895160, 8947849}, 
 	{7837532, 9023041}, {7780738, 9099507}, {7724761, 9177281}, {7669584, 9256395}, 
 	{7615190, 9336885}, {7561562, 9418788}, {7508684, 9502140}, {7456540, 9586981}, 
 	{7405116, 9673350}, {7354396, 9761289}, {7304366, 9850842}, {7255012, 9942054}, 
 	{7206321, 10034970}, {7158279, 10129640}, {7110873, 10226113}, {7064091, 10324441}, 
 	{7017920, 10424678}, {6972350, 10526881}, {6927367, 10631107}, {6882960, 10737418}, 
 	{6839120, 10845877}, {6795834, 10956549}, {6753093, 11069503}, {6710886, 11184811}, 
 	{6669204, 11302546}, {6628036, 11422785}, {6587373, 11545611}, {6547206, 11671107}, 
 	{6507526, 11799361}, {6468324, 11930465}, {6429592, 12064515}, {6391320, 12201612}, 
 	{6353502, 12341860}, {6316128, 12485370}, {6279192, 12632257}, {6242685, 12782641}, 
 	{6206600, 12936648}, {6170930, 13094412}, {6135668, 13256072}, {6100806, 13421773}, 
 	{6066338, 13591669}, {6032257, 13765921}, {5998558, 13944699}, {5965232, 14128182}, 
 	{5932275, 14316558}, {5899680, 14510025}, {5867442, 14708792}, {5835553, 14913081}, 
 	{5804010, 15123124}, {5772806, 15339169}, {5741935, 15561476}, {5711393, 15790321}, 
 	{5681174, 16025997}, {5651273, 16268816}, {5621685, 16519105}, {5592405, 16777216}, 
 	{5563429, 17043521}, {5534752, 17318417}, {5506368, 17602325}, {5478275, 17895697}, 
 	{5450466, 18199014}, {5422939, 18512790}, {5395688, 18837576}, {5368709, 19173961}, 
 	{5341999, 19522579}, {5315554, 19884108}, {5289369, 20259280}, {5263440, 20648881}, 
 	{5237765, 21053761}, {5212339, 21474836}, {5187159, 21913098}, {5162220, 22369621}, 
 	{5137521, 22845571}, {5113056, 23342214}, {5088824, 23860929}, {5064820, 24403223}, 
 	{5041041, 24970740}, {5017485, 25565282}, {4994148, 26188825}, {4971027, 26843546}, 
 	{4948119, 27531842}, {4925421, 28256364}, {4902931, 29020049}, {4880645, 29826162}, 
 	{4858560, 30678338}, {4836675, 31580642}, {4814986, 32537631}, {4793490, 33554432}, 
 	{4772186, 34636833}, {4751070, 35791394}, {4730140, 37025580}, {4709394, 38347922}, 
 	{4688829, 39768216}, {4668443, 41297762}, {4648233, 42949673}, {4628198, 44739243}, 
 	{4608334, 46684427}, {4588640, 48806447}, {4569114, 51130563}, {4549753, 53687091}, 
 	{4530556, 56512728}, {4511520, 59652324}, {4492644, 63161284}, {4473924, 67108864}, 
 	{4455360, 71582788}, {4436950, 76695845}, {4418691, 82595525}, {4400581, 89478485}, 
 	{4382620, 97612893}, {4364804, 107374182}, {4347133, 119304647}, {4329604, 134217728}, 
 	{4312216, 153391689}, {4294967, 178956971}, {4277856, 214748365}, {4260880, 268435456}, 
 	{4244039, 357913941}, {4227330, 536870912}, {4210752, 1073741824}, {4194304, 0}
};

const uint32_t DATA_PHASEINCS[257] = {
	0x000005EA, 0x00000644, 0x000006A3, 0x00000708, 0x00000773, 0x000007E5, 0x0000085D, 
	0x000008DC, 0x00000963, 0x000009F2, 0x00000A89, 0x00000B2A, 0x00000BD4, 0x00000C88, 
	0x00000D46, 0x00000E10, 0x00000EE7, 0x00000FC9, 0x000010BA, 0x000011B8, 0x000012C6, 
	0x000013E4, 0x00001513, 0x00001653, 0x000017A7, 0x0000190F, 0x00001A8D, 0x00001C21, 
	0x00001DCD, 0x00001F93, 0x00002173, 0x00002370, 0x0000258C, 0x000027C8, 0x00002A25, 
	0x00002CA7, 0x00002F4E, 0x0000321E, 0x00003519, 0x00003842, 0x00003B9A, 0x00003F25, 
	0x000042E7, 0x000046E1, 0x00004B18, 0x00004F8F, 0x0000544A, 0x0000594D, 0x00005E9D, 
	0x0000643D, 0x00006A33, 0x00007083, 0x00007734, 0x00007E4B, 0x000085CD, 0x00008DC2, 
	0x00009630, 0x00009F1E, 0x0000A894, 0x0000B29A, 0x0000BD39, 0x0000C87A, 0x0000D465, 
	0x0000E107, 0x0000EE68, 0x0000FC95, 0x00010B9A, 0x00011B84, 0x00012C60, 0x00013E3C, 
	0x00015128, 0x00016535, 0x00017A72, 0x000190F3, 0x0001A8CB, 0x0001C20D, 0x0001DCD0, 
	0x0001F92A, 0x00021734, 0x00023708, 0x000258BF, 0x00027C78, 0x0002A251, 0x0002CA6A, 
	0x0002F4E5, 0x000321E7, 0x00035196, 0x0003841A, 0x0003B9A0, 0x0003F255, 0x00042E69, 
	0x00046E0F, 0x0004B17E, 0x0004F8F0, 0x000544A1, 0x000594D3, 0x0005E9C9, 0x000643CD, 
	0x0006A32B, 0x00070835, 0x00077340, 0x0007E4AA, 0x00085CD1, 0x0008DC1E, 0x000962FD, 
	0x0009F1E0, 0x000A8943, 0x000B29A6, 0x000BD393, 0x000C879A, 0x000D4656, 0x000E1069, 
	0x000EE681, 0x000FC953, 0x0010B9A3, 0x0011B83C, 0x0012C5F9, 0x0013E3C0, 0x00151286, 
	0x0016534C, 0x0017A726, 0x00190F34, 0x001A8CAC, 0x001C20D3, 0x001DCD02, 0x001F92A7, 
	0x00217345, 0x00237078, 0x00258BF2, 0x0027C781, 0x002A250C, 0x002CA698, 0x002F4E4B, 
	0x00321E69, 0x00351958, 0x003841A6, 0x003B9A04, 0x003F254E, 0x0042E68B, 0x0046E0F0, 
	0x004B17E5, 0x004F8F01, 0x00544A17, 0x00594D31, 0x005E9C96, 0x00643CD2, 0x006A32B1, 
	0x0070834C, 0x00773407, 0x007E4A9B, 0x0085CD15, 0x008DC1E1, 0x00962FC9, 0x009F1E03, 
	0x00A8942E, 0x00B29A62, 0x00BD392D, 0x00C879A3, 0x00D46562, 0x00E10697, 0x00EE680F, 
	0x00FC9536, 0x010B9A2B, 0x011B83C2, 0x012C5F93, 0x013E3C06, 0x0151285D, 0x016534C3, 
	0x017A725A, 0x0190F347, 0x01A8CAC3, 0x01C20D2F, 0x01DCD01D, 0x01F92A6D, 0x02173456, 
	0x02370783, 0x0258BF26, 0x027C780B, 0x02A250BA, 0x02CA6987, 0x02F4E4B4, 0x0321E68D, 
	0x03519586, 0x03841A5D, 0x03B9A03A, 0x03F254D9, 0x042E68AC, 0x046E0F07, 0x04B17E4B, 
	0x04F8F017, 0x0544A173, 0x0594D30D, 0x05E9C968, 0x0643CD1A, 0x06A32B0D, 0x070834BA, 
	0x07734075, 0x07E4A9B2, 0x085CD157, 0x08DC1E0D, 0x0962FC96, 0x09F1E02D, 0x0A8942E7, 
	0x0B29A61A, 0x0BD392D0, 0x0C879A35, 0x0D46561A, 0x0E106974, 0x0EE680E9, 0x0FC95364, 
	0x10B9A2AF, 0x11B83C1A, 0x12C5F92C, 0x13E3C05A, 0x151285CE, 0x16534C35, 0x17A725A0, 
	0x190F346A, 0x1A8CAC34, 0x1C20D2E8, 0x1DCD01D3, 0x1F92A6C8, 0x2173455E, 0x23707834, 
	0x258BF258, 0x27C780B4, 0x2A250B9B, 0x2CA69869, 0x2F4E4B3F, 0x321E68D4, 0x35195868, 
	0x3841A5D0, 0x3B9A03A5, 0x3F254D90, 0x42E68ABB, 0x46E0F069, 0x4B17E4B1, 0x4F8F0169, 
	0x544A1737, 0x594D30D3, 0x5E9C967E, 0x643CD1A7, 0x6A32B0CF, 0x70834BA0, 0x7734074B, 
	0x7E4A9B21, 0x85CD1577, 0x8DC1E0D2, 0x962FC962, 0x9F1E02D2, 0xA8942E6D, 0xB29A61A6, 
	0xBD392CFC, 0xC879A34F, 0xD465619E, 0xE1069740, 0xEE680E96
};




const uint32_t DATA_ATTACK_K[128] = {
	0x7FFFFFFF, 0x3CF3CF3C, 0x238E38E3, 0x19191918, 0x1364D936, 0x0F3CF3CF, 0x0C30C30C, 0x0A28A28A, 0x085DB308, 0x071C71C7, 0x0602675C, 
	0x05145145, 0x045A8ECD, 0x03BE20EF, 0x0341CA95, 0x02D35AFC, 0x02775E36, 0x022A7338, 0x01E79E79, 0x01AFB9D8, 0x017DE952, 0x0154451E, 
	0x013040A7, 0x01111111, 0x00F57403, 0x00DD8E1B, 0x00C86A78, 0x00B60B60, 0x00A57EB5, 0x0096DD95, 0x008A1639, 0x007E6B74, 0x007432D6, 
	0x006AE01A, 0x0062947C, 0x005B191F, 0x005447A3, 0x004E2FC4, 0x00489FC5, 0x00438C88, 0x003EF471, 0x003AC16F, 0x0036EA55, 0x00336699, 
	0x00302E5F, 0x002D35AF, 0x002A77B4, 0x0027F383, 0x0025A01D, 0x00237987, 0x00217C0C, 0x001FA43D, 0x001DECD4, 0x001C5571, 0x001AD7F7, 
	0x001975F4, 0x00182BA8, 0x0016F750, 0x0015D749, 0x0014C908, 0x0013CB6B, 0x0012DE33, 0x0011FE98, 0x00112D3C, 0x00106865, 0x000FAEA6, 
	0x000EFFE2, 0x000E5ADC, 0x000DBF01, 0x000D2BC3, 0x000CA0FA, 0x000C1D60, 0x000BA0E6, 0x000B2ACF, 0x000ABAC2, 0x000A50AA, 0x0009EBEC, 
	0x00098C3B, 0x0009314F, 0x0008DAB3, 0x00088831, 0x000839BB, 0x0007EF17, 0x0007A7C5, 0x000763C1, 0x000722DD, 0x0006E4D5, 0x0006A9A0, 
	0x000670FE, 0x00063AE9, 0x00060712, 0x0005D575, 0x0005A5F6, 0x0005787B, 0x00054CDA, 0x000522FE, 0x0004FAE2, 0x0004D453, 0x0004AF5D, 
	0x00048BD2, 0x000469A4, 0x000448CE, 0x00042942, 0x00040A54, 0x0003E9B8, 0x0003C556, 0x00039BE0, 0x00036CCC, 0x00033860, 0x0002FFA9, 
	0x0002C41D, 0x00028786, 0x00024B99, 0x000211CD, 0x0001DB49, 0x0001A8D1, 0x00017B35, 0x000152E8, 0x00012FC6, 0x0001115D, 0x0000F721, 
	0x0000E082, 0x0000CCFB, 0x0000BC16, 0x0000AD6E, 0x0000A0AE, 0x0000958D, 0x00008BCF
};




const uint32_t DATA_SEEK[128] =  {
	0x73333332, 0x44967113, 0x28CB6BE5, 0x2003776F, 0x182CDD6C, 0x13697FF9, 0x1036D92A, 0x0D4AF0BA, 0x0B433A93, 0x0974F107, 0x07EE3E7E, 
	0x06D41946, 0x05E0330D, 0x051152E0, 0x04637C9B, 0x03D1E3A9, 0x0357E0CB, 0x02F15674, 0x029ACAFB, 0x024CBFF9, 0x020B67A6, 0x01D43143, 
	0x01A2D0A7, 0x0178F758, 0x01521D05, 0x013147E6, 0x01153CE7, 0x00FB602C, 0x00E53A73, 0x00D0EAE6, 0x00BF6D1C, 0x00AF67D2, 0x00A12AFA, 
	0x00947B38, 0x0088E5A6, 0x007E9043, 0x007521E9, 0x006CB204, 0x0064FC26, 0x005DF19A, 0x00579DD3, 0x0051BB69, 0x004C6EE2, 0x0047933F, 
	0x00431E46, 0x003EF911, 0x003B2B98, 0x0037AD5F, 0x003476C5, 0x003178A1, 0x002EAF87, 0x002C1E9A, 0x0029BA72, 0x00278550, 0x00257120, 
	0x00238545, 0x0021B9F6, 0x00200CDC, 0x001E7BC2, 0x001D01BE, 0x001BA049, 0x001A5585, 0x00191FBC, 0x0017FB66, 0x0016E959, 0x0015E4F1, 
	0x0014F235, 0x00140B6E, 0x001332B1, 0x001265C5, 0x0011A3F8, 0x0010EBA6, 0x00103E34, 0x000F9927, 0x000EFC20, 0x000E66C2, 0x000DD760, 
	0x000D4DEB, 0x000CCA48, 0x000C4B4D, 0x000BD202, 0x000B5C74, 0x000AEC0A, 0x000A7F80, 0x000A1693, 0x0009B206, 0x000950E0, 0x0008F382, 
	0x00089968, 0x000842E0, 0x0007EFAD, 0x0007A003, 0x000753FD, 0x00070B4E, 0x0006C62C, 0x00068420, 0x00064574, 0x000609D6, 0x0005D139, 
	0x00059B55, 0x00056835, 0x000537AE, 0x00050993, 0x0004DDD3, 0x0004B432, 0x00048CA0, 0x0004671C, 0x00044362, 0x00042172, 0x0004011E, 
	0x0003E267, 0x0003C523, 0x0003A946, 0x00038EC1, 0x00037574, 0x00035D54, 0x00034657, 0x0003305F, 0x00031B6D, 0x00030767, 0x0002F447, 
	0x0002E204, 0x0002D088, 0x0002BFD6, 0x0002AFD8, 0x0002A084, 0x000291DB, 0x00020000
};


const uint32_t DATA_SEEK_S_RATE[128] = {
	0x112814BF, 0x09564743, 0x063269CE, 0x0485F7D1, 0x036DC84D, 0x02B85470, 0x022C9C4C, 0x01C24F31, 0x017137F8, 0x012ED490, 0x00FB3553, 
	0x00D1D8E5, 0x00B02483, 0x00955CF5, 0x007F3422, 0x006CBA09, 0x005DCDB9, 0x0051344C, 0x0046BAD2, 0x003DD3CD, 0x00366895, 0x003008F8, 
	0x002A9239, 0x0025EA82, 0x0021DC0F, 0x001E5EDC, 0x001B5780, 0x0018AE21, 0x00165775, 0x00144B31, 0x00127AFF, 0x0010DFA3, 0x000F7145, 
	0x000E2BC3, 0x000D071D, 0x000C0195, 0x000B1607, 0x000A41B0, 0x0009818D, 0x0008D38E, 0x000835CE, 0x0007A644, 0x00072341, 0x0006AB59, 
	0x00063DF7, 0x0005D978, 0x00057D18, 0x00052845, 0x0004D9E6, 0x000491B3, 0x00044EE9, 0x00041116, 0x0003D7BD, 0x0003A294, 0x00037111, 
	0x000342FF, 0x00031819, 0x0002F014, 0x0002CAAE, 0x0002A7B6, 0x000286F6, 0x00026848, 0x00024B7D, 0x0002307C, 0x00021712, 0x0001FF2B, 
	0x0001E8AD, 0x0001D37B, 0x0001BF7C, 0x0001AC9C, 0x00019AC6, 0x000189E6, 0x000179F4, 0x00016AD8, 0x00015C86, 0x00014EE7, 0x000141E2, 
	0x0001355E, 0x0001294F, 0x00011DA7, 0x0001125B, 0x00010765, 0x0000FCBF, 0x0000F267, 0x0000E85C, 0x0000DE99, 0x0000D520, 0x0000CBF2, 
	0x0000C30D, 0x0000BA73, 0x0000B223, 0x0000AA1E, 0x0000A264, 0x00009AF5, 0x000093D1, 0x00008CF7, 0x00008666, 0x0000801C, 0x00007A1B, 
	0x00007461, 0x00006EF2, 0x000069CD, 0x000064F0, 0x0000605B, 0x00005C0A, 0x000057FA, 0x00005429, 0x00005092, 0x00004D32, 0x00004A07, 
	0x0000470D, 0x00004440, 0x0000419F, 0x00003F25, 0x00003CD1, 0x00003AA0, 0x00003890, 0x0000369E, 0x000034C9, 0x0000330E, 0x0000316C, 
	0x00002FE2, 0x00002E6D, 0x00002D0C, 0x00002BBF, 0x00002A83, 0x00002958, 0x00000000
}; 

// const uint32_t DATA_GAIN[128] = {
	// 0x00000000, 0x00078961, 0x000F12C2, 0x00169C23, 0x001E7967, 0x002656AB, 0x002E87D2, 0x0036B8F9, 0x003EEA20, 0x0047C30D, 0x00509BF9, 
	// 0x005A1CAC, 0x00639D5E, 0x006DC5D6, 0x00784230, 0x0083126E, 0x008E8A71, 0x009A5657, 0x00A6CA03, 0x00B3E575, 0x00C154C9, 0x00CFBFC6, 
	// 0x00DE7EA5, 0x00EE392E, 0x00FE9B7B, 0x010FA58F, 0x0121AB4B, 0x013458CD, 0x014801F7, 0x015CA6CA, 0x0171F362, 0x01883BA3, 0x019FD36F, 
	// 0x01B81301, 0x01D1A21E, 0x01EC2CE4, 0x02080735, 0x0224DD2F, 0x0242AED1, 0x0261CFFE, 0x0282949A, 0x02A454DE, 0x02C764AD, 0x02EBC408, 
	// 0x031172EF, 0x0338C543, 0x03616723, 0x038B588E, 0x03B6ED67, 0x03E425AE, 0x0412AD81, 0x04432CA5, 0x0474FB54, 0x04A86D71, 0x04DDD6E0, 
	// 0x0514E3BC, 0x054D9407, 0x05883BA3, 0x05C486AD, 0x0602C908, 0x064302B4, 0x0684DFCE, 0x06C9081C, 0x070ED3D8, 0x0756EAC8, 0x07A0F909, 
	// 0x07ECFE9B, 0x083B4F61, 0x088B9778, 0x08DE2AC3, 0x0932B55E, 0x0989DF11, 0x09E30014, 0x0A3E6C4C, 0x0A9C779A, 0x0AFCCE1C, 0x0B5F6FD2, 
	// 0x0BC45CBB, 0x0C2BE8BB, 0x0C9667B5, 0x0D057D17, 0x0D7A786C, 0x0DF6A93F, 0x0E7B0B38, 0x0F09E98D, 0x0FA3981F, 0x104A0E40, 0x10FEEF5E, 
	// 0x11C2E33E, 0x1297E132, 0x137EE4E2, 0x147991BC, 0x1588E368, 0x16AED139, 0x17EC56D5, 0x194317AC, 0x1AB40F66, 0x1C40E171, 0x1DEADD58, 
	// 0x1FB3A68A, 0x219C38B0, 0x23A63736, 0x25D29DC6, 0x282363B2, 0x2A9984A0, 0x2D35FC3A, 0x2FF822BB, 0x32DE54B4, 0x35E69AD3, 0x390F51AC, 
	// 0x3C56D5CF, 0x3FBB2FEB, 0x433B1076, 0x46D4801E, 0x4A862F58, 0x4E4DD2F1, 0x5229C779, 0x5618BD65, 0x5A18BD65, 0x5E28240A, 0x62454DE7, 
	// 0x666E43A9, 0x6AA1B5C6, 0x6EDD590B, 0x732031CD, 0x7767F4DA, 0x7BB2FEC4, 0x7FFFFFFF
// };
 
const uint32_t DATA_GAIN[128] = {
	0x00000000, 0x00000863, 0x0000218D, 0x00007570, 0x0000FBA8, 0x0001DE26, 0x0003254E, 0x0004EA4A, 0x00073DE1, 0x000A393E, 0x000DE4C5, 
	0x0012599E, 0x0017A893, 0x001DEACC, 0x00253975, 0x002D948D, 0x00372606, 0x0041EDE1, 0x004E0DAA, 0x005B8DC5, 0x006A8FC0, 0x007B1C00, 
	0x008D4BAD, 0x00A12F90, 0x00B6E0D2, 0x00CE67D7, 0x00E7DDCA, 0x01035BD5, 0x0120EA5B, 0x0140AAEA, 0x0162A5E7, 0x0186F47B, 0x01ADA76D, 
	0x01D6CF85, 0x020285EC, 0x0230E3CD, 0x0261E929, 0x0295BFF0, 0x02CC7086, 0x03060BB2, 0x0342AA9F, 0x03826677, 0x03C53F39, 0x040B5ED8, 
	0x0454CDB7, 0x04A19C9D, 0x04F1E4B4, 0x0545B6C3, 0x059D2BF5, 0x05F85510, 0x065742DC, 0x06BA0620, 0x0720B806, 0x078B71B8, 0x07FA3B9A, 
	0x086D2673, 0x08E45C35, 0x095FD47B, 0x09DFB938, 0x0A640A6B, 0x0AECF206, 0x0B7A7008, 0x0C0C9D9D, 0x0CA393EE, 0x0D3F63C3, 0x0DE01DE2, 
	0x0E85DB76, 0x0F30AD46, 0x0FE09BB6, 0x1095D0B7, 0x11504C48, 0x12102FF8, 0x12D5842B, 0x13A0620A, 0x1470FBEB, 0x15489092, 0x16291750, 
	0x17148FD9, 0x180CF17F, 0x19143393, 0x1A2C2374, 0x1B551505, 0x1C8EA39C, 0x1DD872F3, 0x1F322F26, 0x209B738E, 0x2213E3E2, 0x239B23DD, 
	0x2530DF9B, 0x26D4B274, 0x28864020, 0x2A452C59, 0x2C111ADA, 0x2DE9AF5B, 0x2FCE95FA, 0x31BF61A9, 0x33BBBE87, 0x35C3504B, 0x37D5BAB1, 
	0x39F2A172, 0x3C19A847, 0x3E4A72EA, 0x40849CB1, 0x42C7D1BA, 0x4513B5BE, 0x4767EC77, 0x49C4113B, 0x4C27D028, 0x4E92CCF6, 0x5104A2FC, 
	0x537CF5F4, 0x55FB71FB, 0x587FBACA, 0x5B096BB8, 0x5D9830E3, 0x602BA59F, 0x62C36DA7, 0x655F2CB5, 0x67FE8EE5, 0x6AA12F8F, 0x6D46BACE, 
	0x6FEECBFA, 0x7298FE68, 0x75450699, 0x77F27FE3, 0x7AA11664, 0x7D505D0E, 0x7FFFFFFF
};


// const uint32_t DATA_VELGAIN[128] = {
	// 0x00000000, 0x00049667, 0x0008D8EC, 0x000D6F54, 0x001205BC, 0x0016F006, 0x001BDA51, 0x0021187E, 0x0026AA8E, 0x002C3C9E, 0x00327674, 
	// 0x0039042D, 0x003F91E6, 0x00471B47, 0x004EA4A8, 0x0056D5CF, 0x005FAEBC, 0x0068DB8B, 0x00730403, 0x007D805E, 0x0088F861, 0x0094C447, 
	// 0x00A18BD6, 0x00AEFB2A, 0x00BD6627, 0x00CCCCCC, 0x00DCDB37, 0x00EDE54B, 0x00FFEB07, 0x0112EC6B, 0x0126E978, 0x013C3611, 0x01527E52, 
	// 0x0169C23B, 0x018255B0, 0x019C38B0, 0x01B76B3B, 0x01D3996F, 0x01F16B11, 0x02108C3F, 0x0230FCF8, 0x0252BD3C, 0x027620EE, 0x029AD42C, 
	// 0x02C17EBA, 0x02E978D4, 0x0313165D, 0x033E5753, 0x036B3BB8, 0x0399C38A, 0x03CA42AE, 0x03FC6540, 0x04307F23, 0x04669057, 0x049E44F9, 
	// 0x04D7F0ED, 0x0513E814, 0x055182A9, 0x05916872, 0x05D3458C, 0x061719F7, 0x065D3996, 0x06A5A469, 0x06F05A70, 0x073D07C8, 0x078C5436, 
	// 0x07DD97F6, 0x08317ACC, 0x0887A8D6, 0x08E075F6, 0x093B8E4B, 0x099945B6, 0x09F94855, 0x0A5C3DEE, 0x0AC17EBA, 0x0B29B280, 0x0B94855D, 
	// 0x0C029F16, 0x0C74A771, 0x0CEBEDFA, 0x0D69C23B, 0x0DEE7818, 0x0E7B5F1B, 0x0F11C6D1, 0x0FB256FF, 0x105E0B4D, 0x1115DF65, 0x11DACEED, 
	// 0x12AE2973, 0x1390EA9E, 0x1483BA34, 0x1587E7C0, 0x169E1B08, 0x17C7A397, 0x19057D17, 0x1A58A32F, 0x1BC21187, 0x1D42C3C9, 0x1EDBB59D, 
	// 0x208DE2AB, 0x225A469D, 0x2441DD19, 0x2645F5AD, 0x2867381D, 0x2AA6F3F4, 0x2D05D0F9, 0x2F84CAD5, 0x322584F4, 0x34E8533A, 0x37CE3150, 
	// 0x3AD8C2A3, 0x3E085B17, 0x415E4A37, 0x44DAE3E6, 0x487C30D2, 0x4C3F3E02, 0x50216C60, 0x54201CD5, 0x5839042C, 0x5C698350, 0x60AEA747, 
	// 0x650624DC, 0x696D5CF9, 0x6DE15CA5, 0x725FD8AC, 0x76E631F7, 0x7B71C970, 0x7FFFFFFF
// }; 

const uint32_t DATA_VELGAIN[128] = {
	0x00000000, 0x002F8BDE, 0x005F4A12, 0x008F5C28, 0x00BFFCDA, 0x00F14552, 0x012378AB, 0x0156B00F, 0x018B1DD5, 0x01C0F451, 0x01F85D74, 
	0x0231832F, 0x026C8F75, 0x02A9BCFD, 0x02E924F2, 0x032B020C, 0x036F7E3D, 0x03B6C376, 0x0400FBA8, 0x044E5929, 0x049F0E4D, 0x04F3343F, 
	0x054B0E1B, 0x05A6B50B, 0x060663C7, 0x066A3BDD, 0x06D26FA3, 0x073F316E, 0x07B0AB2E, 0x0826FE71, 0x08A265F0, 0x0923033A, 0x09A908A2, 
	0x0A34A01A, 0x0AC5EB31, 0x0B5CD0BB, 0x0BF950B9, 0x0C9B499C, 0x0D42BB66, 0x0DEF954E, 0x0EA1BE2B, 0x0F592D98, 0x1015D2CF, 0x10D794A6, 
	0x119E7B80, 0x126A5D6B, 0x133B3A68, 0x141101AF, 0x14EB9A17, 0x15CB039E, 0x16AF251B, 0x1797EDC7, 0x18854CDB, 0x197739F3, 0x1A6DA448, 
	0x1B687B13, 0x1C67A52A, 0x1D6B228D, 0x1E72E275, 0x1F7EC354, 0x208ECD8D, 0x21A2DF93, 0x22BAF965, 0x23D6F976, 0x24F6DFC5, 0x261A9328, 
	0x274202D9, 0x286D2ED7, 0x299BF594, 0x2ACE5710, 0x2C0431BD, 0x2D3D859C, 0x2E7A3981, 0x2FBA450A, 0x30FD976F, 0x32441FEA, 0x338DCDB3, 
	0x34DA9003, 0x362A5614, 0x377D1782, 0x38D2C386, 0x3A2B4959, 0x3B868FD1, 0x3CE496ED, 0x3E454583, 0x3FA89331, 0x410E66CA, 0x4276B7EC, 
	0x43E16D6D, 0x454E874C, 0x46BDF4C2, 0x482F9441, 0x49A365CA, 0x4B195896, 0x4C915379, 0x4E0B5674, 0x4F87485D, 0x5105186D, 0x5284B5DC, 
	0x54061847, 0x55892EE7, 0x570DE092, 0x58942D48, 0x5A1BFBDE, 0x5BA53B8D, 0x5D2FE3F2, 0x5EBBDBE3, 0x6049235E, 0x61D798D7, 0x636733EA, 
	0x64F7EC34, 0x6689A88A, 0x681C5824, 0x69AFFB04, 0x6B446F9A, 0x6CD9AD85, 0x6E6FA3FB, 0x70064A9B, 0x719D883A, 0x73355474, 0x74CD961F, 
	0x76664D3B, 0x77FF5839, 0x7998B71A, 0x7B3250B4, 0x7CCC1CA2, 0x7E660A1F, 0x7FFFFFFF
};

const uint16_t DATA_TIME[128] = {
	0x0000, 0x0004, 0x0006, 0x0008, 0x000B, 0x000D, 0x0010, 0x0013, 0x0016, 0x001A, 0x001E, 
	0x0023, 0x0029, 0x002F, 0x0035, 0x003D, 0x0046, 0x004F, 0x0059, 0x0064, 0x0071, 0x007E, 
	0x008D, 0x009C, 0x00AE, 0x00C0, 0x00D3, 0x00E9, 0x00FF, 0x0117, 0x0131, 0x014D, 0x016A, 
	0x0188, 0x01A9, 0x01CB, 0x01F0, 0x0216, 0x023F, 0x0269, 0x0296, 0x02C5, 0x02F6, 0x032A, 
	0x035F, 0x0397, 0x03D2, 0x040F, 0x044F, 0x0492, 0x04D7, 0x051E, 0x0569, 0x05B7, 0x0607, 
	0x065A, 0x06B0, 0x070A, 0x0767, 0x07C7, 0x082A, 0x0890, 0x08FA, 0x0966, 0x09D7, 0x0A4B, 
	0x0AC3, 0x0B3E, 0x0BBD, 0x0C3F, 0x0CC6, 0x0D50, 0x0DDE, 0x0E71, 0x0F08, 0x0FA5, 0x1049, 
	0x10F3, 0x11A6, 0x1261, 0x1326, 0x13F4, 0x14CD, 0x15B1, 0x16A2, 0x17A0, 0x18AB, 0x19C4, 
	0x1AED, 0x1C26, 0x1D6E, 0x1EC8, 0x2034, 0x21B3, 0x2344, 0x24EA, 0x26A4, 0x2870, 0x2A4F, 
	0x2C40, 0x2E43, 0x3056, 0x3279, 0x34AB, 0x36EC, 0x393B, 0x3B98, 0x3E02, 0x4078, 0x42FA, 
	0x4587, 0x481E, 0x4AC0, 0x4D6A, 0x501D, 0x52D9, 0x559B, 0x5865, 0x5B34, 0x5E09, 0x60E3, 
	0x63C2, 0x66A4, 0x6989, 0x6C70, 0x6F5A, 0x7244, 0x7530
}; 

const char DATA_noteLabels[169][5] = {
	"118", "112", "105", "99.4", "93.8", "88.6", "83.6", "78.9", "74.5", 
	"70.3", "66.3", "62.6", "59.1", "55.8", "52.7", "49.7", "46.9", "44.3", 
	"41.8", "39.5", "37.2", "35.1", "33.2", "31.3", "29.6", "27.9", "26.3", 
	"24.9", "23.5", "22.1", "20.9", "19.7", "18.6", "17.6", "16.6", "15.7", 
	"14.8", "13.9", "13.2", "12.4", "11.7", "11.1", "10.4", "9.86", "9.31", 
	"8.79", "8.29", "7.83", "7.39", "6.97", "6.58", "6.21", "5.86", "5.54", 
	"5.22", "4.93", "4.65", "4.39", "4.15", "3.91", "3.69", "3.49", "3.29", 
	"3.11", "2.93", "2.77", "2.61", "2.47", "2.33", "2.20", "2.07", "1.96", 
	"1.85", "1.74", "1.65", "1.55", "1.47", "1.38", "1.31", "1.23", "1.16", 
	"1.10", "1.04", "1.02", "1.08", "1.15", "1.22", "1.29", "1.36", "1.45", 
	"1.53", "1.62", "1.72", "1.82", "1.93", "2.04", "2.17", "2.29", "2.43", 
	"2.58", "2.73", "2.89", "3.06", "3.24", "3.44", "3.64", "3.86", "4.09", 
	"4.33", "4.59", "4.86", "5.15", "5.46", "5.78", "6.12", "6.49", "6.87", 
	"7.28", "7.72", "8.18", "8.66", "9.18", "9.72", "10.3", "10.9", "11.6", 
	"12.2", "13.0", "13.8", "14.6", "15.4", "16.4", "17.3", "18.4", "19.4", 
	"20.6", "21.8", "23.1", "24.5", "26.0", "4.43", "4.70", "4.98", "5.27", 
	"5.59", "5.92", "6.27", "6.64", "7.04", "7.46", "7.90", "8.37", "8.87", 
	"9.40", "9.96", "10.5", "11.2", "11.8", "12.5", "13.3", "14.1", "14.9", 
	"15.8", "16.7", "17.7", "18.8", "19.9", "21.1", "22.4"
};

const char DATA_screens[SCREEN_CNT][4][21] = {
	{
		"@@@@@@@@@ @@@@@@@@@ ",
		"@@@@@@@@@>@@@@@@@@@<",
		"@@@@@@@@@ @@@@@@@@@ "
	},
	{
		" TIME: A:@@@  D:@@@ ",
		"       S:@@@  R:@@@ ",
		" TRGT: D:@@@  S:@@@ "
	},
	{
		"LEGATO:@@@          ",
		"TRANSPOSE: @@@@@@@  ",
		"GLIDE: P:@@@  V:@@@ "
		
	},
	{
		"STAGE:@@@ @@@@@@@@@@",
		"GOAL:@@@@@@@        ",
		"GLIDE:@@@ @@@@@@@@@@"
	},
	{
		"TYPE:@@@@@ TRACK:@@@",
		"CUTFRQ:@@@@@@@      ",
		"RES:@@@             ",
	},
	{
		"STAGE:@@@ @@@@@@@@@@",
		"GOAL:@@@@@@@        ",
		"GLIDE:@@@ @@@@@@@@@@"
	},
	{
		"STEPS:@@  TYPE:@@@@@",
		"SPM:@@@@@@ GLIDE:@@@",
		"STYLE:@@@@-@@@@     "
	},
	{
		"POS:@@  *PIT/ENV/VEL",
		"@@@@@@@@   @@@@@@@@ ",
		"@@@@@@@@   @@@@@@@@ "
	},
	
	{
		"@@@@@@@@@ @@@@@@@@@ ",
		"@@@@@@@@@>@@@@@@@@@<",
		"@@@@@@@@@ @@@@@@@@@ "
	},
	{
		" DIR: @@@@@@@@@ *CLR",
		" EDIT *a>A     MOVE ",
		"  NAME:@@@@@@ *SAVE "
	},
	{
		" MIDICH:@@@ HKEY:@@@",
		" VEL:@@@@@@ LKEY:@@@",
		" LEGATO:@@@         "
	},
	{
		"CC#:WIND:@@@ MDW:@@@",
		"RNG:WIND:@@@ MDW:@@@",
		"    PBND:@@@ SUS:@@@"
	},
	{
		" PAN: L@@@ R@@@     ",
		"                    ",
		"                    "
	},
	{
		"PIT:@@@@@ FCUT:@@@@@",
		"AMP:@@@@@ FRES:@@@@@",
		"GAT:@@@@@ ARPT:@@@@@"
	},
	{
		"NOTES:@@@   EDIT:@@@",
		" @@@@@@@@@ @@@@@@@@@",
		" @@@@@@@@@ @@@@@@@@@"
	},
	{
		"RYTHM:@@@ VEL:@@@@@@",
		"ENV:@@@             ",
		"           @@@@@@@@ "
	},
	{
		"FAV @@: @@@@@@      ",
		"@@@@@@@@@@@@@@@@@@@@",
		"@@@@@@@@@@@@@@@@@@@@"
	},
	{
		"G.FUND @@@ PRTL1 @@@",
		"G.PTL1 @@@ STEP  @@@",
		"G.LAST @@@ COUNT @@@"
	},
	{
		"PRE-HARM  POST-HARM ",
		" TOG:@@@   TOG:@@@  ",
		" @@@@@@    @@@@@@   "
	},
	{
		"(SELF)      @@@@    ",
		" @@@   TO   @@@     ",
		"FINE:@@@    *PITMOD "
	}

};

const char DATA_SCREENS[SCREEN_CNT][9] = {
	"WAV TABL", " AMP ENV", " PIT/VEL", " PIT ENV", "  FILTER", "FILT ENV", 
	"ARP INIT", "ARPSTEPS", "PATCH LD", "PATCH SV", "MIDI INS", "MIDI CCS", 
	"  OUTPUT", "MOD SRCS", "  NOTES ", " ARP REC", "FAVORITE", "HARMONIC",
	"   PHASE", "PIT RTIO"
};

#endif



uint32_t __attribute__( ( always_inline ) ) getPhaseInc(uint32_t pitch)
{
	uint16_t pitchInd = pitch >> PITCH_COARSE;
	if(pitchInd > 255) return PHASEINCS[256]>>1;
	uint32_t inc1 = PHASEINCS[pitchInd];
	uint32_t inc2 = PHASEINCS[pitchInd + 1];
	uint32_t cents = (pitch&PITCH_MASK) << (PITCH_INTERP);
	if(inc2 > 0x7FFFFFFF)
	{
		uint32_t inc = ___SMMUL(inc1>>1,(INT_MAX-cents));
		inc = ___SMMLA(inc2>>1,cents,inc)<<1;
		return inc;
	}
	else
	{
		uint32_t inc = ___SMMUL(inc1,(INT_MAX-cents));
		inc = ___SMMLA(inc2,cents,inc);
		return inc;
	}
	//return 0x012C5F93;
}

uint32_t __attribute__( ( always_inline ) ) getPitch(uint32_t goalPhase)
{
	uint32_t p_ind = 0;
	int32_t cnt = 0;
	while(p_ind < 256)
	{
		if((PHASEINCS[p_ind + 1] >> 1) > goalPhase)
		{
			break;
		}
		if(++p_ind > 255)
		{
			return (255 << PITCH_COARSE) + (63 << PITCH_FINE);
		}
	}
	p_ind <<= PITCH_COARSE;
	while(getPhaseInc(p_ind) < goalPhase)
	{
		p_ind += 1 << PITCH_FINE;
		++cnt;
	}
	if(p_ind == 0)
	{
		p_ind = PHASEINCS[0] >> 1;
	}
	return p_ind;
}

#endif 
