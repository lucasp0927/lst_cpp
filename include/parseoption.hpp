#ifndef PARSEOPTION_HPP
#define PARSEOPTION_HPP
#include <string>
#include "boost/program_options.hpp"
namespace po = boost::program_options;

void conflicting_options(const po::variables_map& vm,
                         std::string const & opt1, std::string const & opt2)
{
    if (vm.count(opt1) && !vm[opt1].defaulted()
        && vm.count(opt2) && !vm[opt2].defaulted())
      throw std::logic_error(std::string("Conflicting options '")
                          + opt1 + "' and '" + opt2 + "'.");
}
void option_dependency(po::variables_map const& vm,
                       std::string const & for_what, std::string const & required_option)
{
  if (vm.count(for_what) && !vm[for_what].defaulted())
    if (vm.count(required_option) == 0 || vm[required_option].defaulted())
      throw std::logic_error(std::string("Option '") + for_what
          + "' requires option '" + required_option + "'.");
}

po::variables_map parse_option(int const argc, char* const argv[])
{
  po::variables_map vm;//variable map
  po::options_description cmdline_opt{"read_lst options"};
  cmdline_opt.add_options()
    ("help", "produce help message")
    //("single,S", "process single file")
    //("inputfile,I", po::value<std::string>(),"Input .lst file") //require single
    //("outputfile,O", po::value<std::string>(),"Output .h5 file")//require single
    ("prefix", po::value<std::string>(), "batch process prefix")
    ("combine", po::value<std::vector<std::string>>()->multitoken(), "Only for pulse.")
    ("combineoutput",po::value<std::string>(), "Output filename after combining data.")
    ("postfix", po::value<std::string>()->default_value(""), "output files postfix")
    ("process,P",po::value<int>()->default_value(1),"multi-thread thread number")
    //("config,C", po::value<std::vector<std::string>>(), "config file for bigtime, phase ans pulse")
    ("config,C", po::value<std::string>(), "config file for bigtime, phase ans pulse")
    ("convert","convert to h5 files.") //require prefix
    /*("removezero","remove zeros") //require prefix*/
    ("bigtime","output bigtime result.") //both, require config
    ("phase","output phase result.") //both, require config
    ("pulse","pulse data.") //both, require config
    ("g2", "calculate photon statistics.")
    ;
  po::store(po::parse_command_line(argc, argv, cmdline_opt), vm);
  //conflicting_options(vm, "single", "prefix");
  //option_dependency(vm, "inputfile", "single");
  //option_dependency(vm, "outputfile", "single");
  option_dependency(vm, "bigtime", "config");
  option_dependency(vm, "phase", "config");
  option_dependency(vm, "pulse", "config");
  //option_dependency(vm, "combine", "pulse");
  option_dependency(vm, "combine", "combineoutput");
  return vm;
}
#endif
