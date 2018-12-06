//
// Created by sida liang on 2018/6/13.
//

#ifndef MINI_MANAGER_H
#define MINI_MANAGER_H
#include <boost/utility/string_view.hpp>
#include <json/json.h>
#include "MySql.h"
#include "DBOperation.h"


class Analyzer {
public:
    static std::pair<bool,ClientReq>
            parse(boost::string_view);


};
class Manager {

public:
    static std::shared_ptr<SqlRes>
            request_from_client(const ClientReq& req);

};


#endif //MINI_MANAGER_H
