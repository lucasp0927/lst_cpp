#include "LstReader.hpp"
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
    throw std::runtime_error("Unkown time_patch.");
  return dlen;
}

std::ifstream& GotoLine(std::ifstream& file, unsigned int num){
  file.seekg(std::ios::beg);
  for(unsigned int i=0; i < num - 1; ++i){
    file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
  }
  return file;
}

LstReader::LstReader(std::string filename):filename(filename)
{
  sw_preset = 0;
  buffer = nullptr;
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
      buffer_sw_preset = std::stoi(line.substr(9,5));
      //range
      GotoLine(file,2);
      getline(file,line);
      assert(line.find("range")==0);
      auto range_l = line.length()-6;
      range = std::stoi(line.substr(6,range_l));
      //cycles
      GotoLine(file,11);
      getline(file,line);
      assert(line.find("cycles")==0);
      auto cycles_l = line.length()-7;
      cycles = std::stoi(line.substr(7,cycles_l));
    }
  file.close();
  bin_width = (1UL<<bit_shift)*RESOLUTION;
  timedata_limit = (unsigned long long)bin_width*(unsigned long long)range;
  prepare();
  read_file();
  iterate_data();
  print_header();
}

LstReader::~LstReader()
{
  if (buffer != nullptr)
    delete[] buffer;
}

void LstReader::prepare()
{
  dlen = time_patch_dlen(time_patch);
}

void LstReader::read_file()
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
  unsigned long long length = (unsigned long long) fileb.tellg();
  fileb.seekg(pos);

  length = length-pos; //length of data in byte
  std::cout << "data size: " << length << " Bytes." << std::endl;
  assert(length%dlen == 0);
  buffer_total_data_count = (unsigned long long)length/dlen;
  assert(buffer == nullptr);
  buffer = new char[length];
  fileb.read(buffer,length);
  fileb.close();
}

void LstReader::print_header() const
{
  std::cout << "------------------------------------------" << std::endl;
  std::cout << "filename: " << filename << std::endl;
  std::cout << "time_patch: " << time_patch << std::endl;
  std::cout << "bitshift: "<< bit_shift << std::endl;
  std::cout << "sw_preset: "<< buffer_sw_preset << std::endl;
  std::cout << "range: "<< range << std::endl;
  std::cout << "cycles: "<< cycles << std::endl;
  std::cout << "bin width: " << bin_width << "ps" << std::endl;
  std::cout << "timedata limit: " << timedata_limit/(1E9) << "ms" << std::endl;
  std::cout <<"total data counts: " << buffer_total_data_count << std::endl;
  std::cout<<"non-zero count: " << buffer_nonzero_data_count << std::endl;
  std::cout << "------------------------------------------" << std::endl;
}

void LstReader::iterate_data()
{
  buffer_nonzero_data_count = 0;
  for (unsigned long long i=0; i < buffer_total_data_count; ++i)
    {
      char test = 0x00;
      for (unsigned long long j = i*dlen; j < (i+1)*dlen; ++j)
        {
          test |= buffer[j];
        }
      if (test != 0x00)
        buffer_nonzero_data_count++;
    }
}

void LstReader::save_non_zero_counts(const std::string& out_filename) const
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
  ofile.close();
  ofile.open(out_filename, std::ios::binary | std::ios::app);
  //write nonzero data
  char* out_buffer = new char[buffer_nonzero_data_count*dlen];
  unsigned long long out_idx = 0;
  for (unsigned long long i=0; i < buffer_total_data_count; ++i)
    {
      char test = 0x00;
      for (unsigned long long j = i*dlen; j < (i+1)*dlen; ++j)
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
  ofile.write(out_buffer,buffer_nonzero_data_count*dlen);
  delete [] out_buffer;
  ofile.close();
}

void LstReader::decode_counts()
{
  std::cout << "start decoding..." << std::endl;
  //counts.clear();
  //assert(counts.size() == 0);
  std::vector<Count> temp(buffer_nonzero_data_count);
  //counts.resize(buffer_nonzero_data_count);
  int data_counter = 0;
  for (unsigned long long i=0; i < buffer_total_data_count; ++i)
    {
      char test = 0x00;
      for (unsigned long long j = i*dlen; j < (i+1)*dlen; ++j)
        {
          test |= buffer[j];
        }
      if (test != 0x00)
        {
          Count c(buffer+i*dlen,time_patch);
          temp[data_counter] = c;
          data_counter++;
          //temp.push_back(c);
        }
    }
  delete [] buffer;
  buffer = nullptr;

  std::cout << "remove out of range data" << std::endl;
  std::vector<Count> select_sw;
  select_sweep(select_sw, temp, 1, buffer_sw_preset);
  std::vector<Count> select_sw_ch;
  select_channel(select_sw_ch, select_sw, 1, 6);
  std::vector<Count> counts_buffer;
  select_timedata(counts_buffer, select_sw_ch, 0ULL, timedata_limit);
  //combine
  unsigned int sw_preset_ = sw_preset;
  std::for_each(counts_buffer.begin(),counts_buffer.end(),[sw_preset_](Count &c){c.increase_sweep(sw_preset_);});
  sw_preset += buffer_sw_preset;
  counts.insert( counts.end(), counts_buffer.begin(), counts_buffer.end());
  //std::cout<< "counts size: "<< counts.size() << std::endl;
  std::cout << "done decoding." << std::endl;
}

void LstReader::read_additional_file(const std::string filename_)
{
  filename = filename_;
  assert(buffer==nullptr);
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
      assert(time_patch == line.substr(11,2));
      //bit shift
      GotoLine(file,35);
      getline(file,line);
      assert(line.find("bitshift")==0);
      assert(bit_shift == std::stoi("0x"+line.substr(9,1),0,16));
      //swpreset
      //GotoLine(file,6);
      //getline(file,line);
      //assert(line.find("swpreset")==0);
      //assert(sw_preset == std::stoi(line.substr(9,5)));
      //range
      GotoLine(file,2);
      getline(file,line);
      assert(line.find("range")==0);
      auto range_l = line.length()-6;
      assert(range == std::stoi(line.substr(6,range_l)));
      //cycles
      GotoLine(file,11);
      getline(file,line);
      assert(line.find("cycles")==0);
      auto cycles_l = line.length()-7;
      assert(cycles == std::stoi(line.substr(7,cycles_l)));
    }
  file.close();
  read_file();
  iterate_data();
  print_header();
  decode_counts();
}

void LstReader::save_counts_to_h5(std::string const filename, std::string const datasetname, bool const append)
{
  //typedef boost::multi_array<unsigned long long, 2> array_type;
  //typedef array_type::index index;
  std::cout << "counts size: " << counts.size() << std::endl;
  boost::multi_array<unsigned long long, 2> counts_arr(boost::extents[3][counts.size()]);
  for (unsigned int i = 0; i < counts.size(); ++i)
    {
      counts_arr[0][i] = (unsigned long long) (counts[i].get_sweep());
      counts_arr[1][i] = (unsigned long long) (counts[i].get_channel());
      counts_arr[2][i] = counts[i].get_timedata();
    }
  save_marray_ull_to_h5(&counts_arr, filename, datasetname, append);
}

void LstReader::save_marray_ull_to_h5(boost::multi_array<unsigned long long,2> const* const data, \
                         std::string const filename,\
                         std::string const datasetname,\
                         bool const append)
{
  const H5std_string FILE_NAME(filename);
  const H5std_string DATASET_NAME(datasetname);
  try{
    std::cout << "writing file: " << filename << std::endl;
    std::cout << "        dataset: /" << datasetname << std::endl;
    Exception::dontPrint();
    int const rank = data->num_dimensions();
    auto const* const shape = data->shape();
    H5File* file = nullptr;
    if (append)
      file = new H5File( FILE_NAME, H5F_ACC_RDWR );
    else
      file = new H5File( FILE_NAME, H5F_ACC_TRUNC );
    /*
     * Create property list for a dataset and set up fill values.
     */
    unsigned long long fillvalue = 0ULL;   /* Fill value for the dataset */
    DSetCreatPropList plist;
    plist.setFillValue(PredType::NATIVE_ULLONG, &fillvalue);
    /*
     * Create dataspace for the dataset in the file.
     */
    hsize_t* const fdim = new hsize_t[rank];
    for (int i = 0; i < rank; ++i)
      fdim[i] = (hsize_t) shape[i];
    DataSpace fspace( rank, fdim );
    /*
     * Create dataset and write it into the file.
     */
    DataSet* dataset = new DataSet(file->createDataSet(DATASET_NAME,\
                                                       PredType::NATIVE_ULLONG, fspace, plist));

    /*
     * Create dataspace for the first dataset.
     */
    DataSpace mspace1( rank, fdim );
    dataset->write( data->data(), PredType::NATIVE_ULLONG, mspace1, fspace );
    delete dataset;
    delete file;
  }
  catch( FileIException error )
    {
      error.printError();
      exit(EXIT_FAILURE);
    }
  catch( DataSetIException error )
    {
      error.printError();
      exit(EXIT_FAILURE);
    }
  catch( DataSpaceIException error )
    {
      error.printError();
      exit(EXIT_FAILURE);
    }
}

void LstReader::print_stat()
{
  for (unsigned int ch = 1; ch <= 6; ++ch)
    {
      std::vector<Count> select_ch;
      select_channel(select_ch,counts,ch);
      std::cout << "Total counts in channel " << ch << ": " << select_ch.size() << std::endl;
      for (unsigned int sw = 1; sw <= sw_preset; ++sw)
        {
          std::vector<Count> select_sw;
          select_sweep(select_sw,select_ch,sw);
          std::cout << "   channel: " << ch << ", sweep: "<< sw <<", counts: " << select_sw.size() << std::endl;
          select_sw.clear();
        }
      select_ch.clear();
    }
}
