#pragma once
#include "InputHandler.h"
#include "ConcurrentQueue.h"
#include "FilePackager.h"
#include "WavFilePackager.h"
#include "InputHandler.h"
#include "FileList.h"
#include "Song.h"
#include "SongFileConverter.h"
#include <atomic>

using namespace std;

/**
* The AudioServer Handler
*/
class AudioServerHandler : public InputHandler {
public:

  //Constructor
  // conQue: Weak_Ptr to a ConcurrentQueue of Packets. This is where we'll send to.
  AudioServerHandler(weak_ptr<Client> client, weak_ptr<ConcurrentQueue<ClientPacket>> packetQueue);
 
  //Handle the Packet we have sent 
  //  sentMessage: The Packet we wish to process.  
  void handlePacket(const Packet& packet);
private: 
  
  // The File List of songs.
  unique_ptr<FileList<Song>> fileList;

  // The Packet Queue 
  weak_ptr<ConcurrentQueue<ClientPacket>> mConQueue;

  // The Client we wish to send to.
  weak_ptr<Client> mClient;
  
  // Requst a song, chop it up and send it to te mConQueue
  //   fileName : The Song File we want to dissect.
  void requestFile(Song song);

  void requestFileList(const FileList<Song>& files);
};