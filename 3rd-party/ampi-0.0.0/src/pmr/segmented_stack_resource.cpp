#include <ampi/pmr/segmented_stack_resource.hpp>

namespace ampi
{
    segmented_stack_resource::~segmented_stack_resource() = default;

    void segmented_stack_resource::do_deallocate(void* p,[[maybe_unused]] size_t bytes,
                                                 size_t alignment)
    {
        assert(static_cast<byte*>(p)+bytes==current_->p_);
        current_->p_ = p;
        while(current_->prev_){
            void* start = reinterpret_cast<byte*>(current_)+sizeof(detail::bidi_block_header);
            size_t space = static_cast<size_t>(-1);
            start = std::align(alignment,1,start,space);
            if(current_->p_!=start)
                return;
            current_ = current_->prev_;
        }
    }
}
