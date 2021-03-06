#ifndef LSTREADER_HPP
#define LSTREADER_HPP
#include "Count.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <cassert>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <numeric>
#include <H5Cpp.h>
#include "boost/multi_array.hpp"
#include <omp.h>
#include <math.h>
#ifdef max
#undef max
#endif
const int CHANNEL_NUM = 6; //total 6 channels
using namespace H5;

unsigned int time_patch_dlen(const std::string& time_patch);
std::ifstream& GotoLine(std::ifstream& file, unsigned int num);
struct compare_timedata //use in bigtime
{
  inline bool operator() (const Count& c1, const Count& c2)
  {
    return (c1.get_timedata() < c2.get_timedata());
  }
};

class LstReader
{
private:
  std::string filename;
  std::string time_patch;
  unsigned int bit_shift;
  unsigned int buffer_sw_preset;
  unsigned int sw_preset;
  unsigned int range;
  unsigned int cycles;
  unsigned int dlen;
  unsigned long bin_width;
  unsigned long long timedata_limit;
  unsigned long long buffer_total_data_count;
  unsigned long buffer_nonzero_data_count;
  char* buffer;
  std::vector<Count> counts;
  std::vector<std::vector<unsigned long>> histogram;
public:
  LstReader(std::string filename);
  virtual ~LstReader();
  //get functions
  unsigned int get_sweep(){return sw_preset;};
  //////
  void prepare();
  void read_file();
  void print_header() const;
  void iterate_data();
  void save_non_zero_counts(const std::string& out_filename) const;
  void decode_counts();
  void decode_counts(unsigned long long tstart, unsigned long long tend);
  void read_additional_file(const std::string filename_);
  void save_counts_to_h5(std::string const filename, std::string const datasetname, bool const append);
  void save_marray_ull_to_h5(boost::multi_array<unsigned long long,2> const* const data,\
                             std::string const filename,\
                             std::string const datasetname,\
                             bool const append);
  void print_stat();
  void big_time(unsigned int const channel,\
                unsigned long long const tstart,\
                unsigned long long const tend,\
                unsigned int const bin_num,\
                unsigned long* const output_buffer) const;
  void big_time(unsigned int const channel,\
                unsigned int const sweep,\
                unsigned long long const tstart,\
                unsigned long long const tend,\
                unsigned int const bin_num,\
                unsigned long* const output_buffer) const;
  void big_time(std::vector<Count>& counts_data,\
                unsigned long long const tstart,\
                unsigned long long const tend,\
                unsigned int const bin_num,\
                unsigned long* const output_buffer) const;
  void big_time(std::vector<int> const channels,\
                unsigned long long const tstart,\
                unsigned long long const tend,\
                unsigned int const bin_num,\
                unsigned long* const output_buffer) const;
  void big_time_sweep(std::vector<int> const channels,\
		      unsigned int sweep,\
		      unsigned long long const tstart,	\
		      unsigned long long const tend,	\
		      unsigned int const bin_num,		\
		      unsigned long* const output_buffer) const;  
  void big_time_normalize(unsigned int const channel,\
                          unsigned long long const tstart,  \
                          unsigned long long const tend,    \
                          unsigned int const bin_num,       \
                          unsigned long long const normalize_tstart,  \
                          unsigned long long const normalize_tend,    \
                          double* const output_buffer,
                          int const thread_num) const;
  ///////////////////
  //sort and select functions
  ///////////////////
  void sort_by_sweep(std::vector<Count>& counts) const;
  void sort_by_time(std::vector<Count>& counts) const;
  void select_sweep(std::vector<Count>& result, std::vector<Count> const& input, unsigned int const sweep) const;
  void select_sweep(std::vector<Count>& result, std::vector<Count> const& input, unsigned int const sweep_start, unsigned int const sweep_end) const;
  void select_channel(std::vector<Count>& result, std::vector<Count> const& input, unsigned int const channel) const;
  void select_channel(std::vector<Count>& result, std::vector<Count> const& input, unsigned int const channel_start, unsigned int const channel_end) const;
  void select_timedata(std::vector<Count>& result, std::vector<Count> const& input, unsigned long long const tstart, unsigned long long const tend) const;
  ///////////////////
  //end of select functions
  ///////////////////
  unsigned long long calculate_lattice_period(std::vector<Count> const& clock) const;
  unsigned long long calculate_lattice_period_var(std::vector<Count> const& clock) const;
  unsigned long period_combined_average(std::vector<unsigned long> const& period_count,\
                                        std::vector<unsigned long> const& periods)const;
  unsigned long long period_combined_variance(std::vector<unsigned long> const& period_count,\
                                              std::vector<unsigned long> const& periods,\
                                              std::vector<unsigned long long> const& periods_var)const;
  ///////////////////
  //phase hist
  ///////////////////
  unsigned long phase_hist(unsigned int const channel,\
                           unsigned long long const tstart, \
                           unsigned long long const tend,   \
                           unsigned int const bin_num,      \
                           unsigned long* const result,     \
                           unsigned int const clock_ch = 3  \
                           ) const;


  unsigned long phase_hist_normalize(unsigned int const channel,\
                                     unsigned long long const tstart,\
                                     unsigned long long const tend,\
                                     unsigned int const bin_num,\
                                     unsigned long long const normalize_tstart,\
                                     unsigned long long const normalize_tend,\
                                     double* const result,\
                                     unsigned int const clock_ch,\
                                     int const thread_num) const;
  ///////////////////
  //pulse
  ///////////////////
  void pulse_hist(unsigned int const channel,\
                  unsigned long long const tstart,\
                  unsigned long long const tend,\
                  unsigned long const pulse_tstart,\
                  unsigned long const pulse_tend,\
                  std::vector<unsigned long>& result_timestamp, \
                  std::vector<unsigned long>& result_count, \
                  unsigned int const clock_ch,\
                  long const clock_delay,\
                  int const thread_num) const;
  ///////////////////
  //g2
  ///////////////////
  // result return all the time difference between ch1 ch2 counts.
  void g2_photon_statistic(std::vector<int> const& channels,    \
                           unsigned long long const tstart,     \
                           unsigned long long const tend,       \
                           unsigned long const pulse_tstart,    \
                           unsigned long const pulse_tend,      \
                           std::vector<long>& result,           \
                           unsigned int const clock_ch,         \
                           long const channel_delay,            \
                           int const thread_num)const;
  ///////////////////
  //Debug
  ///////////////////
  void print_ch3_size() const;
};
#endif
