//
// Created by sida liang on 2018/6/7.
//

#include "MiniServer.h"
#include "TcpConnection.h"
#include <memory>
#include "Log.h"
#include "reply.hpp"
#include "Manager.h"
#include "ThreadPool.h"
void HttpServer::start() {

    if(!start_) {
        do_accept();
        start_ = true;
    }
    //acc_.async_accept(conn->get_socket(),);
//    acc_.async_accept()
}

void HttpServer::do_accept() {
    auto new_conn = std::make_shared<TcpConnection>
            (acc_.get_io_service());
    acc_.async_accept(*(new_conn->get_socket()),
    [new_conn,this](const boost::system::error_code& e) {
        //new_conn->async_read()
        BOOST_LOG_TRIVIAL(info) << "[info]" <<"new connector : "
                                   << new_conn->get_socket()->remote_endpoint().address().to_string();
        if(!e) {
            begin_read(new_conn);
            if(on_connectcallback)
                on_connectcallback(new_conn);
        }
        do_accept();
    });
}

void HttpServer::begin_read(Connectionptr new_conn) {
    auto parser = std::make_shared<http::server::request_parser>();
    auto req = std::make_shared<http::server::request>();
    new_conn->async_read(std::bind(&HttpServer::http_requestcallback,this,
                                   parser,req,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
}

void HttpServer::http_requestcallback(HttpServer::Parserptr parser,
                                   HttpServer::Requestptr req,
                                   HttpServer::Connectionptr connptr,
                                   HttpServer::Bufferptr buf, size_t s) {

    http::server::request_parser::result_type state;
    char* cur = nullptr;
    auto beg_read = buf->get_data() + buf->get_read_pos();
    auto end_read = beg_read + buf->readable();
    std::tie(state,cur) = parser->parse(*req,beg_read,end_read) ;
    int nparsered = (int)(cur - beg_read);
    if(state == http::server::request_parser::result_type::good) {
        BOOST_LOG_TRIVIAL(info) << "[info]" << "Http request state : Good by " <<
                                    connptr->get_socket()->remote_endpoint().address();
        buf->commit(nparsered);
        auto& headers = req->headers;
        auto it = std::find_if(headers.begin(),headers.end(),[](const http::server::header& h)
        {
            return h.name == "Content-Length";
        });
        if(it == headers.end()) {
            if(on_readcompletecallback)
                on_readcompletecallback(connptr,req,boost::string_view());
        } else {
            int len = std::stoi(it->value);
            http_datacallback(len,req,connptr,buf,s);
        }
    } else if(state == http::server::request_parser::result_type::bad) {
        /*
         * to do handle bad
         */

        ServerManager::bad_request(connptr);
        BOOST_LOG_TRIVIAL(warning) << "[warnning]" << "Http request state : Bad by " <<
                                connptr->get_socket()->remote_endpoint().address();
    } else {
       buf->commit(nparsered);
       connptr->async_read(std::bind(&HttpServer::http_requestcallback,this,
       parser,req,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
    }
}


void HttpServer::http_datacallback(int nlen,HttpServer::Requestptr req, HttpServer::Connectionptr connptr,
                                   HttpServer::Bufferptr buf,std::size_t n) {

    if (buf->readable() >= nlen) {
        BOOST_LOG_TRIVIAL(debug) << "[debug]" << "http data ：" << buf->get_data() + + buf->get_read_pos();
        begin_read(connptr);
        if (on_readcompletecallback) {
            boost::string_view temp(buf->get_data() + buf->get_read_pos(),nlen);
            on_readcompletecallback(connptr,req,temp);
        }

    } else {
        connptr->async_read(std::bind(&HttpServer::http_datacallback, this,
                                      nlen, req, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
        );
    }
}

void ServerManager::bad_request(Connectionptr connptr) {
    auto rep = http::server::reply::stock_reply(http::server::reply::status_type::bad_request);
    connptr->async_write([](Connectionptr){},rep.to_buffers());
    connptr->set_context(std::move(rep));
}

void ServerManager::_read_callback(std::shared_ptr<TcpConnection> connptr,
                                  std::shared_ptr<http::server::request> req,
                                  boost::string_view str) {
    bool state = false;
    ClientReq creq;
    std::tie(state,creq) = Analyzer::parse(str);
    if(!state) {
        BOOST_LOG_TRIVIAL(error) << "[error]" << __FILE__ << " : " << __LINE__
                                 << " : Client request json format error";
        ServerManager::bad_request(connptr);
        return ;
    }
    auto res = Manager::request_from_client(creq);
    if(res||creq.flag == DELETION
            ||creq.flag == INSERTION
            ||creq.flag == UPDATION) {
        send2client(connptr,res);
    } else {
        BOOST_LOG_TRIVIAL(error) << "[error]" << __FILE__ << " : " << __LINE__
                                 << "Client request bad";
        ServerManager::bad_request(connptr);
    }

}

void ServerManager::send2client(std::shared_ptr<TcpConnection> connptr,
                                std::shared_ptr<SqlRes> res) {

    Json::Value root;
    std::shared_ptr<http::server::reply> rep =
            std::make_shared<http::server::reply>();
    rep->status = http::server::reply::status_type::ok;
    std::string out;
    if(res) {
        int nrow = (int) res->get_num_row();
        for (int i = 0; i < nrow; ++i) {
            Json::Value temp;
            auto row = res->get_cur_row();
            for (auto it = row->begin(); it != row->end(); ++it) {
                temp[(*it).first] = (*it).second.to_string();
            }
            root.append(std::move(temp));
        }
        out = root.toStyledString();
    }
    rep->content = std::move(out);
    rep->headers.resize(2);
    rep->headers[0].name = "Content-Length";
    rep->headers[0].value = std::to_string(rep->content.size());
    rep->headers[1].name = "Content-Type";
    rep->headers[1].value = "text/html";
    //BOOST_LOG_TRIVIAL(info) << "[info]" << rep->content;	
    connptr->get_socket()->get_io_service().post([connptr,rep]()
                                                 {   connptr->async_write([](
                                                         std::shared_ptr<TcpConnection> ){},
                                                                          rep->to_buffers());
                                                 });

}


void ServerManager::read_callback(std::shared_ptr<TcpConnection> connptr,
                                  std::shared_ptr<http::server::request> req,
                                  boost::string_view str) {
    auto& pool = GetThreadPool();
    pool.enqueue(ServerManager::_read_callback,
                    connptr,req,str);
}
