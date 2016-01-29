// tracer main

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>

#include <capstone/capstone.h>

#include "Trace.pb.h"
#include "tracer.h"
#include "recorder.h"
#include "registerfile.h"


using std::vector;
using std::string;

boost::bimap<std::string, unsigned int> RegisterFile::kRegisterNameMap;
bool RegisterFile::_map_initialized = 0;

int main(int argc, char *argv[]) {
  boost::program_options::variables_map       vm;
  boost::program_options::options_description desc("Options");
  csh handle;
  cs_insn *insn;

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

  if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) {
    return 1;
  }





  auto input_args = vm["input"].as<vector<string>>();
  size_t count;

  Tracer instructionTracer {input_args};
  Recorder stateRecorder {output};
  RegisterFile previous;

  instructionTracer.addListener([&](const struct user_regs_struct &regs) {

    uint64_t rip = (uint64_t) regs.rip;

    if(!instructionTracer.addressWithinImage(rip)) {
      return;
    } else {
      std::cout << ".";
    }


    RegisterFile current {&regs};

    // Delta is not used yet
    auto delta = current.getDelta(previous);

    auto eipbuf = instructionTracer.getClientMemory(rip, 16);
    count = cs_disasm_ex(handle, &eipbuf[0], 16, regs.rip, 1, &insn);
    if (count == 1) {
      eipbuf.resize(insn->size);
    }

    stateRecorder.recordState(Recorder::State{regs, eipbuf});

    previous = current;
  });

  instructionTracer.onComplete([]() {

  });

  instructionTracer.start();
}
