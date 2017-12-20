#include <iostream>
#include <glob.h>
#include <vector>
#include <regex>
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
