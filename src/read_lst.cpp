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
#include <boost/tuple/tuple.hpp>
#include "gnuplot-iostream.h"
namespace po = boost::program_options;

template <typename T>
void plot_bigtime(boost::multi_array<T, 2>& data, FILES& lst_files, CONFIG& config, std::string const filename)
{
  unsigned int bin_num = config.bigtime.bin_num;
  unsigned int const file_num = lst_files.file_num;
  unsigned long long tstart = (unsigned long long)config.bigtime.tstart;
  unsigned long long tend = (unsigned long long)config.bigtime.tend;
  Gnuplot gp;
  typedef boost::tuple<double, double, T> mytriple;
  std::vector<std::vector<mytriple> > big_time_result_v(bin_num,std::vector<mytriple>(file_num));
  for(int i = 0; i<bin_num; i++)
    for (int j = 0; j<file_num; j++)
      {
        double det = config.detuning[j];
        double t_step = (double)(tend-tstart)/bin_num; //TODO:check
        double time = ((double)tstart+t_step*i)/1e9;
        big_time_result_v[i][j] = mytriple(time,det,data[i][j]);
      }
  gp << "set terminal postscript eps enhanced color\n";
  gp << "unset key\n";
  gp << "set output \"" << filename << "\"\n";
  gp << "set view map\n";
  gp << "set title \"" << config.caption <<"\" offset 0,1\n"; //TODO: count \n in caption
  gp << "set xlabel \"time(ms)\"\n";
  gp << "set ylabel \"detuning(MHz)\"\n";
  //gp << "set lmargin 5\n";
  gp << "set border 4095\n";
  gp << "splot '-' with image\n";
  gp.send2d(big_time_result_v);
}

template <typename T>
void plot_phase(boost::multi_array<T, 2>& data, FILES& lst_files, CONFIG& config, unsigned long const avg_period, std::string const filename)
{
  unsigned int bin_num = config.phase.bin_num;
  unsigned int const file_num = lst_files.file_num;
  // unsigned long long tstart = (unsigned long long)config.bigtime.tstart;
  // unsigned long long tend = (unsigned long long)config.bigtime.tend;
  Gnuplot gp;
  typedef boost::tuple<double, double, T> mytriple;
  std::vector<std::vector<mytriple> > big_time_result_v(bin_num,std::vector<mytriple>(file_num));
  for(int i = 0; i<bin_num; i++)
    for (int j = 0; j<file_num; j++)
      {
        double det = config.detuning[j];
        double t_step = (double)(avg_period)/bin_num; //TODO:check
        double time = (t_step*i)/1e3;
        big_time_result_v[i][j] = mytriple(time,det,data[i][j]);
      }
  gp << "set terminal postscript eps enhanced color\n";
  gp << "unset key\n";
  gp << "set output \"" << filename << "\"\n";
  gp << "set view map\n";
  gp << "set title \"" << config.caption <<"\" offset 0,1\n"; //TODO: count \n in caption
  gp << "set xlabel \"time(ns)\"\n";
  gp << "set ylabel \"detuning(MHz)\"\n";
  //gp << "set lmargin 5\n";
  gp << "set border 4095\n";
  gp << "splot '-' with image\n";
  gp.send2d(big_time_result_v);
}

int main(int argc, char *argv[])
{
  po::variables_map vm = parse_option(argc,argv);
  int const omp_thread_num = vm["process"].as<int>();
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
      unsigned int bin_num = config.bigtime.bin_num;
      unsigned int const file_num = lst_files.file_num;
      unsigned long long tstart = (unsigned long long)config.bigtime.tstart;
      unsigned long long tend = (unsigned long long)config.bigtime.tend;
      unsigned long long normalize_tstart = (unsigned long long)config.bigtime.normalize_tstart;
      unsigned long long normalize_tend = (unsigned long long)config.bigtime.normalize_tend;
      std::vector<int> const channels = config.bigtime.channels;
      bool normalize = config.bigtime.normalize;
      if (not normalize)
        {
          typedef boost::multi_array<unsigned long long, 2> array_type;
          unsigned long* const output_array = new unsigned long[bin_num];
          array_type big_time_result(boost::extents[bin_num][file_num]);
          for (int i = 0; i<bin_num; i++)
            for (int j = 0; j<file_num; j++)
              big_time_result[i][j] = 0ULL;
          for (int i = 0; i<lst_files.files.size(); i++)
            {
              std::string const filename = lst_files.files[i];
              LstReader reader(filename);
              reader.decode_counts();
              reader.big_time(channels,tstart,tend,bin_num,output_array);
              for (int j = 0; j<bin_num; j++)
                big_time_result[j][i] = output_array[j];
            }
          //output
          std::string const h5_filename = lst_files.path+lst_files.prefix+"_bigtime.h5";
          std::string const eps_filename = lst_files.path+lst_files.prefix+"_bigtime.eps";
          save_marray_ull_to_h5(&big_time_result,h5_filename,"bigtime",false);
          delete [] output_array;
          plot_bigtime(big_time_result,lst_files,config,eps_filename);
        }
      else
        {
          typedef boost::multi_array<double, 2> array_type;
          double* const output_array = new double[bin_num];
          array_type big_time_result(boost::extents[bin_num][file_num]);
          for (int i = 0; i<bin_num; i++)
            for (int j = 0; j<file_num; j++)
              big_time_result[i][j] = 0.0;
          for (int i = 0; i<lst_files.files.size(); i++)
            {
              std::string const filename = lst_files.files[i];
              LstReader reader(filename);
              reader.decode_counts();
              for (auto it=config.bigtime.channels.begin();it!=config.bigtime.channels.end();it++)
                {
                  reader.big_time_normalize(*it,tstart,tend,bin_num,normalize_tstart,normalize_tend,output_array,omp_thread_num);
                  for (int j = 0; j<bin_num; j++)
                    big_time_result[j][i] += output_array[j]/config.bigtime.channels.size();
                }
            }
          //output
          std::string const h5_filename = lst_files.path+lst_files.prefix+"_bigtime.h5";
          std::string const eps_filename = lst_files.path+lst_files.prefix+"_bigtime.eps";
          save_marray_d_to_h5(&big_time_result,h5_filename,"bigtime",false);
          delete [] output_array;
          plot_bigtime(big_time_result,lst_files,config,eps_filename);
        }
    }
  if (vm.count("phase") && vm.count("prefix"))
    {
      unsigned int bin_num = config.phase.bin_num;
      unsigned int const file_num = lst_files.file_num;
      unsigned long long tstart = (unsigned long long)config.phase.tstart;
      unsigned long long tend = (unsigned long long)config.phase.tend;
      unsigned long long normalize_tstart = (unsigned long long)config.phase.normalize_tstart;
      unsigned long long normalize_tend = (unsigned long long)config.phase.normalize_tend;
      std::vector<int> const channels = config.phase.channels;
      bool normalize = config.phase.normalize;
      if (normalize)
        {
          unsigned long avg_period = 0UL;
          typedef boost::multi_array<double, 2> array_type;
          array_type phase_result(boost::extents[bin_num][file_num]);
          double* const output_array = new double[bin_num];
          for (int i = 0; i<bin_num; i++)
            for (int j = 0; j<file_num; j++)
              phase_result[i][j] = 0.0;
          for (int i = 0; i<lst_files.files.size(); i++)
            {
              std::string const filename = lst_files.files[i];
              LstReader reader(filename);
              reader.decode_counts();
              for (auto it=channels.begin();it!=channels.end();it++)
                {
                  avg_period = reader.phase_hist_normalize(*it,tstart,tend,bin_num,normalize_tstart,normalize_tend,output_array,3,omp_thread_num);
                  for (int j = 0; j<bin_num; j++)
                    phase_result[j][i] += output_array[j]/config.bigtime.channels.size();
                }
            }
          //output
          std::string const h5_filename = lst_files.path+lst_files.prefix+"_phase.h5";
          std::string const eps_filename = lst_files.path+lst_files.prefix+"_phase.eps";
          save_marray_d_to_h5(&phase_result,h5_filename,"phase",false);
          delete [] output_array;
          plot_phase(phase_result, lst_files, config, avg_period,eps_filename);
        }
      else
        {
          unsigned long avg_period = 0UL;
          typedef boost::multi_array<unsigned long long, 2> array_type;
          unsigned long* const output_array = new unsigned long[bin_num];
          array_type phase_result(boost::extents[bin_num][file_num]);
          for (int i = 0; i<bin_num; i++)
            for (int j = 0; j<file_num; j++)
              phase_result[i][j] = 0;
          for (int i = 0; i<lst_files.files.size(); i++)
            {
              std::string const filename = lst_files.files[i];
              LstReader reader(filename);
              reader.decode_counts();
              for (auto it=channels.begin();it!=channels.end();it++)
                {
                  avg_period = reader.phase_hist(*it,tstart,tend,bin_num,output_array);
                  for (int j = 0; j<bin_num; j++)
                    phase_result[j][i] += output_array[j]/channels.size();
                }
            }
          //output
          std::string const h5_filename = lst_files.path+lst_files.prefix+"_phase.h5";
          std::string const eps_filename = lst_files.path+lst_files.prefix+"_phase.eps";
          save_marray_ull_to_h5(&phase_result,h5_filename,"phase",false);
          delete [] output_array;
          plot_phase(phase_result, lst_files, config, avg_period, eps_filename);
        }
    }
  return 0;
}
