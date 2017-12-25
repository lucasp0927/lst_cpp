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
namespace po = boost::program_options;

int main(int argc, char *argv[])
{
  po::variables_map vm = parse_option(argc,argv);
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
              big_time_result[i][j] = 0;
          for (int i = 0; i<lst_files.files.size(); i++)
            {
              std::string const filename = lst_files.files[i];
              LstReader reader(filename);
              reader.decode_counts();
              reader.big_time(channels,tstart,tend,bin_num,output_array);
              for (int j = 0; j<bin_num; j++)
                big_time_result[j][i] = output_array[j];
            }
          save_marray_ull_to_h5(&big_time_result,"big_time_test.h5","bigtime",false);
          delete [] output_array;
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
                  reader.big_time_normalize(*it,tstart,tend,bin_num,normalize_tstart,normalize_tend,output_array);
                  for (int j = 0; j<bin_num; j++)
                    big_time_result[j][i] += output_array[j]/config.bigtime.channels.size();
                }
            }

          save_marray_d_to_h5(&big_time_result,"big_time_test.h5","bigtime",false);
          delete [] output_array;
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
                  reader.phase_hist_normalize(*it,tstart,tend,bin_num,normalize_tstart,normalize_tend,output_array);
                  for (int j = 0; j<bin_num; j++)
                    phase_result[j][i] += output_array[j]/config.bigtime.channels.size();
                }
            }
          save_marray_d_to_h5(&phase_result,"phase_test.h5","phase",false);
          delete [] output_array;
        }
      else
        {
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
                  reader.phase_hist(*it,tstart,tend,bin_num,output_array);
                  for (int j = 0; j<bin_num; j++)
                    phase_result[j][i] += output_array[j]/channels.size();
                }
            }
          save_marray_ull_to_h5(&phase_result,"phase_test.h5","phase",false);
          delete [] output_array;
        }
    }
  return 0;
}
