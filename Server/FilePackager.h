#pragma once

#include "stdadfx.h"

// Interface that packages files into buffers.
class FilePackager
{
public:

  // Open the file with a string.
  virtual bool openFile(const string& filePath) = 0;

  // Open the file. 
  virtual bool openFile(ifstream& file) = 0;

  // Is the file open.
  virtual bool isFileOpen() = 0;

  // Is the file fully packed. 
  virtual bool isFileComplete() = 0;

  // Get the total size of the file.
  virtual size_t getTotalSize() = 0;

  // Close the currently opened file. IS called on open, too.
  virtual bool closeFile() = 0;

  // Fill the provided buffer with the next set of the Package.
  virtual size_t // OUT Char Buffer of the Next Chunk of the file. 
  getNextChunk(
    byte* buffer,
    size_t bufferSize // IN - The Attempted Buffer Size, OUT - How much was written to the buffer.
    ) = 0;

  // Retrieve the header of the file we're wishing to retrieve from.
  virtual size_t // OUT Char buffer storing the Buffer. 
  getHeader(
    byte* buffer, // IN/OUT- The Flag if it's succeeded
    size_t bufferSize // IN - The Attempted Buffer Size, OUT - How much was written to the buffer.
    ) = 0;

};