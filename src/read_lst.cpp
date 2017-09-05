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
    std::cout << filename << std::endl;
    LstReader reader(filename);
    reader.decode_counts();
    // reader.print_stat();
    // unsigned long* big_time = new unsigned long[bin_num];
    // reader.big_time(2, tstart, tend, bin_num, big_time);
    // for (index b=0; b < bin_num; b++)
    //   big_time_result[b][f] = big_time[b];
    // delete [] big_time;
  }
  //output
  // std::ofstream ofile;
  // ofile.open("big_time.txt");
  // for (index f = 0; f < file_num; ++f)
  //   {
  //     for (index i = 0; i < bin_num; ++i)
  //       {
  //         ofile << std::setw(6) <<  big_time_result[i][f];
  //       }
  //     ofile<<std::endl;
  //   }
  // ofile.close();

  // LstReader reader(filename);
  // reader.decode_counts();
  //big time
  // unsigned int bin_num = 50;
  // unsigned long long tstart = (unsigned long long)2e9;
  // unsigned long long tend = (unsigned long long)40e9;
  // unsigned long* big_time = new unsigned long[bin_num];
  // std::ofstream ofile;
  // //big_time
  // ofile.open("big_time.txt");
  // reader.big_time(1, tstart, tend, bin_num, big_time);
  // for (unsigned int i = 0; i < bin_num; ++i)
  //      ofile << big_time[i] << std::endl;
  // ofile.close();
  // delete [] big_time;
  //phase
  // bin_num = 40;
  // tstart = (unsigned long long) 18e9;
  // tend = (unsigned long long) 22e9;
  // ofile.open("phase_hist.txt");
  // unsigned long* phase_hist = new unsigned long[bin_num];
  // reader.phase_hist(1,tstart,tend,bin_num, phase_hist);
  // for (unsigned int i = 0; i < bin_num; ++i)
  //     ofile << phase_hist[i] << std::endl;
  // ofile.close();
  return 0;
}
