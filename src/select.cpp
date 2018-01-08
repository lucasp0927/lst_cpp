#include "LstReader.hpp"
//select functions
void LstReader::select_sweep(std::vector<Count>& result, std::vector<Count> const& input, unsigned int const sweep) const
{
  int count = std::count_if(input.begin(), input.end(),[sweep](Count c){return c.get_sweep()==sweep;});
  assert(count > 0);
  result.resize(count);
  std::copy_if (input.begin(), input.end(), result.begin(), [sweep](Count c){return c.get_sweep()==sweep;} );
  //result.resize(std::distance(result.begin(),it));
  //result.shrink_to_fit();
}

void LstReader::select_sweep(std::vector<Count>& result, std::vector<Count> const& input, unsigned int const sweep_start, unsigned int const sweep_end) const
{
  int count = std::count_if(input.begin(), input.end(),[sweep_start, sweep_end](Count c){return c.get_sweep() >= sweep_start && c.get_sweep() <= sweep_end;});
  assert(count > 0);
  result.resize(count);
  std::copy_if (input.begin(), input.end(), result.begin(), [sweep_start, sweep_end](Count c){return c.get_sweep() >= sweep_start && c.get_sweep() <= sweep_end;} );
  //result.resize(std::distance(result.begin(),it));
  //result.shrink_to_fit();
}

void LstReader::select_channel(std::vector<Count>& result, std::vector<Count> const& input, unsigned int const channel) const
{
  int count = std::count_if(input.begin(), input.end(),[channel](Count c){return c.get_channel()==channel;});
  assert(count > 0);
  result.resize(count);
  std::copy_if (input.begin(), input.end(), result.begin(), [channel](Count c){return c.get_channel()==channel;} );
  //result.resize(std::distance(result.begin(),it));
  //result.shrink_to_fit();
}

void LstReader::select_channel(std::vector<Count>& result, std::vector<Count> const& input, unsigned int const channel_start, unsigned int const channel_end) const
{
  int count = std::count_if(input.begin(), input.end(), [channel_start, channel_end](Count c){return c.get_channel() >= channel_start && c.get_channel() <= channel_end;});
  assert(count > 0);
  result.resize(count);
  std::copy_if (input.begin(), input.end(), result.begin(), [channel_start, channel_end](Count c){return c.get_channel() >= channel_start && c.get_channel() <= channel_end;} );
  //result.resize(std::distance(result.begin(),it));
  //result.shrink_to_fit();
}

void LstReader::select_timedata(std::vector<Count>& result, std::vector<Count> const& input, unsigned long long const tstart, unsigned long long const tend) const
{
  int count = std::count_if(input.begin(), input.end(), [tstart,tend](Count c){return c.get_timedata()>=tstart && c.get_timedata()<tend;});
  assert(count > 0);
  result.resize(count);
  std::copy_if (input.begin(), input.end(), result.begin(), [tstart,tend](Count c){return c.get_timedata()>=tstart && c.get_timedata()<tend;} );
  //result.resize(std::distance(result.begin(),it));
  //result.shrink_to_fit();
}
//end of select functions
