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
