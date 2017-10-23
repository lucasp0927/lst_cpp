#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cassert>
#include "LstReader.hpp"
#include "boost/multi_array.hpp"

int main(int argc, char *argv[])
{
  assert(argc==3);
  std::string filename_prefix(argv[1]);
  int file_num = (int) strtol(argv[2],nullptr,10);
  std:: cout << "read files: " << filename_prefix << std::endl;
  std:: cout << "number of files: " << file_num << std::endl;

  unsigned int bin_num = 70;
  // unsigned long long tstart = (unsigned long long)2e9;
  // unsigned long long tend = (unsigned long long)72e9;

  typedef boost::multi_array<unsigned long, 2> array_type;
  typedef array_type::index index;
  array_type big_time_result(boost::extents[bin_num][file_num]);

  for (index f = 0; f < file_num; f++){
    std::string filename = filename_prefix+std::to_string(f+1)+"_nozero.lst";
    std::string h5_filename = filename_prefix+std::to_string(f+1)+"_nozero.h5";
    std::cout << filename << std::endl;
    LstReader reader(filename);
    reader.decode_counts();
    reader.save_counts_to_h5(h5_filename,"events",false);
  }
  return 0;
}
