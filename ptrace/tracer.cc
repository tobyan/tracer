
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

    if(WIFEXITED(status) || WIFSIGNALED(status)) 
      return WEXITSTATUS(status);
    
    if (WIFSTOPPED(status) && WSTOPSIG(status) != SIGTRAP)
      return -1;
  
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, _pid, NULL, &regs);

    for (auto listener : _listeners)
      listener(regs);

    ptrace(PTRACE_SINGLESTEP, _pid, NULL, NULL);
    _instructionCount++;

  }
}

int
Tracer::start()
{
	_pid = fork();

	if (_pid == 0) {
    boost::filesystem::path bpath {_cmdline[0]};

    ptrace(PTRACE_TRACEME, 0, NULL, NULL);

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

  return startProcessingEvents();
}

template <size_t N> std::array<uint8_t, N>
Tracer::getClientMemory(caddr_t addr)
{
  long val;
  size_t remaining = N;
  size_t offset = 0;
  std::array<uint8_t, N> arr;

  while (remaining > 0) {
    size_t to_copy = std::min(remaining, sizeof(val));
    val = ptrace(PTRACE_PEEKTEXT, _pid, addr, NULL);
    memcpy(arr.data() + offset, &val, to_copy);
    remaining -= to_copy;
    offset += to_copy;
  }

  return arr;
}

template std::array<uint8_t, 16> Tracer::getClientMemory<16>(caddr_t addr);