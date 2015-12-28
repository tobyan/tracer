// tracer main

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>

#include "Trace.pb.h"

TraceEvent mktrace( unsigned char           instbuff[16], 
                    uint64_t                tc, 
                    struct user_regs_struct *r,
                    struct user_regs_struct *oldr,
                    bool                    first) 
{
  TraceEvent  t;

  t.set_tc(tc);
  t.set_instruction(&instbuff[0], 16);
  t.set_tid(1);

  if(first) {
    #define REG(n, a, sz) \
      Registers *r##n = t.add_regs(); \
      r##n->set_register_number(n); \
      r##n->set_register_value(& r->a , sizeof(r->a))
#include "regs.inc"
    #undef REG 
  } else {
    #define REG(n, a, sz) \
      if( r->a != oldr->a ) { \
      Registers *r##n = t.add_regs(); \
      r##n->set_register_number(n); \
      r##n->set_register_value(&r->a, sizeof(r->a)); \
      }
#include "regs.inc"
    #undef REG

  }

  return t;
}

int trace(std::string program_path, std::string program_args, std::string output) {
  //write out the output file with an initial register map
  RegisterMap regMap;

  #define REG(n, nm, sz) \
    Register *r##n = regMap.add_register_map();\
    r##n->set_register_number(n);\
    r##n->set_register_name(#nm);\
    r##n->set_register_size(sz);
#include "regs.inc"
  #undef REG

  std::string regMapOut = regMap.SerializeAsString();

  FILE      *out = fopen(output.c_str(), "w");
  uint64_t  sz = regMapOut.size();
  fwrite(&sz, 1, sizeof(uint64_t), out);
  fwrite(regMapOut.c_str(), 1, regMapOut.size(), out);
  fclose(out);

  //fork now
  pid_t c = fork();

  if(c > 0) {
    uint64_t  count = 0;

    out = fopen(output.c_str(), "a");
    //do the tracing 
    int status;
    
    struct user_regs_struct old_regs = { 0 };
    bool s = true;

    while(true) {
      wait(&status);

      if(WIFEXITED(status)) {
        break;
      }

      //get registers
      struct user_regs_struct regs;

      ptrace(PTRACE_GETREGS, c, NULL, &regs);

      //get the 15 bytes that are at the program counter
      unsigned char eipbuff[16];
      long          peekval;

      peekval = ptrace(PTRACE_PEEKTEXT, c, regs.rip, NULL);
      memcpy(&eipbuff[0], &peekval, 8);

      peekval = ptrace(PTRACE_PEEKTEXT, c, regs.rip+8, NULL);
      memcpy(&eipbuff[8], &peekval, 8);

      //step the child 

      ptrace(PTRACE_SINGLESTEP, c, NULL, NULL);

      //save our stuff to the buffer
      TraceEvent  t = mktrace(eipbuff, count, &regs, &old_regs, s);
      std::string traceOut = t.SerializeAsString();

      sz = traceOut.size();

      fwrite(&sz, 1, sizeof(uint64_t), out);
      fwrite(traceOut.c_str(), 1, traceOut.size(), out);
      
      count++;
      old_regs = regs;
      s = false;
    }
    
    fclose(out);
  } else if(c == 0) {
    boost::filesystem::path p{program_path};
    boost::filesystem::path pc = boost::filesystem::canonical(p);

    // launch the thing to trace
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    int err = execl(pc.string().c_str(), pc.filename().string().c_str(), NULL);

    if(err == -1) {
      perror("err on execl");
    }

  } else {
    // there was an error

  }

  return 0;
}

int main(int argc, char *argv[]) {
  boost::program_options::variables_map       vm;
  boost::program_options::options_description desc("Options");

  desc.add_options()
    ("input,i", boost::program_options::value<std::string>()->required(), "input")
    ("output,o", boost::program_options::value<std::string>(), "output")
    ("help,h", "Help")
  ;

  //parse command line options 

  try {
    boost::program_options::store(
      boost::program_options::command_line_parser(argc, argv).options(desc).run(), 
      vm);
    boost::program_options::notify(vm);    
  } catch (std::exception& e) {
    std::cout << "ERROR\n" << e.what() << "\n" << desc << "\n";
    return 1;
  } 

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 1;
  }

  std::string output = "trace";

  if(vm.count("output")) {
    output = vm["output"].as<std::string>();
  }

  return trace(vm["input"].as<std::string>(), "", output);
}
