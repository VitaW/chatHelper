//
// Created by sida liang on 2018/6/7.
//

#ifndef MINI_TCPCONNECTION_H
#define MINI_TCPCONNECTION_H
#include <memory>
#include <boost/asio.hpp>
#include "Buffer.hpp"
#include "request.hpp"
#include "Log.h"
#include <boost/any.hpp>
#include <boost/noncopyable.hpp>

class Buffer;
class TcpConnection :  public boost::noncopyable,
        public std::enable_shared_from_this<TcpConnection> {
public:
    typedef std::shared_ptr<Buffer> Bufferptr;
    typedef std::shared_ptr<boost::asio::ip::tcp::socket> Socketptr;
    typedef std::shared_ptr<TcpConnection> Connectionptr;
    explicit TcpConnection(boost::asio::io_service& s)
            :sockptr_(std::make_shared<boost::asio::ip::tcp::socket>
                               (s)),
             buffer_(std::make_shared<Buffer>()){

    }

    Bufferptr get_buffer() const {
        return buffer_;
    }

    Socketptr get_socket() const {
        return sockptr_;
    }

    /*
     * void call_back(Connectionptr ,Bufferptr,size_t);
     */
    template <typename Func>
    void async_read(Func&& f);

    /*
     * void call_back(Connectionptr);
     */
    template <typename Func>
    void async_write(Func&& f,Bufferptr buf);

    template <typename Func,typename Buffers>
    void async_write(Func&& f,Buffers&& buf);

    template <typename T>
    void set_context(T&& con) {
        context_ = std::forward<T>(con);
    }

    template <typename T>
    T get_context() const {
        return boost::any_cast<T>(context_);
    }

private:
    Bufferptr buffer_;
    Socketptr sockptr_;
    boost::any context_;
};

template <typename Func>
void TcpConnection::async_read(Func &&f) {
    auto connptr = shared_from_this();
    auto read_callback = [connptr,f](const boost::system::error_code& e,
            std::size_t nread) {
        if(e)
            /*
             * log
             */
            return;
        auto buffer_ = connptr->get_buffer();
        buffer_->write_n((int)(nread));
        if(!buffer_->writeable())
            buffer_->double_size();
        f(connptr,connptr->buffer_,nread);
    };
    sockptr_->async_read_some(boost::asio::buffer(
            buffer_->get_data() + buffer_->get_write_pos(),buffer_->writeable()),
            read_callback);
}

template <typename Func>
void TcpConnection::async_write(Func &&f, TcpConnection::Bufferptr buf) {
     auto connptr = shared_from_this();
     std::function<void(typename std::remove_reference<Func>::type& f,const boost::system::error_code&)>
             write_callback = [&write_callback,connptr,buf](Func& f,
            const boost::system::error_code& e,
            size_t nwrite) {
        if(e) {
            BOOST_LOG_TRIVIAL(error) << "[error]" << e.message() << ":" << e.value();
            return;
        }
        buf->commit(static_cast<int>(nwrite));
        auto temp_sockptr = connptr->get_socket();
        if(!buf->readable()) {
            f(connptr);
        } else {
            temp_sockptr->async_write_some(boost::asio::buffer(buf->get_data() + buf->get_read_pos(),
            buf->readable()),std::bind(write_callback,std::move(f)));
        }
    };
    sockptr_->async_write_some(boost::asio::buffer(buf->get_data() + buf->get_read_pos(),
                                                   buf->readable()),std::bind(write_callback,std::forward<Func>(f)));

}

template <typename Func,typename Buffers>
void TcpConnection::async_write(Func&& f,Buffers&& buf) {
    auto connptr = shared_from_this();
    auto call_back = [connptr](typename std::remove_reference<Func>::type& f,
            const boost::system::error_code& e,size_t n) {
        if(!e)
            f(connptr);
    };
    boost::asio::async_write(*sockptr_,std::forward<Buffers>(buf),std::bind(call_back,std::forward<Func>(f),std::placeholders::_1,
    std::placeholders::_2));
};

#endif //MINI_TCPCONNECTION_H
