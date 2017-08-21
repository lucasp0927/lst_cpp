#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include "LstReader.hpp"

int main(int argc, char *argv[])
{
  assert(argc==2);
  std::string filename(argv[1]);
  LstReader reader(filename);
  reader.decode_counts();
  //big time
  unsigned int bin_num = 50;
  unsigned long long tstart = (unsigned long long)2e9;
  unsigned long long tend = (unsigned long long)40e9;
  unsigned long* big_time = new unsigned long[bin_num];
  std::ofstream ofile;
  //big_time
  ofile.open("big_time.txt");
  reader.big_time(2, tstart, tend, bin_num, big_time);
  for (unsigned int i = 0; i < bin_num; ++i)
       ofile << big_time[i] << std::endl;
  ofile.close();
  delete [] big_time;
  //phase
  bin_num = 40;
  tstart = (unsigned long long) 18e9;
  tend = (unsigned long long) 22e9;
  ofile.open("phase_hist.txt");
  unsigned long* phase_hist = new unsigned long[bin_num];
  reader.phase_hist(2,tstart,tend,bin_num, phase_hist);
  for (unsigned int i = 0; i < bin_num; ++i)
      ofile << phase_hist[i] << std::endl;
  ofile.close();
  return 0;
}
