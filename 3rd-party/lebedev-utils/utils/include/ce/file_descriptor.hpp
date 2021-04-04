#ifndef UUID_9B12FE85_0EE7_4FE9_8719_37E79229088C
#define UUID_9B12FE85_0EE7_4FE9_8719_37E79229088C

#include <ce/errno.hpp>
#include <ce/utils/export.h>

#include <fcntl.h>
#include <unistd.h>

#include <span>
#include <utility>

namespace ce
{
    class CE_UTILS_EXPORT file_descriptor
    {
    public:
        file_descriptor(int fd,const char* what)
            : fd_{fd}
        {
            throw_errno_if_negative(fd_,what);
        }

        file_descriptor(file_descriptor&& other) noexcept
            : fd_{std::exchange(other.fd_,-1)}
        {}

        file_descriptor& operator=(file_descriptor&& other) noexcept
        {
            if(this!=&other){
                destroy();
                fd_ = std::exchange(other.fd_,-1);    
            }
            return *this;
        }

        ~file_descriptor()
        {
            destroy();
        }

        operator int() const noexcept
        {
            return fd_;
        }

        ssize_t read(std::span<std::byte> buf)
        {
            return ::read(fd_,buf.data(),buf.size());
        }

        ssize_t write(std::span<const std::byte> buf)
        {
            return ::write(fd_,buf.data(),buf.size());
        }

        void read_as_single(std::span<std::byte> buf);
        void write_as_single(std::span<const std::byte> buf);
        size_t read_as_chunks(std::span<std::byte> buf);
        size_t write_as_chunks(std::span<const std::byte> buf);

        void set_non_blocking();
    protected:
        int fd_;
    private:
        void destroy()
        {
            if(fd_>=0)
                close(fd_);
        }
    };
}

#endif
