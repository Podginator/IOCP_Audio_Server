#pragma once
#include <string>

// Simple Model class that stores the Client Settings:
struct Settings {

  //Share Name (How we will store the users plays.
  char ShareName[64];

  // Should we share our activity
  bool ShareActivity;

  // Should we ALSO send Album Art.
  bool AlbumArt;

  // Set up with default settings originally 
  Settings() : ShareName("John Smith"), ShareActivity(true), AlbumArt(true) { }


  // If we assign a new operator, do a deep copy.
  Settings& operator=(const Settings &rhs) {
    strncpy_s(ShareName, rhs.ShareName, 64);
    ShareActivity = rhs.ShareActivity;
    AlbumArt = rhs.AlbumArt;

    return *this;
  }

};

