#include "Socket.h"
#include "InputHandler.h"

//
// <Method>
//    Constructor
// <Summary>
//  Read the data from the Socket.
//
// @param  The Data retrieved from the server
//
Socket::Socket(SOCKET socket, struct sockaddr_in& mAddress) {
  mSocketFileDesc = socket;
  socketAddr = mAddress;

  // Attempt to set up the wsa.
  WORD wVersionRequested;
  WSADATA wsaData;
  wVersionRequested = MAKEWORD(2, 2);
  int err = WSAStartup(wVersionRequested, &wsaData);
  if (err != 0) {
    throw runtime_error("Error starting server socket. Error Number: " + WSAGetLastError());
  }
}

//
// <Method>
//    Destructor
// <Summary>
//    Close
//
Socket::~Socket() {
  cout << "Ending Socket" << endl;
  WSACleanup();
  close();
}

//
// <Method>
//    Read
// <Summary>
//    Read the data from the Socket.
// @param  The size of the data from the server.
//

size_t Socket::read(char* buffer, const size_t& bytesRead) {
  int retrieved = ::recv(mSocketFileDesc, buffer, bytesRead, 0);

  if (retrieved == SOCKET_ERROR) {
    std::string err("Socket error while reading: " + WSAGetLastError());
    throw runtime_error(err.c_str());
  } else if (retrieved > 0) {
    return static_cast<size_t>(retrieved);
  }
}

//
// <Method>
//    Send data
// <Summary>
//    Send the data to the socket
// @param  data to send
//
int Socket::send(byte* data, size_t dataSize) {
  int returnVal = -1;

  if (data) {
    returnVal = ::send(mSocketFileDesc, reinterpret_cast<char*>(data), static_cast<int>(dataSize), 0);
  }

  if (returnVal == SOCKET_ERROR) {
    std::string err("Socket error while sending: " + WSAGetLastError());
    throw runtime_error(err.c_str());
  }

  return returnVal;
}

//
// <Method>
//    Close
// <Summary>
//    Close the socket
//
void Socket::close() {
  closesocket(mSocketFileDesc);
}