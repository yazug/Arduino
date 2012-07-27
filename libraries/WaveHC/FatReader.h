/* Arduino FatReader Library
 * Copyright (C) 2009 by William Greiman
 *
 * This file is part of the Arduino FatReader Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with the Arduino FatReader Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef FatReader_h
#define FatReader_h
#include "SdReader.h"
#include "FatStructs.h"
// format dir.name into name[13] as standard 8.3 string
void dirName(dir_t &dir, char name[]);
// offsets for structures used in volume init
/** Offset to BIOS Parameter Block in FAT Boot Sector */
#define BPB_OFFSET 11
/** Byte count for part of BIOS Parameter Block to be read by init() */
#define BPB_COUNT 37   
/** offset to partition table in mbr */
#define PART_OFFSET (512-64-2) 
//macros for file types
/** Directory entry is part of a long name */
#define DIR_IS_LONG_NAME(dir) (((dir).attributes & DIR_ATT_LONG_NAME_MASK) == DIR_ATT_LONG_NAME)
/** Mask for file/subdirectory tests */
#define DIR_ATT_FILE_TYPE_MASK (DIR_ATT_VOLUME_ID | DIR_ATT_DIRECTORY)
/** Directory entry is for a file */
#define DIR_IS_FILE(dir) (((dir).attributes & DIR_ATT_FILE_TYPE_MASK) == 0)
/** Directory entry is for a subdirectory */
#define DIR_IS_SUBDIR(dir) (((dir).attributes & DIR_ATT_FILE_TYPE_MASK) == DIR_ATT_DIRECTORY)
/** \class FatVolume
 * \brief FatVolume provides access to FAT volumes.
 */ 
class FatVolume {
  /** Allow FatReader access to FatVolume private data. */
  friend class FatReader;
  uint8_t blocksPerCluster_;
  uint32_t blocksPerFat_;
  uint32_t clusterCount_;
  uint32_t dataStartBlock_;
  uint8_t fatCount_;
  uint32_t fatStartBlock_;
  uint8_t fatType_;
  SdReader *rawDevice_;
  uint16_t rootDirEntryCount_;
  uint32_t rootDirStart_;
  uint32_t totalBlocks_;
  uint32_t chainSize(uint32_t cluster);
  uint32_t nextCluster(uint32_t cluster);
  uint8_t rawRead(uint32_t block, uint16_t offset, uint8_t *dst, uint16_t count) {
    return rawDevice_->readData(block, offset, dst, count);}
  uint8_t validCluster(uint32_t cluster) {
   return (1 < cluster && cluster < (clusterCount_ + 2));}
public:
/** Create an instance of FatVolume */
  FatVolume(void) : fatType_(0){}
  /**
   * Initialize a FAT volume.  Try partition one first then try super
   * floppy format.
   *
   * \param[in] dev The SdReader where the volume is located.
   *
   * \return The value one, true, is returned for success and
   * the value zero, false, is returned for failure.  Reasons for
   * failure include not finding a valid partition, not finding a valid
   * FAT file system or an I/O error.
   */
  uint8_t init(SdReader &dev) { return init(dev, 1) ? 1 : init(dev, 0);}
  uint8_t init(SdReader &dev, uint8_t part);

  // inline functions that return volume info
  /** \return The volume's cluster size in blocks. */   
  uint8_t blocksPerCluster(void) {return blocksPerCluster_;}
  /** \return The number of blocks in one FAT. */ 
  uint32_t blocksPerFat(void)  {return blocksPerFat_;}
  /** \return The total number of clusters in the volume. */ 
  uint32_t clusterCount(void) {return clusterCount_;}
  /** \return The logical block number for the start of file data. */
  uint32_t dataStartBlock(void) {return dataStartBlock_;}
  /** \return The number of FAT structures on the volume. */
  uint8_t fatCount(void) {return fatCount_;}
  /** \return The logical block number for the start of the first FAT. */
  uint32_t fatStartBlock(void) {return fatStartBlock_;}
  /** \return The FAT type of the volume. Values are 12, 16 or 32. */
  uint8_t fatType(void) {return fatType_;}
  /** Raw device for this volume */
  SdReader *rawDevice(void) {return rawDevice_;}
  /** \return The number of entries in the root directory for FAT16 volumes. */
  uint32_t rootDirEntryCount(void) {return rootDirEntryCount_;}
  /** \return The logical block number for the start of the root directory
       on FAT16 volumes or the first cluster number on FAT32 volumes. */
  uint32_t rootDirStart(void) {return rootDirStart_;}
  /** \return The total number of blocks in the volume. */
  uint32_t totalBlocks(void) {return totalBlocks_;}
};
/** \class FatReader
 * \brief FatReader implements a minimal FAT16/FAT32 file reader class.
 */ 
class FatReader {
// values for type_
/** This FatReader has not been opened. */
#define FAT_READER_TYPE_CLOSED 0
/** FatReader for a file */
#define FAT_READER_TYPE_NORMAL 1
/** FatReader for a FAT16 root directory */
#define FAT_READER_TYPE_ROOT16 2
/** FatReader for a FAT32 root directory */
#define FAT_READER_TYPE_ROOT32 3
/** FatReader for a subdirectory */
#define FAT_READER_TYPE_SUBDIR 4
/** Test value for directory type */
#define FAT_READER_TYPE_MIN_DIR FAT_READER_TYPE_ROOT16
  uint8_t type_;
  uint32_t fileSize_;
  uint32_t readCluster_;  
  uint32_t readPosition_;
  uint32_t firstCluster_;
  FatVolume *vol_;
  int16_t readBlockData(uint8_t *dst, uint16_t count);
public:
/** Create an instance of FatReader. */
  FatReader(void) : type_(FAT_READER_TYPE_CLOSED) {}
  uint8_t openRoot(FatVolume &vol);
  uint8_t open(FatVolume &vol, dir_t &dir);
  uint8_t open(FatReader &dir, char *name);
  int16_t read(uint8_t *dst, uint16_t count);
  int8_t readDir(dir_t &dir);
  void rewind(void);
  uint8_t seekCur(uint32_t pos);
  //inline functions
  /** Close this instance of FatReader. */
  void close(void) {type_ = 0;}
  /** \return The total number of bytes in a file or directory. */
  uint32_t fileSize(void) {return fileSize_;}
  /** \return The first cluster number for a file or directory. */
  uint32_t firstCluster(void) {return firstCluster_;}
  /** \return True if this is a FatReader for a directory else false */
  uint8_t isDir(void) {return type_ >= FAT_READER_TYPE_MIN_DIR;}
  /** \return True if this is a FatReader for a file else false */
  uint8_t isFile(void) {return type_ == FAT_READER_TYPE_NORMAL;}
  /** \return True if this is a FatReader for an open file/directory else false */
  uint8_t isOpen(void) {return type_ == FAT_READER_TYPE_NORMAL;}
  /** \return The current cluster number for a file or directory. */
  uint32_t readCluster(void) {return readCluster_;}
  /** \return The read position for a file or directory. */
  uint32_t readPosition(void) {return readPosition_;}
  /**
   * Set the read position for a file or directory to \a pos.
   *
   * \param[in] pos The new read position in bytes from the beginning of the file.
   * 
   * \return The value one, true, is returned for success and
   * the value zero, false, is returned for failure.   
   */
  uint8_t seekSet(uint32_t pos) {
    if (pos >= readPosition_) return seekCur(pos - readPosition_);
    rewind(); return seekCur(pos);}
  /** Type of this FatReader.  You should use isFile() or isDir() instead of type()
   * if possible.
   *
   * \return The file or directory type. */
  uint8_t type(void) {return type_;}
   /** Parent volume */
  FatVolume *volume(void) {return vol_;}
};
#endif//FatReader_h
