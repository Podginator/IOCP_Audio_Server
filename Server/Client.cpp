#include "Client.h"


//Constructor. 
Client::Client(SOCKET socket, size_t bufferSize) : mSocket(socket), mBuffer(bufferSize) {
	mOverlapped = make_shared<OVERLAPPED>();
	mWSARecvBuffer = make_shared<WSABUF>();
	mWSASendBuffer = make_shared<WSABUF>();

	ZeroMemory(mOverlapped.get(), sizeof(OVERLAPPED));
	ZeroMemory(mBuffer.getBuffer(), bufferSize);

	mWSARecvBuffer->buf = mBuffer.getBuffer();
	mWSARecvBuffer->len = static_cast<ULONG>(bufferSize);

	mReceivedHandler = [](Buffer&, size_t size) {};
	mSentHandler = [](Buffer&) {};

	mState = -1;
}

// Destructor. Wait then disconnect. 
// Warning:: Will Block until the Status Pending has been completed.
Client::~Client() {
	// Await Overlapped to finish pending events.
	//while (mOverlapped->Internal != STATUS_PENDING) {}
	Disconnect();
}

//Copy constructor. 
// Ensure that we cannot copy this class elsewhere, as it is The thread 0x3784 has exited with code -1073741510 (0xc000013a).

// Returns the Socket 
SOCKET Client::getSocket() {
	return mSocket;
}

//// Begin the Read, calling WSA Recv.
void Client::BeginRead() {
	DWORD dwBytes = 0;
	DWORD dwFlags = 0;
	mState = CLIENT_STATE::READ;
	int status = WSARecv(mSocket, mWSARecvBuffer.get(), 1, &dwBytes, &dwFlags, mOverlapped.get(), nullptr);

	if (status == SOCKET_ERROR) {
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING) {
			DebugBreak();
		}
	}

}

// Return the Packet from the 
Buffer Client::getMessage() {
	return mBuffer;
}

// Complete the Read, in this instance we call the Handler we've passed.
void Client::CompleteRead(size_t size) {
	mReceivedHandler(getMessage(), size);
}

// Begin The Send Operation
void Client::BeginSend(byte* data, size_t dataSize) {
	//Get data.
	mWSASendBuffer->len = (ULONG) dataSize;
	mWSASendBuffer->buf = reinterpret_cast<char*>(data);
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;

	mState = CLIENT_STATE::SEND;
	int result = WSASend(mSocket, mWSASendBuffer.get(), 1, &dwBytes, dwFlags, mOverlapped.get(), 0);

	if (result == SOCKET_ERROR) {
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING) {
			DebugBreak();
		}
	}

}

// Complete the Read, and perforrm the "Sent Handler" 
void Client::CompleteSend() {
	Buffer buff(mWSASendBuffer->buf, mWSASendBuffer->len);
	mSentHandler(buff);
}

// Reset the buffers.
void Reset() {
	//Reset The Buffers. 
}

// Do the IO Event. 
void Client::BeginIOEvent(DWORD transferred) {
	switch (mState) {
	case CLIENT_STATE::READ:
		CompleteRead((size_t)transferred);
		//Start up for a new read.
		BeginRead();
		break;
	case CLIENT_STATE::SEND:
		CompleteSend();
		break;
	default:
		break;
	}
}

// Set the Received Handler 
void Client::SetReceivedHandler(const ReceivedHandler& handler) {
	mReceivedHandler = handler;
}

// Set the Sent Handler
void Client::SetSentHandler(const SentHandler& handler) {
	mSentHandler = handler;
}

// Disconnect the Socket.
void Client::Disconnect() {}