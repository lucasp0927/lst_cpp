#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cassert>
#include "LstReader.hpp"
#include "parseoption.hpp"
#include "boost/multi_array.hpp"
#include "boost/program_options.hpp"
#include <glob.h>
#include <vector>
namespace po = boost::program_options;

int main(int argc, char *argv[])
{
  po::options_description* cmdline_opt;
  cmdline_opt = new po::options_description("read_lst options");
  cmdline_opt->add_options()
    ("help", "produce help message")
    ("single,S", "process single file")
    ("inputfile,I", po::value<std::string>(),"Input .lst file") //require single
    ("outputfile,O", po::value<std::string>(),"Output .h5 file")//require single
    ("prefix,P", po::value<std::string>(), "batch process prefix")
    ("config,C","config file for bigtime, phase ans pulse")
    ("convert","convert to h5 files") //require prefix
    ("bigtime","output bigtime result") //both, require config
    ("phase","output phase result") //both, require config
    ("pulse","pulse data") //both, require config
    ;
  po::variables_map vm;//variable map
  po::store(po::parse_command_line(argc, argv, *cmdline_opt), vm);
  conflicting_options(vm, "single", "prefix");
  option_dependency(vm, "inputfile", "single");
  option_dependency(vm, "outputfile", "single");
  option_dependency(vm, "bigtime", "config");
  option_dependency(vm, "phase", "config");
  option_dependency(vm, "pulse", "config");

  //Convert
  if (vm.count("convert"))
    {
      std::cout << "convert to h5 files" << std::endl;
      std::string pat = vm["prefix"].as<std::string>();
      std::cout << "Prefix: " << pat << std::endl;
      glob_t glob_result;
      glob(pat.c_str(),GLOB_TILDE,NULL,&glob_result);
      std::cout << glob_result.gl_pathc << " files found." << std::endl;
      std::vector<std::string> ret;
      for(unsigned int i=0;i<glob_result.gl_pathc;++i){
        std::cout << std::string(glob_result.gl_pathv[i]) << std::endl;
        ret.push_back(std::string(glob_result.gl_pathv[i]));
      }
      globfree(&glob_result);
    }
  /*
  assert(argc==3);
  std::string filename(argv[1]);
  std::string h5_filename(argv[2]);
  */
  //int file_num = (int) strtol(argv[2],nullptr,10);
  //std:: cout << "read files: " << filename << std::endl;
  //std:: cout << "number of files: " << file_num << std::endl;

  //unsigned int bin_num = 70;
  // unsigned long long tstart = (unsigned long long)2e9;
  // unsigned long long tend = (unsigned long long)72e9;

  //typedef boost::multi_array<unsigned long, 2> array_type;
  //typedef array_type::index index;
  //array_type big_time_result(boost::extents[bin_num][file_num]);
  /*
  LstReader reader(filename);
  reader.decode_counts();
  reader.save_counts_to_h5(h5_filename,"events",false);
  */
  /*
  for (index f = 0; f < file_num; f++){
    std::string filename = filename_prefix+std::to_string(f+1)+"_nozero.lst";
    std::string h5_filename = filename_prefix+std::to_string(f+1)+"_nozero.h5";
    std::cout << filename << std::endl;
    LstReader reader(filename);
    reader.decode_counts();
    reader.save_counts_to_h5(h5_filename,"events",false);
  }
  */
  return 0;
}
