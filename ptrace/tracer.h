
#include <string>
#include <functional>
#include <memory>

#pragma once

class Tracer {

  using ProgramStateT = struct user_regs_struct;

  using ListenerT = std::function<void(pid_t pid, const ProgramStateT&)>;

  public:
    explicit Tracer(const std::vector<std::string> &args)
    : _cmdline{args} {}
    explicit Tracer(const std::string &path):_cmdline{path} { }
    
    void     addListener(ListenerT listener);
    
    template <size_t N> std::array<uint8_t, N> 
    getClientMemory(caddr_t addr);
    

    void     start();
    

    uint64_t count() { return _instructionCount; }

  private:
  	void startProcessingEvents();

    pid_t _pid;
    uint64_t _instructionCount;

    std::vector<ListenerT> _listeners;

    std::vector<std::string> _cmdline;
};
