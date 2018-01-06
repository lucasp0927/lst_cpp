# Installation
## Install on OSX
1. brew reinstall --with-toolchain llvm
2. brew tap homebrew/science
3. brew install boost hdf5 h5utils python3
4. cd ./src
5. make

# Usage
## Example
cd ./build
python3 convert_h5.py "/Users/lucaspeng/research/apd_data/2017-11-08/*.lst"
