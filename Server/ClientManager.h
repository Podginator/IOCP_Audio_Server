#pragma once

#include "stdadfx.h"
#include "ServerSocket.h"
#include "Socket.h"
#include "InputHandler.h"
#include <vector>
#include "ConcurrentQueue.h"
#include <atomic>

//
// The Manager Class to handle a single instance of 
// of a Socket.
//
class ClientManager {

public:

  //
  // Constructor
  //  serPtr : The Server Socket we want to listen on.
  explicit ClientManager(unique_ptr<Socket> socket);

  //
  // Constructor
  //  serPtr : The Server Socket we want to listen on.
  //  queue  : The Queue we are sending from.
  ClientManager(unique_ptr<Socket> socket,
    shared_ptr<ConcurrentQueue<Packet>> queue);


  //Destructor.
  ~ClientManager();

  // Start 
  //    Start the Client.
  void ClientManager::start();

  // Add a Listener to handle incoming responses.
  //   handler : The handler we want to deal with 
  void addListener(const shared_ptr<InputHandler>& handler);


  // Returns the Vector of the Input Handlers.
  vector<shared_ptr<InputHandler>> getHandlers();

  // Returns a pointer to the send queue.
  weak_ptr<ConcurrentQueue<Packet>> getSendQueue();

  // Close the client, perform clean up.
  void closeClient();

private:

  //The receive thread
  thread receiveThread; 

  //The Send Thread.
  thread sendThread;

  // The internal socket
  unique_ptr<Socket> mSocket;

  // An Input Handler.
  vector<shared_ptr<InputHandler>> mInputHandlers;

  // A Send Queue. We drain this queue and send back to the C++.
  shared_ptr<ConcurrentQueue<Packet>> mSendQueue;

  // The mutex
  mutex mMutex;

  //We are running.
  atomic_bool mIsRunning = false; 

  // Send Task.
  //  Send the Packets in the queue.
  void sendTask();

  // Receive Task
  //  Receive the Packets in the queue
  void recvTask();
};