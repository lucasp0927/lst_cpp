#include "fileio.hpp"
std::string extract_file_name(std::string const & fullPath)
{
  const size_t lastSlashIndex = fullPath.find_last_of("/\\");
  return fullPath.substr(lastSlashIndex + 1);
}

std::string extract_file_path(std::string const & fullPath)
{
  const size_t lastSlashIndex = fullPath.find_last_of("/\\");
  return fullPath.substr(0,lastSlashIndex+1);
}

bool check_files_format(std::string const pattern, FILES* const lst_files)
{
  try{
    //std::cout << "Pattern: " << pattern << std::endl;
    glob_t glob_result;
    glob(pattern.c_str(),GLOB_TILDE,NULL,&glob_result);
    //std::cout << glob_result.gl_pathc << " file(s) found." << std::endl;
    std::vector<std::string> files;
    for(unsigned int i=0;i<glob_result.gl_pathc;++i){
      files.push_back(std::string(glob_result.gl_pathv[i]));
    }
    globfree(&glob_result);

    if (files.size() == 0)
      throw std::runtime_error("No file found.");
    //check filename format
    const std::regex filename_re("(\\d{6})(_lst-)(\\d+)(_nozero.lst)");
    for (auto it = files.begin(); it != files.end(); it++)
      {
        std::string filename =  extract_file_name(*it);
        if (not regex_match(filename,filename_re))
          throw std::runtime_error("invalid filename: "+filename);
      }
    std::smatch m;
    std::string filename =  extract_file_name(files[0]);
    const std::string filepath =  extract_file_path(files[0]);
    std::regex_search (filename,m,filename_re);
    const int prefix = stoi(m[1]);
    //std::cout<<"prefix: " << prefix << std::endl;
    int file_num = files.size();
    for (int i = 1; i <= file_num; i++)
      {
        std::string filename_test = std::to_string(prefix)+"_lst-"+std::to_string(i)+"_nozero.lst";
        if (std::find(files.begin(), files.end(), filepath+filename_test) == files.end())
          throw std::runtime_error(filename_test+" missing.");
      }
    //reorder
    for (int i = 1; i <= file_num; i++)
        files[i-1] = filepath+std::to_string(prefix)+"_lst-"+std::to_string(i)+"_nozero.lst";
    lst_files->prefix = prefix;
    lst_files->file_num = file_num;
    lst_files->files = files;
    lst_files->path = filepath;
  }
  catch(const std::exception& e){
    std::cout << e.what() << std::endl;
    return false;
  }
  return true;
}

void save_marray_ull_to_h5(boost::multi_array<unsigned long long,2> const* const data, \
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
