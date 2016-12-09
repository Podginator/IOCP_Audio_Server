#pragma once

// The Smart Wrapper for the Buffer
// Wraps a Char* in a memory handling class
class Buffer {

public:
  // Constructor, instantiate the size
	Buffer(const size_t& size);

  // Copy Constructor.
  // If we copy then we should copy the buffer,
  // and allow the new copied object to handle it's own memory.
  Buffer(const Buffer& other);

  Buffer(const char* copyBuffer, const size_t& size);
  
  // Destructor:
  // If we leave scope delete all the hanging pointers. 
  ~Buffer();

  // Return the Buffer.
  char* getBuffer();

  // Return the size of the buffer.
  size_t getSize();

  // Clear the buffer.
  void clearBuffer();
private:
  char* mBuffer;
  size_t mBufferSize;
};
