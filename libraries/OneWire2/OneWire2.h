#ifndef OneWire_h
#define OneWire_h

#include <avr/pgmspace.h>
#include <inttypes.h>

#include <WProgram.h>

// you can exclude onewire_search by defining that to 0
#ifndef ONEWIRE_SEARCH
#define ONEWIRE_SEARCH 1
#endif

// You can exclude CRC checks altogether by defining this to 0
#ifndef ONEWIRE_CRC
#define ONEWIRE_CRC 1
#endif

// Select the table-lookup method of computing the 8-bit CRC by setting this to 1
#ifndef ONEWIRE_CRC8_TABLE
#define ONEWIRE_CRC8_TABLE 0
#endif

// You can allow 16-bit CRC checks by defining this to 1
// (Note that ONEWIRE_CRC must also be 1.)
#ifndef ONEWIRE_CRC16
#define ONEWIRE_CRC16 0
#endif

class OneWire {
  private:
#if ONEWIRE_SEARCH
    int device_no;  
	byte last0junction;
    byte exhausted;
#endif
    byte pin;
    byte port;
    byte bitmask;
    volatile byte *outputReg;
    volatile byte *inputReg;
    volatile byte *modeReg;

public:
  int device() { return device_no; }
  
  const static byte READ_ROM    = 0x33; // or 0x0f
  const static byte MATCH_ROM   = 0x55;
  const static byte SKIP_ROM    = 0xCC;
  const static byte SEARCH_ROM  = 0xF0;
  
  
  public:
    OneWire( byte pin);
  
  byte type(byte address[8]) { return *address; }
    
    // Perform a 1-Wire reset cycle. Returns 1 if a device responds
    // with a presence pulse.  Returns 0 if there is no device or the
    // bus is shorted or otherwise held low for more than 250uS
    byte reset();

    // Issue a 1-Wire rom select command, you do the reset first.
  void select( byte address[8]);

    // Issue a 1-Wire rom skip command, to address all on bus.
    void skip();

    // Write a byte. If 'power' is one then the wire is held high at
    // the end for parasitically powered devices. You are responsible
    // for eventually depowering it by calling depower() or doing
    // another read or write.
    void write( byte v, byte power = 0);

    // Read a byte.
    byte read();
  byte * read(byte buf[], int bytes) { int i; for (i = 0; i < bytes; i++) buf[i] = read(); return buf; }

    // Write a bit. The bus is always left powered at the end, see
    // note in write() about that.
    void write_bit( byte v);

    // Read a bit.
    byte read_bit();

    // Stop forcing power onto the bus. You only need to do this if
    // you used the 'power' flag to write() or used a write_bit() call
    // and aren't about to do another read or write. You would rather
    // not leave this powered if you don't have to, just in case
    // someone shorts your bus.
    void depower();

#if ONEWIRE_SEARCH
    // Clear the search state so that if will start from the beginning again.
    void search_begin(byte address[8]);
    
    // Look for the next device. Returns 1 if a new address has been
    // returned. A zero might mean that the bus is shorted, there are
    // no devices, or you have already retrieved all of them.  It
    // might be a good idea to check the CRC to make sure you didn't
    // get garbage.  The order is deterministic. You will always get
    // the same devices in the same order.
    byte search_next(byte address[8]);
	
	byte search_exhausted() { return exhausted; }
	
	// address of nth(including 0th) device
	byte find(int devNo, byte address[8]);
	
#endif

#if ONEWIRE_CRC
    // Compute a Dallas Semiconductor 8 bit CRC, these are used in the
    // ROM and scratchpad registers.
    static byte crc8(byte * addr, byte len);

#if ONEWIRE_CRC16
    // Compute a Dallas Semiconductor 16 bit CRC. Maybe. I don't have
    // any devices that use this so this might be wrong. I just copied
    // it from their sample code.
    static unsigned short crc16(unsigned short *data, unsigned short len);
#endif
  
  static char * family_name(byte addr[8], char * name);
  
  static byte is_temperature_sensor(byte addr[8]) {
    switch (*addr) {
      case 0x10:
      case 0x22: 
      case 0x28:
        return 1;
    }
    return 0;
  }
  
#endif

};

#endif
