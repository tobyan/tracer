
#include <string>
#include <vector>
#include <algorithm>

#include <boost/filesystem.hpp>

#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>

#include "tracer.h"


void
Tracer::addListener(Tracer::ListenerT listener)
{
  _listeners.push_back(listener);
}

int
Tracer::startProcessingEvents()
{
	int status;

	for (;;) { 
		wait(&status);

    if(WIFEXITED(status) || WIFSIGNALED(status)) {
      status = WEXITSTATUS(status);
      break;
    }
    
    if (WIFSTOPPED(status) && WSTOPSIG(status) != SIGTRAP) {
      status = -1;
    }
  
    struct user_regs_struct regs;
    _os_trace->getRegisters((void*)&regs);

    for (auto listener : _listeners)
      listener(regs);

    _os_trace->step();

    _instructionCount++;
  }

  _complete_callback();
  
  return status;
}

int
Tracer::start()
{
	_pid = fork();

	if (_pid == 0) {
    boost::filesystem::path bpath {_cmdline[0]};

    _os_trace->traceMe();

    std::vector<const char *> arguments;

    for (auto arg : _cmdline)
      arguments.push_back(arg.c_str());
    arguments.push_back(nullptr);
    
    char **cmd = (char **) &arguments[0];
    int status = execvp(cmd[0], cmd);

    if (status < 0) {
      perror("Error starting client: ");
    } else {
      assert(0);      /** NORETURN */
    }
	} 

  if (_pid < 0) {
    return -1;
  }

  _os_trace->setPid(_pid);

  return startProcessingEvents();
}

std::vector<uint8_t>
Tracer::getClientMemory(uint64_t addr, size_t n)
{
  long val;
  size_t remaining = n;
  size_t offset = 0;
  std::vector<uint8_t> arr(n);

  while (remaining > 0) {
    size_t to_copy = std::min(remaining, sizeof(val));

    val = _os_trace->peekText(addr);
    
    memcpy(arr.data() + offset, &val, to_copy);
    remaining -= to_copy;
    offset += to_copy;
  }

  return arr;
}

//template std::array<uint8_t, 16> Tracer::getClientMemory<16>(caddr_t addr);