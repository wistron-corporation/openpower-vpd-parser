#include <iostream>
#include <string>
#include "wEE.hpp"

#include <bits/stdc++.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h> // Includes explicit declaration of lseek.
#include <nlohmann/json.hpp> // For parsing json file.

static const auto JSON_FILE = "/home/root/vpddata.json";

using json = nlohmann::json; // For parsing json file.

int main(void)
{
    // Open the file of eeprom.
    openBinFile();

    // Parse the json file.
    ParseJsonFromFile(JSON_FILE);

    if (fd)
    {
        // Write the data.
        rwPrettyName(pName_addr_0);
        rwPrettyName(pName_addr_1);
        rwPartNumber(pNumber_addr);
        rwSerialNumber(sNumber_addr);
        rwMAC(mac0_addr, vpdMAC0);
        rwMAC(mac1_addr, vpdMAC1);
    }

    // Close the file.
    close(fd);

    return 0;
}

// Open binary file of eeprom.
void openBinFile()
{
    fd = open("/home/root/eeprom", O_RDWR);
    if (fd < 0)
    {
        close(fd);
    }
}

// Write the data of PrettyName.
void rwPrettyName(int addr)
{
    int len, size;
    char* bufw = const_cast<char*>(vpdPTN.c_str());

    // Data lengh.
    len = strlen(bufw);

    if (!fd)
    {
        openBinFile();
    }

    // Position first and then write data.
    lseek(fd, addr, SEEK_SET);
    if ((size = write(fd, bufw, len)) < 0)
    {
        close(fd);
    }
}

// Write the data of PartNumber.
void rwPartNumber(int addr)
{
    int len, size;
    char* bufw = const_cast<char*>(vpdPN.c_str());

    // Data lengh.
    len = strlen(bufw);

    if (!fd)
    {
        openBinFile();
    }

    // Position first and then write data.
    lseek(fd, addr, SEEK_SET);
    if ((size = write(fd, bufw, len)) < 0)
    {
        close(fd);
    }
}

// Write the data of SerialNumber.
void rwSerialNumber(int addr)
{
    int len, size;
    char* bufw = const_cast<char*>(vpdSN.c_str());

    // Data lengh.
    len = strlen(bufw);

    if (!fd)
    {
        openBinFile();
    }

    // Position first and then write data.
    lseek(fd, addr, SEEK_SET);
    if ((size = write(fd, bufw, len)) < 0)
    {
        close(fd);
    }
}

// Write the data of MAC.
// Because the MAC data parsed from the json file
// is a string, if the function writes this string
// directly to the eeprom binary, there will be
// errors in the data (the data written to the eeprom
// binary must be in ASCII format), so this function
// need to separate this string and write separately.
void rwMAC(int addr, string data)
{
    int size;
    string hvalue0 = data.substr(0, 2);
    string hvalue1 = data.substr(2, 2);
    string hvalue2 = data.substr(4, 2);
    string hvalue3 = data.substr(6, 2);
    string hvalue4 = data.substr(8, 2);
    string hvalue5 = data.substr(10, 2);

    int m = hexToASCII(hvalue0).size();
    int n = hexToASCII(hvalue1).size();
    int o = hexToASCII(hvalue2).size();
    int p = hexToASCII(hvalue3).size();
    int q = hexToASCII(hvalue4).size();
    int r = hexToASCII(hvalue5).size();

    char bufw0[m + 1];
    char bufw1[n + 1];
    char bufw2[o + 1];
    char bufw3[p + 1];
    char bufw4[q + 1];
    char bufw5[r + 1];

    strcpy(bufw0, hexToASCII(hvalue0).c_str());
    strcpy(bufw1, hexToASCII(hvalue1).c_str());
    strcpy(bufw2, hexToASCII(hvalue2).c_str());
    strcpy(bufw3, hexToASCII(hvalue3).c_str());
    strcpy(bufw4, hexToASCII(hvalue4).c_str());
    strcpy(bufw5, hexToASCII(hvalue5).c_str());

    if (!fd)
    {
        openBinFile();
    }

    // Position first and then write data separately.
    lseek(fd, addr, SEEK_SET);
    if ((size = write(fd, bufw0, m)) < 0)
    {
        close(fd);
    }

    lseek(fd, addr + 1, SEEK_SET);
    if ((size = write(fd, bufw1, n)) < 0)
    {
        close(fd);
    }

    lseek(fd, addr + 2, SEEK_SET);
    if ((size = write(fd, bufw2, o)) < 0)
    {
        close(fd);
    }

    lseek(fd, addr + 3, SEEK_SET);
    if ((size = write(fd, bufw3, p)) < 0)
    {
        close(fd);
    }

    lseek(fd, addr + 4, SEEK_SET);
    if ((size = write(fd, bufw4, q)) < 0)
    {
        close(fd);
    }

    lseek(fd, addr + 5, SEEK_SET);
    if ((size = write(fd, bufw5, r)) < 0)
    {
        close(fd);
    }
}

// hex-to-ascii c++
string hexToASCII(string hex)
{
    // Initialize the ASCII code string as empty.
    string ascii_value = "";
    for (size_t i = 0; i < hex.length(); i += 2)
    {
        // Extract two characters from hex string.
        string part_value = hex.substr(i, 2);

        // Change it into base 16 and typecast as the character.
        char ch = std::stoul(part_value, nullptr, 16);

        // Add this char to final ASCII string.
        ascii_value += ch;
    }
    return ascii_value;
}

// Parse the json file.
void ParseJsonFromFile(const char* filename)
{
    ifstream jfile;
    jfile.open(filename);
    if (!jfile.is_open())
    {
        std::cerr << "Can't open json file." << std::endl;
    }

    auto jfdata = json::parse(jfile, nullptr, false);
    if (jfdata.is_discarded())
    {
        std::cerr << "Parser json file fail." << std::endl;
    }

    static const std::vector<json> empty{};
    std::vector<json> reading = jfdata.value("vpddata", empty);
    if (!reading.empty())
    {
        for (const auto& instance : reading)
        {
            vpdMAC0 = instance.value("MAC0", "");
            vpdMAC1 = instance.value("MAC1", "");
            vpdPN = instance.value("PN", "");
            vpdSN = instance.value("SN", "");
            vpdPTN = instance.value("PTN", "");
        }
    }
    else
    {
        std::cerr << "Can't get the VPDdata" << std::endl;
    }

    jfile.close();
}
