#include <ce/file_descriptor.hpp>

#include <stdexcept>

namespace ce
{
    void file_descriptor::read_as_single(std::span<std::byte> buf)
    {
        ssize_t ret = read(buf);
        throw_errno_if_negative(ret,"read");
        if(std::size_t (ret)!=buf.size())
            throw std::runtime_error{"short read"};
    }

    void file_descriptor::write_as_single(std::span<const std::byte> buf)
    {
        ssize_t ret = write(buf);
        throw_errno_if_negative(ret,"write");
        if(std::size_t (ret)!=buf.size())
            throw std::runtime_error{"short write"};
    }

    size_t file_descriptor::read_as_chunks(std::span<std::byte> buf)
    {
        size_t done = 0;
        errno = 0;
        while(!buf.empty()){
            ssize_t ret = read(buf);
            if(ret==-1){
                if(errno==EWOULDBLOCK)
                    break;
                throw_errno("read");
            }
            if(!ret)
                break;
            auto n = std::size_t(ret);
            if(n>buf.size())
                throw std::runtime_error{"long read"};
            done += n;
            buf = buf.subspan(n);
        }
        return done;
    }

    size_t file_descriptor::write_as_chunks(std::span<const std::byte> buf)
    {
        size_t done = 0;
        errno = 0;
        while(!buf.empty()){
            ssize_t ret = write(buf);
            if(ret==-1){
                if(errno==EWOULDBLOCK)
                    break;
                throw_errno("write");
            }
            auto n = std::size_t(ret);
            if(n>buf.size())
                throw std::runtime_error{"long write"};
            done += n;
            buf = buf.subspan(n);
        }
        return done;
    }

    void file_descriptor::set_non_blocking()
    {
        int ret = fcntl(fd_,F_GETFL,0);
        throw_errno_if_negative(ret,"fcntl:F_GETFL");
        throw_errno_if_negative(fcntl(fd_,F_SETFL,ret|O_NONBLOCK),"fcntl:F_SETFL:O_NONBLOCK");
    }
}
