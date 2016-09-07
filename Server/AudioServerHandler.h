#pragma once
#include "InputHandler.h"
#include "ConcurrentQueue.h"
#include "FilePackager.h"
#include "WavFilePackager.h"
#include "InputHandler.h"
#include "FileList.h"
#include "Song.h"
#include "Socket.h"
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
  explicit AudioServerHandler();
 
  //Handle the Packet we have sent 
  //  sentMessage: The Packet we wish to process.  
  void handlePacket(const Socket&& socket, weak_ptr<ConcurrentQueue<Packet>> queue, const Packet& packet);
private: 
  
  // Is the current thread running
  atomic_bool isRunning = false;

  // Is the read thread running
  atomic_bool readThreadActive = false;
  
  // The Mutex.
  mutex mMutex;

  // Is this the only thread running.
  condition_variable_any mIsOnly;

  // The File List of songs.
  unique_ptr<FileList<Song>> fileList;


  // Requst a song, chop it up and send it to te mConQueue
  //   fileName : The Song File we want to dissect.
  void requestFile(Song songFile);


  void requestFileList(const FileList<Song>& files);
};