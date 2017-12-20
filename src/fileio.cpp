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
