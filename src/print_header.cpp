#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cassert>
#include "LstReader.hpp"
#include "boost/multi_array.hpp"

int main(int argc, char *argv[])
{
  assert(argc==2);
  std::string filename(argv[1]);
  //int file_num = (int) strtol(argv[2],nullptr,10);
  std:: cout << "read files: " << filename << std::endl;

  LstReader reader(filename);
  //reader.print_header();
  return 0;
}
