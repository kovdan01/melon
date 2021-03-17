#include <ampi/serialize/serializable.hpp>

#include <ampi/utils/format.hpp>

#include <bit>
#include <stdexcept>

namespace ampi
{
    std::ostream& operator<<(std::ostream& stream,object_kind_set oks)
    {
        bool first = true;
        for(unsigned x = oks.set_;x;){
            stream << (first?'[':',');
            int pos = std::countr_zero(x);
            stream << static_cast<object_kind>(pos);
            first = false;
            x ^= 1u<<pos;
        }
        return stream << ']';
    }

    namespace
    {
        const auto fallback_structure_error = std::make_shared<std::string>("structure_error");
    }

    const char* structure_error::what() const noexcept
    {
        if(!what_)
            try{
                switch(reason_){
                    case reason_t::unexpected_event:
                        if(event_)
                            what_ = std::make_shared<std::string>(format(
                                "unexpected event when deserializing ",object_type_.pretty_name(),
                                ": ",*event_,", wanted ",expected_kinds_));
                        else
                            what_ = std::make_shared<std::string>(format(
                                "unexpected end of event stream when deserializing ",
                                object_type_.pretty_name(),", wanted ",expected_kinds_));
                        break;
                    case reason_t::out_of_range:
                        if(event_)
                            what_ = std::make_shared<std::string>(format(
                                "when deserializing ",object_type_.pretty_name(),
                                ", the value of event ",*event_," is out of range"));
                        else
                            what_ = std::make_shared<std::string>(format(
                                "size of container of tyoe ",object_type_.pretty_name(),
                                " is too big"));
                        break;
                    case reason_t::duplicate_key:
                        what_ = std::make_shared<std::string>(format(
                            "duplicate key when deserializing ",object_type_.pretty_name()));
                        break;
                    case reason_t::unknown_key:
                        what_ = std::make_shared<std::string>(format(
                            "unknown key ",*event_," when deserializing ",
                            object_type_.pretty_name()));
                        break;
                    default:
                        BOOST_UNREACHABLE_RETURN(nullptr);
                }
            }
            catch(...){
                what_ = fallback_structure_error;
            }
        return what_->data();
    }
}
