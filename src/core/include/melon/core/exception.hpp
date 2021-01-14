#ifndef MELON_CORE_EXCEPTION_HPP_
#define MELON_CORE_EXCEPTION_HPP_

#include <melon/core/export.h>

#include <stdexcept>

namespace melon
{

class MELON_CORE_EXPORT Exception : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
    ~Exception() override;
};

}  // namespace melon

namespace melon::core
{

class MELON_CORE_EXPORT Exception : public melon::Exception
{
public:
    using melon::Exception::Exception;
    ~Exception() override;
};

}  // namespace melon::core

#endif  // MELON_CORE_EXCEPTION_HPP_
