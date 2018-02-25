0;95;0c# Installation
## Install on OSX
1. brew reinstall --with-toolchain llvm
2. brew install boost hdf5 python3
3. cd ./src
4. make read_lst or make remove_zero

# Usage
## Example
cd ./build
python3 convert_h5.py "/Users/lucaspeng/research/apd_data/2017-11-08/*.lst"
