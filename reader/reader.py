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

  rm = {}
  for u in regmap.register_map:
    rm[u.register_number] = u.register_name

  mr = {}
  for u in regmap.register_map:
    mr[u.register_name] = u.register_number

  #now read each trace event 
  while cur < len(buf):
    reclen = struct.unpack("@Q", buf[cur:cur+8])[0]
    cur = cur + 8
    te = Trace_pb2.TraceEvent()
    te.ParseFromString(buf[cur:cur+reclen])
    cur = cur + reclen
    
    pc = 0
    for i in te.regs:
      if i.register_number == mr["rip"]:
        pc = struct.unpack("@Q", i.register_value)[0]
        break

    print "REGISTERS BEFORE"
    print "==============="
    s = ""
    c = 0
    for i in te.regs:
      if i.register_number != mr["rip"]:
        val = struct.unpack("@Q", i.register_value)[0]
        if c > 80:
          c = 0
          s = s + "\n"
        u = "%s = %x  " % (rm[i.register_number], val)
        s = s + u
        c = c + len(u)
    print s
    
    print "0x%x %s\n\n" % (pc, distorm3.Decode(0, te.instruction, distorm3.Decode64Bits)[0][2])

  return

if __name__ == '__main__':
  main()
