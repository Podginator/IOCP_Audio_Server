#pragma once

#include "IClientHandlerFactory.h"
#include "ConcurrentQueue.h"
#include "AudioServerHandler.h"
#include "SettingsHandler.h"
#include "Settings.h"
#include <vector>

class AudioClientHandlerFactory : public IClientHandlerFactory {
public:
  // Constructor. Take a packet queue to push to.
  AudioClientHandlerFactory(shared_ptr<ConcurrentQueue<ClientPacket>> packetQueue);

  // Add to the client.
  virtual void addHandlerToClient(weak_ptr<Client> client);

private:
  shared_ptr<ConcurrentQueue<ClientPacket>> mPacketQueue; 

};