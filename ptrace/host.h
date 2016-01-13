
#include <sys/ptrace.h>
#include <sys/types.h>

#pragma once

class OSTraceSupport {
public:
  virtual ~OSTraceSupport(){}

  void setPid(pid_t pid) { _pid = pid; }
  virtual void traceMe() = 0;
  virtual void step() = 0;
  virtual void getRegisters(void *dest) = 0;
  virtual long peekText(uint64_t addr) = 0;
  virtual long peekData(uint64_t addr) = 0;

protected:
  pid_t _pid;
};

class LinuxHostSupport : public OSTraceSupport {
public:
  virtual void traceMe() override {
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
  }
  virtual void step() override {
    ptrace(PTRACE_SINGLESTEP, _pid, NULL, NULL);
  }

  virtual void getRegisters(void *dest) override {
    ptrace(PTRACE_GETREGS, _pid, NULL, (struct user_regs_struct *)dest);
  }

  virtual long peekText(uint64_t addr) override {
    return ptrace(PTRACE_PEEKTEXT, _pid, (caddr_t)addr, NULL);
  }

  virtual long peekData(uint64_t addr) override {
    return ptrace(PTRACE_PEEKDATA, _pid, (caddr_t)addr, NULL);
  }
};