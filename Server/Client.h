#pragma once

#include "stdadfx.h"

class Client {
private: 
  shared_ptr<WSABUF> mWSABuffer; 
  shared_ptr<OVERLAPPED> mOverlapped;
  SOCKET mSocket;
  char mBuffer[1044];
  int mState; 

  enum CLIENT_STATE {
    READ = 0,
    SEND = 1,
    RESET = 2
  };

public: 

  //Constructor. 
  Client(SOCKET socket) : mSocket(socket) {
    mOverlapped = make_shared<OVERLAPPED>();
    mWSABuffer = make_shared<WSABUF>();

    ZeroMemory(mOverlapped.get(), sizeof(OVERLAPPED));
    ZeroMemory(mBuffer, 1044);

    mWSABuffer->buf = mBuffer;
    mWSABuffer->len = 1044;

    mState = -1;
  }

  // Destructor. Wait then disconnect. 
  ~Client() {
    // Await Overlapped to finish pending events.
    while (mOverlapped->Internal != STATUS_PENDING) {}
    Disconnect();
  }

  //Copy constructor.
  // Ensure that we cannot copy this class elsewhere, as it is 
  // a noncopyable resource (A socket.)
  Client(const Client& other) = default;

  //Move Constructor 
  // We don't have many resources that need to be freed etc. 
  // So we can rely on the compiler here.
  Client(Client&& other) = default;

  //Move Operator.
  // We don't have many resources that need to be freed etc. 
  // So we can rely on the compiler here.
  Client& operator=(Client&&) & = default;

  
  void BeginRead() {
    //Get data.
    DWORD dwFlags = 0;
    DWORD dwBytes = 0;
    int nBytesRecv = WSARecv(mSocket, mWSABuffer.get(), 1,
      &dwBytes, &dwFlags, mOverlapped.get(), NULL);
  }

  virtual void CompleteRead() {}

  void BeginSend(byte* data, size_t dataSize) { }

  virtual void CompleteSend() {}

  void Reset() {} 

  void BeginIOEvent(DWORD transferred) {}

  void Disconnect() {}

};