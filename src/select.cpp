#include "LstReader.hpp"
//select functions
void LstReader::select_sweep(std::vector<Count>& result, std::vector<Count> const& input, unsigned int const sweep) const
{
  auto it = std::copy_if (input.begin(), input.end(), result.begin(), [sweep](Count c){return c.get_sweep()==sweep;} );
  result.resize(std::distance(result.begin(),it));
}

void LstReader::select_sweep(std::vector<Count>& result, std::vector<Count> const& input, unsigned int const sweep_start, unsigned int const sweep_end) const
{
  auto it = std::copy_if (input.begin(), input.end(), result.begin(), [sweep_start, sweep_end](Count c){return c.get_sweep() >= sweep_start && c.get_sweep() <= sweep_end;} );
  result.resize(std::distance(result.begin(),it));
}

void LstReader::select_channel(std::vector<Count>& result, std::vector<Count> const& input, unsigned int const channel) const
{
  auto it = std::copy_if (input.begin(), input.end(), result.begin(), [channel](Count c){return c.get_channel()==channel;} );
  result.resize(std::distance(result.begin(),it));
}

void LstReader::select_channel(std::vector<Count>& result, std::vector<Count> const& input, unsigned int const channel_start, unsigned int const channel_end) const
{
  auto it = std::copy_if (input.begin(), input.end(), result.begin(), [channel_start, channel_end](Count c){return c.get_channel() >= channel_start && c.get_channel() <= channel_end;} );
  result.resize(std::distance(result.begin(),it));
}

void LstReader::select_timedata(std::vector<Count>& result, std::vector<Count> const& input, unsigned long long const tstart, unsigned long long const tend) const
{
  auto it = std::copy_if (input.begin(), input.end(), result.begin(), [tstart,tend](Count c){return c.get_timedata()>=tstart && c.get_timedata()<tend;} );
  result.resize(std::distance(result.begin(),it));
}
//end of select functions
