#pragma once

#include "stdadfx.h"
#include "FilePackager.h"
#include <iostream> 
#include <fstream>


// Class that packages files into buffers.
class WavFilePackager :  public FilePackager
{
public:


  // The header for a wav file. 
  struct waveHeader {
    byte chunk_id[4]; //4 Bytes Big Endian.
    DWORD chunk_size; // 4 bytes Little Endian
    byte format[4]; // 4 bytes 8 Endian 
    byte subchunk1_id[4]; // 4 bytes Big Endian
    DWORD subchunk1_size; // 4 bytes little endian
    WORD audio_format; // 2 bytes little endian
    WORD num_channels; // 2 bytes little endian
    DWORD sample_rate; // 4 bytes, Little Endian
    DWORD byte_rate; // 4 Bytes Little Endian 
    WORD block_align; // 2 Bytes Little Endian
    WORD bits_per_sample; // 2 Bytes Little Endian
    byte subchunk2_id[4]; // 4 Bytes Big Endian
    DWORD subchunk2_size; // 4 Bytes Little Endian.
  };

  // Constructor
  // Create an instance of a file with which we're wanting to disect. 
  WavFilePackager();


  // Is the file open.
  virtual bool isFileOpen();

  // Is the file fully packed. 
  virtual bool isFileComplete();

  //Name: Open File
  //  Open a file
  //Param: String : filePath
  //  The Path to the file.
  //Return Bool: 
  //  The File has opened and been read correctly.
  virtual bool openFile(const string& filePath);

  //Name: OpenFile
  //  Open a file.
  //Param: FILE*
  //  The File we want to open.
  //Return : Bool
  //  Successfully opened a file.
  virtual bool openFile(ifstream& file);

  //Name: getTotalSize
  //  get the size of the file.
  //Return : size_t
  //  Get the total size.
  virtual size_t getTotalSize();

  //Name : Close file
  //  Close the file
  //Return:
  //  Return whether the file has been closed.
  virtual bool closeFile();

  //Name: Get Next Chunk.
  //  Get the next chunk of <bufferSize> of this file.
  //Param : Char* Buffer.
  //  The buffer we want to write to.
  //Param : size_t bufferSize.
  //  The Size of the buffer we want to write to.
  //Return : Size_T
  //   The size of the written buffer.
  virtual size_t getNextChunk(byte* buffer, size_t bufferSize);

  //Name: Get Header
  //  Get the Header of this file.
  //Param : Char* Buffer.
  //  The buffer we want to write to.
  //Param : size_t bufferSize.
  //  The Size of the buffer we want to write to.
  //Return : Size_T
  //   The size of the written buffer.
  virtual size_t getHeader(byte* buffer, size_t bufferSize);

private:

  // The Header once we've processed the file.
  waveHeader mHeader;

  //The open file.
  ifstream mOpenFile; 

  // The size of the file. 
  size_t mTotalSize; 

  // The size of the amount extracted.
  size_t mExtracted; 


  //Name: Process the Wav File
  //  Process the wav file.
  //Param : FILE* file.
  //  The File we want to read;
  //Param : WaveHeader wavHdr
  //  Process the heaver
  //Param :  Size_T size 
  //   size of the buffer..
  bool processWavFile(ifstream& file, waveHeader& wavhdr, size_t& size);

};