#include <ampi/msgpack.hpp>

#include <boost/io/ios_state.hpp>

namespace ampi
{
    std::ostream& operator<<(std::ostream& stream,object_kind kind)
    {
        static const char* names[] = {
            "null",
            "bool",
            "unsigned_int",
            "signed_int",
            "float",
            "double",
            "map",
            "sequence",
            "binary",
            "extension",
            "string",
            "timestamp"
        };
        return stream << names[static_cast<uint8_t>(kind)];
    }

    std::ostream& operator<<(std::ostream& stream,const extension& ext)
    {
        return stream << "Extension(" << ext.type << ',' << ext.data << ')';
    }

    std::ostream& operator<<(std::ostream& stream,timestamp_t ts)
    {
        // FIXME:: no operator<< for std::chrono::sys_time in libc++.
        auto dp = std::chrono::floor<std::chrono::days>(ts);
        std::chrono::year_month_day ymd{dp};
        std::chrono::hh_mm_ss hms{ts-dp};
        boost::io::ios_fill_saver ifs{stream};
        return stream << std::setfill('0')
                      << std::setw(4) << int(ymd.year()) << '-'
                      << std::setw(2) << unsigned(ymd.month()) << '-'
                      << std::setw(2) << unsigned(ymd.day()) << ' '
                      << std::setw(2) << hms.hours().count() << ':'
                      << std::setw(2) << hms.minutes().count() << ':'
                      << std::setw(2) << hms.seconds().count();
    }

    namespace
    {
        struct event_visitor
        {
            std::ostream& stream_;

            void operator()(auto x)
            {
                stream_ << x;
            }

            void operator()(sequence_header sh)
            {
                stream_ << "sequence(" << sh.size << ')';
            }

            void operator()(map_header sh)
            {
                stream_ << "map(" << sh.size << ')';
            }

            void operator()(const piecewise_string& ps)
            {
                stream_ << quoted(ps);
            }

            void operator()(const extension& ext)
            {
                stream_ << ext.type << ':' << ext.data;
            }
        };
    }

    std::ostream& operator<<(std::ostream& stream,const event& e)
    {
        stream << e.kind();
        if(e.kind()!=object_kind::null){
            stream << ':';
            visit(event_visitor{stream},e.v_);
        }
        return stream;
    }
}
