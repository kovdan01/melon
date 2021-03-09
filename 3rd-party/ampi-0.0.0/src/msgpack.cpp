#include <ampi/msgpack.hpp>

#include <ostream>

namespace ampi
{
    std::ostream& operator<<(std::ostream& stream,const extension& ext)
    {
        return stream << "Extension(" << ext.type << ',' << ext.data << ')';
    }
}
