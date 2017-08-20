import struct
import numpy as np
import os
import sys
from bitarray import bitarray
import scipy.io as sio

class Count(object):
    """
    """
    def __init__(self, d, time_patch, binwidth):
        """
        """
        self.channel = bitarray2int(d[0:3])
        if d[3] == True:
            self.edge = 1
        else:
            self.edge = 0
        if time_patch == "1a":
            self.timedata = bitarray2int(d[4:32])*binwidth
            self.sweeps = bitarray2int(d[32:48])
        elif time_patch == "0":
            self.timedata = bitarray2int(d[4:16])*binwidth
            self.sweeps = 0
        elif time_patch == "5":
            self.timedata = bitarray2int(d[4:24])*binwidth
            self.sweeps = bitarray2int(d[24:32])
        elif time_patch == "5b":
            self.timedata = bitarray2int(d[4:32])*binwidth
            self.sweeps = bitarray2int(d[32:48])
        elif time_patch == "1":
            self.timedata = bitarray2int(d[4:32])*binwidth
            self.sweeps = 0
        elif time_patch == "2":
            self.timedata = bitarray2int(d[4:48])*binwidth
            self.sweeps = 0
        elif time_patch == "32":
            self.timedata = bitarray2int(d[4:40])*binwidth
            self.sweeps = bitarray2int(d[40:47])
        elif time_patch == "f3":
            self.timedata = bitarray2int(d[4:40])*binwidth
            self.sweeps = bitarray2int(d[40:47])
        else:
            raise NameError("unrecognizable time patch")
    def __repr__(self):
        return 'Channel: {:>1} edge: {:>2} timedata: {:>10}ps sweeps: {:>10}'.format(self.channel,self.edge,self.timedata,self.sweeps)

def bitarray2int(arr):
    ll = arr.length()
    if ll <= 16:
        arr.extend((16-ll)*bitarray('0'))
        b = arr.tobytes()
        return struct.unpack('H',b)[0]
    elif ll <= 32:
        arr.extend((32-ll)*bitarray('0'))
        b = arr.tobytes()
        return struct.unpack('I',b)[0]
    elif ll <= 64:
        arr.extend((64-ll)*bitarray('0'))
        b = arr.tobytes()
        return struct.unpack('Q',b)[0]

def check_non_zero(arr,start,num):
    s = False
    for i in range(num):
        s = s or (arr[start+i] != 0)
    return s

def read_lst(filename,verbose=False):
    """
    read_lst(filename) reads a .lst file from the P7888 card
    verbose is bool.
    """
    f = open(filename,'r',encoding="latin-1")
    sweepmode = " "
    if verbose == True:
        print("opened file...")

    last_pos = 0
    while True:
        line = f.readline()
        last_pos = f.tell()
        if line.find("time_patch") != -1:
            time_patch = line[len("time_patch")+1:-1]
            print("time_patch: ", time_patch)
        elif line.find("bitshift") != -1:
            bitshift = line[len("bitshift")+1:-1]
            if verbose == True:
                print("bitshift: ", bitshift)
        elif line.find("swpreset") != -1:
            swpreset = int(line[len("swpreset")+1:-1])
            if verbose == True:
                print("sweep preset: ", swpreset)
        elif line.find("range") != -1:
            bin_range = int(line[len("range")+1:-1])
            if verbose == True:
                print("binrange: ", bin_range)
        elif line.find("cycles") != -1:
            cycles = int(line[len("cycles")+1:-1])
            if verbose == True:
                print("cycles: ", cycles)
        elif line.find("[DATA]") != -1:
            break
    f.close()
    binwidth = 2**(int(bitshift,16))*200
    print("binwidth: ",binwidth, "ps")
    timedata_limit = binwidth*bin_range
    sz = os.path.getsize(filename)
    f = open(filename, "rb")
    f.seek(last_pos)

    a = bitarray(endian='little')
    a.fromfile(f)
    ll=a.length()
    print(ll/64)
    if time_patch == "0":
        dlen = 2*8
    elif time_patch == "5":
        dlen = 4*8
    elif time_patch == "5b":
        dlen = 8*8
    elif time_patch == "1":
        dlen = 4*8
    elif time_patch == "1a":
        dlen = 6*8
    elif time_patch == "32":
        dlen = 6*8
    elif time_patch == "2":
        dlen = 6*8
    elif time_patch == "f3":
        dlen = 8*8
    else:
        raise NameError("unrecognizable time patch")

    # setup toolbar
    toolbar_width = 50
    toolbar_count = 0
    sys.stdout.write("[%s]" % (" " * toolbar_width))
    sys.stdout.flush()
    sys.stdout.write("\b" * (toolbar_width+1)) # return to start of line, after '['
    data = []
    abandon_count = 0
    abandon_count_ch = 0
    abandon_count_swp = 0
    abandon_count_time = 0
    swcount = np.zeros(swpreset,dtype=np.int)
    for i in range(0,ll,dlen):
        if i >= (toolbar_count/toolbar_width)*ll:
            toolbar_count += 1
            sys.stdout.write("-")
            sys.stdout.flush()
        d = a[i:i+dlen]
        if d.any() == True:
            c = Count(d,time_patch,200)#time unit in ps
            if (c.channel in range(1,7)) and c.sweeps <= swpreset*cycles and c.timedata <= timedata_limit:
                if verbose == True:
                    print(c)
                data.append(c)
                swcount[c.sweeps-1] += 1
            else:
                abandon_count += 1
                if not((c.channel in range(1,7))):
                    abandon_count_ch += 1
                if not(c.sweeps <= swpreset*cycles):
                    abandon_count_swp += 1
                if not(c.timedata <= timedata_limit):
                    abandon_count_time += 1
    sys.stdout.write("\n")
    print("counts: ",len(data))
    print("sweep histogram: ",swcount)
    print("abandoned: ", abandon_count)
    print("abandoned (channel out of range): ", abandon_count_ch)
    print("abandoned (sweep count out of range): ", abandon_count_swp)
    print("abandoned (timedata out of range): ", abandon_count_time)
    return data
    # for c in data:
    #     print(c)

def counts2mat(counts,filename):
    #channel,edge,timedata,sweeps
    data = np.zeros((4,len(counts)),dtype=int)
    for i in range(len(counts)):
        data[0,i] = counts[i].channel
        data[1,i] = counts[i].edge
        data[2,i] = counts[i].timedata
        data[3,i] = counts[i].sweeps
    sio.savemat(filename, {'data':data})

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("read_stt.py LST_FILE MAT_FILE")
    else:
        counts = read_lst(sys.argv[1],True)
        counts2mat(counts,sys.argv[2])
