#pragma once

#include "IClientHandlerFactory.h"
#include "ConcurrentQueue.h"
#include "AudioServerHandler.h"
#include <vector>

class AudioClientHandlerFactory : public IClientHandlerFactory {
public:
  // Constructor. Take a packet queue to push to.
  AudioClientHandlerFactory(shared_ptr<ConcurrentQueue<ClientPacket>> packetQueue) : mPacketQueue(packetQueue) {}

  // Add to the client.
  virtual void addHandlerToClient(weak_ptr<Client> client) {
    vector<shared_ptr<InputHandler>> handlers; 
    shared_ptr<AudioServerHandler> audioHandler = make_shared<AudioServerHandler>(client, mPacketQueue);
    handlers.push_back(audioHandler);

    if (auto clientPtr = client.lock()) {
      clientPtr->SetReceivedHandler([=](Buffer& buffer, size_t size) {
        try {
          Packet packet;
          packet.Deserialize(buffer.getBuffer(), size);
          for (auto handler : handlers) {
            handler->handlePacket(packet);
          }
        } catch (exception& e) {
          cout << "Error Converting Buffer to Packet: " <<  e.what() << endl;
        }
      });
    }
  }

private:
  shared_ptr<ConcurrentQueue<ClientPacket>> mPacketQueue; 

};