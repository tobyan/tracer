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

using std::vector;
using std::string;

int main(int argc, char *argv[]) {
  boost::program_options::variables_map       vm;
  boost::program_options::options_description desc("Options");

  desc.add_options()
    ("input,i", boost::program_options::value<vector<string>>()->required(), "input")
    ("output,o", boost::program_options::value<string>(), "output")
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

  string output = "trace";
  if(vm.count("output")) {
    output = vm["output"].as<string>();
  }

  auto input_args = vm["input"].as<vector<string>>();

  Tracer instructionTracer {input_args};
  Recorder stateRecorder {output};

  instructionTracer.addListener([&](const struct user_regs_struct &regs) {

      caddr_t addr = (caddr_t) regs.rip;
      auto eipbuf = instructionTracer.getClientMemory(addr, 16);

      stateRecorder.recordState(Recorder::State{regs, eipbuf});
  });

  instructionTracer.start();
}
