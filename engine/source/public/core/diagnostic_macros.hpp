#pragma once

// TODO: Add GCC support
#if ANTON_COMPILER_CLANG
  #define ANTON_DISABLE_WARNINGS()    \
    _Pragma("clang diagnostic push"); \
    _Pragma("clang diagnostic ignored \"-Weverything\"");
  #define ANTON_RESTORE_WARNINGS() _Pragma("clang diagnostic pop");

  #define ANTON_WARNING_PUSH() _Pragma("clang diagnostic push");
  #define ANTON_WARNING_POP() _Pragma("clang diagnostic pop");
  #define ANTON_WARNING_IGNORE(WARNING) _Pragma(WARNING);

  #define ANTON_WARNING_UNUSED_CONST_VARIABLE \
    "clang diagnostic ignored \"-Wunused-const-variable\""
#elif ANTON_COMPILER_MSVC
  #define ANTON_DISABLE_WARNINGS() __pragma(warning(push, 0));
  #define ANTON_RESTORE_WARNINGS() __pragma(warning(pop));

  #define ANTON_WARNING_PUSH()
  #define ANTON_WARNING_POP()
  #define ANTON_WARNING_IGNORE(WARNING)

  #define ANTON_WARNING_UNUSED_CONST_VARIABLE
#else
  #define ANTON_DISABLE_WARNINGS()
  #define ANTON_RESTORE_WARNINGS()
  #define ANTON_WARNING_PUSH()
  #define ANTON_WARNING_POP()
  #define ANTON_WARNING_IGNORE(WARNING)

  #define ANTON_WARNING_UNUSED_CONST_VARIABLE
#endif

// Note: Clang 8 does not support __declspec(allocator)
#if defined(_MSC_VER) && ANTON_COMPILER_CLANG
  // Makes the memory allocations visible via Event Tracing for Windows (ETW),
  // which allows Visual Studio's native memory profiler to track memory allocations.
  #define ANTON_DECLSPEC_ALLOCATOR __declspec(allocator)
#else
  #define ANTON_DECLSPEC_ALLOCATOR
#endif
