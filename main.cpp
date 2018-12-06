#include <iostream>
#include <boost/asio.hpp>
#include "request.hpp"
#include "request_parser.hpp"
#include <string>
#include <tuple>
#include "MiniServer.h"
#include <boost/log/trivial.hpp>
#include "Log.h"
#include "request.hpp"
#include "reply.hpp"
#include "TcpConnection.h"
#include "MySql.h"


void test_readcallback(std::shared_ptr<TcpConnection> connptr,
                        std::shared_ptr<http::server::request> req,
                        boost::string_view str) {
    std::cout << str;
    auto rep = std::make_shared<http::server::reply>
            (http::server::reply::stock_reply(
            http::server::reply::status_type::hello
            ));
    connptr->async_write([rep](std::shared_ptr<TcpConnection>){},
    rep->to_buffers());
}


using namespace boost::asio;
int main() {
   /* MySql::init();
    MySql sql;
    if(sql.connect("localhost","root","xieqiuhan123","my_first",3306))
        std::cout << " 11";
    sql.query("select * from test");
    auto res = sql.get_result();
    sql.query("select id from test");
    auto l = sql.get_result();
    std::map<std::string,std::pair<FT,std::string>> map;
    map.insert({"id",{DB_INT,"20"}});
    auto str = MySql::build_update_sql("test",map,"where id=10");*/
    LOG_INIT("log.test");
    io_service service;
    HttpServer server(service,12345);
    server.set_readcallback(ServerManager::read_callback);
    server.start();
    while(1) {
        try {
    		service.run();
	}catch(...) {
	      			
		}
    }
}
