import os
import sys
import glob
import shlex
import subprocess

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("convert_h5.py IN_FILES")
    else:
        path = sys.argv[1]
        for fname in glob.glob(path):
            h5_fname = fname[:-3]+"h5"
            if not(os.path.isfile(h5_fname)):
                print("converting "+fname+" to "+h5_fname)
                subprocess.call(["./read_lst",fname,h5_fname])
            else:
                print(h5_fname+" exist.")
