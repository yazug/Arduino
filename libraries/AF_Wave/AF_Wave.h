
#ifndef _AF_Wave_h_
#define _AF_Wave_h_

#include "sd_raw.h"
#include "sd_raw_config.h"
#include "sd-reader_config.h"
#include "partition.h"
#include "partition_config.h"
#include "fat16.h"

typedef struct fat16_file_struct * File;

class AF_Wave {
  struct partition_struct *partition;
  struct fat16_fs_struct* fs;
  struct fat16_dir_struct* dd;
  
 public:
  AF_Wave(void);
  uint8_t init_card(void);
  uint8_t open_partition(void);
  uint8_t open_filesys(void);
  uint8_t open_rootdir();
  void close_dir(void);
  uint8_t get_next_name_in_dir(char *);
  void reset_dir(void);
  File open_file(char *name);
  void close_file(File f);
};

#endif
