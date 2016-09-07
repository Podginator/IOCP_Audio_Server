#pragma once

#include "stdadfx.h"
#include "InputHandler.h"

// Extract the Packet. 
// Handle Errors in case the packet is not true.
extern Packet extractPacket(char* msg, const size_t& packetSize) {
  Packet packet;
  char* currPos = msg;
  // Okay, so check that we have the right size first.

  if (packetSize == sizeof(Packet) && msg) {
    int size;
    memcpy(&size, currPos, sizeof(int));
    currPos += sizeof(int);

    // As long as the packet isn't greater than the max size we can continue.
    if (size <= Packet::maxPacketSize && size >= 0) {
      //Then convert type
      Type type;
      memcpy(&type, currPos, sizeof(Type));
      currPos += sizeof(Type);

      //Ensure that we're not NO_OPP, and that we're less than or equal to the last val
      //And that we're a power of 2 (Bitflag)    
      if (type > Type::INVALID && type <= Type::Last && !(type & (type - 1))) {
        // we can continue. 
        packet.type = type;
        packet.size = size;
        if (packet.size > 0) {
          //and apply the byte data.
          std::copy(currPos, currPos + size, packet.packetData);
        }
      }
    }
  }

  return packet; 
}
