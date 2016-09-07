#pragma once

#include "ClientManager.h"
#include "PacketExtracter.h"

//
// Constructor
//  serPtr : The Server Socket we want to listen on.
ClientManager::ClientManager(unique_ptr<Socket> socket) {
  mSocket = std::move(socket);
  mSendQueue = make_shared<ConcurrentQueue<Packet>>();
}

//
// Constructor
//  serPtr : The Server Socket we want to listen on.
//  queue  : The Queue we are sending from.
ClientManager::ClientManager(unique_ptr<Socket> socket,
  shared_ptr<ConcurrentQueue<Packet>> queue) {
  mSocket = std::move(socket);
  mSendQueue = queue;
}

// Start 
//    Start the Client.
void ClientManager::start() {
  mIsRunning.store(true);

  receiveThread = thread(&ClientManager::recvTask, this);
  sendThread = thread(&ClientManager::sendTask, this);

  receiveThread.join();

  //Unblock the send queue, and then join.
  mSendQueue->push(Packet(Type::INVALID, 0, nullptr));
  sendThread.join();


  mInputHandlers.clear();
  closeClient();
}


//Destructor.
ClientManager::~ClientManager() {
  cout << "Deleted Client Manager" << endl; 
}

// Add a Listener to handle incoming responses.
//   handler : The handler we want to deal with 
void ClientManager::addListener(const shared_ptr<InputHandler>& handler) {
  mInputHandlers.push_back(handler);
}

// Return all the handlers associated with this Client.
vector<shared_ptr<InputHandler>> ClientManager::getHandlers() {
  return mInputHandlers;
}

// Returns the Send queue
weak_ptr<ConcurrentQueue<Packet>> ClientManager::getSendQueue() {
  return mSendQueue;
}


// Close this client.
void ClientManager::closeClient() {
  mIsRunning = false; 

  //Wait to get to here, then close.
  mSocket->close();
}


// Send Task.
//  Send the Packets in the queue.
void ClientManager::sendTask() {
  while (mIsRunning) {
    unique_lock<mutex> drainLock(mMutex);
    auto item = mSendQueue->pop();
    //Send the response to the server.
    if ((item.type != Type::INVALID)) {
      
      //Convert Packet down to byte array. 
      // There's no need to send the entire
      size_t packetSize = item.size + sizeof(Type) + sizeof(item.size);
      byte* data = new byte[packetSize];
      
      // Convert a Packet down to a char* buffer and 
      // Send to the client
      memcpy(data, &item, packetSize);
      
      try {
        mSocket->send(data, packetSize);
      }
      catch (exception& e) {
        mIsRunning.store(false);
        cout << "Error when attempting to send on socket. " << e.what() << endl;
      }

      delete[] data;
    }
  }
}

// Receive Task
//  Receive the Packets in the queue
void ClientManager::recvTask() {
  while (mIsRunning) {

    //Attempt to retrieve a packet.
    size_t msgSize = 0;
    size_t recvSize = sizeof(Packet);
    char* buffer = new char[recvSize];
    
    try {
      msgSize = mSocket->read(buffer, recvSize);
    } catch (exception& e) {
      mIsRunning.store(false);
      cout << e.what() << endl;
    }

    if ((mIsRunning.load()) && (msgSize > 0) && (buffer != nullptr)) {
      Packet packet = extractPacket(buffer, msgSize);
      for each (auto handler in mInputHandlers) {
        if (handler->listensFor(packet.type)) {
          //handler->handlePacket(packet);
        }
      }
    } else if (msgSize == 0) {
      mIsRunning.store(false);
    }

    delete[] buffer;
  }
}
