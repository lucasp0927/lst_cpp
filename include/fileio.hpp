#ifndef FILEIO_HPP
#define FILEIO_HPP
#include <iostream>
#include <glob.h>
#include <vector>
#include <regex>
#include <H5Cpp.h>
#include <yaml-cpp/yaml.h>
#include "boost/multi_array.hpp"
using namespace H5;
struct FILES
{
  std::string prefix;
  int file_num;
  std::string path;
  std::vector<std::string> files;
};

struct COMBINE_FILES
{
  std::vector<std::string> files;
};

struct CONFIG
{
  std::vector<double> detuning;
  std::string caption;
  struct BIGTIME
  {
    std::vector<int> channels;
    bool normalize;
    bool cycle;
    unsigned long long normalize_tstart; //in ps
    unsigned long long normalize_tend;
    int bin_num;
    unsigned long long tstart; //in ps
    unsigned long long tend;
  };
  struct PHASE
  {
    std::vector<int> channels;
    bool normalize;
    unsigned long long normalize_tstart; //in ps
    unsigned long long normalize_tend;
    int bin_num;
    unsigned long long tstart; //in ps
    unsigned long long tend;
  };
  struct PULSE
  {
    std::vector<int> channels;
    unsigned long long tstart;
    unsigned long long tend;
    unsigned long pulse_tstart;
    unsigned long pulse_tend;
    long clock_delay;
  };
  struct G2
  {
    std::vector<int> channels; //2 channels
    unsigned long long tstart;
    unsigned long long tend;
    unsigned long pulse_tstart;
    unsigned long pulse_tend;
    long channel_delay;
  };
  BIGTIME bigtime;
  PHASE phase;
  PULSE pulse;
  G2 g2;
};

std::string extract_file_name(std::string const & fullPath);
std::string extract_file_path(std::string const & fullPath);
bool check_combine_files_format(std::vector<std::string> const & files,COMBINE_FILES* const c_lst_files);
bool check_files_format(std::string const pattern, FILES* const lst_files);
void save_marray_d_to_h5(boost::multi_array<double,2> const* const data, \
                           std::string const filename,\
                           std::string const datasetname,\
                           bool const append);
void save_marray_ull_to_h5(boost::multi_array<unsigned long long,2> const* const data, \
                           std::string const filename,\
                           std::string const datasetname,\
                           bool const append);

void save_marray_ull_to_h5(boost::multi_array<unsigned long long,3> const* const data, \
                           std::string const filename,\
                           std::string const datasetname,\
                           bool const append);
template <long unsigned int N>
void save_marray_l_to_h5(boost::multi_array<long,N> const* const data, \
                         std::string const filename,                   \
                         std::string const datasetname,                \
                         bool const append);
void read_yaml_config(std::string const filename, CONFIG& config);
#endif
