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

LstReader::LstReader(std::string filename):filename(filename)
{
  buffer = nullptr;
  //std::cout << "reading file: "<< filename << std::endl;
  std::ifstream file;
  file.open(filename);
  assert(file.is_open());
  if (file.is_open())
    {
      std::string line;
      //time_patch
      GotoLine(file,132);
      getline(file,line);
      assert(line.find("time_patch")==0);
      time_patch = line.substr(11,2);
      //bit shift
      GotoLine(file,35);
      getline(file,line);
      assert(line.find("bitshift")==0);
      bit_shift = std::stoi("0x"+line.substr(9,1),0,16);
      //swpreset
      GotoLine(file,6);
      getline(file,line);
      assert(line.find("swpreset")==0);
      sw_preset = std::stoi(line.substr(9,5));
      //range
      GotoLine(file,2);
      getline(file,line);
      assert(line.find("range")==0);
      auto range_l = line.length()-6;
      range = std::stoi(line.substr(6,range_l));
      //cycles
      GotoLine(file,11);
      getline(file,line);
      assert(line.find("cycles")==0);
      auto cycles_l = line.length()-7;
      cycles = std::stoi(line.substr(7,cycles_l));
    }
  file.close();
  bin_width = (1UL<<bit_shift)*RESOLUTION;
  timedata_limit = (unsigned long long)bin_width*(unsigned long long)range;
  prepare();
  read_file();
  iterate_data();
  print_header();
}
