#pragma once

#define pName_addr_0 0x74
#define pName_addr_1 0xbc
#define pNumber_addr 0xcf
#define sNumber_addr 0xe2
#define mac0_addr 0x87
#define mac1_addr 0x100

using namespace std;

int fd;         // eeprom binary file.
string vpdMAC0; // The VPD_Date of MAC0.
string vpdMAC1; // The VPD_Date of MAC1.
string vpdPN;   // The VPD_Date of PartNumber.
string vpdSN;   // The VPD_Date of SerialNumber.
string vpdPTN;  // The VPD_Date of PrettyName.

/**
 * Open the binary file of eeprom.
 */
void openBinFile();

/**
 * Write "PrettyName" to the binary file of eeprom.
 * @param[in] addr - the address which will be written on eeprom.
 */
void rwPrettyName(int addr);

/**
 * Write "PartNumber" to the binary file of eeprom.
 * @param[in] addr - the address which will be written on eeprom.
 */
void rwPartNumber(int addr);

/**
 * Write "SerialNumber" to the binary file of eeprom.
 * @param[in] addr - the address which will be written on eeprom.
 */
void rwSerialNumber(int addr);

/**
 * Write "MAC" to the binary file of eeprom.
 * @param[in] addr - the address which will be written on eeprom.
 * @param[in] data - the data which will be written on eeprom.
 */
void rwMAC(int addr, string data);

/**
 * Transfer the hex to ASCII.
 * @param[in] hex - the hex which will be transfered to the ASCII.
 */
string hexToASCII(string hex);

/**
 * Paser the json file.
 * @param[in] filename - the Json file.
 */
void ParseJsonFromFile(const char* filename);
