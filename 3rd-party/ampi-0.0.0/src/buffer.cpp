#include <ampi/buffer.hpp>

#include <boost/container/pmr/global_resource.hpp>

namespace ampi
{
    buffer::buffer(size_t n)
        : buffer{n,boost::container::pmr::get_default_resource()}
    {}
}
