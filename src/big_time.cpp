#include "LstReader.hpp"
void LstReader::big_time(unsigned int const channel,\
                         unsigned long long const tstart,   \
                         unsigned long long const tend,     \
                         unsigned int const bin_num,        \
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

void LstReader::big_time(unsigned int const channel,\
                         unsigned int const sweep,\
                         unsigned long long const tstart,   \
                         unsigned long long const tend,     \
                         unsigned int const bin_num,        \
                         unsigned long* const output_buffer) const
{
  std::vector<Count> counts_ch(counts.size());
  select_channel(counts_ch, counts, channel);
  std::vector<Count> counts_temp(counts_ch.size());
  select_sweep(counts_temp, counts_ch, sweep);
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

void LstReader::big_time(std::vector<int> const channels,   \
                         unsigned long long const tstart,   \
                         unsigned long long const tend,     \
                         unsigned int const bin_num,        \
                         unsigned long* const output_buffer) const
{
  //Can be optimize
  for (unsigned int i = 0; i < bin_num; ++i)
    output_buffer[i] = 0ULL;
  unsigned long long const interval = tend-tstart;
  unsigned long long const bin_size = interval/bin_num;
  for (auto it=channels.begin();it!=channels.end();it++)
    {
      int channel = *it;
      std::vector<Count> counts_temp(counts.size());
      select_channel(counts_temp, counts, channel);
      std::sort(counts_temp.begin(), counts_temp.end(), compare_timedata());
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
}

void LstReader::big_time_normalize(unsigned int const channel,  \
                          unsigned long long const tstart,  \
                          unsigned long long const tend,    \
                          unsigned int const bin_num,       \
                          unsigned long long const normalize_tstart,  \
                          unsigned long long const normalize_tend,    \
                          double* const output_buffer) const
  {
    //normalize each cycle individually
    //then average by sw_preset
    for (unsigned int i = 0; i < bin_num; ++i)
      output_buffer[i] = 0.0;
    unsigned long* const output_array_raw = new unsigned long[bin_num];
    unsigned long long const interval = tend-tstart;
    unsigned long long const bin_size = interval/bin_num;
    for (unsigned int sw = 1; sw <= sw_preset; ++sw)
      {
        std::vector<Count> counts_ch(counts.size());
        select_channel(counts_ch, counts, channel);
        std::vector<Count> counts_ch_sw(counts_ch.size());
        select_sweep(counts_ch_sw, counts_ch, sw);
        std::vector<Count> counts_ch_sw_time(counts_ch_sw.size());
        select_timedata(counts_ch_sw_time, counts_ch_sw, normalize_tstart,normalize_tend);
        unsigned long long const norm_interval = normalize_tend - normalize_tstart;
        double const norm_count_rate = ((double)counts_ch_sw_time.size())/((double) norm_interval);
        double const norm_count = bin_size*norm_count_rate;
        assert(norm_count != 0);
        big_time(channel, sw, tstart, tend, bin_num, output_array_raw);
        for (unsigned int i = 0; i < bin_num; ++i)
          output_buffer[i] += ((double)output_array_raw[i])/(norm_count*sw_preset);
      }
    // for (unsigned int i = 0; i < bin_num; ++i)
    //   output_buffer[i]/=sw_preset;
    delete [] output_array_raw;
  }
