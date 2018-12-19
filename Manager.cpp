#include "Manager.h"
#include "Log.h"
#include "MiniServer.h"


std::pair<bool,ClientReq> Analyzer::parse(boost::string_view str) {
    Json::Reader reader; 
    Json::Value root;
    std::pair<bool,ClientReq> out({false,ClientReq()});
    if(reader.parse(str.begin(),str.end(),root)) {
        BOOST_LOG_TRIVIAL(info) <<"[info]" << "Client Json request good";
        out.first = true;
        out.second = std::move(Parser::analyze(root));
    }
    return out;
}


std::shared_ptr<SqlRes> Manager::request_from_client(const ClientReq &req) {
    MySql sql;
    std::string ssql;
    auto& info = GetConnectInfo();
    if(!sql.connect(info)) {
        BOOST_LOG_TRIVIAL(error) << "[inter error]" << "mysql connect error";
    }
    /*
     * build
     */
    switch(req.flag) {
        case INSERTION:
            ssql = MySql::build_insert_sql(req.table_names,
                    req.cols,
                    req.conditions);
            break;
        case SELECTION:
            ssql = MySql::build_select_sql(req.table_names,
                    req.cols,
                    req.conditions);
            break;
        case UPDATION:
            ssql = MySql::build_updae_sql(req.table_names,
                    req.cols,
                    req.conditions);
            break;
        case DELETION:
            ssql = MySql::build_delete_sql(req.table_names,
                    req.conditions);
            break;
        default:
            BOOST_LOG_TRIVIAL(error) << "[error]" << "client request error";
    }
    std::shared_ptr<SqlRes> out;
    BOOST_LOG_TRIVIAL(info) << "[info]" << ssql;	
    if(sql.query(ssql) == 0
            &&req.flag == SELECTION)
   {
   	 out = sql.get_result();
	BOOST_LOG_TRIVIAL(info) << "[info]" << "select";
	 }	
    return out;
}

