#pragma once

// Simple Model class that stores:
// Song Name
// Artist Name
// Song Length. 
struct Song {

  //Song Name
  char SongName[64]; 

  //Artists name
  char ArtistName[64];

  // The length of the song in seconds.
  int SongLength; 

  // Constructor. 
  Song() {};

  Song(const std::string& song, const std::string& artist, int len) {

    // Illegal Argument Exception.
    if ((song.size() > 64) || (artist.size() > 64)) {
      throw; 
    }

    strncpy_s(SongName, song.c_str(), song.size());
    strncpy_s(ArtistName, artist.c_str(), artist.size());

    SongLength = len;

  }

  ~Song() {}

  Song(const Song &obj) {
    //Copy Song and Artist Names. 
    strncpy_s(SongName, obj.SongName, 64);
    strncpy_s(ArtistName, obj.ArtistName, 64);
    SongLength = obj.SongLength;
  }

  Song(Song&& other) {
    memmove_s(ArtistName, 64, other.ArtistName, 64);
    memmove_s(SongName, 64, other.SongName, 64);
    SongLength = other.SongLength;
  }

  Song & operator= (const Song &obj) {
    //Copy Song and Artist Names. 
    strncpy_s(SongName, obj.SongName, 64);
    strncpy_s(ArtistName, obj.ArtistName, 64);

    SongLength = obj.SongLength;

    return *this;
  }

  Song & operator= (Song &&other) {
    memmove_s(ArtistName, 64, other.ArtistName, 64);
    memmove_s(SongName, 64, other.SongName, 64);
    SongLength = other.SongLength;

    return *this;
  }

  //Check equality. 
  bool operator ==(const Song &b) const {
    //Equal if all inner components are equal
    if (strcmp(SongName, b.SongName) != 0) {
      return false; 
    }

    if (strcmp(ArtistName, b.ArtistName) != 0) {
      return false;
    }

    if (SongLength != b.SongLength) {
      return false; 
    }

    return true;
  }
};

