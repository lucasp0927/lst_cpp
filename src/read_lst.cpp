#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cassert>
#include <stdexcept>
#include "LstReader.hpp"
#include "parseoption.hpp"
#include "fileio.hpp"
#include "boost/multi_array.hpp"
#include "boost/program_options.hpp"
namespace po = boost::program_options;

int main(int argc, char *argv[])
{
  po::variables_map vm = parse_option(argc,argv);
  FILES lst_files;
  //Convert
  if (vm.count("prefix"))
    {
      std::string pat = vm["prefix"].as<std::string>();
      if(not check_files_format(pat,&lst_files))
        exit(EXIT_FAILURE);
      std::cout << "prefix: " << lst_files.prefix << std::endl;
      std::cout << "path: " << lst_files.path << std::endl;
      std::cout << "file(s) found: " << lst_files.file_num << std::endl;
      for (auto it = lst_files.files.begin(); it != lst_files.files.end(); it++)
        std::cout << *it << std::endl;
    }
  if (vm.count("convert") && vm.count("prefix"))
    {
      std::cout << "Convert to h5 files." << std::endl;
      for (auto it = lst_files.files.begin(); it != lst_files.files.end(); it++)
        {
          std::string const filename = *it;
          std::string const h5_filename = filename.substr(0,filename.length()-4)+".h5";
          LstReader reader(filename);
          reader.decode_counts();
          reader.save_counts_to_h5(h5_filename,"events",false);
        }
    }
  if (vm.count("bigtime") && vm.count("prefix"))
    {
      unsigned int bin_num = 160;
      unsigned long long tstart = (unsigned long long)2e9;
      unsigned long long tend = (unsigned long long)72e9;
      typedef boost::multi_array<unsigned long, 2> array_type;
      typedef array_type::index index;
      array_type big_time_result(boost::extents[bin_num][file_num]);
    }

  return 0;
}
