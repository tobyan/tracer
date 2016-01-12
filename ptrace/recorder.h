
#include <cstdint>
#include <array>
#include <vector>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <fstream>

#include "tracer.h"
#include "Trace.pb.h"

//template <typename RegMap>
class Recorder {

public:

  struct State {
    struct user_regs_struct registerState;
    std::vector<uint8_t> instruction;
  };

public:

  explicit Recorder(const std::string  &path)
  : _outs {path}
  {
    _writeHeader();
  }

  /**
   * @brief Log an instance of State to disk.
   *
   * @param The program state to be logged
   */
  void recordState(const State &state);

private:
  void _writeState(const std::string &data);
  void _writeHeader();

  std::ofstream  _outs;
  State _previous_state;
  uint64_t _written = 0;
};