// tracer main

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>

#include "Trace.pb.h"

int trace(std::string program_path, std::string program_args, std::string output) {
  //write out the output file with an initial register map

  //fork now
  pid_t c = fork();

  if(c > 0) {
    //do the tracing 
    int status;
    
    while(true) {
      wait(&status);

      if(WIFEXITED(status)) {
        break;
      }

      //get a register 
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
    }
    
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
