#include <iostream>
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
  unsigned long long tstart = 2e9;
  unsigned long long tend = 40e9;
  unsigned long* big_time = new unsigned long[bin_num];

  reader.big_time(2, tstart, tend, bin_num, big_time);
  // for (unsigned int i = 0; i < bin_num; ++i)
  //     std::cout << big_time[i] << std::endl;
  delete [] big_time;
  //phase
  bin_num = 40;
  unsigned long* phase_hist = new unsigned long[bin_num];
  reader.phase_hist(2,18E9,22E9,bin_num, phase_hist);
  for (unsigned int i = 0; i < bin_num; ++i)
      std::cout << phase_hist[i] << std::endl;
  return 0;
}
