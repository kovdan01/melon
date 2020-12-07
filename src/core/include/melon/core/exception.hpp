#ifndef MELON_CORE_EXCEPTION_HPP_
#define MELON_CORE_EXCEPTION_HPP_

#include <stdexcept>

namespace melon
{

class Exception : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

}  // namespace melon

namespace melon::core
{

class Exception: public melon::Exception
{
public:
    using melon::Exception::Exception;
};

} // namespace melon::core

#endif // EXCEPTION_HPP
