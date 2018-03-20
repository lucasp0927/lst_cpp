#include "LstReader.hpp"
void LstReader::pulse_hist(unsigned int const channel, \
                           unsigned long long const tstart,\
                           unsigned long long const tend,\
                           unsigned long const pulse_tstart,\
                           unsigned long const pulse_tend,\
                           std::vector<unsigned long>& result_timestamp, \
                           std::vector<unsigned long>& result_count, \
                           unsigned int const clock_ch,\
			   long const clock_delay,\
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

  sort_by_sweep(clock_tot);
  sort_by_sweep(data_tot);
  // omp_set_num_threads(thread_num);
  // #pragma omp parallel for
  auto clock_tot_it = clock_tot.begin();
  auto data_tot_it = data_tot.begin();
  for (unsigned int sweep = 1; sweep <= sw_preset; ++sweep)
    {
      while (clock_tot_it->get_sweep()==sweep)
      {
        clock[sweep-1].push_back(*clock_tot_it);
	if (clock_tot_it == clock_tot.end())
	  break;
        clock_tot_it++;
      }

      while (data_tot_it->get_sweep()==sweep)
      {
        data[sweep-1].push_back(*data_tot_it);
	if (data_tot_it == data_tot.end())
	  break;
        data_tot_it++;
      }
    }

  std::vector<std::vector<unsigned long>> delta_t(sw_preset);
  #pragma omp parallel for
  for (unsigned int sw = 0; sw < sw_preset; ++sw)
    {
      if (data[sw].size() == 0 || clock[sw].size() <2)
        continue; //TODO continue or break?
      //auto clock_it = clock[sw].begin();
      auto data_it = data[sw].begin();
      for (auto clock_it = clock[sw].begin(); clock_it < clock[sw].end()-1; clock_it++)
	{
	  while (data_it->get_timedata() > (clock_it->get_timedata()+clock_delay) && data_it->get_timedata() < ((clock_it+1)->get_timedata()+clock_delay))
	    {
	      unsigned long long dt = (data_it->get_timedata()-(clock_it->get_timedata()+clock_delay));
	      if (dt >= pulse_tstart && dt < pulse_tend)
		{
		  assert((dt-pt)%RESOLUTION==0);
		  assert((dt-pt)/RESOLUTION<result_timestamp.size());
		  result_count[(dt-pt)/RESOLUTION]++;
		}
	      if (data_it == data[sw].end())
		{
		  break;
		}
	      else
		data_it++;
	    }
	  if (data_it == data[sw].end())
	    {
	      break;
	    }
	}
      // while (data[sw][0].get_timedata() > (clock_it+1)->get_timedata()+clock_delay && (clock_it+1)!=clock[sw].end())
      //   ++clock_it;
      // for (auto it=data[sw].begin(); it < data[sw].end()-1; it++) //go over all data, except for the last one.
      //   {
      //     unsigned long long dt = (it->get_timedata()-(clock_it->get_timedata()+clock_delay));
      //     if (dt >= pulse_tstart && dt < pulse_tend)
      //       {
      //         assert((dt-pt)%RESOLUTION==0);
      //         assert((dt-pt)/RESOLUTION<result_timestamp.size());
      //         result_count[(dt-pt)/RESOLUTION]++;
      //       }
      //     while ((it+1)->get_timedata() >= (clock_it+1)->get_timedata()+clock_delay)
      //       {
      //         ++clock_it;
      //         if(clock_it+1 == clock[sw].end())
      // 		{
      // 		  //process all counts 
      // 		  break;
      // 		}
      //       }
      //     if(clock_it+1 == clock[sw].end())
      //       break;
      //   }
      //process the last count
      
    }
}
