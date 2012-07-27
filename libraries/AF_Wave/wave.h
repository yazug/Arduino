typedef struct fat16_file_struct * File;
class Wavefile {
 public:
  Wavefile(void);
  void play(void);
  uint8_t create(File f);
  void seek(int32_t location);
  void stop(void);
  uint32_t getSize(void);
  void setSampleRate(uint32_t);

  uint8_t Channels;
  uint32_t dwSamplesPerSec;
  uint16_t wBlockAlign;
  uint8_t BitsPerSample;
  uint32_t remainingBytesInChunk;
  uint32_t chunkSize;
  

  volatile uint8_t isplaying;
  uint32_t errors;
  uint8_t inmiddlesector;
  uint8_t volume;

  struct fat16_file_struct* fd;
};


uint16_t readwavhack(Wavefile *wav, uint8_t *buff, uint16_t len);
