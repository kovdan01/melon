#ifndef MELON_CORE_MELON_CORE_HPP_
#define MELON_CORE_MELON_CORE_HPP_

#include <melon/core/export.h>
#include <yaml_config.hpp>

namespace melon
{

class Exception: public std::exception
{
public:
    virtual const char* what() const noexcept{
       return msg_.c_str();
    }

protected:
    std::string msg_ = "melon exception";
};

}  // namespace melon

namespace melon::core
{

class Exception: public melon::Exception
{
public:
    virtual const char* what() const noexcept
    {
       return msg_.c_str();
    }

protected:
std::string msg_ = "melon::core exception";
};

class MissingParamsException: public melon::core::Exception
{
public:
    MissingParamsException(const std::string& message = "missing params"):msg_(message){}
    virtual const char* what() const noexcept
    {
       return msg_.c_str();
    }

protected:
    std::string msg_;
};

class ExtraParamsException: public melon::core::Exception
{
public:
    ExtraParamsException(const std::string& message = "extra params"):msg_(message){}
    virtual const char* what() const noexcept
    {
       return msg_.c_str();
    }

protected:
    std::string msg_ = "extra params";
};

MELON_CORE_EXPORT void hello();

}  // namespace melon::core

#endif  // MELON_CORE_MELON_CORE_HPP_
