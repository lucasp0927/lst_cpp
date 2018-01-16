#include "LstReader.hpp"
unsigned long long LstReader::calculate_lattice_period(std::vector<Count> const& clock) const
{
  unsigned long long s = 0;
  for (auto it=clock.begin(); it < --clock.end(); it++)
      s += (it+1)->get_timedata() - it->get_timedata();
  s /= clock.size()-1;
  return s;
}

unsigned long long LstReader::calculate_lattice_period_var(std::vector<Count> const& clock) const
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

unsigned long LstReader::period_combined_average(std::vector<unsigned long> const& period_count, \
                                      std::vector<unsigned long> const& periods)const
{
  unsigned long total_counts = std::accumulate( period_count.begin(), period_count.end(), 0UL);
  unsigned long long total_period = std::inner_product(periods.begin(), periods.end(), period_count.begin(), 0ULL);
  return (unsigned long)(total_period/total_counts);
}

unsigned long long LstReader::period_combined_variance(std::vector<unsigned long> const& period_count, \
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

unsigned long LstReader::phase_hist(unsigned int const channel,  \
                                    unsigned long long const tstart,    \
                                    unsigned long long const tend,      \
                                    unsigned int const bin_num,         \
                                    unsigned long* const result,        \
                                    unsigned int const clock_ch         \
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
      std::vector<Count> select_sw;
      select_sweep(select_sw,counts,sweep);
      std::vector<Count> select_sw_td;
      select_timedata(select_sw_td,select_sw,tstart,tend);
      //get clock_ch
      select_channel(clock[sweep-1],select_sw_td, clock_ch);
      period_count[sweep-1] = (unsigned long) clock[sweep-1].size()-1;
      //get data_ch
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
  unsigned long time_bin = avg_period/bin_num;
  for (auto it=delta_t.begin(); it < delta_t.end(); it++)
    {
      unsigned int hist_bin = (unsigned int)*it/time_bin;
      assert(hist_bin >=0);
      assert(hist_bin < bin_num);
      result[hist_bin]++;
    }
  return avg_period;
}

// int omp_thread_count() {
//   int n = 0;
// #pragma omp parallel reduction(+:n)
//   n += 1;
//   return n;
// }

unsigned long LstReader::phase_hist_normalize(unsigned int const channel,  \
                                              unsigned long long const tstart, \
                                              unsigned long long const tend, \
                                              unsigned int const bin_num, \
                                              unsigned long long const normalize_tstart, \
                                              unsigned long long const normalize_tend, \
                                              double* const result,\
                                              unsigned int const clock_ch,\
                                              int const thread_num) const
{
  assert(tend > tstart);
  for (unsigned int i = 0; i < bin_num; ++i)
      result[i] = 0.0;
  std::vector<std::vector<Count>> clock(sw_preset);
  std::vector<std::vector<Count>> data(sw_preset);
  std::vector<unsigned long> periods(sw_preset);
  std::vector<unsigned long> period_count(sw_preset);
  std::vector<unsigned long long> periods_var(sw_preset);

  std::vector<Count> select_td;
  select_timedata(select_td,counts,tstart,tend);
  std::vector<Count> clock_tot;
  select_channel(clock_tot,select_td, clock_ch);
  std::vector<Count> data_tot;
  select_channel(data_tot,select_td, channel);
  omp_set_num_threads(thread_num);
  //std::cout << "using "  <<omp_thread_count() << " threads." << std::endl;
  #pragma omp parallel for
  for (unsigned int sweep = 1; sweep <= sw_preset; ++sweep)
    {
      //get clock_ch
      //clock[sweep-1].resize(clock_tot.size());
      select_sweep(clock[sweep-1],clock_tot,sweep);
      period_count[sweep-1] = (unsigned long) clock[sweep-1].size()-1;
      //get data_ch
      //data[sweep-1].resize(data_tot.size());
      select_sweep(data[sweep-1],data_tot,sweep);
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
  std::vector<std::vector<unsigned long>> delta_t(sw_preset);
  //std::vector<unsigned long> delta_t;
  unsigned long time_bin = avg_period/bin_num;

  std::vector<Count> counts_ch;
  select_channel(counts_ch, counts, channel);
  std::vector<Count> counts_ch_time;
  select_timedata(counts_ch_time, counts_ch, normalize_tstart,normalize_tend);
  #pragma omp parallel for
  for (unsigned int sw = 0; sw < sw_preset; ++sw)
    {
      unsigned long long* const result_raw = new unsigned long long[bin_num];
      for (unsigned int i = 0; i < bin_num; ++i)
          result_raw[i] = 0ULL;
      //calculate normalize count rate
      std::vector<Count> counts_ch_sw_time;
      select_sweep(counts_ch_sw_time, counts_ch_time, sw+1);
      unsigned long long const norm_interval = normalize_tend - normalize_tstart;
      double const norm_count_rate = ((double)counts_ch_sw_time.size())/((double) norm_interval);
      double const norm_count = norm_count_rate*((double)(tend-tstart))/((double)bin_num);
      assert(norm_count != 0);
      //
      if (data[sw].size() == 0 || clock[sw].size() <2)
        continue; //TODO continue or break?
      auto clock_it = clock[sw].begin();
      while (data[sw][0].get_timedata() > (clock_it+1)->get_timedata() && (clock_it+1)!=clock[sw].end())
        ++clock_it;
      for (auto it=data[sw].begin(); it < data[sw].end()-1; it++) //go over all data
        {
          unsigned long long dt = (it->get_timedata()-clock_it->get_timedata());
          if ((clock_it + 1)->get_timedata() - clock_it->get_timedata() == 0ULL)
            dt = 0ULL;
          else
            dt = dt*avg_period/((clock_it+1)->get_timedata()-clock_it->get_timedata());
          if (dt < avg_period)
            delta_t[sw].push_back((unsigned long)dt);
          while ((it+1)->get_timedata() >= (clock_it+1)->get_timedata())
            {
              ++clock_it;
              if((clock_it+1) == clock[sw].end())
                break;
            }
          if ((clock_it+1) == clock[sw].end())
            break;
        }
      //make histogram
      for (auto it=delta_t[sw].begin(); it < delta_t[sw].end(); it++)
        {
          unsigned int hist_bin = (unsigned int)*it/time_bin;
          assert(hist_bin >=0);
          assert(hist_bin < bin_num);
          result_raw[hist_bin]++;
        }
      for (unsigned int i = 0; i < bin_num; ++i)
          result[i] += ((double)result_raw[i])/norm_count/sw_preset;
      delete [] result_raw;
    }
  return avg_period;
}
