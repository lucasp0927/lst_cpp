#include "LstReader.hpp"
void LstReader::pulse_hist(unsigned int const channel, \
                           unsigned long long const tstart,             \
                           unsigned long long const tend,               \
                           unsigned long const pulse_tstart,            \
                           unsigned long const pulse_tend,              \
                           std::vector<unsigned long>& result_timestamp, \
                           std::vector<unsigned long>& result_count, \
                           unsigned int const clock_ch,                 \
                           int const thread_num) const
{
  assert(tend > tstart);
  //result.resize();
  std::vector<std::vector<Count>> clock(sw_preset);
  std::vector<std::vector<Count>> data(sw_preset);
  /*
  std::vector<unsigned long> periods(sw_preset);
  std::vector<unsigned long> period_count(sw_preset);
  std::vector<unsigned long long> periods_var(sw_preset);
  */

  result_timestamp.clear();
  unsigned long pt = (unsigned long) ceil(pulse_tstart/RESOLUTION)*RESOLUTION;
  for (unsigned long t = pt; t <= pulse_tend; t += RESOLUTION)
      result_timestamp.push_back(t);
  result_count.resize(result_timestamp.size());
  for (int i = 0;i < result_timestamp.size();i++)
    result_count[i] = 0;
  std::vector<Count> select_td;
  select_timedata(select_td,counts,tstart,tend);
  std::vector<Count> clock_tot;
  select_channel(clock_tot,select_td, clock_ch);
  std::vector<Count> data_tot;
  select_channel(data_tot,select_td, channel);
  omp_set_num_threads(thread_num);
  #pragma omp parallel for
  for (unsigned int sweep = 1; sweep <= sw_preset; ++sweep)
    {
      //clock[sweep-1].resize(clock_tot.size());
      select_sweep(clock[sweep-1],clock_tot,sweep);
      //data[sweep-1].resize(data_tot.size());
      select_sweep(data[sweep-1],data_tot,sweep);
    }
  std::vector<std::vector<unsigned long>> delta_t(sw_preset);
  #pragma omp parallel for
  for (unsigned int sw = 0; sw < sw_preset; ++sw)
    {
      if (data[sw].size() == 0 || clock[sw].size() <2)
        continue; //TODO continue or break?
      auto clock_it = clock[sw].begin();
      while (data[sw][0].get_timedata() > (clock_it+1)->get_timedata() && (clock_it+1)!=clock[sw].end())
        ++clock_it;
      for (auto it=data[sw].begin(); it < data[sw].end(); it++) //go over all data
        {
          unsigned long long dt = (it->get_timedata()-clock_it->get_timedata());
          if (dt >= pulse_tstart && dt < pulse_tend)
            {
              assert((dt-pt)%RESOLUTION==0);
              assert((dt-pt)/RESOLUTION<result_timestamp.size());
              result_count[(dt-pt)/RESOLUTION]++;
            }
          while ((it+1)->get_timedata() >= (clock_it+1)->get_timedata())
            {
              if(clock_it == clock[sw].end())
                break;
              ++clock_it;
            }
        }
    }
}
