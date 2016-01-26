
#include <string>
#include <functional>
#include <memory>

#include "host.h"
#include "registerfile.h"

#pragma once

class Tracer {

  using ProgramStateT = struct user_regs_struct;

  using ListenerT = std::function<void(const ProgramStateT&)>;

  public:
    explicit Tracer(const std::vector<std::string> &args)
    : _cmdline{args}, _os_trace{std::make_unique<LinuxHostSupport>()} {}
    explicit Tracer(const std::string &path)
    :Tracer{std::vector<std::string>{path}} {}

    Tracer(Tracer &rhs) = delete;
    
    /**
     * @brief Add a functor that is invoked each time an instruction is 
     * executed. Functor is passed the register state at the time of
     * execution.
     *
     * @param listener What to invoke with register state
     */
    void     addListener(ListenerT listener);
    
    /**
     * @brief A handler to call when tracing is finished
     */
    void     onComplete(std::function<void()> cb) {
        _complete_callback = cb;
    } 

    /**
     * @brief Read n bytes of memory from inferior process.
     *
     * @param addr The address at which to read from
     * @param n The number of bytes
     * @return An std::vector containing the bytes
     */

    std::vector<uint8_t> 
    getClientMemory(uint64_t addr, size_t n);
    
     /**
      * @brief Start executing the child process and invoking all interested
      * listeners.
      *
      * @return The return value of the child process when done executing, or a
      *   negative value if the child process crashed.
      */
    int      start();
    
    /**
     * @brief Return the number of executions executes so far
     *
     * @param Number of instructions
     */
    uint64_t count() { return _instructionCount; }

    /**
     * @brief Get the pid of the inferior process
     * 
     * @return The pid
     */
    pid_t pid() const { return _pid; }

  private:
    /**
     * @brief Start the actual loop that receives tracing events from the OS
     */
  	int startProcessingEvents();

    pid_t _pid;
    uint64_t _instructionCount;

    std::vector<ListenerT> _listeners;

    std::function<void(void)> _complete_callback;

    std::vector<std::string> _cmdline;

    std::unique_ptr<OSTraceSupport> _os_trace;
};
