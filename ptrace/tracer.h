
#include <string>
#include <functional>
#include <memory>

#pragma once

class Tracer {

  using ProgramStateT = struct user_regs_struct;

  using ListenerT = std::function<void(const ProgramStateT&)>;

  public:
    explicit Tracer(const std::vector<std::string> &args)
    : _cmdline{args} {}
    explicit Tracer(const std::string &path):_cmdline{path} { }

    Tracer(Tracer *rhs) = delete;
    
    /**
     * @brief Add a functor that is invoked each time an instruction is 
     * executed. Functor is passed the register state at the time of
     * execution.
     *
     * @param listener What to invoke with register state
     */
    void     addListener(ListenerT listener);
    
    /**
     * @brief Read N bytes of memory from inferior process.
     *
     * @param addr The address at which to read from
     * @return An std::array containing the bytes
     */

    template <size_t N> std::array<uint8_t, N> 
    getClientMemory(caddr_t addr);
    
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

    std::vector<std::string> _cmdline;
};
