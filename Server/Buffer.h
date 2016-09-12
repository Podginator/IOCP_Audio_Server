#pragma once

// The Smart Wrapper for the Buffer
// Wraps a Char* in a memory handling class
class Buffer {

public:
  // Constructor, instantiate the size
  Buffer(const size_t& size) : mBufferSize(size) {
    mBuffer = new char[size];
  }

  // Copy Constructor.
  // If we copy then we should copy the buffer,
  // and allow the new copied object to handle it's own memory.
  Buffer(const Buffer& other) {
    mBuffer = new char[other.mBufferSize];
    mBufferSize = other.mBufferSize;
    memcpy(mBuffer, other.mBuffer, other.mBufferSize);
  }

  Buffer(const char* copyBuffer, const size_t& size) : mBufferSize(size)
  {
    mBuffer = new char[size];
    memcpy(mBuffer, copyBuffer, size);
  }
  
  // Destructor:
  // If we leave scope delete all the hanging pointers. 
  ~Buffer() {
    delete[] mBuffer;
  }

  // Return the Buffer.
  char* getBuffer() {
    return mBuffer;
  }

  // Return the size of the buffer.
  size_t getSize() {
    return mBufferSize;
  }

  // Clear the buffer.
  void clearBuffer() {
    ZeroMemory(mBuffer, mBufferSize);
  }

private:
  char* mBuffer;
  size_t mBufferSize;
};
