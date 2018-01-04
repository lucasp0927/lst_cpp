#ifndef COUNT_HPP
#define COUNT_HPP
#include <iostream>
#include <iomanip>

const unsigned int RESOLUTION = 200;//200 ps

class Count
{
private:
  unsigned int edge;
  unsigned int channel;
  unsigned long long timedata;
  unsigned int sweep;
  friend std::ostream& operator<<(std::ostream &strm, const Count &c);
public:
  unsigned int get_channel() const {return channel;}
  unsigned int get_edge() const {return edge;}
  unsigned long long get_timedata() const {return timedata*RESOLUTION;}
  unsigned long long get_timedata_d_res() const {return timedata*RESOLUTION;}
  unsigned int get_sweep() const {return sweep;}
  Count(){};
  Count(char* data, const std::string& time_patch);
  virtual ~Count()=default;
};

#endif
