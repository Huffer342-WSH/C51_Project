#include "rc522.h"

#include "string.h"

#include "delay.h"

#define MAXRLEN 18

/*========================== ���Žӿ� ==============================*/
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
/*========================== �����ӿ� ==============================*/
#define RC522_Delayms delayms

/*========================== SPIͨѶ�� ==============================*/

/**
 * @brief SPI����һ���ֽ�
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
 * @brief SPI����һ���ֽ�
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

/*========================== RC522ͨѶ�� ==============================*/

/**
 * @brief ��RC522�ļĴ�������д����
 *
 * @param Address �Ĵ����ĵ�ַ
 * @param value ��д�������
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
 * @brief ����RC522�Ĵ��������ֵ
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
 * @brief RC522�Ĵ�����λ
 *
 * @param reg �Ĵ�����ַ
 * @param mask ��λֵ
 */
static void SetBitMask(uint8 reg, uint8 mask)
{
    uint8 tmp = 0x00;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp | mask); // clear bit mask
}

/**
 * @brief RC522�Ĵ���λ����
 *
 * @param reg �Ĵ�����ַ
 * @param mask ��λֵ
 */
static void ClearBitMask(uint8 reg, uint8 mask)
{
    uint8 tmp = 0x00;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask); // clear bit mask
}

/*========================== RC522������ ==============================*/

/**
 * @brief ��������
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
    RC522_Delayms(1); //ÿ��������ر����շ���֮��Ӧ������1ms�ļ��
}

/**
 * @brief �ر�����
 *
 */
void PcdAntennaOff()
{
    ClearBitMask(TxControlReg, 0x03);
}

/**
 * @brief ��λRC522
 *
 * @return int8 �ɹ�����MI_OK
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

    WriteRawRC(ModeReg, 0x3D);   //������������SIGIN����Ч  ����Mifare��ͨѶ��CRC��ʼֵ0x6363
    WriteRawRC(TReloadRegL, 30); //��ʱ����װ��ֵ
    WriteRawRC(TReloadRegH, 0);
    WriteRawRC(TModeReg, 0x8D);      //��ʱ�����ã���Ƶֵ
    WriteRawRC(TPrescalerReg, 0x3E); //��ʱ������
    WriteRawRC(TxAutoReg, 0x40);

    return MI_OK;
}

/**
 * @brief ��RC522����CRC16����
 *
 * @param pIndata CRC����
 * @param len �������ݳ���
 * @param pOutData CRC����bit��ַ
 */
void CalulateCRC(uint8 *pIndata, uint8 len, uint8 *pOutData)
{
    uint8 i, n;
    ClearBitMask(DivIrqReg, 0x04);    ///��CRC��ɱ�־λ
    WriteRawRC(CommandReg, PCD_IDLE); //ȡ����ǰ����
    SetBitMask(FIFOLevelReg, 0x80); // FIFO����������дָ���Լ�FIFO��������������־BufferOvfl����
    for (i = 0; i < len; i++)
    {
        WriteRawRC(FIFODataReg, *(pIndata + i));
    }
    WriteRawRC(CommandReg, PCD_CALCCRC); //����CRC����
    i = 0xFF;
    do
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    } while ((i != 0) && !(n & 0x04));      //���CRC��ɱ�־λ
    pOutData[0] = ReadRawRC(CRCResultRegL); //��CRC������
    pOutData[1] = ReadRawRC(CRCResultRegM);
}

/**
 * @brief ͨ��RC522��ISO14443��ͨѶ
 *
 * @param Command RC522������
 * @param pInData ͨ��RC522���͵���Ƭ������
 * @param InLenByte �������ݵ��ֽڳ���
 * @param pOutData ���յ��Ŀ�Ƭ��������
 * @param pOutLenBit �������ݵ�λ����
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

    WriteRawRC(ComIEnReg, irqEn | 0x80); //���ø��ж����󴫵ݵ�IRQ���ţ�D7=1:���ж�ʱIRQΪ�ͣ�
    ClearBitMask(ComIrqReg, 0x80);       // ComIrqReg����λ����
    WriteRawRC(CommandReg, PCD_IDLE);    //ȡ����ǰ����
    SetBitMask(FIFOLevelReg, 0x80); // FIFO����������дָ���Լ�FIFO��������������־BufferOvfl����

    for (i = 0; i < InLenByte; i++)
    {
        WriteRawRC(FIFODataReg, pInData[i]); //Ҫ���͵�����д��FIFO������
    }
    WriteRawRC(CommandReg, Command);

    if (Command == PCD_TRANSCEIVE)
    {
        SetBitMask(BitFramingReg, 0x80); // start TX
    }

    i = 600; //����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
    do
    {
        n = ReadRawRC(ComIrqReg); //���ж������־
        i--;
    } while ((i != 0) && !(n & 0x01) && !(n & waitFor));

    ClearBitMask(BitFramingReg, 0x80); // stop TX

    if (i != 0)
    {
        if (!(ReadRawRC(ErrorReg) & 0x1B)) //����Ƿ��г��ֵ��ִ���
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {
                status = MI_NOTAGERR;
            }
            if (Command == PCD_TRANSCEIVE)
            {
                n = ReadRawRC(FIFOLevelReg); //��FIFO�������е��ֽ���
                lastBits = ReadRawRC(ControlReg) & 0x07; //�鿴�����յ����ֽ���Чλ����Ŀ�����Ϊ0�������ֽ���Ч
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
                } //��ȡ���յ�������
            }
        }
        else
        {
            status = MI_ERR;
        }
    }
    SetBitMask(ControlReg, 0x80);     // stop timer now
    WriteRawRC(CommandReg, PCD_IDLE); //ȡ����ǰ����
    return status;
}

/**
 * @brief Ѱ��
 *
 * @param req_code Ѱ����ʽ
 * @param pTagType ��Ƭ���ʹ���
 *                 0x4400 = Mifare_UltraLight
 *                 0x0400 = Mifare_One(S50)
 *                 0x0200 = Mifare_One(S70)
 *                 0x0800 = Mifare_Pro(X)
 *                 0x4403 = Mifare_DESFire
 * @return int8 �ɹ�����MI_OK
 */
int8 PcdRequest(uint8 req_code, CardSerial_t *pTagType)
{
    uint8 status;
    unsigned int unLen;
    uint8 ucComRC522Buf[MAXRLEN];

    ClearBitMask(Status2Reg, 0x08); //��������־λ
    WriteRawRC(BitFramingReg,
               0x07); //ֹͣ���ͣ����ý��յ���LSB����λ0�����յ��ĵ�2 λ����λ1��D2D1D0=111��ʾ���һ���ֽڷ�7λ
    SetBitMask(TxControlReg, 0x03); //�������Ƿ��͵����������ã���������

    ucComRC522Buf[0] = req_code;

    status = PcdComRC522(PCD_TRANSCEIVE, ucComRC522Buf, 1, ucComRC522Buf, &unLen);

    if ((status == MI_OK) && (unLen == 0x10)) //����16λ��Ƭ���ʹ���
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
 * @brief ����ײ
 *
 * @param pSnr ��Ƭ���к�
 * @return int8 �ɹ�����MI_OK
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
 * @brief ���Ƭ��������״̬
 *
 * @return int8 �ɹ�����MI_OK
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
 * @brief ѡ����Ƭ
 *
 * @param pSnr ��Ƭ���к�
 * @return uint8 �ɹ�����MI_OK
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
 * @brief ��֤��Ƭ����
 *
 * @param auth_mode ������֤ģʽ
 *                  0x60 = ��֤A��Կ
 *                  0x61 = ��֤B��Կ
 * @param addr  ���ַ
 * @param pKey  ����
 * @param pSnr  ��Ƭ���кţ�4�ֽ�
 * @return int8 �ɹ�����MI_OK
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
 * @brief  ��ȡM1��һ������
 *
 * @param addr ���ַ
 * @param pData ���������ݣ�16�ֽ�
 * @return int8 �ɹ�����MI_OK
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
 * @brief д���ݵ�M1��һ��
 *
 * @param addr ���ַ
 * @param pData д������ݣ�16�ֽ�
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

    //ѡ��ַ
    status = PcdComRC522(PCD_TRANSCEIVE, ucComRC522Buf, 4, ucComRC522Buf, &unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComRC522Buf[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }

    //������
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
/*========================== RC522Ӧ�ò� ==============================*/

void PcdInit()
{
    PcdReset();
    M500PcdConfigISOType('A');
    PcdAntennaOff();
    PcdAntennaOn();
}

/**
 * @brief ����RC522�Ĺ�����ʽ
 *
 * @param type ������ʽ
 * @return uint8 �ɹ�:MI_OK ʧ��:-1
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
        status = PcdRequest(0x52, id); //Ѱ��
        if (status)
        {
            PcdReset();
            PcdAntennaOff();
            PcdAntennaOn();
            continue;
        }
        status = PcdAnticoll(id); //����ײ
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
//��    �ܣ��ۿ�ͳ�ֵ
//����˵��: dd_mode[IN]��������
//               0xC0 = �ۿ�
//               0xC1 = ��ֵ
//          addr[IN]��Ǯ����ַ
//          pValue[IN]��4�ֽ���(��)ֵ����λ��ǰ
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ�����Ǯ��
//����˵��: sourceaddr[IN]��Դ��ַ
//          goaladdr[IN]��Ŀ���ַ
//��    ��: �ɹ�����MI_OK
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