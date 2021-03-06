#pragma once
#include "stdadfx.h"

// Type Enum, Bit Flags as we want the handlers
// To be able to handle a lot of them.
enum Type {
  INVALID = 0,
  TRACK = 1 << 1,
  AUDIO = 1 << 2,
  FILELIST = 1 << 3,
  IMAGE = 1 << 4,
  SETTINGS = 1 << 5,
  END = 1 << 6,
  First = INVALID,
  Last = END
};

//Expose bitwise operators.
inline Type operator~ (Type a) { return (Type)~(int)a; }
inline Type operator| (Type a, Type b) { return (Type)((int)a | (int)b); }
inline Type operator& (Type a, Type b) { return (Type)((int)a & (int)b); }
inline Type operator^ (Type a, Type b) { return (Type)((int)a ^ (int)b); }
inline Type& operator|= (Type& a, Type b) { return (Type&)((int&)a |= (int)b); }
inline Type& operator&= (Type& a, Type b) { return (Type&)((int&)a &= (int)b); }
inline Type& operator^= (Type& a, Type b) { return (Type&)((int&)a ^= (int)b); }

/**
* The response object.
*/
struct Packet {
public:

  // The maximum available packet size to send .
  // Note: Not the sizeof(Packet), but instead the size
  // Of the data stored in the packet.
  const int static maxPacketSize = 1024;

  // Size of the Data. 
  int size;

  //Type of the Packet. 
  Type type;

  // The Id of the packet.
  int id;

  //Byte Array, max length set to 1024.
  byte packetData[maxPacketSize];

  Packet() : type(Type::INVALID), size(0), packetData(), id(0) {};

  Packet(Type type, int size, int id, byte* data) : type(type), size(size), id(id) {
    if (data != nullptr) {
      std::copy(data, data + size, packetData);
    }
  }

  Packet(Type type, int size, byte* data) : Packet(type, size, 0, data) {};

  void Deserialize(char* msg, const size_t& packetSize) throw (runtime_error) {
    char* currPos = msg;
    bool isValid = false;
    // Okay, so check that we have the right size first.

    if (packetSize == sizeof(Packet) && msg) {
      int datasize;
      memcpy(&datasize, currPos, sizeof(int));
      currPos += sizeof(int);


      // As long as the packet isn't greater than the max size we can continue.
      if (datasize <= Packet::maxPacketSize && datasize >= 0) {
        int packetId;
        memcpy(&packetId, currPos, sizeof(int));
        currPos += sizeof(int);

        //Then convert type
        Type datatype = Type::INVALID;
        memcpy(&datatype, currPos, sizeof(Type));
        currPos += sizeof(Type);

        //Ensure that we're not NO_OPP, and that we're less than or equal to the last val
        //And that we're a power of 2 (Bitflag)    
        if (datatype > Type::INVALID && type <= Type::Last && !(type & (type - 1))) {
          isValid = true;
          // we can continue. 
          id = packetId;
          type = datatype;
          size = datasize;
          if (size > 0) {
            //and apply the byte data.
            std::copy(currPos, currPos + size, packetData);
          }
        }
      }

      if (!isValid) {
        throw std::runtime_error("Unable to deserialize data to packet");
      }
    }
  }
};

