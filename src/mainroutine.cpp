#include "mainroutine.hpp"
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

void phase(po::variables_map& vm, int const omp_thread_num, FILES& lst_files, CONFIG& config)
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
      std::string postfix = vm["postfix"].as<std::string>();
      std::string h5_filename = "";
      std::string eps_filename = "";
      if (postfix.empty())
	{
	  h5_filename = lst_files.path+lst_files.prefix+"_phase.h5";
	  eps_filename = lst_files.path+lst_files.prefix+"_phase.eps";
	}
      else
	{
	  h5_filename = lst_files.path+lst_files.prefix+"_phase_"+postfix+".h5";
	  eps_filename = lst_files.path+lst_files.prefix+"_phase_"+postfix+".eps";
	}
      //std::string const h5_filename = lst_files.path+lst_files.prefix+"_phase.h5";
      //std::string const eps_filename = lst_files.path+lst_files.prefix+"_phase.eps";
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
      std::string postfix = vm["postfix"].as<std::string>();
      std::string h5_filename = "";
      std::string eps_filename = "";
      if (postfix.empty())
	{
	  h5_filename = lst_files.path+lst_files.prefix+"_phase.h5";
	  eps_filename = lst_files.path+lst_files.prefix+"_phase.eps";
	}
      else
	{
	  h5_filename = lst_files.path+lst_files.prefix+"_phase_"+postfix+".h5";
	  eps_filename = lst_files.path+lst_files.prefix+"_phase_"+postfix+".eps";
	}
      //std::string const h5_filename = lst_files.path+lst_files.prefix+"_phase.h5";
      //std::string const eps_filename = lst_files.path+lst_files.prefix+"_phase.eps";
      save_marray_ull_to_h5(&phase_result,h5_filename,"phase",false);
      delete [] output_array;
      plot_phase(phase_result, lst_files, config, avg_period, eps_filename);
    }
}

void bigtime(po::variables_map& vm, int const omp_thread_num, FILES& lst_files, CONFIG& config)
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
      std::string postfix = vm["postfix"].as<std::string>();
      std::string h5_filename = "";
      std::string eps_filename = "";
      if (postfix.empty())
	{
	  h5_filename = lst_files.path+lst_files.prefix+"_bigtime.h5";
	  eps_filename = lst_files.path+lst_files.prefix+"_bigtime.eps";
	}
      else
	{
	  h5_filename = lst_files.path+lst_files.prefix+"_bigtime_"+postfix+".h5";
	  eps_filename = lst_files.path+lst_files.prefix+"_bigtime_"+postfix+".eps";
	}
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
      std::string postfix = vm["postfix"].as<std::string>();
      std::string h5_filename = "";
      std::string eps_filename = "";
      if (postfix.empty())
	{
	  h5_filename = lst_files.path+lst_files.prefix+"_bigtime.h5";
	  eps_filename = lst_files.path+lst_files.prefix+"_bigtime.eps";
	}
      else
	{
	  h5_filename = lst_files.path+lst_files.prefix+"_bigtime_"+postfix+".h5";
	  eps_filename = lst_files.path+lst_files.prefix+"_bigtime_"+postfix+".eps";
	}
      //std::string const h5_filename = lst_files.path+lst_files.prefix+"_bigtime.h5";
      //std::string const eps_filename = lst_files.path+lst_files.prefix+"_bigtime.eps";
      save_marray_d_to_h5(&big_time_result,h5_filename,"bigtime",false);
      delete [] output_array;
      plot_bigtime(big_time_result,lst_files,config,eps_filename);
    }  
}

void pulse(po::variables_map& vm, int const omp_thread_num, FILES& lst_files, CONFIG& config)
{
  unsigned int const file_num = lst_files.file_num;
  unsigned long long tstart = (unsigned long long)config.pulse.tstart;
  unsigned long long tend = (unsigned long long)config.pulse.tend;
  unsigned long pulse_tstart = (unsigned long)config.pulse.pulse_tstart;
  unsigned long pulse_tend = (unsigned long)config.pulse.pulse_tend;
  long clock_delay = (long)config.pulse.clock_delay;
  std::vector<int> const channels = config.pulse.channels;

  //std::vector<unsigned long> result_timestamp;
  std::vector<std::vector<unsigned long>> result_timestamp(lst_files.files.size());
  std::vector<std::vector<unsigned long>> result_count(lst_files.files.size());
  //result_count.resize(lst_files.files.size());
  for (int i = 0; i<lst_files.files.size(); i++)
    {
      std::string const filename = lst_files.files[i];
      LstReader reader(filename);
      reader.decode_counts();
      for (auto it=channels.begin();it!=channels.end();it++)
	{
	  reader.pulse_hist(*it, tstart, tend, pulse_tstart, pulse_tend,\
			    result_timestamp[i], result_count[i], 3, clock_delay, omp_thread_num);
	}
    }
  //save file
  typedef boost::multi_array<unsigned long long, 2> array_type;
  int timestamp_size = result_timestamp[0].size();
  array_type pulse_result_timestamp(boost::extents[timestamp_size][file_num]);
  array_type pulse_result_count(boost::extents[timestamp_size][file_num]);
  for (int i = 0; i<file_num; i++)
    {
      for (int j = 0; j<timestamp_size; j++)
	{
	  pulse_result_timestamp[j][i] = result_timestamp[i][j];
	  pulse_result_count[j][i] = result_count[i][j];
	}
    }
  std::string postfix = vm["postfix"].as<std::string>();
  std::string h5_filename = "";
  //std::string eps_filename = "";
  if (postfix.empty())
    h5_filename = lst_files.path+lst_files.prefix+"_pulse.h5";
  else
    h5_filename = lst_files.path+lst_files.prefix+"_pulse_"+postfix+".h5";
  //std::string const h5_filename = lst_files.path+lst_files.prefix+"_pulse.h5";
  save_marray_ull_to_h5(&pulse_result_timestamp,h5_filename,"timestamp",false);
  save_marray_ull_to_h5(&pulse_result_count,h5_filename,"count",true);
}

void pulse(po::variables_map& vm, int const omp_thread_num, COMBINE_FILES& c_lst_files, CONFIG& config)
{
  unsigned int const file_num = c_lst_files.files.size();
  std::string const output_filename = vm["combineoutput"].as<std::string>();
  unsigned long long tstart = (unsigned long long)config.pulse.tstart;
  unsigned long long tend = (unsigned long long)config.pulse.tend;
  unsigned long pulse_tstart = (unsigned long)config.pulse.pulse_tstart;
  unsigned long pulse_tend = (unsigned long)config.pulse.pulse_tend;
  long clock_delay = (long)config.pulse.clock_delay;
  std::vector<int> const channels = config.pulse.channels;
  std::vector<std::vector<unsigned long>> result_timestamp(1);
  std::vector<std::vector<unsigned long>> result_count(1);
  LstReader reader(c_lst_files.files[0]);
  reader.decode_counts(tstart, tend);
  for (int i = 1; i<file_num; i++)
    {
      std::string const filename = c_lst_files.files[i];
      reader.read_additional_file(filename);
      reader.decode_counts(tstart, tend);
    }
  //TODO: Need to add channel delays.
  for (auto it=channels.begin();it!=channels.end();it++)
    {
      reader.pulse_hist(*it, tstart, tend, pulse_tstart, pulse_tend, \
			result_timestamp[0], result_count[0], 3, clock_delay, omp_thread_num);
    }
  //save file
  typedef boost::multi_array<unsigned long long, 2> array_type;
  int timestamp_size = result_timestamp[0].size();
  array_type pulse_result_timestamp(boost::extents[timestamp_size][1]);
  array_type pulse_result_count(boost::extents[timestamp_size][1]);
  for (int j = 0; j<timestamp_size; j++)
    {
      pulse_result_timestamp[j][0] = result_timestamp[0][j];
      pulse_result_count[j][0] = result_count[0][j];
    }
  save_marray_ull_to_h5(&pulse_result_timestamp,output_filename,"timestamp",false);
  save_marray_ull_to_h5(&pulse_result_count,output_filename,"count",true);
}
