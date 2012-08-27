//*****************************************************************************
//
//      COPYRIGHT (c) ATMEL Norway, 1996-2001
//
//      The copyright to the document(s) herein is the property of
//      ATMEL Norway, Norway.
//
//      The document(s) may be used  and/or copied only with the written
//      permission from ATMEL Norway or in accordance with the terms and
//      conditions stipulated in the agreement/contract under which the
//      document(s) have been supplied.
//
//*****************************************************************************
//
//  File........: DATAFLASH.H
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: Independent
//
//  Description.: Defines and prototypes for AT45Dxxx
//
//  Revisions...:
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20010117 - 0.10 - Generated file                                -  RM
//  20031009          port to avr-gcc/avr-libc                      - M.Thomas
//
//*****************************************************************************
///#ifndef __DATAFLASH_INCLUDED
//#define __DATAFLASH_INCLUDED

#include <Arduino.h>
#include <Print.h>

//Dataflash opcodes
/*
#define FlashPageRead			0x52	// Main memory page read
#define FlashToBuf1Transfer 	0x53	// Main memory page to buffer 1 transfer
#define Buf1Read				0x54	// Buffer 1 read
#define FlashToBuf2Transfer 	0x55	// Main memory page to buffer 2 transfer
#define Buf2Read				0x56	// Buffer 2 read
#define StatusReg				0x57	// Status register
#define AutoPageReWrBuf1		0x58	// Auto page rewrite through buffer 1
#define AutoPageReWrBuf2		0x59	// Auto page rewrite through buffer 2
#define FlashToBuf1Compare    	0x60	// Main memory page to buffer 1 compare
#define FlashToBuf2Compare	    0x61	// Main memory page to buffer 2 compare
#define ContArrayRead			0x68	// Continuous Array Read (Note : Only A/B-parts supported)
#define FlashProgBuf1			0x82	// Main memory page program through buffer 1
#define Buf1ToFlashWE   		0x83	// Buffer 1 to main memory page program with built-in erase
#define Buf1Write				0x84	// Buffer 1 write
#define FlashProgBuf2			0x85	// Main memory page program through buffer 2
#define Buf2ToFlashWE   		0x86	// Buffer 2 to main memory page program with built-in erase
#define Buf2Write				0x87	// Buffer 2 write
#define Buf1ToFlash     		0x88	// Buffer 1 to main memory page program without built-in erase
#define Buf2ToFlash		        0x89	// Buffer 2 to main memory page program without built-in erase
#define PageErase               0x81	// Page erase, added by Martin Thomas
*/
//Command Opcode
#define		MainMemoryPageRead					0xD2
#define		ContinuousArrayReadLegacy			0xE8
#define		ContinuousArrayReadLowFrequency		0x03
#define		ContinuousArrayReadHighFrequency	0x0B
#define		Buffer1ReadLowFrequency				0xD1
#define		Buffer2ReadLowFrequency				0xD3
#define		Buffer1Read							0xD4
#define		Buffer2Read							0xD6

#define		Buffer1Write						0x84
#define		Buffer2Write						0x87
#define		Buffer1toMainMemoryPageProgramWithBuiltinErase 0x83
#define		Buffer2toMainMemoryPageProgramWithBuiltinErase 0x86
#define		Buffer1toMainMemoryPageProgramWithOutBuiltinErase 0x88
#define		Buffer2toMainMemoryPageProgramWithOutBuiltinErase 0x89
#define		PageErase							0x81
#define		BlockErase							0x50
#define		SectorErase							0x7C
#define		ChipErase							0xC7
#define		MainMemoryPageProgramThroughBuffer1	0x82
#define		MainMemoryPageProgramThroughBuffer2 0x85

/*
Enable Sector Protection 3DH + 2AH + 7FH + A9H
Disable Sector Protection 3DH + 2AH + 7FH + 9AH
Erase Sector Protection Register 3DH + 2AH + 7FH + CFH
Program Sector Protection Register 3DH + 2AH + 7FH + FCH
Read Sector Protection Register 32H
Sector Lockdown 3DH + 2AH + 7FH + 30H
Read Sector Lockdown Register 35H
Program Security Register 9BH + 00H + 00H + 00H
Read Security Register 77H
*/

#define		MainMemoryPagetoBuffer1Transfer		0x53
#define		MainMemoryPagetoBuffer2Transfer		0x55
#define		MainMemoryPageToBuffer1Compare		0x60
#define		MainMemoryPageToBuffer2Compare		0x61
//Auto Page Rewrite through Buffer 1 58H
//Auto Page Rewrite through Buffer 2 59H
//Deep Power-down B9H
//Resume from Deep Power-down ABH
#define		StatusRegisterRead					0xD7
//Manufacturer and Device ID Read 9FH


#include <inttypes.h>
#include <Arduino.h>
#include <avr/pgmspace.h>


#define Status_RDYBUSY 0x80

class DataFlash : Stream {
	byte chipID;
	unsigned int bytesPerPage;
	unsigned int pageAddressBits;
	//
	unsigned int cachedPage[2];
	bool cached[2];
	unsigned long writepointer, readpointer;

private:
//	unsigned char DF_SPI_RW (unsigned char output);
//	void DF_SPI_init (void);
	unsigned char ReadStatusRegister (void);
	
	void deselect(void);
	void select(void);
	
	bool notBusy();
	
public:
	DataFlash(byte);
	~DataFlash() {}
	bool init(void);
	byte status();

	void PageToBufferTransfer (unsigned int addr, byte bufsel = 1);
	unsigned char BufferRead (unsigned int addr, byte bufsel = 1);
	void BufferRead (unsigned int addr, unsigned int length, unsigned char *BufferPtr);
	void Buffer_Write_Enable (unsigned int IntPageAdr);
	void BufferWrite (unsigned int IntPageAdr, unsigned char data, byte bufsel = 1);
	void BufferWrite (unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr);
	void BufferToPageTransfer (unsigned int PageAdr, byte bufsel = 1);
	void Cont_Flash_Read_Enable (unsigned int PageAdr, unsigned int IntPageAdr);
	void ContinuousArrayRead(unsigned long addr, byte array[], byte stop, bool stopEnable);
	void ContinuousArrayRead(unsigned long addr, byte array[], byte stop) { ContinuousArrayRead(addr, array, stop, true); }
	void ContinuousArrayRead(unsigned long addr, byte array[]) { ContinuousArrayRead(addr, array, 0, false); }
	
	void reset();
	virtual int read();
	virtual int read(unsigned long address );
	virtual size_t write(byte b);
	virtual size_t write(byte b, unsigned long addr);
	void flush();
	
	void Page_Erase (unsigned int PageAdr); 
	unsigned char Page_Buffer_Compare(unsigned int PageAdr); 
	
	unsigned int pageSize() { return bytesPerPage; }
	unsigned int pageBits() { return pageAddressBits; }

};
// *****************************[ End Of DATAFLASH.H ]*****************************
