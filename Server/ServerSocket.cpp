#include "ServerSocket.h"
#include <errno.h>

// <Method>
//    Constructor
// <Summary>
//    Constructs the Server Objects
// @param hostName the host name to connect to (Ie: IP-Port)
// @param port the port to listen to
//
ServerSocket::ServerSocket(const string& hostName, int port)
  : mPort(port), mAddress(hostName), mSocketFileDesc(0) {
  // Attempt to set up the wsa.
  WORD wVersionRequested;
  WSADATA wsaData;
  wVersionRequested = MAKEWORD(2, 2);
  int err = WSAStartup(wVersionRequested, &wsaData);
  if (err != 0) {
    throw runtime_error("Error starting server socket. Error Number: " + WSAGetLastError());
  }
}

// On destruction ensure we call WSA Cleanup.
ServerSocket::~ServerSocket() {
  close();
}

// <Method>
//  accept
// <Summary>
//   Accept a connection that is attempting to connection
//   Return a unique pointer to the Socket that is created.
// @return Pointer to a Socket.
//
unique_ptr<Socket> ServerSocket::acceptSocket() {
  struct sockaddr_in cliAddr;
  socklen_t cliLen = sizeof(cliAddr);
  SOCKET s =
    accept(mSocketFileDesc, (sockaddr*)&cliAddr, &cliLen);

  if (s != SOCKET_ERROR) {
    cout << "Client connected to the Server. " << endl;
    return make_unique<Socket>(s, cliAddr);
  }

  return nullptr;
}

// Returns the Socket File Descriptor. 
SOCKET ServerSocket::getSocketFileDescriptor() {
  return mSocketFileDesc;
}

//
// <Method>
//   Begin Listening
// <Summary>
//   Starts listening on the Socket.
// @return True if successful, false otherwise.
//
bool ServerSocket::begin() {
  bool ok = true;
  struct addrinfo *result = NULL;
  struct addrinfo hints;
  
  if (ok) {
    mSocketFileDesc = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
    //Check to see if we've failed.
    if (mSocketFileDesc == INVALID_SOCKET) {
      ok = false;
    }

    if (ok) {
      setupServerHints(hints);
      if (getaddrinfo(mAddress.c_str(), to_string(mPort).c_str(), &hints, &result)) {
        ok = false;
      }
    }

    if (ok) {

      if (::bind(mSocketFileDesc, result->ai_addr, (int)result->ai_addrlen) < 0) {
        ok = false;
      }

      freeaddrinfo(result);
    }
  }

  //If we've successfully set up our server, then listen
  //on it, otherwise close.
  if (ok) {
    int res = listen(mSocketFileDesc, SOMAXCONN);

    if (res == SOCKET_ERROR) {
      ok = false;
    }
  }

  return ok;
}

// 
// <Method>
//  Close
// <Summary> 
//  Close the socket.
//
void ServerSocket::close() {
  int errNum = WSAGetLastError();

  if (errNum != 0) {
    cout << "Server Socket closed due to error: " << errNum << endl;
  }

  WSACleanup();

  if (mSocketFileDesc != 0) {
    closesocket(mSocketFileDesc);
    mSocketFileDesc = 0;
  }
}

// <Method>
//    setupServerAddressStruct
// <Summary>
//    Sets up pthe Sockaddr_in 
// @param ref The Socket_addr_in to modify.
//
void ServerSocket::setupServerHints(addrinfo& hints) {
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;
}
