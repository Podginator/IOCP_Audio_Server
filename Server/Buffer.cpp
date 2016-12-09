#include "Buffer.h"
#include <cstring>


// Constructor, instantiate the size
Buffer::Buffer(const size_t& size) : mBufferSize(size) {
	mBuffer = new char[size];
}

// Copy Constructor.
// If we copy then we should copy the buffer,
// and allow the new copied object to handle it's own memory.
Buffer::Buffer(const Buffer& other) {
	mBuffer = new char[other.mBufferSize];
	mBufferSize = other.mBufferSize;
	memcpy(mBuffer, other.mBuffer, other.mBufferSize);
}

Buffer::Buffer(const char* copyBuffer, const size_t& size) : mBufferSize(size)
{
	mBuffer = new char[size];
	memcpy(mBuffer, copyBuffer, size);
}

// Destructor:
// If we leave scope delete all the hanging pointers. 
Buffer::~Buffer() {
	delete[] mBuffer;
}

// Return the Buffer.
char* Buffer::getBuffer() {
	return mBuffer;
}

// Return the size of the buffer.
size_t Buffer::getSize() {
	return mBufferSize;
}

// Clear the buffer.
void Buffer::clearBuffer() {
	memset(mBuffer, 0, mBufferSize);
}
