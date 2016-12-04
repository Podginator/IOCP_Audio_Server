#pragma once

#include "stdadfx.h"

//
// Class representing a single instance of a socket connection file
// 
//
class Socket {
public:

  // Constructor: 
  // Tales a spclet 
  Socket(SOCKET socketfd, struct sockaddr_in& mAddress) {
    mSocketFileDesc = socketfd;
    socketAddr = mAddress;

    // Attempt to set up the wsa.
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);
    int err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
      throw runtime_error("Error starting server socket. Error Number: " + WSAGetLastError());
    }

    mOverlapped = make_shared<OVERLAPPED>();
    mWSABuffer = make_shared<WSABUF>();

  }

  //
  // Method:    Destructor
  //
  ~Socket() {
    cout << "Ending Socket" << endl;
    WSACleanup();
    close();
  }

  //Copy constructor.
  // Ensure that we cannot copy this class elsewhere, as it is 
  // a noncopyable resource (A socket.)
  Socket(const Socket& other) = delete;

  //Move Constructor 
  // We don't have many resources that need to be freed etc. 
  // So we can rely on the compiler here.
  Socket(Socket&& other) = default;

  //Move Operator.
  // We don't have many resources that need to be freed etc. 
  // So we can rely on the compiler here.
  Socket& operator=(Socket&&) & = default;

  //
  // <Method>
  //    Read
  // <Summary>
  //    Read the data from the Socket.
  // @param  The Data retrieved from the server
  //
  int beginReceive(const LPDWORD& flag = 0) {
    int res = WSARecv(mSocketFileDesc, mWSABuffer.get(), 1, 0 ,flag, mOverlapped.get(), NULL);

    return res;
  }

  //
  // <Method>
  //    Send data
  // <Summary>
  //    Send the data to the socket
  // @param  data to send
  //
  int send(byte* data, size_t dataSize) {
    //Do something
  }

  //
  // <Method>
  //    Close
  // <Summary>
  //    Close the socket
  //
  void close() {
    // do something
  }

  SOCKET getFileDescriptor() {
    return mSocketFileDesc;
  }

  //
  // <Method>
  //    Validate
  // <Summary>
  //    Check the Socket is correct.
  //
  boolean validate() {
    return mSocketFileDesc != 0; 
  }

private:
  //The Socket file descriptor
  SOCKET mSocketFileDesc;
  //Struct of SocAddr_in.
  sockaddr_in socketAddr;

  shared_ptr<OVERLAPPED> mOverlapped;

  shared_ptr<WSABUF> mWSABuffer;

  int               mTotalBytes;
  int               mSentBytes;

  int               mOpCode; //will be used by the worker thread to decide what operation to perform
  
  char              m_szBuffer[1044];

};