#include <type_traits>
#include <boost/bimap.hpp>
#pragma once

/**
 * @brief Rerpresents an instance of all the registers
 */
class RegisterFile {

public:

  /** @brief The type of the register itself. */
  using type = unsigned long long;

  /* @brief Container type for register window */
  using register_window = type[sizeof(struct user_regs_struct)/sizeof(type)];

  /* @brief Map type for storing differences in RegisterFiles */
  using register_delta = std::map<unsigned, type>;


  RegisterFile():_registers_struct{0} {
  }

  /**
   * @brief Create a RegisterFile from a ptrace-returned register window
   */
  RegisterFile(const struct user_regs_struct *window)
  {
    _registers_struct = *window;

    if (!_map_initialized) {
      _initializeMap();
      _map_initialized = true;
    }
  }

  /**
   * @brief Produce a difference between two register states
   *
   * @param rhs The register file with which to compare
   */
  register_delta getDelta(const RegisterFile &rhs) const {
    register_delta delta;

    for (int i = 0; i < sizeof(_registers)/sizeof(_registers[0]); ++i) {
      if (_registers[i] != rhs._registers[i]) {
        delta.emplace(i, rhs._registers[i]);
      }
    }
    return delta;
  }

  /**
   * @brief Return the value of a register by name
   *
   * @param name The name of the register (i.e. "rip")
   */
  type getValue(std::string &name) {
    return _registers[getIndex(name)];
  }
private:

  size_t getIndex(const std::string &name) {
    return kRegisterNameMap.left.find(name)->second;
  }


  static boost::bimap<std::string, unsigned int> kRegisterNameMap;
  
  static bool _map_initialized;

  static void _initializeMap() {
    using entry = boost::bimap<std::string, unsigned int>::value_type;

    kRegisterNameMap.insert( entry ("r15", 0) );
    kRegisterNameMap.insert( entry ("r14", 1) );
    kRegisterNameMap.insert( entry ("r13", 2) );
    kRegisterNameMap.insert( entry ("r12", 3) );
    kRegisterNameMap.insert( entry ("rbp", 4) );
    kRegisterNameMap.insert( entry ("rbx", 5) );
    kRegisterNameMap.insert( entry ("r11", 6) );
    kRegisterNameMap.insert( entry ("r10", 7) );
    kRegisterNameMap.insert( entry ("r9",  8) );
    kRegisterNameMap.insert( entry ("r8",  9) );
    kRegisterNameMap.insert( entry ("rax", 10) );
    kRegisterNameMap.insert( entry ("rcx", 11) );
    kRegisterNameMap.insert( entry ("rdx", 12) );
    kRegisterNameMap.insert( entry ("rsi", 13) );
    kRegisterNameMap.insert( entry ("rdi", 14) );
    kRegisterNameMap.insert( entry ("orig_rax", 15) );
    kRegisterNameMap.insert( entry ("rip", 16) );
    kRegisterNameMap.insert( entry ("cs",  17) );
    kRegisterNameMap.insert( entry ("eflags", 18) );
    kRegisterNameMap.insert( entry ("rsp", 19) );
    kRegisterNameMap.insert( entry ("ss",  20) );
    kRegisterNameMap.insert( entry ("fs_base", 21) );
    kRegisterNameMap.insert( entry ("gs_base", 22) );
    kRegisterNameMap.insert( entry ("ds",  23) );
    kRegisterNameMap.insert( entry ("es",  24) );
    kRegisterNameMap.insert( entry ("fs",  25) );
    kRegisterNameMap.insert( entry ("gs",  26) );

  }

  union {
    register_window _registers;
    struct user_regs_struct _registers_struct;
  };
};
