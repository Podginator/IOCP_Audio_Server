#pragma once

#include "stdadfx.h"
#include "Packet.h"
#include "Client.h"
#include "ConcurrentQueue.h"

// The Response Handler. 
class InputHandler
{
public:

  //Constructor.
  //Can call with either 1 Type Ie: InputHandler(Type::Audio)
  //Or can call with multiple, IE InputHandler(Type::Audio & Type::FileList)
  explicit InputHandler(Type listenFor) : mListenFor(listenFor) {}

  // Returns True if Type & Listen for > 1
  bool listensFor(Type type) {
    return (type & mListenFor) != 0;
  }

  //
  // <Method> 
  //  getResponse
  // <Summary> 
  //  return a Response Struct to send back.
  // @param sentMessage the Message we have been set.
  virtual void handlePacket(const Packet& packet) = 0;
private:
  // What type this is listening for.
  Type mListenFor;
};