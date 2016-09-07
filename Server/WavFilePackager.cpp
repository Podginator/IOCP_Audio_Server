#pragma once

#include "WavFilePackager.h"


//CTOR: Create an instance of a file with which we're wanting to disect. 
WavFilePackager::WavFilePackager()
  : mHeader(), mTotalSize(0), mExtracted(0) {}

//Name: isFileOpen
//  Is the file open
//Return: bool
//  True if file open.
bool WavFilePackager::isFileOpen() {
  return mOpenFile.is_open();
}

//Name: isFileComplete
//  Is the file completely read.
//Return : Bool
//  True if file is complete.
bool WavFilePackager::isFileComplete() {
  return mExtracted >= mTotalSize;
}

//Name: getTotalSize
//  get the size of the file.
//Return : size_t
//  Get the total size.
size_t WavFilePackager::getTotalSize() {
  return mTotalSize;
}

//Name: Open File
//  Open a file
//Param: String : filePath
//  The Path to the file.
//Return Bool: 
//  The File has opened and been read correctly.
bool WavFilePackager::openFile(const string& filePath) {
  ifstream file(filePath, ios::binary);

  //Then open the file.
  return openFile(file);
}

//Name: OpenFile
//  Open a file.
//Param: FILE*
//  The File we want to open.
//Return : Bool
//  Successfully opened a file.
bool WavFilePackager::openFile(ifstream& file) {
  bool isOpen = false; 

  // Close the currently open file.
  if (mOpenFile.is_open()) {
    closeFile();
  }

  // Reset the Size and the total extracted. 
  mExtracted = 0;
  mOpenFile = move(file);


  //Then process the wav file. 
  isOpen = processWavFile(mOpenFile, mHeader, mTotalSize);
  mExtracted = sizeof(waveHeader);

  return isOpen;
}

//Name : Close file
//  Close the file
//Return:
//  Return whether the file has been closed.
bool WavFilePackager::closeFile() {
  bool closed = false;
 
  if (mOpenFile.is_open()) {
    mOpenFile.close();
  }

  //mHeader = waveHeader();
  return closed;
}

//Name: Get Next Chunk.
//  Get the next chunk of <bufferSize> of this file.
//Param : Char* Buffer.
//  The buffer we want to write to.
//Param : size_t bufferSize.
//  The Size of the buffer we want to write to.
//Return : Size_T
//   The size of the written buffer.
size_t WavFilePackager::getNextChunk(byte* buffer, size_t bufferSize) {
  size_t res = 0;
  // Calculate how much we have to read.
  size_t calculatedRead = (mExtracted + bufferSize > mTotalSize) ? mTotalSize - mExtracted : bufferSize;
  // Calculate the amount of buffer we need to use. 
  
  //seek from extracted location.


  //Check that we haven't failed (failbit)
  //if ((mOpenFile.rdstate() & std::ifstream::failbit) != 0) {
    //Then extract from the file the next chunk. 
    mOpenFile.read(reinterpret_cast<char*>(buffer), calculatedRead);

    //if (amtRead > 0) {
      mExtracted += calculatedRead;
      res += calculatedRead;
    //}
  //}

  //Return the size of the buffer.
  return res;
}

//Name: Get Header
//  Get the Header of this file.
//Param : Char* Buffer.
//  The buffer we want to write to.
//Param : size_t bufferSize.
//  The Size of the buffer we want to write to.
//Return : Size_T
//   The size of the written buffer.
size_t WavFilePackager::getHeader(byte* buffer, size_t bufferSize) {
  size_t read = bufferSize >= sizeof(waveHeader) ? sizeof(waveHeader) : bufferSize;

  //Then copy the header into the buffer.
  mOpenFile.read(reinterpret_cast<char*>(buffer), read);

  memcpy(buffer, &mHeader, read);

  return read;
}

//Name: Process the Wav File
//  Process the wav file.
//Param : FILE* file.
//  The File we want to read;
//Param : WaveHeader wavHdr
//  Process the heaver
//Param :  Size_T size 
//   size of the buffer..
bool WavFilePackager::processWavFile(ifstream& file, waveHeader& wavhdr, size_t& size) {
  bool ok = true;
  waveHeader header = { 0 };

  file.read((char*)&header.chunk_id, sizeof(byte) * 4);

  if (strncmp(reinterpret_cast<char*>(header.chunk_id), "RIFF", 4) == 0) {
    //So continue.
    //Read the chunksizes
    file.read((char*)&header.chunk_size, sizeof(DWORD));
    file.read((char*)&header.format, sizeof(DWORD));
    if (strncmp(reinterpret_cast<char*>(header.format), "WAVE", 4) == 0) {
      file.read((char*)&header.subchunk1_id, sizeof(DWORD));
      file.read((char*)&header.subchunk1_size, sizeof(DWORD));
      file.read((char*)&header.audio_format, sizeof(WORD));
      file.read((char*)&header.num_channels, sizeof(WORD));
      file.read((char*)&header.sample_rate, sizeof(byte) * 4);
      file.read((char*)&header.byte_rate, sizeof(DWORD));
      file.read((char*)&header.block_align, sizeof(WORD));
      file.read((char*)&header.bits_per_sample, sizeof(WORD));
      file.read((char*)&header.subchunk2_id, sizeof(byte) * 4);
      file.read((char*)&header.subchunk2_size, sizeof(byte) * 4);
    }
    else {
      ok = false;
    }
  }
  else {
    ok = false;
  }

  if (ok) {
    wavhdr = header;
    //The total size of the data in this WAV File. We will request it in chunks.
    size = header.subchunk2_size;
  } else {
    wavhdr = { 0 };
    size = 0;
  }

  return ok;
}
