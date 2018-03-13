#include "Count.hpp"
std::ostream& operator<<(std::ostream &strm, const Count &c)
{
  return strm << std::setw(7) << c.edge \
              << std::setw(7) << c.channel\
              << std::setw(20) << c.timedata\
              << std::setw(7) << c.sweep;
}

Count::Count(char* data, const std::string& time_patch)
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
    //    timedata *= RESOLUTION;
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
    //timedata *= RESOLUTION;
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
    //timedata *= RESOLUTION;
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
    //timedata *= RESOLUTION;
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
    //timedata *= RESOLUTION;
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
    //timedata *= RESOLUTION;
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
    //timedata *= RESOLUTION;
  }
}

void Count::increase_sweep(unsigned int inc)
{
  sweep += inc;
}
