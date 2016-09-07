#pragma once

// The File Converter, Takes a File Path and a name, and attempts to convert it 
// To the Template.
template <class T> 
class FileConverter {
public :
  // Get the class from the file name.
  virtual bool getClassFromFile(const string& fileDir, const string& fileName, T& file) = 0;
};