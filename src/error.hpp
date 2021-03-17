#ifndef ERROR_HPP
#define ERROR_HPP

#include <stdint.h>

namespace error
{

enum class errc : int32_t
{
    not_error           = 0x00000000,
    unknown_error       = 0x00000001,

    file_too_large      = 0x50000001
};

}   // namespace error

#endif  // ERROR_HPP