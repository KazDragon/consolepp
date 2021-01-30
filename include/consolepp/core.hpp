#include "consolepp/version.hpp"
#include "consolepp/detail/export.hpp"
#include <gsl/gsl-lite.hpp>
#include <cstring>

namespace consolepp {

// A byte in Console++ is represented consistently as an unsigned 8-bit integer.
using byte = std::uint8_t;

// A stream of bytes in Console++ is exposed as a non-owning span.  It is 
// expected that the data will live for no longer than any function in which
// it is found.  For that reason, these spans should never be stored directly.
// If storage is necessary, it must be converted into a longer-living data
// structure.
using bytes = gsl::span<byte const>;

// Where necessary, bytes are stored in this type, which has the small string
// optimization, meaning that most trivial cases will not cause an allocation.
using byte_storage = std::basic_string<byte>;

}