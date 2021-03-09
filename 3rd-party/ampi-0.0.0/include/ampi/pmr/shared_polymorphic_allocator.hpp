// Copyright 2020 Pavel A. Lebedev
// Licensed under the Apache License, Version 2.0.
// (See accompanying file LICENSE.txt or copy at
//  http://www.apache.org/licenses/LICENSE-2.0)
// SPDX-License-Identifier: Apache-2.0

#ifndef UUID_D749618C_438A_4851_A09B_050461A6E526
#define UUID_D749618C_438A_4851_A09B_050461A6E526

#include <ampi/export.h>
#include <ampi/utils/tagged_pointer.hpp>

#include <boost/container/pmr/memory_resource.hpp>

#include <atomic>
#include <cassert>
#include <memory>
#include <new>
#include <utility>

namespace ampi
{
    namespace detail
    {
        struct AMPI_EXPORT shared_memory_resource_base
        {
            std::atomic<unsigned> refcount_{1};

            virtual ~shared_memory_resource_base();

            void ref()
            {
                refcount_.fetch_add(1,std::memory_order::relaxed);
            }

            void unref()
            {
                if(refcount_.load(std::memory_order_relaxed)!=1&&
                        refcount_.fetch_sub(1,std::memory_order::release)!=1)
                    return;
                std::atomic_thread_fence(std::memory_order::acquire);
                delete this;
            }

            virtual boost::container::pmr::memory_resource* get() = 0;
        };

        template<typename T>
        struct shared_memory_resource : shared_memory_resource_base
        {
            T mr_;

            boost::container::pmr::memory_resource* get() override
            {
                return &mr_;
            }
        };

        class AMPI_EXPORT shared_polymorphic_allocator_base
        {
        public:
            shared_polymorphic_allocator_base() noexcept = default;

            shared_polymorphic_allocator_base(boost::container::pmr::memory_resource* mr)
                : p_{mr}
            {
                assert(mr);
            }

            template<typename T,typename... Args>
            shared_polymorphic_allocator_base(std::in_place_type_t<T>,Args&&... args)
                : p_{new shared_memory_resource<T>{{},{std::forward<Args>(args)...}},1}
            {}

            shared_polymorphic_allocator_base(const shared_polymorphic_allocator_base& other) noexcept
                : p_{other.p_}
            {
                if(p_.tag())
                    shared()->ref();
            }

            shared_polymorphic_allocator_base(shared_polymorphic_allocator_base&& other) noexcept
                : p_{std::exchange(other.p_,{})}
            {}

            shared_polymorphic_allocator_base& operator=(
                const shared_polymorphic_allocator_base& other) noexcept
            {
                if(this!=&other){
                    destroy();
                    p_ = other.p_;
                    if(p_.tag())
                        shared()->ref();
                }
                return *this;
            }

            shared_polymorphic_allocator_base& operator=(
                shared_polymorphic_allocator_base&& other) noexcept
            {
                if(this!=&other){
                    destroy();
                    p_ = std::exchange(other.p_,{});
                }
                return *this;
            }

            ~shared_polymorphic_allocator_base()
            {
                destroy();
            }

            explicit operator bool() const noexcept
            {
                return uintptr_t(p_);
            }

            boost::container::pmr::memory_resource* resource() const noexcept
            {
                return p_.tag()?shared()->get():non_owning();
            }

            [[nodiscard]] void* allocate_bytes(size_t bytes,
                                               size_t alignment = alignof(std::max_align_t))
            {
                return resource()->allocate(bytes,alignment);
            }

            void deallocate_bytes(void* p,size_t bytes,
                                  size_t alignment = alignof(std::max_align_t))
            {
                return resource()->deallocate(p,bytes,alignment);
            }

            bool operator==(const shared_polymorphic_allocator_base& other) const noexcept = default;
        protected:
            tagged_pointer<void,1> p_ = {};
        private:
            boost::container::pmr::memory_resource* non_owning() const noexcept
            {
                return static_cast<boost::container::pmr::memory_resource*>(p_.pointer());
            }

            shared_memory_resource_base* shared() const noexcept
            {
                return static_cast<shared_memory_resource_base*>(p_.pointer());
            }

            void destroy() noexcept
            {
                if(p_.tag())
                    shared()->unref();
            }
        };
    }

    template<typename T = byte>
    class shared_polymorphic_allocator : public detail::shared_polymorphic_allocator_base
    {
    public:
        using value_type = T;

        using detail::shared_polymorphic_allocator_base::shared_polymorphic_allocator_base;

        shared_polymorphic_allocator(const detail::shared_polymorphic_allocator_base& other)
            : detail::shared_polymorphic_allocator_base{other}
        {}

        shared_polymorphic_allocator(detail::shared_polymorphic_allocator_base&& other)
            : detail::shared_polymorphic_allocator_base{std::move(other)}
        {}

        T* allocate(size_t n)
        {
            return static_cast<T*>(allocate_bytes(n*sizeof(T),alignof(T)));
        }

        void deallocate(T* p,size_t n)
        {
            return deallocate_bytes(p,n*sizeof(T),alignof(T));
        }

        template<typename U,typename... Args>
        void construct(U* p,Args&&... args)
        {
            // FIXME: no support in libc++.
            // std::uninitialized_construct_using_allocator(p,*this,std::forward<Args>(args)...);
            std::construct_at(p,std::forward<Args>(args)...);
        }

        template<typename U>
        [[nodiscard]] U* allocate_object(size_t n = 1)
        {
            if(size_t(-1)/sizeof(U)<n)
                throw std::bad_array_new_length{};
            return static_cast<U*>(allocate_bytes(n*sizeof(U),alignof(U)));
        }

        template<typename U>
        void deallocate_object(U* p,size_t n = 1) noexcept
        {
            deallocate_bytes(p,n*sizeof(U),alignof(U));
        }

        template<typename U,typename... Args>
        [[nodiscard]] U* new_object(Args&&... args)
        {
            U* p = allocate_object<U>();
            try{
                construct(p,std::forward<Args>(args)...);
            }
            catch(...){
                deallocate_object(p);
                throw;
            }
            return p;
        }

        template<typename U>
        void delete_object(U* p) noexcept
        {
            std::destroy_at(p);
            deallocate_object(p);
        }
    };
}

#endif
