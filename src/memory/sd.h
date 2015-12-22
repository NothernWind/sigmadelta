/*!
 --------------------------------------------------------------------
 \file
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#ifndef SD_H
#define SD_H

#include "main.h"

// Defines ----------------------------------------------------------

// Команды в режиме SPI
#define CMD0	0	// R1  | GO_IDLE_STATE
#define CMD1	1	// R1  | SEND_OP_COND
#define CMD6	6	// R1  | SWITCH_FUNC
#define CMD8	8	// R7  | SEND_IF_COND
#define CMD9	9	// R1  | SEND_CSD
#define CMD10	10	// R1  | SEND_CID
#define CMD12	12	// R1b | STOP_TRANSMISSION
#define CMD13	13	// R2  | SEND_STATUS
#define CMD16	16	// R1  | SET_BLOCKLEN
#define CMD17	17	// READ_SINGLE_BLOCK
#define CMD18	18	// READ_MULTIPLE_BLOCK
#define CMD24	24	// WRITE_BLOCK
#define CMD25	25	// WRITE_MULTIPLE_BLOCK
#define CMD27	27	// PROGRAM_CSD
#define CMD28	28	// SET_WRITE_PROT
#define CMD29	29	// CLR_WRITE_PROT
#define CMD30	30	// SEND_WRITE_PROT
#define CMD32	32	// ERASE_WR_BLK_START_ADDR
#define CMD33	33	// ERASE_WR_BLK_END_ADDR
#define CMD38	38	// ERASE
#define CMD42	42	// LOCK_UNLOCK
#define CMD55	55	// APP_CMD
#define CMD56	56	// GEN_CMD
#define CMD58	58	// READ_OCR
#define CMD59	59	// CRC_ON_OFF

#define ACMD13	13	// SD_STATUS
#define ACMD18	18	// SEND_NUM_WR_BLOCKS
#define ACMD22	22	// SET_WR_BLK_ERASE_COUNT
#define ACMD23	23	// SD_SEND_OP_COND
#define ACMD25	25	// 
#define ACMD26	26	// 
#define ACMD38	38	// 
#define ACMD41	41	// SET_CLR_CARD_DETECT
#define ACMD42	42	// SET_CLR_CARD_DETECT
#define ACMD43	43	// 
#define ACMD44	44	// 
#define ACMD45	45	// 
#define ACMD46	46	// 
#define ACMD47	47	// 
#define ACMD48	48	// 
#define ACMD49	49	// 
#define ACMD51	51	// SEND_SCR

// R1 Code mask
#define SD_IDLE_STATE			0x01
#define SD_ERASE_RESET			0x02
#define SD_ILLEGAL_COMMAND		0x04
#define SD_COM_CRC_ERROR		0x08
#define SD_ERASE_SEQUENCE_ERROR	0x10
#define SD_ADDRESS_ERROR		0x20
#define SD_PARAMETER_ERROR		0x40

// Коды ошибок
#define SD_OK 				0
#define SD_TIMEOUT_ERROR	1
#define SD_BAD_VOLTAGE		2
#define SD_NOT_INITIALIZED	3
#define SD_READ_ERROR		4
#define SD_READ_OCR_ERROR	5
#define SD_CHECK_OCR_ERROR	6
#define SD_INIT_TIMEOUT		7
#define SD_WRONG_VERSION	8
#define SD_UNKNOWN_ERROR	20

// Typedefs ---------------------------------------------------------
typedef struct {
	volatile unsigned char	CSDStruct;            /*!< CSD structure */
	volatile unsigned char	SysSpecVersion;       /*!< System specification version */
	volatile unsigned char	Reserved1;            /*!< Reserved */
	volatile unsigned char	TAAC;                 /*!< Data read access-time 1 */
	volatile unsigned char	NSAC;                 /*!< Data read access-time 2 in CLK cycles */
	volatile unsigned char	MaxBusClkFrec;        /*!< Max. bus clock frequency */
	volatile unsigned short	CardComdClasses;      /*!< Card command classes */
	volatile unsigned char	RdBlockLen;           /*!< Max. read data block length */
	volatile unsigned char	PartBlockRead;        /*!< Partial blocks for read allowed */
	volatile unsigned char	WrBlockMisalign;      /*!< Write block misalignment */
	volatile unsigned char	RdBlockMisalign;      /*!< Read block misalignment */
	volatile unsigned char	DSRImpl;              /*!< DSR implemented */
	volatile unsigned char	Reserved2;            /*!< Reserved */
	volatile unsigned int 	DeviceSize;           /*!< Device Size */
	volatile unsigned char	MaxRdCurrentVDDMin;   /*!< Max. read current @ VDD min */
	volatile unsigned char	MaxRdCurrentVDDMax;   /*!< Max. read current @ VDD max */
	volatile unsigned char	MaxWrCurrentVDDMin;   /*!< Max. write current @ VDD min */
	volatile unsigned char	MaxWrCurrentVDDMax;   /*!< Max. write current @ VDD max */
	volatile unsigned char	DeviceSizeMul;        /*!< Device size multiplier */
	volatile unsigned char	EraseGrSize;          /*!< Erase group size */
	volatile unsigned char	EraseGrMul;           /*!< Erase group size multiplier */
	volatile unsigned char	WrProtectGrSize;      /*!< Write protect group size */
	volatile unsigned char	WrProtectGrEnable;    /*!< Write protect group enable */
	volatile unsigned char	ManDeflECC;           /*!< Manufacturer default ECC */
	volatile unsigned char	WrSpeedFact;          /*!< Write speed factor */
	volatile unsigned char	MaxWrBlockLen;        /*!< Max. write data block length */
	volatile unsigned char	WriteBlockPaPartial;  /*!< Partial blocks for write allowed */
	volatile unsigned char	Reserved3;            /*!< Reserded */
	volatile unsigned char	ContentProtectAppli;  /*!< Content protection application */
	volatile unsigned char	FileFormatGrouop;     /*!< File format group */
	volatile unsigned char	CopyFlag;             /*!< Copy flag (OTP) */
	volatile unsigned char	PermWrProtect;        /*!< Permanent write protection */
	volatile unsigned char	TempWrProtect;        /*!< Temporary write protection */
	volatile unsigned char	FileFormat;           /*!< File Format */
	volatile unsigned char	ECC;                  /*!< ECC code */
	volatile unsigned char	CSD_CRC;              /*!< CSD CRC */
	volatile unsigned char	Reserved4;            /*!< always 1*/
} SD_CSD;

typedef struct {
	volatile unsigned char  ManufacturerID;       /*!< ManufacturerID */
	volatile unsigned short OEM_AppliID;          /*!< OEM/Application ID */
	volatile unsigned int ProdName1;            /*!< Product Name part1 */
	volatile unsigned char  ProdName2;            /*!< Product Name part2*/
	volatile unsigned char  ProdRev;              /*!< Product Revision */
	volatile unsigned int ProdSN;               /*!< Product Serial Number */
	volatile unsigned char  Reserved1;            /*!< Reserved1 */
	volatile unsigned short ManufactDate;         /*!< Manufacturing Date */
	volatile unsigned char  CID_CRC;              /*!< CID CRC */
	volatile unsigned char  Reserved2;            /*!< always 1 */
} SD_CID;

typedef struct {
	SD_CSD SD_csd;
	SD_CID SD_cid;
	unsigned long CardCapacity;  /*!< Card Capacity */
	unsigned int CardBlockSize; /*!< Card Block Size */
	unsigned short RCA;
	unsigned char CardType;		// 0 - unk. 1 - Ver1; 2 - Ver 2 SDSC; 3 - Ver 2 SDHC;
} SD_Info;

// Extern Variables -------------------------------------------------
extern unsigned char SD_R1_Response;
extern unsigned int SD_R7_Response;
extern unsigned int SD_OCR_Register;
extern int SD_ErrorCode;

// Prototypes -------------------------------------------------------
int SD_Card_Init(void);
int SD_Read_Single_Block(unsigned int sector_addr, unsigned char * block);
int SD_Read_Multi_Blocks(unsigned int sector_addr, 
	unsigned char * block, unsigned char count);

#endif // SD_H
