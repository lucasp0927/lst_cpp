#include <iostream>
#include <glob.h>
#include <vector>
#include <regex>
#include <H5Cpp.h>
#include "boost/multi_array.hpp"
using namespace H5;
struct FILES
{
  int prefix;
  int file_num;
  std::string path;
  std::vector<std::string> files;
};
std::string extract_file_name(std::string const & fullPath);
std::string extract_file_path(std::string const & fullPath);
bool check_files_format(std::string const pattern, FILES* const lst_files);
void save_marray_ull_to_h5(boost::multi_array<unsigned long long,2> const* const data, \
                           std::string const filename,\
                           std::string const datasetname,\
                           bool const append);
