#ifndef Buffer_hpp
#define Buffer_hpp
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/uio.h>
#include <boost/noncopyable.hpp>
#define BUFFERSIZE 4096
#define PRERESERVE 8
class Buffer : boost::noncopyable
{
public:
    explicit Buffer(int n = BUFFERSIZE):buffer(n),
    read_pos(PRERESERVE),
    write_pos(PRERESERVE){}
    
    int readable() const;
    int writeable() const;
    
    //从buffer拿出数据
    std::string get_n_bytes(int n);
    std::string get_all();
    int writefd(int fd);

    const char* get_data() const {
        return buffer.data();
    }

    char* get_data() {
        return buffer.data();
    }

    int get_read_pos() const {
        return read_pos;
    }

    int get_write_pos() const {
        return write_pos;
    }

    void write_n(int n) {
        int w = writeable();
        if(n > w)
            n = w;
        write_pos += n;
    }

    void commit(int n) {
        int r = readable();
        if(n > r)
            move_front();
        else
            read_pos += n;
    }
    void resize(int n) {
        buffer.resize(n);
    }

    int get_size() const {
        return buffer.size();
    }

    char peek_char() const {
        return buffer[read_pos];
    }

    char peek_back() const {
        return buffer[write_pos - 1];
    }
    void double_size() {
        buffer.resize(2 * get_size());
    }
    //往buffer中放入数据
    int readfd(int );
    void append(const char* ,int lenth);
    void append(const std::string& s);
    void pre_append(const char* ,int)/*小于8字节0copy，其余不保证*/;
    inline void pre_append(const std::string& s)/*小于8字节0copy，其余不保证*/;
private:
    //将buffer里面的数据挪到前面
    void move_front();
    std::vector<char> buffer;
    int read_pos;
    int write_pos;
};
#endif /* Buffer_hpp */
