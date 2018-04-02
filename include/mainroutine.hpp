#ifndef MAINROUTINE_HPP
#define MAINROUTINE_HPP
#include "LstReader.hpp"
//#include "parseoption.hpp"
#include "fileio.hpp"
#include "boost/multi_array.hpp"
#include "boost/program_options.hpp"
#include <boost/tuple/tuple.hpp>
#include "gnuplot-iostream.h"
namespace po = boost::program_options;

template <typename T> void plot_bigtime(boost::multi_array<T, 2>& data, FILES& lst_files, CONFIG& config, std::string const filename);

template <typename T> void plot_phase(boost::multi_array<T, 2>& data, FILES& lst_files, CONFIG& config, unsigned long const avg_period, std::string const filename);

void bigtime(po::variables_map& vm, int const omp_thread_num, FILES& lst_files, CONFIG& config);
void phase(po::variables_map& vm, int const omp_thread_num, FILES& lst_files, CONFIG& config);
void pulse(po::variables_map& vm, int const omp_thread_num, FILES& lst_files, CONFIG& config);
void pulse(po::variables_map& vm, int const omp_thread_num, COMBINE_FILES& c_lst_files, CONFIG& config);
void g2(po::variables_map& vm, int const omp_thread_num, COMBINE_FILES& c_lst_files, CONFIG& config);
#endif
