#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <cassert>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <bitset>
const unsigned int RESOLUTION = 200;//200 ps
unsigned int time_patch_dlen(const std::string& time_patch)
{
  unsigned int dlen = 0;
  if (time_patch.compare("0")==0)
    dlen = 2;
  else if (time_patch.compare("5")==0)
    dlen = 4;
  else if (time_patch.compare("5b")==0)
    dlen = 8;
  else if (time_patch.compare("1")==0)
    dlen = 4;
  else if (time_patch.compare("1a")==0)
    dlen = 6;
  else if (time_patch.compare("32")==0)
    dlen = 6;
  else if (time_patch.compare("2")==0)
    dlen = 6;
  else if (time_patch.compare("f3")==0)
    dlen = 8;
  else
    std::cout << "wrong time_patch." << std::endl;
  return dlen;
}

class Count
{
private:
  unsigned int edge;
  unsigned int channel;
  unsigned long long timedata;
  unsigned int sweep;
  friend std::ostream& operator<<(std::ostream &strm, const Count &c)
  {
    return strm << std::setw(7) << c.edge \
                << std::setw(7) << c.channel\
                << std::setw(20) << c.timedata\
                << std::setw(7) << c.sweep;
  }
public:
  unsigned int get_channel() const {return channel;}
  unsigned int get_edge() const {return edge;}
  unsigned long long get_timedata() const {return timedata;}
  unsigned int get_sweep() const {return sweep;}
  Count(){};
  Count(char* data, const std::string& time_patch)
  {
    // unsigned int dlen = time_patch_dlen(time_patch);
    // for (unsigned int i = 0; i < dlen; ++i)
    //   {
    //     std::bitset<8> x(*(data+i));
    //     std::cout << std::setw(9) << x;
    //   }
    // std::cout<<std::endl;
    channel = (unsigned char)*data&0x07;
    edge = (unsigned char)(*data&0x08)>>3;
    sweep = 0;
    timedata = 0;
    if (time_patch.compare("f3")==0){
      sweep = (unsigned char)(*(data+5))&0x7F;
      timedata = 0ULL;
      timedata |= (unsigned char) *(data+4);
      timedata <<= 8;
      timedata |= (unsigned char) *(data+3);
      timedata <<= 8;
      timedata |= (unsigned char) *(data+2);
      timedata <<= 8;
      timedata |= (unsigned char) *(data+1);
      timedata <<= 4;
      timedata |= (unsigned char)(*(data)&0xF0)>>4;
      timedata *= RESOLUTION;
    }else if (time_patch.compare("1a")==0){
      sweep = 0;
      sweep |= (unsigned char) *(data+5);
      sweep <<= 8;
      sweep |= (unsigned char) *(data+4);
      timedata = 0ULL;
      timedata |= (unsigned char) *(data+3);
      timedata <<= 8;
      timedata |= (unsigned char) *(data+2);
      timedata <<= 8;
      timedata |= (unsigned char) *(data+1);
      timedata <<= 4;
      timedata |= (unsigned char)(*(data)&0xF0)>>4;
      timedata *= RESOLUTION;
    }else if (time_patch.compare("0")==0){
      sweep = 0;
      timedata = 0ULL;
      timedata |= (unsigned char) *(data+1);
      timedata <<= 4;
      timedata |= (unsigned char)(*(data)&0xF0)>>4;
      timedata *= RESOLUTION;
    }else if (time_patch.compare("5")==0){
      sweep = (unsigned char)(*(data+3));
      timedata = 0ULL;
      timedata |= (unsigned char) *(data+2);
      timedata <<= 8;
      timedata |= (unsigned char) *(data+1);
      timedata <<= 4;
      timedata |= (unsigned char)(*(data)&0xF0)>>4;
      timedata *= RESOLUTION;
    }else if (time_patch.compare("5b")==0){
      sweep = 0;
      sweep |= (unsigned char) *(data+5);
      sweep <<= 8;
      sweep |= (unsigned char) *(data+4);
      timedata = 0ULL;
      timedata |= (unsigned char) *(data+3);
      timedata <<= 8;
      timedata |= (unsigned char) *(data+2);
      timedata <<= 8;
      timedata |= (unsigned char) *(data+1);
      timedata <<= 4;
      timedata |= (unsigned char)(*(data)&0xF0)>>4;
      timedata *= RESOLUTION;
    }else if (time_patch.compare("1")==0){
      sweep = 0;
      timedata = 0ULL;
      timedata |= (unsigned char) *(data+3);
      timedata <<= 8;
      timedata |= (unsigned char) *(data+2);
      timedata <<= 8;
      timedata |= (unsigned char) *(data+1);
      timedata <<= 4;
      timedata |= (unsigned char)(*(data)&0xF0)>>4;
      timedata *= RESOLUTION;
    }else if (time_patch.compare("2")==0){
      sweep = 0;
      timedata = 0ULL;
      timedata |= (unsigned char) *(data+5);
      timedata <<= 8;
      timedata |= (unsigned char) *(data+4);
      timedata <<= 8;
      timedata |= (unsigned char) *(data+3);
      timedata <<= 8;
      timedata |= (unsigned char) *(data+2);
      timedata <<= 8;
      timedata |= (unsigned char) *(data+1);
      timedata <<= 4;
      timedata |= (unsigned char)(*(data)&0xF0)>>4;
      timedata *= RESOLUTION;
    }else if (time_patch.compare("32")==0){
      sweep = (unsigned char)(*(data+5))&0x7F;
      timedata = 0ULL;
      timedata |= (unsigned char) *(data+4);
      timedata <<= 8;
      timedata |= (unsigned char) *(data+3);
      timedata <<= 8;
      timedata |= (unsigned char) *(data+2);
      timedata <<= 8;
      timedata |= (unsigned char) *(data+1);
      timedata <<= 4;
      timedata |= (unsigned char)(*(data)&0xF0)>>4;
      timedata *= RESOLUTION;
    }
  }
  virtual ~Count()=default;
};

//const unsigned int BUFFER_SIZE = 4800000;
std::ifstream& GotoLine(std::ifstream& file, unsigned int num){
  file.seekg(std::ios::beg);
  for(unsigned int i=0; i < num - 1; ++i){
    file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
  }
  return file;
}
struct compare_timedata
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
  unsigned long total_data_count;
  unsigned long nonzero_data_count;
  char* buffer;
  std::vector<Count> counts;
public:
  LstReader(std::string filename):filename(filename)
  {
    buffer = nullptr;
    //std::cout << "reading file: "<< filename << std::endl;
    std::ifstream file;
    file.open(filename);
    assert(file.is_open());
    if (file.is_open())
      {
        std::string line;
        //time_patch
        GotoLine(file,132);
        getline(file,line);
        assert(line.find("time_patch")==0);
        time_patch = line.substr(11,2);
        //bit shift
        GotoLine(file,35);
        getline(file,line);
        assert(line.find("bitshift")==0);
        bit_shift = std::stoi("0x"+line.substr(9,1),0,16);
        //swpreset
        GotoLine(file,6);
        getline(file,line);
        assert(line.find("swpreset")==0);
        sw_preset = std::stoi(line.substr(9,5));
        //range
        GotoLine(file,2);
        getline(file,line);
        assert(line.find("range")==0);
        int range_l = line.length()-6;
        range = std::stoi(line.substr(6,range_l));
        //cycles
        GotoLine(file,11);
        getline(file,line);
        assert(line.find("cycles")==0);
        int cycles_l = line.length()-7;
        cycles = std::stoi(line.substr(7,cycles_l));
      }
    file.close();
    bin_width = pow(2,bit_shift)*RESOLUTION;
    timedata_limit = bin_width*range;
    prepare();
    read_file();
    iterate_data();
    print_header();
  }
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
    int length = fileb.tellg();
    fileb.seekg(pos);

    length = length-pos; //length of data in byte
    assert(length%dlen == 0);
    total_data_count = length/dlen;
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
    std::cout << "timedata limit:" << timedata_limit/(1e9) << "ms" << std::endl;
    std::cout <<"total data counts: " << total_data_count << std::endl;
    std::cout<<"non-zero count: " << nonzero_data_count << std::endl;
    std::cout << "------------------------------------------" << std::endl;
  }

  void iterate_data()
  {
    nonzero_data_count = 0;
    for (unsigned long i=0; i < total_data_count; ++i)
      {
        char test = 0x00;
        for (unsigned long j = i*dlen; j < (i+1)*dlen; ++j)
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
    //write nonzero data
    char* out_buffer = new char[nonzero_data_count*dlen];
    unsigned long out_idx = 0;
    for (unsigned long i=0; i < total_data_count; ++i)
      {
        char test = 0x00;
        for (unsigned long j = i*dlen; j < (i+1)*dlen; ++j)
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
    for (unsigned long i=0; i < total_data_count; ++i)
      {
        char test = 0x00;
        for (unsigned long j = i*dlen; j < (i+1)*dlen; ++j)
          {
            test |= buffer[j];
          }
        if (test != 0x00)
          {
            Count c(buffer+i*dlen,time_patch);
            //std::cout << c << std::endl;
            counts.push_back(c);
          }
      }
  }

  void big_time(unsigned int const channel,\
                unsigned long long const tstart,\
                unsigned long long const tend,\
                unsigned int const bin_num,\
                unsigned long* const output_buffer) const
  {
    auto counts_temp = counts;
    std::sort(counts_temp.begin(), counts_temp.end(), compare_timedata());

    for (unsigned int i = 0; i < bin_num; ++i)
      output_buffer[i] = 0;
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

  void select_sweep(std::vector<Count>& result, std::vector<Count> const& input, unsigned int const sweep) const
  {
    auto it = std::copy_if (input.begin(), input.end(), result.begin(), [sweep](Count c){return c.get_sweep()==sweep;} );
    result.resize(std::distance(result.begin(),it));
  }

  void select_channel(std::vector<Count>& result, std::vector<Count> const& input, unsigned int const channel) const
  {
    auto it = std::copy_if (input.begin(), input.end(), result.begin(), [channel](Count c){return c.get_channel()==channel;} );
    result.resize(std::distance(result.begin(),it));
  }

  void select_timedata(std::vector<Count>& result, std::vector<Count> const& input, unsigned long long const tstart, unsigned long long const tend) const
  {
    auto it = std::copy_if (input.begin(), input.end(), result.begin(), [tstart,tend](Count c){return c.get_timedata()>=tstart && c.get_timedata()<tend;} );
    result.resize(std::distance(result.begin(),it));
  }

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
    unsigned long long s = 0;
    for (auto it=clock.begin(); it < --clock.end(); it++)
      s += pow((long long)(it+1)->get_timedata() - (long long)it->get_timedata() - p,2);
    s /= clock.size()-1;
    return s;
  }

  void phase_hist(unsigned int const channel, unsigned long long const tstart, unsigned long long const tend ,unsigned int const clock_ch = 3) const
  {

    std::vector<unsigned long> periods(sw_preset);
    std::vector<unsigned long> periods_var(sw_preset);
    for (unsigned int sweep = 1; sweep <= sw_preset; ++sweep)
      {
        //select sweep and timedata
        std::vector<Count> select_sw(counts.size());
        select_sweep(select_sw,counts,sweep);
        std::vector<Count> select_sw_td(select_sw.size());
        select_timedata(select_sw_td,select_sw,tstart,tend);

        //get clock_ch
        std::vector<Count> select_sw_td_clock(select_sw_td.size());
        select_channel(select_sw_td_clock,select_sw_td, clock_ch);
        std::cout <<"sweep "<<sweep<<", clock : " \
                  << select_sw_td_clock.size() << std::endl;
        //get data_ch
        std::vector<Count> select_sw_td_data(select_sw_td.size());
        select_channel(select_sw_td_data,select_sw_td, channel);
        std::cout <<"sweep "<<sweep<<", channel "<< channel<<" : "\
                  << select_sw_td_data.size() << std::endl;
        //calculate average period
        periods[sweep-1] = calculate_lattice_period(select_sw_td_clock);
        std::cout <<"average period: " << periods[sweep-1]/1e6 <<"us"<<std::endl;
        periods_var[sweep-1] = calculate_lattice_period_var(select_sw_td_clock);
        std::cout <<"average period standard deviation: " << sqrt(periods_var[sweep-1])/1e6 <<"us"<<std::endl;

      }


    // std::cout << "selected contains:";
    // for (Count& x: selected) std::cout << ' ' << x<<std::endl;
    // std::cout << '\n';
  }
};
