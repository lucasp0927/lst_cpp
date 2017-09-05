#include "LstReader.hpp"
unsigned int time_patch_dlen(const std::string& time_patch)
{
  unsigned int dlen = 0;
  if (time_patch.compare("0")==0)
    dlen = 2;
  else if (time_patch.compare("5")==0)
    dlen = 4;
  else if (time_patch.compare("5b")==0)
    dlen = 8;
  else if (time_patch.compare("1")==0)
    dlen = 4;
  else if (time_patch.compare("1a")==0)
    dlen = 6;
  else if (time_patch.compare("32")==0)
    dlen = 6;
  else if (time_patch.compare("2")==0)
    dlen = 6;
  else if (time_patch.compare("f3")==0)
    dlen = 8;
  else
    std::cout << "wrong time_patch." << std::endl;
  return dlen;
}

std::ifstream& GotoLine(std::ifstream& file, unsigned int num){
  file.seekg(std::ios::beg);
  for(unsigned int i=0; i < num - 1; ++i){
    file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
  }
  return file;
}
