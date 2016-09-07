#pragma once

#include "FileConverter.h"
#include "Song.h"
#include "WavFilePackager.h"
#include "stdadfx.h"

// The File Converter
// From File To Song.
class SongFileConverter : public FileConverter<Song> {
public: 

  // Returns the Class from the file. 
  virtual bool getClassFromFile(const string& fileDir, const string& fileName, Song& song) {
    WavFilePackager::waveHeader header;
    WavFilePackager packager;
    size_t bufferSize = 44;
    byte* buffer = new byte[bufferSize];

    bool ok = packager.openFile(fileDir + '\\' + fileName);
    if (ok && packager.isFileOpen()) {
      packager.getHeader(buffer, bufferSize);
      memcpy(&header, buffer, bufferSize);
      delete[] buffer;

      ok = header.chunk_size > 0;
      if (ok) {
        int timeLen = packager.getTotalSize() / (header.sample_rate * header.num_channels * header.bits_per_sample / 8);

        size_t found = fileName.find("_");
        if (found != string::npos) {
          //eh. Do this later.
          string songName = fileName.substr(0, found);
          string artistName = fileName.substr(found);

          strcpy_s(song.SongName, songName.size() + 1, songName.c_str());
          strcpy_s(song.ArtistName, artistName.size() + 1, artistName.c_str());
        }
        else {
          strcpy_s(song.SongName, fileName.size() + 1, fileName.c_str());
          strcpy_s(song.ArtistName, 8, "Unknown");
        }

        song.SongLength = timeLen;
      }
      packager.closeFile();
    }

    return ok;
  }
};