#include <ce/hex.hpp>

#include <boost/io/ios_state.hpp>

#include <cassert>
#include <cstddef>
#include <iomanip>

namespace ce::detail
{
    std::ostream& operator<<(std::ostream& stream,const hex_t& ht)
    {
        assert(ht.columns);
        if(!ht.data.empty()){
            boost::io::ios_flags_saver fls{stream};
            boost::io::ios_fill_saver fis{stream};
            stream << std::hex << std::setfill('0');
            auto p = ht.data.data();
            stream << std::setw(2) << std::to_integer<unsigned>(*p);
            for(std::size_t i=1,j=1;i<ht.data.size();++i,++j){
                if(j==ht.columns)
                    j = 0;
                stream << (j?' ':'\n')  << std::setw(2) << std::to_integer<unsigned>(*++p);
            }
        }
        return stream;
    }
}
