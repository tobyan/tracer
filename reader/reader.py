#!/usr/bin/env python2

import distorm3
import Trace_pb2
import struct

def main():
  inf = open("trace", "r")
  buf = inf.read()
  #first, read the register map 
  cur = 0
  regmap_size = struct.unpack("@Q", buf[cur:cur+8])[0]
  cur = cur + 8 
  regmap = Trace_pb2.RegisterMap()
  regmap.ParseFromString(buf[cur:cur+regmap_size])
  cur = cur + regmap.ByteSize()

  #now read each trace event 
  while cur < len(buf):
    reclen = struct.unpack("@Q", buf[cur:cur+8])[0]
    te = Trace_pb2.TraceEvent()
    te.ParseFromString(buf[cur:cur+reclen])
    cur = cur + reclen

  return

if __name__ == '__main__':
  main()
