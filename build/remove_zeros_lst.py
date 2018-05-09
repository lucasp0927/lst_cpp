#!/usr/bin/python3
import os
import sys
import glob
import subprocess

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("remove_zeros_lst.py IN_FILE_PREFIX")
    else:
        path = sys.argv[1]
        for fname in glob.glob(path):
            if not("nozero" in fname):
                no_zeros_fname = fname[0:-4]+"_nozero.lst"
                mpa_fname = fname[0:-4]+".mpa"
                print("\nconverting ",fname," to ",no_zeros_fname)
                subprocess.run(['remove_zero',fname,no_zeros_fname])                
                os.remove(fname)
                
                try:
                    os.remove(mpa_fname)
                except:
                    print("no mpa file.")

