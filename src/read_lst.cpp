#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cassert>
#include <stdexcept>
#include "LstReader.hpp"
#include "mainroutine.hpp"
#include "parseoption.hpp"
#include "fileio.hpp"
#include "boost/multi_array.hpp"
#include "boost/program_options.hpp"
#include <boost/tuple/tuple.hpp>
#include "gnuplot-iostream.h"
namespace po = boost::program_options;

int main(int argc, char *argv[])
{
  po::variables_map vm = parse_option(argc,argv);
  int const omp_thread_num = vm["process"].as<int>();
  COMBINE_FILES c_lst_files;
  FILES lst_files;
  CONFIG config;
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
  if (vm.count("combine"))
    {
      auto files = vm["combine"].as<std::vector<std::string>>();
      if (not check_combine_files_format(files,&c_lst_files))
        exit(EXIT_FAILURE);
      for (auto it = c_lst_files.files.begin(); it != c_lst_files.files.end(); it++)
        std::cout << *it << std::endl;
    }
  if (vm.count("config"))
    {
      std::string const config_filename = vm["config"].as<std::string>();
      std::cout << "Config file:" << config_filename << std::endl;
      read_yaml_config(config_filename, config);
    }
  if (vm.count("config") && vm.count("prefix"))
    {
      if (lst_files.file_num != config.detuning.size())
        {
          std::cout << "File number does not match detuning range!";
          exit(EXIT_FAILURE);
        }
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
      bigtime(vm, omp_thread_num, lst_files, config);
    }
  if (vm.count("phase") && vm.count("prefix"))
    {
      phase(vm, omp_thread_num, lst_files, config);
    }
  if (vm.count("pulse") && vm.count("combine"))
    {
      pulse(vm, omp_thread_num, c_lst_files, config);
    }
  if (vm.count("pulse") && vm.count("prefix"))
    {
      pulse(vm, omp_thread_num, lst_files, config);
    }
  if (vm.count("g2") && vm.count("combine"))
    {
      std::cout << "calculate g2..." << std::endl;
      g2(vm, omp_thread_num, c_lst_files, config);
    }
  return 0;
}
