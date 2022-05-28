#include "rc522.h"

#include "string.h"

#include "delay.h"

#define MAXRLEN 18

/*========================== 引脚接口 ==============================*/
// #define RC522_NSS RC522_NSS
// #define RC522_SCK RC522_SCK
// #define RC522_MOSI RC522_MOSI
// #define RC522_MISO RC522_MISO
// #define RC522_RST RC522_RST

#define RC522_NSS P23
#define RC522_SCK P24
#define RC522_MOSI P25
#define RC522_MISO P26
#define RC522_RST P27
/*========================== 函数接口 ==============================*/
#define RC522_Delayms delayms

/*========================== SPI通讯层 ==============================*/

/**
 * @brief SPI发送一个字节
 *
 * @param byte
 */
static void SPI_Transmit(uint8 byte)
{
    uint8 i;
    for (i = 0; i < 8; i++)
    {
        byte <<= 1;
        RC522_MOSI = CY;
        RC522_SCK = 1;
        RC522_SCK = 0;
    }
}

/**
 * @brief SPI接收一个字节
 *
 * @return uint8
 */
static uint8 SPI_Receive(void)
{
    uint8 i;
    uint8 byte;
    for (i = 0; i < 8; i++)
    {
        RC522_SCK = 1;
        byte <<= 1;
        byte |= (RC522_MISO == 1);
        RC522_SCK = 0;
    }
    return byte;
}

/*========================== RC522通讯层 ==============================*/

/**
 * @brief 向RC522的寄存器里面写命令
 *
 * @param Address 寄存器的地址
 * @param value 待写入的命令
 */
static void WriteRawRC(uint8 Address, uint8 value)
{
    RC522_SCK = 0;
    RC522_NSS = 0;
    SPI_Transmit((Address << 1) & 0x7E);
    SPI_Transmit(value);
    RC522_NSS = 1;
    RC522_SCK = 1;
}

/**
 * @brief 读出RC522寄存器里面的值
 *
 * @param Address
 * @return uint8
 */
static uint8 ReadRawRC(uint8 Address)
{
    uint8 res;
    RC522_SCK = 0;
    RC522_NSS = 0;
    SPI_Transmit((((Address << 1) & 0x7e) | 0x80));
    res = SPI_Receive();
    RC522_NSS = 1;
    RC522_SCK = 1;
    return res;
}

/**
 * @brief RC522寄存器置位
 *
 * @param reg 寄存器地址
 * @param mask 置位值
 */
static void SetBitMask(uint8 reg, uint8 mask)
{
    uint8 tmp = 0x00;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp | mask); // clear bit mask
}

/**
 * @brief RC522寄存器位清零
 *
 * @param reg 寄存器地址
 * @param mask 清位值
 */
static void ClearBitMask(uint8 reg, uint8 mask)
{
    uint8 tmp = 0x00;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask); // clear bit mask
}

/*========================== RC522驱动层 ==============================*/

/**
 * @brief 开启天线
 *
 */
void PcdAntennaOn()
{
    uint8 i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
    RC522_Delayms(1); //每次启动或关闭天险发射之间应至少有1ms的间隔
}

/**
 * @brief 关闭天线
 *
 */
void PcdAntennaOff()
{
    ClearBitMask(TxControlReg, 0x03);
}

/**
 * @brief 复位RC522
 *
 * @return int8 成功返回MI_OK
 */
int8 PcdReset(void)
{
    RC522_RST = 1;
    _nop_();
    RC522_RST = 0;
    _nop_();
    RC522_RST = 1;
    _nop_();
    WriteRawRC(CommandReg, PCD_RESETPHASE);
    _nop_();

    WriteRawRC(ModeReg, 0x3D);   //启动发送器，SIGIN高有效  ，和Mifare卡通讯，CRC初始值0x6363
    WriteRawRC(TReloadRegL, 30); //定时器重装载值
    WriteRawRC(TReloadRegH, 0);
    WriteRawRC(TModeReg, 0x8D);      //定时器设置，分频值
    WriteRawRC(TPrescalerReg, 0x3E); //定时器设置
    WriteRawRC(TxAutoReg, 0x40);

    return MI_OK;
}

/**
 * @brief 用RC522计算CRC16函数
 *
 * @param pIndata CRC输入
 * @param len 输入数据长度
 * @param pOutData CRC附加bit地址
 */
void CalulateCRC(uint8 *pIndata, uint8 len, uint8 *pOutData)
{
    uint8 i, n;
    ClearBitMask(DivIrqReg, 0x04);    ///清CRC完成标志位
    WriteRawRC(CommandReg, PCD_IDLE); //取消当前命令
    SetBitMask(FIFOLevelReg, 0x80); // FIFO缓冲区读和写指针以及FIFO缓冲区溢出错误标志BufferOvfl清零
    for (i = 0; i < len; i++)
    {
        WriteRawRC(FIFODataReg, *(pIndata + i));
    }
    WriteRawRC(CommandReg, PCD_CALCCRC); //进行CRC计算
    i = 0xFF;
    do
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    } while ((i != 0) && !(n & 0x04));      //检测CRC完成标志位
    pOutData[0] = ReadRawRC(CRCResultRegL); //读CRC计算结果
    pOutData[1] = ReadRawRC(CRCResultRegM);
}

/**
 * @brief 通过RC522和ISO14443卡通讯
 *
 * @param Command RC522命令字
 * @param pInData 通过RC522发送到卡片的数据
 * @param InLenByte 发送数据的字节长度
 * @param pOutData 接收到的卡片返回数据
 * @param pOutLenBit 返回数据的位长度
 * @return int8
 */
int8 PcdComRC522(uint8 Command, uint8 *pInData, uint8 InLenByte, uint8 *pOutData, unsigned int *pOutLenBit)
{
    uint8 status = MI_ERR;
    uint8 irqEn = 0x00;
    uint8 waitFor = 0x00;
    uint8 lastBits;
    uint8 n;
    unsigned int i;
    switch (Command)
    {
    case PCD_AUTHENT:
        irqEn = 0x12;
        waitFor = 0x10;
        break;
    case PCD_TRANSCEIVE:
        irqEn = 0x77;
        waitFor = 0x30;
        break;
    default:
        break;
    }

    WriteRawRC(ComIEnReg, irqEn | 0x80); //设置各中断请求传递到IRQ引脚，D7=1:有中断时IRQ为低，
    ClearBitMask(ComIrqReg, 0x80);       // ComIrqReg屏蔽位清零
    WriteRawRC(CommandReg, PCD_IDLE);    //取消当前命令
    SetBitMask(FIFOLevelReg, 0x80); // FIFO缓冲区读和写指针以及FIFO缓冲区溢出错误标志BufferOvfl清零

    for (i = 0; i < InLenByte; i++)
    {
        WriteRawRC(FIFODataReg, pInData[i]); //要发送的数据写入FIFO缓冲区
    }
    WriteRawRC(CommandReg, Command);

    if (Command == PCD_TRANSCEIVE)
    {
        SetBitMask(BitFramingReg, 0x80); // start TX
    }

    i = 600; //根据时钟频率调整，操作M1卡最大等待时间25ms
    do
    {
        n = ReadRawRC(ComIrqReg); //读中断请求标志
        i--;
    } while ((i != 0) && !(n & 0x01) && !(n & waitFor));

    ClearBitMask(BitFramingReg, 0x80); // stop TX

    if (i != 0)
    {
        if (!(ReadRawRC(ErrorReg) & 0x1B)) //检测是否有出现的种错误
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {
                status = MI_NOTAGERR;
            }
            if (Command == PCD_TRANSCEIVE)
            {
                n = ReadRawRC(FIFOLevelReg); //读FIFO缓冲区中的字节数
                lastBits = ReadRawRC(ControlReg) & 0x07; //查看最后接收到的字节有效位的数目，如果为0则整个字节有效
                if (lastBits)
                {
                    *pOutLenBit = (n - 1) * 8 + lastBits;
                }
                else
                {
                    *pOutLenBit = n * 8;
                }
                if (n == 0)
                {
                    n = 1;
                }
                if (n > MAXRLEN)
                {
                    n = MAXRLEN;
                }
                for (i = 0; i < n; i++)
                {
                    pOutData[i] = ReadRawRC(FIFODataReg);
                } //获取接收到的数据
            }
        }
        else
        {
            status = MI_ERR;
        }
    }
    SetBitMask(ControlReg, 0x80);     // stop timer now
    WriteRawRC(CommandReg, PCD_IDLE); //取消当前命令
    return status;
}

/**
 * @brief 寻卡
 *
 * @param req_code 寻卡方式
 * @param pTagType 卡片类型代码
 *                 0x4400 = Mifare_UltraLight
 *                 0x0400 = Mifare_One(S50)
 *                 0x0200 = Mifare_One(S70)
 *                 0x0800 = Mifare_Pro(X)
 *                 0x4403 = Mifare_DESFire
 * @return int8 成功返回MI_OK
 */
int8 PcdRequest(uint8 req_code, CardSerial_t *pTagType)
{
    uint8 status;
    unsigned int unLen;
    uint8 ucComRC522Buf[MAXRLEN];

    ClearBitMask(Status2Reg, 0x08); //软件清零标志位
    WriteRawRC(BitFramingReg,
               0x07); //停止发送，设置接收到的LSB放在位0，接收到的第2 位放在位1，D2D1D0=111表示最后一个字节发7位
    SetBitMask(TxControlReg, 0x03); //对天线是否发送调制数据设置，开启天线

    ucComRC522Buf[0] = req_code;

    status = PcdComRC522(PCD_TRANSCEIVE, ucComRC522Buf, 1, ucComRC522Buf, &unLen);

    if ((status == MI_OK) && (unLen == 0x10)) //传回16位卡片类型代码
    {
        pTagType->u8[0] = ucComRC522Buf[0];
        pTagType->u8[1] = ucComRC522Buf[1];
        // *(pTagType + 1) = ucComRC522Buf[1];
    }
    else
    {
        status = MI_ERR;
    }

    return status;
}

/**
 * @brief 防冲撞
 *
 * @param pSnr 卡片序列号
 * @return int8 成功返回MI_OK
 */
int8 PcdAnticoll(CardSerial_t *pSnr)
{
    uint8 status;
    uint8 i, snr_check = 0;
    unsigned int unLen;
    uint8 ucComRC522Buf[MAXRLEN];

    ClearBitMask(Status2Reg, 0x08);
    WriteRawRC(BitFramingReg, 0x00);
    ClearBitMask(CollReg, 0x80);

    ucComRC522Buf[0] = PICC_ANTICOLL1;
    ucComRC522Buf[1] = 0x20;

    status = PcdComRC522(PCD_TRANSCEIVE, ucComRC522Buf, 2, ucComRC522Buf, &unLen);

    if (status == MI_OK)
    {
        for (i = 0; i < 4; i++)
        {
            // *(pSnr + i) = ucComRC522Buf[i];
            pSnr->u8[i] = ucComRC522Buf[i];
            snr_check ^= ucComRC522Buf[i];
        }
        if (snr_check != ucComRC522Buf[i])
        {
            status = MI_ERR;
        }
    }

    SetBitMask(CollReg, 0x80);
    return status;
}

/**
 * @brief 命令卡片进入休眠状态
 *
 * @return int8 成功返回MI_OK
 */
int8 PcdHalt(void)
{
    uint8 status;
    unsigned int unLen;
    uint8 ucComRC522Buf[MAXRLEN];

    ucComRC522Buf[0] = PICC_HALT;
    ucComRC522Buf[1] = 0;
    CalulateCRC(ucComRC522Buf, 2, &ucComRC522Buf[2]);
    status = PcdComRC522(PCD_TRANSCEIVE, ucComRC522Buf, 4, ucComRC522Buf, &unLen);

    return MI_OK;
}

#ifdef ADVANCE_RC522
/**
 * @brief 选定卡片
 *
 * @param pSnr 卡片序列号
 * @return uint8 成功返回MI_OK
 */
int8 PcdSelect(CardSerial_t *pSnr)
{
    uint8 status;
    uint8 i;
    unsigned int unLen;
    uint8 ucComRC522Buf[MAXRLEN];

    ucComRC522Buf[0] = PICC_ANTICOLL1;
    ucComRC522Buf[1] = 0x70;
    ucComRC522Buf[6] = 0;
    for (i = 0; i < 4; i++)
    {
        // ucComRC522Buf[i + 2] = *(pSnr + i);
        // ucComRC522Buf[6] ^= *(pSnr + i);
        ucComRC522Buf[i + 2] = pSnr->u8[i];
        ucComRC522Buf[6] ^= pSnr->u8[i];
    }
    CalulateCRC(ucComRC522Buf, 7, &ucComRC522Buf[7]);

    ClearBitMask(Status2Reg, 0x08);

    status = PcdComRC522(PCD_TRANSCEIVE, ucComRC522Buf, 9, ucComRC522Buf, &unLen);

    if ((status == MI_OK) && (unLen == 0x18))
    {
        status = MI_OK;
    }
    else
    {
        status = MI_ERR;
    }

    return status;
}

/**
 * @brief 验证卡片密码
 *
 * @param auth_mode 密码验证模式
 *                  0x60 = 验证A密钥
 *                  0x61 = 验证B密钥
 * @param addr  块地址
 * @param pKey  密码
 * @param pSnr  卡片序列号，4字节
 * @return int8 成功返回MI_OK
 */
int8 PcdAuthState(uint8 auth_mode, uint8 addr, uint8 *pKey, uint8 *pSnr)
{
    uint8 status;
    unsigned int unLen;
    uint8 i, ucComRC522Buf[MAXRLEN];

    ucComRC522Buf[0] = auth_mode;
    ucComRC522Buf[1] = addr;
    for (i = 0; i < 6; i++)
    {
        ucComRC522Buf[i + 2] = *(pKey + i);
    }
    for (i = 0; i < 6; i++)
    {
        ucComRC522Buf[i + 8] = *(pSnr + i);
    }
    //   memcpy(&ucComRC522Buf[2], pKey, 6);
    //   memcpy(&ucComRC522Buf[8], pSnr, 4);

    status = PcdComRC522(PCD_AUTHENT, ucComRC522Buf, 12, ucComRC522Buf, &unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
    {
        status = MI_ERR;
    }
    return status;
}

/**
 * @brief  读取M1卡一块数据
 *
 * @param addr 块地址
 * @param pData 读出的数据，16字节
 * @return int8 成功返回MI_OK
 */
int8 PcdRead(uint8 addr, uint8 *pData)
{
    uint8 status;
    unsigned int unLen;
    uint8 i, ucComRC522Buf[MAXRLEN];

    ucComRC522Buf[0] = PICC_READ;
    ucComRC522Buf[1] = addr;
    CalulateCRC(ucComRC522Buf, 2, &ucComRC522Buf[2]);

    status = PcdComRC522(PCD_TRANSCEIVE, ucComRC522Buf, 4, ucComRC522Buf, &unLen);
    if ((status == MI_OK) && unLen == 0x90)
    {
        for (i = 0; i < 16; i++)
        {
            *(pData + i) = ucComRC522Buf[i];
        }
    }
    else
    {

        status = MI_ERR;
    }

    return status;
}

/**
 * @brief 写数据到M1卡一块
 *
 * @param addr 块地址
 * @param pData 写入的数据，16字节
 * @return int8
 */
int8 PcdWrite(uint8 addr, uint8 *pData)
{
    uint8 status;
    unsigned int unLen;
    uint8 i, ucComRC522Buf[MAXRLEN];

    ucComRC522Buf[0] = PICC_WRITE;
    ucComRC522Buf[1] = addr;
    CalulateCRC(ucComRC522Buf, 2, &ucComRC522Buf[2]);

    //选地址
    status = PcdComRC522(PCD_TRANSCEIVE, ucComRC522Buf, 4, ucComRC522Buf, &unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComRC522Buf[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }

    //发数据
    if (status == MI_OK)
    {
        // memcpy(ucComRC522Buf, pData, 16);
        for (i = 0; i < 16; i++)
        {
            ucComRC522Buf[i] = *(pData + i);
        }
        CalulateCRC(ucComRC522Buf, 16, &ucComRC522Buf[16]);

        status = PcdComRC522(PCD_TRANSCEIVE, ucComRC522Buf, 18, ucComRC522Buf, &unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComRC522Buf[0] & 0x0F) != 0x0A))
        {
            status = MI_ERR;
        }
    }

    return status;
}
#endif
/*========================== RC522应用层 ==============================*/

void PcdInit()
{
    PcdReset();
    M500PcdConfigISOType('A');
    PcdAntennaOff();
    PcdAntennaOn();
}

/**
 * @brief 设置RC522的工作方式
 *
 * @param type 工作方式
 * @return uint8 成功:MI_OK 失败:-1
 */
int8 M500PcdConfigISOType(uint8 type)
{
    if (type == 'A') // ISO14443_A
    {
        ClearBitMask(Status2Reg, 0x08);
        WriteRawRC(ModeReg, 0x3D);   // 3
        WriteRawRC(RxSelReg, 0x86);  //
        WriteRawRC(RFCfgReg, 0x7F);  // 4F
        WriteRawRC(TReloadRegL, 30); // tmoLength);// TReloadVal = 'h6a =tmoLength(dec)
        WriteRawRC(TReloadRegH, 0);
        WriteRawRC(TModeReg, 0x8D);
        WriteRawRC(TPrescalerReg, 0x3E);
        RC522_Delayms(10);
        PcdAntennaOn();
    }
    else
    {
        return -1;
    }

    return MI_OK;
}

int8 PcdFindcard(CardSerial_t *id)
{
    uint8 count = 0;
    uint8 status;
    while (count++ < 5)
    {
        status = PcdRequest(0x52, id); //寻卡
        if (status)
        {
            PcdReset();
            PcdAntennaOff();
            PcdAntennaOn();
            continue;
        }
        status = PcdAnticoll(id); //防冲撞
        if (status)
        {
            continue;
        }
        else
        {
            PcdHalt();
            return 0;
        }
    }
    return count;
}

#ifdef ADVANCE_RC522
/////////////////////////////////////////////////////////////////////
//功    能：扣款和充值
//参数说明: dd_mode[IN]：命令字
//               0xC0 = 扣款
//               0xC1 = 充值
//          addr[IN]：钱包地址
//          pValue[IN]：4字节增(减)值，低位在前
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
int8 PcdValue(unsigned char dd_mode, unsigned char addr, unsigned char *pValue)
{
    char status;
    unsigned int unLen;
    unsigned char ucComMF522Buf[MAXRLEN];
    // unsigned char i;

    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

    status = PcdComRC522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }

    if (status == MI_OK)
    {
        memcpy(ucComMF522Buf, pValue, 4);
        // for (i=0; i<16; i++)
        //{    ucComMF522Buf[i] = *(pValue+i);   }
        CalulateCRC(ucComMF522Buf, 4, &ucComMF522Buf[4]);
        unLen = 0;
        status = PcdComRC522(PCD_TRANSCEIVE, ucComMF522Buf, 6, ucComMF522Buf, &unLen);
        if (status != MI_ERR)
        {
            status = MI_OK;
        }
    }

    if (status == MI_OK)
    {
        ucComMF522Buf[0] = PICC_TRANSFER;
        ucComMF522Buf[1] = addr;
        CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

        status = PcdComRC522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {
            status = MI_ERR;
        }
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：备份钱包
//参数说明: sourceaddr[IN]：源地址
//          goaladdr[IN]：目标地址
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr)
{
    char status;
    unsigned int unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_RESTORE;
    ucComMF522Buf[1] = sourceaddr;
    CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

    status = PcdComRC522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }

    if (status == MI_OK)
    {
        ucComMF522Buf[0] = 0;
        ucComMF522Buf[1] = 0;
        ucComMF522Buf[2] = 0;
        ucComMF522Buf[3] = 0;
        CalulateCRC(ucComMF522Buf, 4, &ucComMF522Buf[4]);

        status = PcdComRC522(PCD_TRANSCEIVE, ucComMF522Buf, 6, ucComMF522Buf, &unLen);
        if (status != MI_ERR)
        {
            status = MI_OK;
        }
    }

    if (status != MI_OK)
    {
        return MI_ERR;
    }

    ucComMF522Buf[0] = PICC_TRANSFER;
    ucComMF522Buf[1] = goaladdr;

    CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

    status = PcdComRC522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }

    return status;
}

#endif