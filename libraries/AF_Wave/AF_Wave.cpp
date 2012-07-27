#include <avr/io.h>
#include "WProgram.h"
#include <string.h>
#include "AF_Wave.h"
#include "fat16.h"
#include "sd_raw.h"

AF_Wave::AF_Wave(void) {

}

uint8_t AF_Wave::init_card(void) {
  return sd_raw_init();
}

uint8_t AF_Wave::open_partition(void) {
  /* open first partition */
  partition = 
    partition_open(sd_raw_read,
		   sd_raw_read_interval,
		   0,
		   0,
		   0);

  if(!partition) {
    /* If the partition did not open, assume the storage device
     * is a "superfloppy", i.e. has no MBR.
     */
    partition = partition_open(sd_raw_read,
			       sd_raw_read_interval,
			       0,
			       0,
			       -1);
  }
  if(!partition) {
    return 0;
  }
  return 1;
}
uint8_t AF_Wave::open_filesys(void) {
  /* open file system */
  fs = fat16_open(partition);
  if(!fs) {
    return 0;
  }
  
  return 1;
}

uint8_t AF_Wave::open_rootdir() {
  // Open root directory
  struct fat16_dir_entry_struct rootdirectory;

  fat16_get_dir_entry_of_root(fs, &rootdirectory);
  dd = fat16_open_dir(fs, &rootdirectory);
  if(!dd) {
    return 0;
  }
  return 1;
}
void AF_Wave::close_dir(void) {
  // close it
  fat16_close_dir(dd);
}

uint8_t AF_Wave::get_next_name_in_dir(char *name) {
  struct fat16_dir_entry_struct dir_entry;

  if(fat16_read_dir(dd, &dir_entry))  {
    strncpy(name, dir_entry.long_name, 12);
    name[12] = 0;
    return 1;
  }
  return 0;
}

void AF_Wave::reset_dir(void) {
  fat16_reset_dir(dd);
}

uint8_t find_file_in_dir(struct fat16_fs_struct* fs, struct fat16_dir_struct* dd, const char* name, struct fat16_dir_entry_struct* dir_entry)
{
    while(fat16_read_dir(dd, dir_entry))
    {
      if(strcmp(dir_entry->long_name, name) == 0)
        {
	  fat16_reset_dir(dd);
	  return 1;
        }
    }
    
    return 0;
}


struct fat16_file_struct* open_file_in_dir(struct fat16_fs_struct* fs, struct fat16_dir_struct* dd, const char* name)
{
    struct fat16_dir_entry_struct file_entry;
    if(!find_file_in_dir(fs, dd, name, &file_entry)) {

      //Serial.println("File not found");

        return 0;
    }
    return fat16_open_file(fs, &file_entry);
}


File AF_Wave::open_file(char *name) {
  return open_file_in_dir(fs, dd, name);
}

void AF_Wave::close_file(File f) {
   fat16_close_file(f);
}

