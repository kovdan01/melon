#ifndef UUID_B1E6BBC8_EC00_4E86_A368_B18B5DB85B28
#define UUID_B1E6BBC8_EC00_4E86_A368_B18B5DB85B28

#if __has_include(<coroutine>)
#include <coroutine>
namespace ce { namespace stdcoro = std; }
#else
#include <experimental/coroutine>
namespace ce { namespace stdcoro = std::experimental; }
#endif

#endif
