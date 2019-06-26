#ifndef BUILD_CONFIG_HPP_INCLUDE
#define BUILD_CONFIG_HPP_INCLUDE

#ifdef NDEBUG
#    define GE_DEBUG 0
#else
#    define GE_DEBUG 1
#endif

#ifndef GE_WITH_EDITOR
#    define GE_WITH_EDITOR 1
#endif

#define SERIALIZE_ON_QUIT 1
#define DESERIALIZE 0

#endif // !BUILD_CONFIG_HPP_INCLUDE
