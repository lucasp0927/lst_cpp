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

#ifdef max
#undef max
#endif

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
  const std::string filename;
  std::string time_patch;
  unsigned int bit_shift;
  unsigned int sw_preset;
  unsigned int range;
  unsigned int cycles;
  unsigned int dlen;
  unsigned long bin_width;
  unsigned long long timedata_limit;
  unsigned long long total_data_count;
  unsigned long nonzero_data_count;
  char* buffer;
  std::vector<Count> counts;
public:
  LstReader(std::string filename);
  virtual ~LstReader()
  {
    delete[] buffer;
  };

  void prepare()
  {
    dlen = time_patch_dlen(time_patch);
  }

  void read_file()
  {
    std::ifstream file;
    file.open(filename);
    std::streampos pos=0;
    if (file.is_open())
      {
        GotoLine(file,141);
        pos = file.tellg();
      }
    file.close();
    std::ifstream fileb (filename, std::ios::in | std::ios::binary);
    fileb.seekg (0, fileb.end);
    unsigned long long length = (unsigned long long) fileb.tellg();
    fileb.seekg(pos);

    length = length-pos; //length of data in byte
    assert(length%dlen == 0);
    total_data_count = (unsigned long long)length/dlen;
    assert(buffer == nullptr);
    buffer = new char[length];
    fileb.read(buffer,length);
    fileb.close();
  }

  void print_header() const
  {
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "time_patch: " << time_patch << std::endl;
    std::cout << "bitshift: "<< bit_shift << std::endl;
    std::cout << "sw_preset: "<< sw_preset << std::endl;
    std::cout << "range: "<< range << std::endl;
    std::cout << "cycles: "<< cycles << std::endl;
    std::cout << "bin width: " << bin_width << "ps" << std::endl;
    std::cout << "timedata limit:" << timedata_limit/(1E9) << "ms" << std::endl;
    std::cout <<"total data counts: " << total_data_count << std::endl;
    std::cout<<"non-zero count: " << nonzero_data_count << std::endl;
    std::cout << "------------------------------------------" << std::endl;
  }

  void iterate_data()
  {
    nonzero_data_count = 0;
    for (unsigned long long i=0; i < total_data_count; ++i)
      {
        char test = 0x00;
        for (unsigned long long j = i*dlen; j < (i+1)*dlen; ++j)
          {
            test |= buffer[j];
          }
        if (test != 0x00)
          nonzero_data_count++;
      }
  }

  void save_non_zero_counts(const std::string& out_filename) const
  {
    //copy header
    std::ifstream file;
    std::ofstream ofile;
    file.open(filename);
    ofile.open(out_filename);
    assert(file.is_open());
    assert(ofile.is_open());
    std::string line;
    for (int i = 0; i < 140; ++i)
      {
        getline(file,line);
        ofile << line<<std::endl;
      }
    file.close();
	ofile.close();
	ofile.open(out_filename, std::ios::binary | std::ios::app);
    //write nonzero data
    char* out_buffer = new char[nonzero_data_count*dlen];
    unsigned long long out_idx = 0;
    for (unsigned long long i=0; i < total_data_count; ++i)
      {
        char test = 0x00;
        for (unsigned long long j = i*dlen; j < (i+1)*dlen; ++j)
          {
            test |= buffer[j];
          }
        if (test != 0x00) //nonzero
          {
            //use memcopy>
            for (unsigned int j = 0; j < dlen; j++)
              {
                out_buffer[out_idx+j] = buffer[i*dlen+j];
              }
            out_idx += dlen;
          }
      }
    ofile.write(out_buffer,nonzero_data_count*dlen);
    delete [] out_buffer;
    ofile.close();
  }

  void decode_counts()
  {
    counts.clear();
    std::vector<Count> temp;
    for (unsigned long long i=0; i < total_data_count; ++i)
      {
        char test = 0x00;
        for (unsigned long long j = i*dlen; j < (i+1)*dlen; ++j)
          {
            test |= buffer[j];
          }
        if (test != 0x00)
          {
            Count c(buffer+i*dlen,time_patch);
            temp.push_back(c);
          }
      }
    std::cout << "remove out of range data" << std::endl;
    std::vector<Count> select_sw(temp.size());
    select_sweep(select_sw, temp, 1, sw_preset);
    std::vector<Count> select_sw_ch(select_sw.size());
    select_channel(select_sw_ch, select_sw, 1, 6);
    counts.resize(select_sw_ch.size());
    select_timedata(counts, select_sw_ch, 0ULL, timedata_limit);
  }


  void save_counts_to_h5(std::string const filename, std::string const datasetname, bool const append)
  {
    //typedef boost::multi_array<unsigned long long, 2> array_type;
    //typedef array_type::index index;
    std::cout << "counts size: " << counts.size() << std::endl;
    boost::multi_array<unsigned long long, 2> counts_arr(boost::extents[3][counts.size()]);
    for (unsigned int i = 0; i < counts.size(); ++i)
      {
        counts_arr[0][i] = (unsigned long long) (counts[i].get_sweep());
        counts_arr[1][i] = (unsigned long long) (counts[i].get_channel());
        counts_arr[2][i] = counts[i].get_timedata();
      }
    save_marray_ull_to_h5(&counts_arr, filename, datasetname, append);
  }

  void save_marray_ull_to_h5(boost::multi_array<unsigned long long,2> const* const data,\
                         std::string const filename,\
                         std::string const datasetname,\
                         bool const append)
  {
    const H5std_string FILE_NAME(filename);
    const H5std_string DATASET_NAME(datasetname);
    try{
      std::cout << "writing file: " << filename << std::endl;
      std::cout << "        dataset: /" << datasetname << std::endl;
      Exception::dontPrint();
      int const rank = data->num_dimensions();
      auto const* const shape = data->shape();
      H5File* file = nullptr;
      if (append)
        file = new H5File( FILE_NAME, H5F_ACC_RDWR );
      else
        file = new H5File( FILE_NAME, H5F_ACC_TRUNC );
      /*
       * Create property list for a dataset and set up fill values.
       */
      unsigned long long fillvalue = 0ULL;   /* Fill value for the dataset */
      DSetCreatPropList plist;
      plist.setFillValue(PredType::NATIVE_ULLONG, &fillvalue);
      /*
       * Create dataspace for the dataset in the file.
       */
      hsize_t* const fdim = new hsize_t[rank];
      for (int i = 0; i < rank; ++i)
        fdim[i] = (hsize_t) shape[i];
      DataSpace fspace( rank, fdim );
      /*
       * Create dataset and write it into the file.
       */
      DataSet* dataset = new DataSet(file->createDataSet(DATASET_NAME,\
                                                         PredType::NATIVE_ULLONG, fspace, plist));

      /*
       * Create dataspace for the first dataset.
       */
      DataSpace mspace1( rank, fdim );
      dataset->write( data->data(), PredType::NATIVE_ULLONG, mspace1, fspace );
      delete dataset;
      delete file;
    }
    catch( FileIException error )
      {
        error.printError();
        exit(EXIT_FAILURE);
      }
    catch( DataSetIException error )
      {
        error.printError();
        exit(EXIT_FAILURE);
      }
    catch( DataSpaceIException error )
      {
        error.printError();
        exit(EXIT_FAILURE);
      }
  }

  void print_stat()
  {
    for (unsigned int ch = 1; ch <= 6; ++ch)
      {
        std::vector<Count> select_ch(counts.size());
        select_channel(select_ch,counts,ch);
        std::cout << "Total counts in channel " << ch << ": " << select_ch.size() << std::endl;
        for (unsigned int sw = 1; sw <= sw_preset; ++sw)
          {
            std::vector<Count> select_sw(select_ch.size());
            select_sweep(select_sw,select_ch,sw);
            std::cout << "   channel: " << ch << ", sweep: "<< sw <<", counts: " << select_sw.size() << std::endl;
            select_sw.clear();
          }
        select_ch.clear();
      }
  }

  void big_time(unsigned int const channel,\
                unsigned long long const tstart,\
                unsigned long long const tend,\
                unsigned int const bin_num,\
                unsigned long* const output_buffer) const
  {
    std::vector<Count> counts_temp(counts.size());
    select_channel(counts_temp, counts, channel);
    std::sort(counts_temp.begin(), counts_temp.end(), compare_timedata());

    for (unsigned int i = 0; i < bin_num; ++i)
      output_buffer[i] = 0ULL;
    unsigned long long interval = tend-tstart;
    unsigned long long bin_size = interval/bin_num;
    unsigned int bin_idx = 0;
    for (auto it=counts_temp.begin(); it < counts_temp.end(); it++)
      {
        if (it->get_channel() == channel)
          {
            while (it->get_timedata() > tstart+(bin_idx+1)*bin_size &&\
                   bin_idx < bin_num-1)
              ++bin_idx;

            if (it->get_timedata() >= tstart+bin_idx*bin_size &&\
                it->get_timedata() <  tstart+(bin_idx+1)*bin_size)
              {
                ++output_buffer[bin_idx];
              }
          }
      }
  }

  ///////////////////
  //select functions
  ///////////////////
  void select_sweep(std::vector<Count>& result, std::vector<Count> const& input, unsigned int const sweep) const
  {
    auto it = std::copy_if (input.begin(), input.end(), result.begin(), [sweep](Count c){return c.get_sweep()==sweep;} );
    result.resize(std::distance(result.begin(),it));
  }

  void select_sweep(std::vector<Count>& result, std::vector<Count> const& input, unsigned int const sweep_start, unsigned int const sweep_end) const
  {
    auto it = std::copy_if (input.begin(), input.end(), result.begin(), [sweep_start, sweep_end](Count c){return c.get_sweep() >= sweep_start && c.get_sweep() <= sweep_end;} );
    result.resize(std::distance(result.begin(),it));
  }

  void select_channel(std::vector<Count>& result, std::vector<Count> const& input, unsigned int const channel) const
  {
    auto it = std::copy_if (input.begin(), input.end(), result.begin(), [channel](Count c){return c.get_channel()==channel;} );
    result.resize(std::distance(result.begin(),it));
  }

  void select_channel(std::vector<Count>& result, std::vector<Count> const& input, unsigned int const channel_start, unsigned int const channel_end) const
  {
    auto it = std::copy_if (input.begin(), input.end(), result.begin(), [channel_start, channel_end](Count c){return c.get_channel() >= channel_start && c.get_channel() <= channel_end;} );
    result.resize(std::distance(result.begin(),it));
  }


  void select_timedata(std::vector<Count>& result, std::vector<Count> const& input, unsigned long long const tstart, unsigned long long const tend) const
  {
    auto it = std::copy_if (input.begin(), input.end(), result.begin(), [tstart,tend](Count c){return c.get_timedata()>=tstart && c.get_timedata()<tend;} );
    result.resize(std::distance(result.begin(),it));
  }
  ///////////////////
  //end of select functions
  ///////////////////


  unsigned long long calculate_lattice_period(std::vector<Count> const& clock) const
  {
    unsigned long long s = 0;
    for (auto it=clock.begin(); it < --clock.end(); it++)
        s += (it+1)->get_timedata() - it->get_timedata();
    s /= clock.size()-1;
    return s;
  }

  unsigned long long calculate_lattice_period_var(std::vector<Count> const& clock) const
  {
    long long p = calculate_lattice_period(clock);
    long long s = 0;
	long long t = 0;
	for (auto it = clock.begin(); it < --clock.end(); it++)
	{
		t = ((long long)((it + 1)->get_timedata() - it->get_timedata()) - p)*((long long)((it + 1)->get_timedata() - it->get_timedata()) - p);
		s += t/(clock.size()-1);
	}
    return s;
  }

  unsigned long period_combined_average(std::vector<unsigned long> const& period_count,\
                                        std::vector<unsigned long> const& periods)const
  {
    unsigned long total_counts = std::accumulate( period_count.begin(), period_count.end(), 0UL);
    unsigned long long total_period = std::inner_product(periods.begin(), periods.end(), period_count.begin(), 0ULL);
    return (unsigned long)(total_period/total_counts);
  }

  unsigned long long period_combined_variance(std::vector<unsigned long> const& period_count,\
                                         std::vector<unsigned long> const& periods,\
                                         std::vector<unsigned long long> const& periods_var)const
  {
    unsigned long avg_period = period_combined_average(period_count, periods);
    unsigned long total_counts = std::accumulate( period_count.begin(), period_count.end(), 0UL);
    std::vector<unsigned long long> temp; //S1^2 - (X1-Xc)^2
    std::transform(periods_var.begin(), periods_var.end(),
                   periods.begin(),
                   std::back_inserter(temp),
                   [avg_period](unsigned long long s, unsigned long x) {return s+\
		                       (unsigned long long)((long long)x-(long long)avg_period)*((long long)x - (long long)avg_period);});
    return std::inner_product(temp.begin(), temp.end(), period_count.begin(), 0ULL)/total_counts;
  }

  void phase_hist(unsigned int const channel,\
	              unsigned long long const tstart,\
	              unsigned long long const tend,\
	              unsigned int const bin_num,\
	              unsigned long* const result,\
	              unsigned int const clock_ch = 3\
                  ) const
  {
    assert(tend > tstart);
    for (unsigned int i = 0; i < bin_num; ++i)
        result[i] = 0ULL;
    std::vector<std::vector<Count>> clock(sw_preset);
    std::vector<std::vector<Count>> data(sw_preset);
    std::vector<unsigned long> periods(sw_preset);
    std::vector<unsigned long> period_count(sw_preset);
    std::vector<unsigned long long> periods_var(sw_preset);
    for (unsigned int sweep = 1; sweep <= sw_preset; ++sweep)
      {
        //select sweep and timedata
        std::vector<Count> select_sw(counts.size());
        select_sweep(select_sw,counts,sweep);
        std::vector<Count> select_sw_td(select_sw.size());
        select_timedata(select_sw_td,select_sw,tstart,tend);
        //get clock_ch
        clock[sweep-1].resize(select_sw_td.size());
        select_channel(clock[sweep-1],select_sw_td, clock_ch);
        period_count[sweep-1] = (unsigned long) clock[sweep-1].size()-1;
        //get data_ch
        data[sweep-1].resize(select_sw_td.size());
        select_channel(data[sweep-1],select_sw_td, channel);
        //calculate average period
        periods[sweep-1] = (unsigned long)calculate_lattice_period(clock[sweep-1]);
        periods_var[sweep-1] = calculate_lattice_period_var(clock[sweep-1]);
		// std::cout << "period: " << periods[sweep - 1] << std::endl;
		// std::cout << "period variance: " << periods_var[sweep - 1] << std::endl;
      }

    unsigned long avg_period = period_combined_average(period_count,periods);
    unsigned long long combined_var = period_combined_variance(period_count, periods, periods_var);
    std::cout << "average period: " << avg_period/1e6 << "us" << std::endl;
    std::cout << "combined standard deviation: " << sqrt(combined_var)/1e6 << "us" << std::endl;

    // calculate phase_hist
    // calculate all delta_t
    std::cout << "calculate dt" << std::endl;
    std::vector<unsigned long> delta_t;
    for (unsigned int sw = 0; sw < sw_preset; ++sw)
      {
        if (data[sw].size() == 0 || clock[sw].size() <2)
          continue; //TODO continue or break?
        auto clock_it = clock[sw].begin();
        while (data[sw][0].get_timedata() > (clock_it+1)->get_timedata() && (clock_it+1)!=clock[sw].end())
          ++clock_it;
        for (auto it=data[sw].begin(); it < data[sw].end(); it++)
          {
            unsigned long long dt = (it->get_timedata()-clock_it->get_timedata());
			if ((clock_it + 1)->get_timedata() - clock_it->get_timedata() == 0ULL)
              dt = 0ULL;
            else
              dt = dt*avg_period/((clock_it+1)->get_timedata()-clock_it->get_timedata());
			if (dt < avg_period)
              delta_t.push_back((unsigned long)dt);
            while ((it+1)->get_timedata() >= (clock_it+1)->get_timedata())
              {
                if(clock_it == clock[sw].end())
                  break;
                ++clock_it;
              }
          }
      }
    //make histogram
    std::cout <<"hist" << std::endl;
    unsigned long time_bin = avg_period/bin_num;
    for (auto it=delta_t.begin(); it < delta_t.end(); it++)
      {
        unsigned int hist_bin = (unsigned int)*it/time_bin;
        assert(hist_bin >=0);
        assert(hist_bin < bin_num);
        result[hist_bin]++;
      }
  }
};

#endif
