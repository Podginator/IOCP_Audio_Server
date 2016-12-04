#include "AudioServerHandler.h"
#include "WavFilePackager.h"
#include <iostream>
#include <fstream>


//Constructor
// conQue: Weak_Ptr to a ConcurrentQueue of Packets. This is where we'll send to.
// Tell the InputHandler that we deal with Track, Exit and File List.
AudioServerHandler::AudioServerHandler(weak_ptr<Client> client, weak_ptr<ConcurrentQueue<ClientPacket>> packetQueue)
  : InputHandler(Type::TRACK | Type::FILELIST), mConQueue(packetQueue), mClient(client) {
    fileList = make_unique<FileList<Song>>("C:\\Temp", "wav", make_unique<SongFileConverter>());
}

//Handle the Packet we have sent 
//  sentMessage: The Packet we wish to process.  
void AudioServerHandler::handlePacket(const Packet& sentMessage) {

  switch (sentMessage.type) {
  case Type::TRACK:
  {
    // Store the song on the stack.
    Song packetSong;

    //Ensure that the packets contents are the size of a song (as a preliminary check)
    if (sentMessage.size == sizeof(Song)) {
      //Copy the memory of the song to a song object. 
      memcpy(&packetSong, sentMessage.packetData, sentMessage.size);
      requestFile(sentMessage.id, packetSong);
    }
    break;
  }
  case Type::FILELIST:
    requestFileList(sentMessage.id, *fileList);
    break;
  }
 
}

// Requst a song, chop it up and send it to te mConQueue
//   fileName : The Song File we want to dissect. 
//   Note: We perform a copy here to avoid any issues with synchronization.
//   Song is trivially copyable and the performance overhead is minimal.
void AudioServerHandler::requestFile(int id, Song song){
  std::vector<byte> vectorBytes;
  //Get the index of the thread.
  int index = fileList->indexOf(song);

  //If we've found the file. 
  if (index > -1) {

    // Get the filename, catch any exceptions. 
    string fileName = fileList->get().at(index).second;

    //First grab the file
    WavFilePackager packager;
    packager.openFile(fileName);

    // Check that we can 
    if (packager.isFileOpen()) {

      size_t headerSize = 44;
      byte* headerBuffer = new byte[headerSize];
      packager.getHeader(headerBuffer, headerSize);
      for (int i = 0; i < headerSize; i++) {
        vectorBytes.push_back(headerBuffer[i]);
      }
      Packet packet(Type::AUDIO, (int) headerSize, id, headerBuffer);
      delete[] headerBuffer;

      if (auto queue = mConQueue.lock()) {
        queue->push(ClientPacket(mClient, packet));
      }

      while (!packager.isFileComplete()) {
        size_t bufferSize = Packet::maxPacketSize;
        byte* dataBuffer = new byte[bufferSize];

        size_t actualSize = packager.getNextChunk(dataBuffer, bufferSize);

        for (int i = 0; i < actualSize; i++) {
          vectorBytes.push_back(dataBuffer[i]);
        }
        
        Packet dataPacket(Type::AUDIO, (int) actualSize, id, dataBuffer);

        if (auto queue = mConQueue.lock()) {
          queue->push(ClientPacket(mClient, dataPacket));
        }

        delete[] dataBuffer;
      }
    }

    packager.closeFile();
  }
}


void AudioServerHandler::requestFileList(int id, const FileList<Song>& files) {
  size_t songSize = sizeof(Song);

  byte* byteArray = new byte[Packet::maxPacketSize];
  byte* pointer = byteArray;
  int used = 0;

  for (auto file : files.get()) {
    auto type = file.first;

    if ((used + songSize) > Packet::maxPacketSize) {

      if (auto queue = mConQueue.lock()) {
        //Make the packet. 
        Packet intermediatePacket(Type::FILELIST, used, id, byteArray);
        //Delete the existing one.
        delete[] byteArray;
        queue->push(ClientPacket(mClient, intermediatePacket));
       } else {
         break;
      }

      //Clear buffer. 
      byteArray = new byte[Packet::maxPacketSize];
      pointer = byteArray;
      used = 0;
    }

    memcpy(pointer, &type, songSize);
    pointer += songSize;
    used += songSize;
  }

  if (auto queue = mConQueue.lock()) {
    Packet finalPacket(Type::FILELIST, used, id, byteArray);
    queue->push(ClientPacket(mClient, finalPacket));
  }

  //Clear buffer. 
  delete[] byteArray;
}