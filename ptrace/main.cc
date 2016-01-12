// tracer main

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>

#include "Trace.pb.h"
#include "tracer.h"
#include "recorder.h"

int trace(std::string program_path, std::string program_args, std::string output) {

  Tracer instructionTracer {program_path};
  Recorder stateRecorder {output};

  instructionTracer.addListener([&](pid_t pid, const struct user_regs_struct &regs) {
      std::array<uint8_t, 16> eipbuff;

      uint64_t count = instructionTracer.count();

      auto eipbuf = instructionTracer.getClientMemory<16>((caddr_t) regs.rip);

      stateRecorder.recordState(Recorder::State{regs, eipbuff});

  });

  instructionTracer.start();

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
