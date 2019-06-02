#ifndef PITCH_TABLES_C
#define PITCH_TABLES_C

#include "./settings.h"

const uint32_t PHASEINCS[257] = {
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

/* const uint32_t PHASEINCS[257] = {
	0x000002F5, 0x00000322, 0x00000352, 0x00000384, 0x000003BA, 0x000003F2, 0x0000042E, 
	0x0000046E, 0x000004B1, 0x000004F9, 0x00000545, 0x00000595, 0x000005EA, 0x00000644, 
	0x000006A3, 0x00000708, 0x00000773, 0x000007E5, 0x0000085D, 0x000008DC, 0x00000963, 
	0x000009F2, 0x00000A89, 0x00000B2A, 0x00000BD4, 0x00000C88, 0x00000D46, 0x00000E10, 
	0x00000EE7, 0x00000FC9, 0x000010BA, 0x000011B8, 0x000012C6, 0x000013E4, 0x00001513, 
	0x00001653, 0x000017A7, 0x0000190F, 0x00001A8D, 0x00001C21, 0x00001DCD, 0x00001F93, 
	0x00002173, 0x00002370, 0x0000258C, 0x000027C8, 0x00002A25, 0x00002CA7, 0x00002F4E, 
	0x0000321E, 0x00003519, 0x00003842, 0x00003B9A, 0x00003F25, 0x000042E7, 0x000046E1, 
	0x00004B18, 0x00004F8F, 0x0000544A, 0x0000594D, 0x00005E9D, 0x0000643D, 0x00006A33, 
	0x00007083, 0x00007734, 0x00007E4B, 0x000085CD, 0x00008DC2, 0x00009630, 0x00009F1E, 
	0x0000A894, 0x0000B29A, 0x0000BD39, 0x0000C87A, 0x0000D465, 0x0000E107, 0x0000EE68, 
	0x0000FC95, 0x00010B9A, 0x00011B84, 0x00012C60, 0x00013E3C, 0x00015128, 0x00016535, 
	0x00017A72, 0x000190F3, 0x0001A8CB, 0x0001C20D, 0x0001DCD0, 0x0001F92A, 0x00021734, 
	0x00023708, 0x000258BF, 0x00027C78, 0x0002A251, 0x0002CA6A, 0x0002F4E5, 0x000321E7, 
	0x00035196, 0x0003841A, 0x0003B9A0, 0x0003F255, 0x00042E69, 0x00046E0F, 0x0004B17E, 
	0x0004F8F0, 0x000544A1, 0x000594D3, 0x0005E9C9, 0x000643CD, 0x0006A32B, 0x00070835, 
	0x00077340, 0x0007E4AA, 0x00085CD1, 0x0008DC1E, 0x000962FD, 0x0009F1E0, 0x000A8943, 
	0x000B29A6, 0x000BD393, 0x000C879A, 0x000D4656, 0x000E1069, 0x000EE681, 0x000FC953, 
	0x0010B9A3, 0x0011B83C, 0x0012C5F9, 0x0013E3C0, 0x00151286, 0x0016534C, 0x0017A726, 
	0x00190F34, 0x001A8CAC, 0x001C20D3, 0x001DCD02, 0x001F92A7, 0x00217345, 0x00237078, 
	0x00258BF2, 0x0027C781, 0x002A250C, 0x002CA698, 0x002F4E4B, 0x00321E69, 0x00351958, 
	0x003841A6, 0x003B9A04, 0x003F254E, 0x0042E68B, 0x0046E0F0, 0x004B17E5, 0x004F8F01, 
	0x00544A17, 0x00594D31, 0x005E9C96, 0x00643CD2, 0x006A32B1, 0x0070834C, 0x00773407, 
	0x007E4A9B, 0x0085CD15, 0x008DC1E1, 0x00962FC9, 0x009F1E03, 0x00A8942E, 0x00B29A62, 
	0x00BD392D, 0x00C879A3, 0x00D46562, 0x00E10697, 0x00EE680F, 0x00FC9536, 0x010B9A2B, 
	0x011B83C2, 0x012C5F93, 0x013E3C06, 0x0151285D, 0x016534C3, 0x017A725A, 0x0190F347, 
	0x01A8CAC3, 0x01C20D2F, 0x01DCD01D, 0x01F92A6D, 0x02173456, 0x02370783, 0x0258BF26, 
	0x027C780B, 0x02A250BA, 0x02CA6987, 0x02F4E4B4, 0x0321E68D, 0x03519586, 0x03841A5D, 
	0x03B9A03A, 0x03F254D9, 0x042E68AC, 0x046E0F07, 0x04B17E4B, 0x04F8F017, 0x0544A173, 
	0x0594D30D, 0x05E9C968, 0x0643CD1A, 0x06A32B0D, 0x070834BA, 0x07734075, 0x07E4A9B2, 
	0x085CD157, 0x08DC1E0D, 0x0962FC96, 0x09F1E02D, 0x0A8942E7, 0x0B29A61A, 0x0BD392D0, 
	0x0C879A35, 0x0D46561A, 0x0E106974, 0x0EE680E9, 0x0FC95364, 0x10B9A2AF, 0x11B83C1A, 
	0x12C5F92C, 0x13E3C05A, 0x151285CE, 0x16534C35, 0x17A725A0, 0x190F346A, 0x1A8CAC34, 
	0x1C20D2E8, 0x1DCD01D3, 0x1F92A6C8, 0x2173455E, 0x23707834, 0x258BF258, 0x27C780B4, 
	0x2A250B9B, 0x2CA69869, 0x2F4E4B3F, 0x321E68D4, 0x35195868, 0x3841A5D0, 0x3B9A03A5, 
	0x3F254D90, 0x42E68ABB, 0x46E0F069, 0x4B17E4B1, 0x4F8F0169, 0x544A1737, 0x594D30D3, 
	0x5E9C967E, 0x643CD1A7, 0x6A32B0CF, 0x70834BA0, 0x7734074B
}; */




//0x8E38E38D

const uint32_t ATTACK_K[128] = {
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
/* const uint32_t ATTACK_K[128] = {
	0x02AAAAAA, 0x00E38E38, 0x00888888, 0x00618618, 0x004BDA12, 0x003E0F83, 0x00348348, 0x002D82D8, 0x00282828, 0x0023EE08, 0x00208208, 
	0x001DAE60, 0x001B4E81, 0x001948B0, 0x0016C16C, 0x00155555, 0x00141414, 0x0012F684, 0x00118118, 0x0010A681, 0x000F83E0, 0x000ED730, 
	0x000DEE95, 0x000D62B8, 0x000CA458, 0x000BFA02, 0x000B9214, 0x000B02C0, 0x000A80A8, 0x000A0A0A, 0x00099D72, 0x000939A8, 0x0008DDA5, 
	0x00086D90, 0x00082082, 0x0007C1F0, 0x00078078, 0x00072F9B, 0x0006F74A, 0x0006B15C, 0x000670B4, 0x000634C0, 0x0005FD01, 0x0005BC98, 
	0x00058CD5, 0x00056015, 0x00052BF5, 0x0004FBA3, 0x0004D773, 0x0004AD01, 0x0004855E, 0x00046046, 0x000436C8, 0x0004167B, 0x0003F230, 
	0x0003D5D0, 0x0003B5CC, 0x000397CD, 0x00037BA5, 0x00036129, 0x00034034, 0x00031A60, 0x0002E485, 0x0002A2C2, 0x00025680, 0x00020E64, 
	0x0001CBE6, 0x000191C0, 0x0001610E, 0x00013785, 0x00011485, 0x0000F730, 0x0000DDC7, 0x0000C82F, 0x0000B5AA, 0x0000A57E, 0x00009770, 
	0x00008B08, 0x00008020, 0x0000767B, 0x00006DE9, 0x00006642, 0x00005F72, 0x00005941, 0x000053B2, 0x00004EA6, 0x00004A0D, 0x000045E0, 
	0x00004212, 0x00003E98, 0x00003B61, 0x0000386D, 0x000035B4, 0x00003330, 0x000030DE, 0x00002EB3, 0x00002CB2, 0x00002AD2, 0x00002914, 
	0x00002773, 0x000025ED, 0x00002480, 0x00002328, 0x000021E5, 0x000020B6, 0x00001F98, 0x00001E8C, 0x00001D8C, 0x00001C9C, 0x00001BBA, 
	0x00001AE2, 0x00001A16, 0x00001955, 0x0000189D, 0x000017EE, 0x00001749, 0x000016AB, 0x00001614, 0x00001585, 0x000014FC, 0x00001479, 
	0x000013FD, 0x00001385, 0x00001313, 0x000012A6, 0x0000123E, 0x000011D9, 0x00001179
}; */

const uint32_t ATTACK[128] = {
	0x1AAAAAAA, 0x02F684BD, 0x01919191, 0x01069069, 0x00C30C30, 0x0097B425, 0x007C1F07, 0x0065E3FA, 0x00555555, 0x00489FC5, 0x003E0F83, 
	0x00355555, 0x002E2049, 0x002864FC, 0x00235F0F, 0x001F2C07, 0x001B86E1, 0x00186186, 0x0015AC05, 0x001356C8, 0x00115399, 0x000F9603, 
	0x000E048E, 0x000CAA59, 0x000B7930, 0x000A6C21, 0x00097B42, 0x0008A6FD, 0x0007E90F, 0x00074133, 0x0006A900, 0x000620C9, 0x0005A583, 
	0x0005381B, 0x0004D3F1, 0x0004793B, 0x000427AE, 0x0003DD09, 0x000398C5, 0x00035AC8, 0x00032208, 0x0002EE60, 0x0002BEB4, 0x000292FC, 
	0x00026AA1, 0x00024592, 0x00022328, 0x00020386, 0x0001E643, 0x0001CB0E, 0x0001B1DE, 0x00019A6F, 0x0001849E, 0x0001704F, 0x00015D62, 
	0x00014BBE, 0x00013B3A, 0x00012BC3, 0x00011D47, 0x00010FB5, 0x000102FC, 0x0000F70C, 0x0000EBD0, 0x0000E13D, 0x0000D747, 0x0000CDE4, 
	0x0000C50C, 0x0000BCB0, 0x0000B4C8, 0x0000AD52, 0x0000A644, 0x00009F92, 0x0000993B, 0x0000933B, 0x00008D87, 0x0000881E, 0x000082FA, 
	0x00007E17, 0x00007970, 0x00007503, 0x000070CD, 0x00006CC8, 0x000068F4, 0x0000654E, 0x000061D1, 0x00005E7B, 0x00005B41, 0x00005820, 
	0x00005510, 0x00005210, 0x00004F1C, 0x00004C32, 0x00004953, 0x0000467D, 0x000043B2, 0x000040F2, 0x00003E3C, 0x00003B94, 0x000038FB, 
	0x00003671, 0x000033F7, 0x00003191, 0x00002F43, 0x00002D0E, 0x00002AF4, 0x000028F5, 0x00002712, 0x0000254A, 0x0000239C, 0x00002208, 
	0x0000208B, 0x00001F26, 0x00001DD7, 0x00001C9C, 0x00001B75, 0x00001A5F, 0x0000195B, 0x00001866, 0x00001780, 0x000016A9, 0x000015DE, 
	0x0000151F, 0x0000146B, 0x000013C2, 0x00001323, 0x0000128C, 0x000011FF, 0x00001179
};

/* const uint32_t DECAY[128] = {
	0x0CCCCCCC, 0x3B698EEB, 0x50C335D3, 0x5C1EA8FD, 0x65AC8C2E, 0x69A6D826, 0x6DC8FAAC, 0x70A14D48, 0x7347C769, 0x7526D2E9, 0x76D649AB, 
	0x7811C180, 0x792BE6B8, 0x7A1FCCF1, 0x7AD7FB92, 0x7B8B887B, 0x7C14171A, 0x7C89F8C5, 0x7CEF5358, 0x7D4661EE, 0x7D913C17, 0x7DD1BDFD, 
	0x7E0CD569, 0x7E3C9CF1, 0x7E687EA4, 0x7E8E6B6D, 0x7EAF651F, 0x7ECD7A9A, 0x7EE7AC06, 0x7EFE9F34, 0x7F1396DB, 0x7F26A83D, 0x7F36E0F9, 
	0x7F463AC3, 0x7F53CC0C, 0x7F60328E, 0x7F6B39D3, 0x7F7598A1, 0x7F7ED625, 0x7F871D3B, 0x7F8EE7D8, 0x7F960D4B, 0x7F9C7AEE, 0x7FA28593, 
	0x7FA7F980, 0x7FAD19BA, 0x7FB1D319, 0x7FB61C9B, 0x7FBA25CC, 0x7FBDF082, 0x7FC16490, 0x7FC4A4BB, 0x7FC7A852, 0x7FCA74D2, 0x7FCD17F0, 
	0x7FCF9396, 0x7FD1E28F, 0x7FD408E1, 0x7FD6101C, 0x7FD7F9E0, 0x7FD9C2D7, 0x7FDB7267, 0x7FDD09FB, 0x7FDE8725, 0x7FDFEFA6, 0x7FE147DE, 
	0x7FE28A47, 0x7FE3BB6E, 0x7FE4DC5B, 0x7FE5EE06, 0x7FE6F158, 0x7FE7E72F, 0x7FE8D05A, 0x7FE9AF4D, 0x7FEA82D0, 0x7FEB4B96, 0x7FEC0A48, 
	0x7FECC0C4, 0x7FED71AF, 0x7FEE1BC6, 0x7FEEC12E, 0x7FEF61A0, 0x7FEFFEA7, 0x7FF09892, 0x7FF12E10, 0x7FF1C19A, 0x7FF2511A, 0x7FF2DDF2, 
	0x7FF366EA, 0x7FF3ED19, 0x7FF46F62, 0x7FF4EE36, 0x7FF5697D, 0x7FF5E0C1, 0x7FF65455, 0x7FF6C3D7, 0x7FF72F8D, 0x7FF7972B, 0x7FF7FAB8, 
	0x7FF85A3E, 0x7FF8B5CB, 0x7FF90D6E, 0x7FF960C9, 0x7FF9AFED, 0x7FF9FAD8, 0x7FFA417A, 0x7FFA842A, 0x7FFAC2D7, 0x7FFAFDDD, 0x7FFB353D, 
	0x7FFB6950, 0x7FFB9A32, 0x7FFBC815, 0x7FFBF327, 0x7FFC1BA4, 0x7FFC41A8, 0x7FFC655C, 0x7FFC86F3, 0x7FFCA690, 0x7FFCC44A, 0x7FFCE041, 
	0x7FFCFA9B, 0x7FFD1374, 0x7FFD2AE3, 0x7FFD40FA, 0x7FFD55DC, 0x7FFD698E, 0x7FFFFFFF
}; */

const uint32_t SEEK[128] = {
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

const uint32_t seekInterp[128] = { 
 	0x4E740F5A, 0x4E262EA7, 0x4DD75E22, 0x4D87999B, 0x4D36DCC7, 0x4CE52347, 0x4C92689D, 0x4C3EA838, 0x4BE9DD67, 0x4B940362, 
	0x4B3D1543, 0x4AE50E08, 0x4A8BE894, 0x4A319FAA, 0x49D62DEF, 0x49798DEA, 0x491BBA00, 0x48BCAC76, 0x485C5F70, 0x47FACCEF, 
	0x4797EED2, 0x4733BED1, 0x46CE3683, 0x46674F55, 0x45FF0290, 0x45954956, 0x452A1C9E, 0x44BD7538, 0x444F4BC8, 0x43DF98C9, 
	0x436E5486, 0x42FB7723, 0x4286F890, 0x4210D094, 0x4198F6C1, 0x411F627D, 0x40A40AFB, 0x4026E73C, 0x3FA7EE0F, 0x3F27160E, 
	0x3EA455A3, 0x3E1FA2FF, 0x3D98F420, 0x3D103ED0, 0x3C8578A2, 0x3BF896F3, 0x3B698EEB, 0x3AD8557D, 0x3A44DF67, 0x39AF2132, 
	0x39170F33, 0x387C9D8D, 0x37DFC033, 0x37406AE7, 0x369E9140, 0x35FA26A9, 0x35531E66, 0x34A96B99, 0x33FD0145, 0x334DD251, 
	0x329BD192, 0x31E6F1D1, 0x312F25D1, 0x30746059, 0x2FB6943F, 0x2EF5B475, 0x2E31B414, 0x2D6A866F, 0x2CA01F1F, 0x2BD2721E, 
	0x2B0173DA, 0x2A2D194E, 0x29555820, 0x287A26C4, 0x279B7C9C, 0x26B95227, 0x25D3A12E, 0x24EA64F9, 0x23FD9A89, 0x230D40E2, 
	0x2219594E, 0x2121E7B9, 0x2026F30F, 0x1F2885A6, 0x1E26ADB9, 0x1D217DEE, 0x1C190DF2, 0x1B0D7B1B, 0x19FEE92D, 0x18ED832E, 
	0x17D97C4A, 0x16C310E3, 0x15AA87AB, 0x149032E9, 0x137471D3, 0x1257B212, 0x113A715A, 0x101D3F2D, 0x0F00BEAB, 0x0DE5A87F, 
	0x0CCCCCCC, 0x0BB71517, 0x0AA58605, 0x099940DB, 0x08938476, 0x0795AD90, 0x06A135D3, 0x05B7B15A, 0x04DAC9E9, 0x040C3713, 
	0x034DB25B, 0x02A0E62A, 0x0207567A, 0x01824247, 0x01127D7E, 0x00B8449B, 0x00730E02, 0x00416179, 0x0020C49B, 0x000DD172, 
	0x00048D97, 0x00010945, 0x00002165, 0x0000017D, 0x00000002, 0x00000000, 0x00000000, 0x00000000
};



const uint8_t noteLabelTypes[3] = {82, 139, 168};
const uint8_t SET2 = 140;
const uint8_t maxNoteLabelInd = 168;
const char noteLabels[169][5] = {
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







const uint32_t SEEK_S_RATE[128] = {
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

const uint32_t GAIN[128] = {
	0x00000000, 0x00078961, 0x000F12C2, 0x00169C23, 0x001E7967, 0x002656AB, 0x002E87D2, 0x0036B8F9, 0x003EEA20, 0x0047C30D, 0x00509BF9, 
	0x005A1CAC, 0x00639D5E, 0x006DC5D6, 0x00784230, 0x0083126E, 0x008E8A71, 0x009A5657, 0x00A6CA03, 0x00B3E575, 0x00C154C9, 0x00CFBFC6, 
	0x00DE7EA5, 0x00EE392E, 0x00FE9B7B, 0x010FA58F, 0x0121AB4B, 0x013458CD, 0x014801F7, 0x015CA6CA, 0x0171F362, 0x01883BA3, 0x019FD36F, 
	0x01B81301, 0x01D1A21E, 0x01EC2CE4, 0x02080735, 0x0224DD2F, 0x0242AED1, 0x0261CFFE, 0x0282949A, 0x02A454DE, 0x02C764AD, 0x02EBC408, 
	0x031172EF, 0x0338C543, 0x03616723, 0x038B588E, 0x03B6ED67, 0x03E425AE, 0x0412AD81, 0x04432CA5, 0x0474FB54, 0x04A86D71, 0x04DDD6E0, 
	0x0514E3BC, 0x054D9407, 0x05883BA3, 0x05C486AD, 0x0602C908, 0x064302B4, 0x0684DFCE, 0x06C9081C, 0x070ED3D8, 0x0756EAC8, 0x07A0F909, 
	0x07ECFE9B, 0x083B4F61, 0x088B9778, 0x08DE2AC3, 0x0932B55E, 0x0989DF11, 0x09E30014, 0x0A3E6C4C, 0x0A9C779A, 0x0AFCCE1C, 0x0B5F6FD2, 
	0x0BC45CBB, 0x0C2BE8BB, 0x0C9667B5, 0x0D057D17, 0x0D7A786C, 0x0DF6A93F, 0x0E7B0B38, 0x0F09E98D, 0x0FA3981F, 0x104A0E40, 0x10FEEF5E, 
	0x11C2E33E, 0x1297E132, 0x137EE4E2, 0x147991BC, 0x1588E368, 0x16AED139, 0x17EC56D5, 0x194317AC, 0x1AB40F66, 0x1C40E171, 0x1DEADD58, 
	0x1FB3A68A, 0x219C38B0, 0x23A63736, 0x25D29DC6, 0x282363B2, 0x2A9984A0, 0x2D35FC3A, 0x2FF822BB, 0x32DE54B4, 0x35E69AD3, 0x390F51AC, 
	0x3C56D5CF, 0x3FBB2FEB, 0x433B1076, 0x46D4801E, 0x4A862F58, 0x4E4DD2F1, 0x5229C779, 0x5618BD65, 0x5A18BD65, 0x5E28240A, 0x62454DE7, 
	0x666E43A9, 0x6AA1B5C6, 0x6EDD590B, 0x732031CD, 0x7767F4DA, 0x7BB2FEC4, 0x7FFFFFFF
};

/* const uint32_t PITCHGAIN[128] = {
	0x7FFFFFFF, 0x7F3721D4, 0x7E6E43A9, 0x7DA5657E, 0x7CDC8753, 0x7C135546, 0x7B4A771B, 0x7A8198F0, 0x79B866E3, 0x78EF88B8, 0x782656AA, 
	0x775D249D, 0x7693F28F, 0x75CAC082, 0x75018E74, 0x74380884, 0x736ED676, 0x72A55086, 0x71DB76B2, 0x7111F0C2, 0x704816EF, 0x6F7E3D1B, 
	0x6EB46348, 0x6DEA8975, 0x6D205BBF, 0x6C562E09, 0x6B8BAC70, 0x6AC17EBA, 0x69F6A93E, 0x692C27A5, 0x68615229, 0x679628CB, 0x66CB534F, 
	0x65FFD60D, 0x6534ACAF, 0x64692F6D, 0x639D5E49, 0x62D18D25, 0x6205681E, 0x61394316, 0x606D1E0F, 0x5FA05143, 0x5ED3D859, 0x5E06B7A9, 
	0x5D3996F9, 0x5C6C764A, 0x5B9F01B7, 0x5AD13942, 0x5A0370CD, 0x59355474, 0x5866E439, 0x579873FE, 0x56C9AFE1, 0x55FA97E0, 0x552B7FDF, 
	0x545BC019, 0x538C0053, 0x52BC408C, 0x51EBD900, 0x511B7174, 0x504AB606, 0x4F79A6B4, 0x4EA89762, 0x4DD6E04B, 0x4D052934, 0x4C32CA57, 
	0x4B606B7A, 0x4A8DB8BA, 0x49BAB217, 0x48E7AB74, 0x4813FD0C, 0x473FFAC1, 0x466BA493, 0x45974E65, 0x44C25071, 0x43ED527D, 0x431800A7, 
	0x42425AED, 0x416C6151, 0x409613D2, 0x3FBFC653, 0x3EE8D10E, 0x3E11DBCA, 0x3D3A92A2, 0x3C63497A, 0x3B8B588D, 0x3AB3BB83, 0x39DB76B3, 
	0x390331E3, 0x382A9930, 0x3751AC9A, 0x3678C004, 0x359F7F8C, 0x34C5EB30, 0x33EC56D5, 0x3312C279, 0x3238DA3B, 0x315E9E1A, 0x308461F9, 
	0x2FAA25D8, 0x2ECF41F1, 0x2DF4B1ED, 0x2D19CE07, 0x2C3EEA20, 0x2B63B256, 0x2A887A8D, 0x29ACEEE0, 0x28D16334, 0x27F5D787, 0x271A4BDB, 
	0x263E6C4C, 0x25628CBC, 0x2486594A, 0x23AA79BB, 0x22CE4649, 0x21F212D7, 0x2115DF65, 0x20395810, 0x1F5D249E, 0x1E809D49, 0x1DA415F4, 
	0x1CC78E9F, 0x1BEB074A, 0x1B0E7FF5, 0x1A31A4BD, 0x19551D68, 0x18789613, 0x179BBADB
}; */

const uint32_t VELGAIN[128] = {
	0x00000000, 0x00049667, 0x0008D8EC, 0x000D6F54, 0x001205BC, 0x0016F006, 0x001BDA51, 0x0021187E, 0x0026AA8E, 0x002C3C9E, 0x00327674, 
	0x0039042D, 0x003F91E6, 0x00471B47, 0x004EA4A8, 0x0056D5CF, 0x005FAEBC, 0x0068DB8B, 0x00730403, 0x007D805E, 0x0088F861, 0x0094C447, 
	0x00A18BD6, 0x00AEFB2A, 0x00BD6627, 0x00CCCCCC, 0x00DCDB37, 0x00EDE54B, 0x00FFEB07, 0x0112EC6B, 0x0126E978, 0x013C3611, 0x01527E52, 
	0x0169C23B, 0x018255B0, 0x019C38B0, 0x01B76B3B, 0x01D3996F, 0x01F16B11, 0x02108C3F, 0x0230FCF8, 0x0252BD3C, 0x027620EE, 0x029AD42C, 
	0x02C17EBA, 0x02E978D4, 0x0313165D, 0x033E5753, 0x036B3BB8, 0x0399C38A, 0x03CA42AE, 0x03FC6540, 0x04307F23, 0x04669057, 0x049E44F9, 
	0x04D7F0ED, 0x0513E814, 0x055182A9, 0x05916872, 0x05D3458C, 0x061719F7, 0x065D3996, 0x06A5A469, 0x06F05A70, 0x073D07C8, 0x078C5436, 
	0x07DD97F6, 0x08317ACC, 0x0887A8D6, 0x08E075F6, 0x093B8E4B, 0x099945B6, 0x09F94855, 0x0A5C3DEE, 0x0AC17EBA, 0x0B29B280, 0x0B94855D, 
	0x0C029F16, 0x0C74A771, 0x0CEBEDFA, 0x0D69C23B, 0x0DEE7818, 0x0E7B5F1B, 0x0F11C6D1, 0x0FB256FF, 0x105E0B4D, 0x1115DF65, 0x11DACEED, 
	0x12AE2973, 0x1390EA9E, 0x1483BA34, 0x1587E7C0, 0x169E1B08, 0x17C7A397, 0x19057D17, 0x1A58A32F, 0x1BC21187, 0x1D42C3C9, 0x1EDBB59D, 
	0x208DE2AB, 0x225A469D, 0x2441DD19, 0x2645F5AD, 0x2867381D, 0x2AA6F3F4, 0x2D05D0F9, 0x2F84CAD5, 0x322584F4, 0x34E8533A, 0x37CE3150, 
	0x3AD8C2A3, 0x3E085B17, 0x415E4A37, 0x44DAE3E6, 0x487C30D2, 0x4C3F3E02, 0x50216C60, 0x54201CD5, 0x5839042C, 0x5C698350, 0x60AEA747, 
	0x650624DC, 0x696D5CF9, 0x6DE15CA5, 0x725FD8AC, 0x76E631F7, 0x7B71C970, 0x7FFFFFFF
};

const uint16_t TIME[128] = {
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

#endif 