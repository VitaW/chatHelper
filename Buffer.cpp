#include "Buffer.hpp"
#include <string.h>
int Buffer::readable() const
{
    return write_pos - read_pos;
}

int Buffer::writeable() const
{
    return (int)(buffer.size() - write_pos);
}

int Buffer::readfd(int fd)
{
    char temp[BUFFERSIZE];
    iovec io[2];
    io[0].iov_base = &buffer[write_pos];
    io[0].iov_len = writeable();
    io[1].iov_base = temp;
    io[1].iov_len = BUFFERSIZE;
    auto nread = readv(fd, io, 2);
    
    auto sneed = nread - writeable();
    if(sneed > 0)
    {
        write_pos = (int)buffer.size();
        append(temp,(int)sneed);
    }
    else
        write_pos += (int)nread;
    return (int)nread;
}

void Buffer::append(const char* data,int lenth)
{
    if(lenth < writeable() - PRERESERVE)
    {
        move_front();
    }
    buffer.insert(buffer.begin(), data,data + lenth);
    write_pos += lenth;
}

void Buffer::append(const std::string& s)
{
    append(s.data(),static_cast<int>(s.size()));
}

void Buffer::pre_append(const char* data, int lenth)
{
    if(lenth <= read_pos)
    {
        memcpy(&buffer[read_pos - lenth],data,lenth);
        read_pos -= lenth;
    }
    else
    {
        std::vector<char> temp(lenth + writeable());
        memcpy(&temp[0],data,lenth);
        memcpy(&temp[lenth],&buffer[read_pos],writeable());
        read_pos = 0;
        write_pos = (int)temp.size();
        buffer = std::move(temp);
    }
}

void Buffer::pre_append(const std::string& s)
{
    pre_append(s.data(), (int)s.size());
}

inline void Buffer::move_front()
{
    int lenth = readable();
    memmove(&buffer[8], &buffer[read_pos], lenth);
    read_pos = PRERESERVE;
    write_pos = read_pos + lenth;
}


std::string Buffer::get_n_bytes(int n)
{
    int _readable = readable();
    std::string out;
    int _read = n;
    if(n > _readable)
        _read = _readable;
    out.append(&buffer[read_pos],&buffer[read_pos + _read]);
    read_pos += _read;
    if(read_pos >= write_pos)
        move_front();
    return out;
}

std::string Buffer::get_all()
{
    int n = readable();
    return get_n_bytes(n);
}

int Buffer::writefd(int fd)
{
    int rest = readable();
    int count = (int)::write(fd, &buffer[read_pos], rest);
    read_pos+=count;
    if(read_pos == writeable())
        move_front();
    return count;
}

