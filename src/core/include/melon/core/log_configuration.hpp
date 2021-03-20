#ifndef MELON_CORE_LOG_CONFIGURATION_HPP_
#define MELON_CORE_LOG_CONFIGURATION_HPP_

#include <ce/socket_session.hpp>

#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>

namespace melon::core::log
{

void setup();

}  // namespace melon::core::log

#endif // MELON_CORE_LOG_CONFIGURATION_HPP_
