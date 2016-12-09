#pragma once

#include "stdadfx.h"
#include "Packet.h"
#include "Buffer.h"
#include <atomic>

typedef std::function<void(Buffer&)> SentHandler;
typedef std::function<void(Buffer&, size_t)> ReceivedHandler;


// A Class to represent the WSASocket Client. 
// A State machine: 
//  Perform Completion Operation (Read/Write).
class Client {
private: 
  // The WSA Buffer for receiving
  shared_ptr<WSABUF> mWSARecvBuffer;
  // The WSA Buffer for sending.
  shared_ptr<WSABUF> mWSASendBuffer;
  // The Overlapped Structure 
  shared_ptr<OVERLAPPED> mOverlapped;
  // The Socket 
  SOCKET mSocket;
  // The Buffer to store in 
  Buffer mBuffer;
  // The Receive handler 
  ReceivedHandler mReceivedHandler;
  // The Send Handler
  SentHandler mSentHandler;
  // The internal state of the Client.
  atomic_int mState;
  // The Client State.
  enum CLIENT_STATE {
    READ = 0,
    SEND = 1,
    RESET = 2
  };

public: 

  //Constructor. 
  Client(SOCKET socket, size_t bufferSize);

  // Destructor. Wait then disconnect. 
  // Warning:: Will Block until the Status Pending has been completed.
  ~Client();

  //Copy constructor. 
  // Ensure that we cannot copy this class elsewhere, as it is The thread 0x3784 has exited with code -1073741510 (0xc000013a).

  // a noncopyable resource (A socket.)
  Client::Client(const Client& other) = default;

  //Move Constructor 
  // We don't have many resources that need to be freed etc. 
  // So we can rely on the compiler here.
  Client::Client(Client&& other) = default;

  //Move Operator.
  // We don't have many resources that need to be freed etc. 
  // So we can rely on the compiler here.
  Client& Client::operator=(Client&&) & = default;

  // Returns the Socket 
  SOCKET getSocket();

  //// Begin the Read, calling WSA Recv.
  void BeginRead();

  // Return the Packet from the 
  Buffer getMessage();
  
  // Complete the Read, in this instance we call the Handler we've passed.
  virtual void CompleteRead(size_t size);

  // Begin The Send Operation
  void BeginSend(byte* data, size_t dataSize);

  // Complete the Read, and perforrm the "Sent Handler" 
  virtual void CompleteSend();

  // Reset the buffers.
  void Reset() {}

  // Do the IO Event. 
  void BeginIOEvent(DWORD transferred);

  // Set the Received Handler 
  void SetReceivedHandler(const ReceivedHandler& handler);

  // Set the Sent Handler
  void SetSentHandler(const SentHandler& handler);

  // Disconnect the Socket.
  void Disconnect();

};

typedef std::pair<weak_ptr<Client>, Packet> ClientPacket;