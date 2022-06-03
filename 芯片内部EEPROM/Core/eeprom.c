#include "eeprom.h"

void IapIdle();

/*----------------------------
Disable ISP/IAP/EEPROM function
Make MCU in a safe state
----------------------------*/
void IapIdle()
{
    IAP_CONTR = 0;    // Close IAP function
    IAP_CMD = 0;      // Clear command to standby
    IAP_TRIG = 0;     // Clear trigger register
    IAP_ADDRH = 0x80; // Data ptr point to non-EEPROM area
    IAP_ADDRL = 0;    // Clear IAP address to prevent misuse
}

/*----------------------------
Read one byte from ISP/IAP/EEPROM area
Input: addr (ISP/IAP/EEPROM address)
Output:Flash data
----------------------------*/
uint8 IapReadByte(uint16 addr)
{
    uint8 dat; // Data buffer

    IAP_CONTR = ENABLE_IAP; // Open IAP function, and set wait time
    IAP_CMD = CMD_READ;     // Set ISP/IAP/EEPROM READ command
    IAP_ADDRL = addr;       // Set ISP/IAP/EEPROM address low
    IAP_ADDRH = addr >> 8;  // Set ISP/IAP/EEPROM address high
    IAP_TRIG = 0x5a;        // Send trigger command1 (0x5a)
    IAP_TRIG = 0xa5;        // Send trigger command2 (0xa5)
    _nop_();                // MCU will hold here until ISP/IAP/EEPROM operation complete
    dat = IAP_DATA;         // Read ISP/IAP/EEPROM data
    IapIdle();              // Close ISP/IAP/EEPROM function

    return dat; // Return Flash data
}

/*----------------------------
Program one byte to ISP/IAP/EEPROM area
Input: addr (ISP/IAP/EEPROM address)
       dat (ISP/IAP/EEPROM data)
Output:-
----------------------------*/
void IapProgramByte(uint16 addr, uint8 dat)
{
    IAP_CONTR = ENABLE_IAP; // Open IAP function, and set wait time
    IAP_CMD = CMD_PROGRAM;  // Set ISP/IAP/EEPROM PROGRAM command
    IAP_ADDRL = addr;       // Set ISP/IAP/EEPROM address low
    IAP_ADDRH = addr >> 8;  // Set ISP/IAP/EEPROM address high
    IAP_DATA = dat;         // Write ISP/IAP/EEPROM data
    IAP_TRIG = 0x5a;        // Send trigger command1 (0x5a)
    IAP_TRIG = 0xa5;        // Send trigger command2 (0xa5)
    _nop_();                // MCU will hold here until ISP/IAP/EEPROM operation complete
    IapIdle();
}

/*----------------------------
Erase one sector area
Input: addr (ISP/IAP/EEPROM address)
Output:-
----------------------------*/
void IapEraseSector(uint16 addr)
{
    IAP_CONTR = ENABLE_IAP; // Open IAP function, and set wait time
    IAP_CMD = CMD_ERASE;    // Set ISP/IAP/EEPROM ERASE command
    IAP_ADDRL = addr;       // Set ISP/IAP/EEPROM address low
    IAP_ADDRH = addr >> 8;  // Set ISP/IAP/EEPROM address high
    IAP_TRIG = 0x5a;        // Send trigger command1 (0x5a)
    IAP_TRIG = 0xa5;        // Send trigger command2 (0xa5)
    _nop_();                // MCU will hold here until ISP/IAP/EEPROM operation complete
    IapIdle();
}

void eeprom_WriteData(uint16 addr, void *dat, uint16 len)
{
    uint16 i;
    uint8 *p = (uint8 *)dat;
    for (i = 0; i < len; i++)
    {
        IapProgramByte(addr + i, p[i]);
    }
}

void eeprom_ReadData(uint16 addr, void *dat, uint16 len)
{
    uint16 i;
    uint8 *p = (uint8 *)dat;
    for (i = 0; i < len; i++)
    {
        p[i] = IapReadByte(addr + i);
    }
}