#include "LstReader.hpp"
void LstReader::g2_photon_statistic(std::vector<int> const& channels,    \
                                    unsigned long long const tstart,    \
                                    unsigned long long const tend,      \
                                    unsigned long const pulse_tstart,   \
                                    unsigned long const pulse_tend,     \
                                    std::vector<long>& result,          \
                                    unsigned int const clock_ch,        \
                                    long const channel_delay,           \
                                    int const thread_num)const
{
  assert(tend > tstart);
  assert(pulse_tend > pulse_tstart);
  assert(channels.size() == 2);
  result.clear();
  std::vector<std::vector<Count>> clock(sw_preset);
  std::vector<std::vector<Count>> data_ch1(sw_preset);//channel channels[0]
  std::vector<std::vector<Count>> data_ch2(sw_preset);//channel channels[1]
  // unsigned long pt = (unsigned long) ceil(pulse_tstart/RESOLUTION)*RESOLUTION;
  std::vector<Count> select_td;
  select_timedata(select_td,counts,tstart,tend);
  std::vector<Count> clock_tot;
  select_channel(clock_tot,select_td, clock_ch);
  std::vector<Count> data_ch1_tot;
  select_channel(data_ch1_tot,select_td, channels[0]);
  std::vector<Count> data_ch2_tot;
  select_channel(data_ch2_tot,select_td, channels[1]);

#pragma omp parallel sections
  {
#pragma omp section
    {
      sort_by_sweep(clock_tot);
      auto clock_tot_it = clock_tot.begin();
      for (unsigned int sweep = 1; sweep <= sw_preset; ++sweep)
        {
          while (clock_tot_it->get_sweep()==sweep)
            {
              clock[sweep-1].push_back(*clock_tot_it);
              if (clock_tot_it == clock_tot.end())
                break;
              clock_tot_it++;
            }
        }
    }


#pragma omp section
    {
      sort_by_sweep(data_ch1_tot);
      auto data_ch1_tot_it = data_ch1_tot.begin();
      for (unsigned int sweep = 1; sweep <= sw_preset; ++sweep)
        {
          while (data_ch1_tot_it->get_sweep()==sweep)
            {
              data_ch1[sweep-1].push_back(*data_ch1_tot_it);
              if (data_ch1_tot_it == data_ch1_tot.end())
                break;
              data_ch1_tot_it++;
            }
        }
    }

#pragma omp section
    {
      sort_by_sweep(data_ch2_tot);
      auto data_ch2_tot_it = data_ch2_tot.begin();
      for (unsigned int sweep = 1; sweep <= sw_preset; ++sweep)
        {
          while (data_ch2_tot_it->get_sweep()==sweep)
            {
              data_ch2[sweep-1].push_back(*data_ch2_tot_it);
              if (data_ch2_tot_it == data_ch2_tot.end())
                break;
              data_ch2_tot_it++;
            }
        }
    }
  }

  #pragma omp parallel for
  for (unsigned int sweep = 1; sweep <= sw_preset; ++sweep)
    {
      sort_by_time(data_ch1[sweep-1]);
      sort_by_time(data_ch2[sweep-1]);
      sort_by_time(clock[sweep-1]);
    }
  //calculating timediff
}
