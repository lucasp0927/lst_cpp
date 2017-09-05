#include <iostream>
#include <string>
#include <cassert>
#include "LstReader.hpp"


int main(int argc, char *argv[])
{
  assert(argc==3);
  std::string in_filename(argv[1]);
  std::string out_filename(argv[2]);
  LstReader reader(in_filename);
  reader.save_non_zero_counts(out_filename);
  return 0;

}
