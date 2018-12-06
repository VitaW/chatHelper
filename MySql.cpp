//
// Created by sida liang on 2018/6/12.
//

#include <boost/utility/string_view.hpp>
#include "MySql.h"
#include "Log.h"
std::shared_ptr<SqlRes>
        MySql::get_result() {
        auto resptr = std::make_shared<SqlRes>
                (mysql_store_result(&sql_));
        resptr->prepare();
        return resptr;
}


std::string MySql::build_select_sql(const std::string &tabnames, const std::string &cols,
                                    const std::string &conditions) {
    std::string out;
    const char s[] = "select ";
    out += s;
    out += cols;
    out += " from ";
    out += tabnames;
    if(!conditions.empty()) {
        out += " where ";
        out += conditions;
    }
    return out;
}

std::string MySql::build_delete_sql(const std::string &tabnames,
                                    const std::string &conditions) {
    std::string out;
    out += "delete from ";
    out += tabnames;
    out += " where ";
    out += conditions;
    return out;
}

std::string MySql::build_insert_sql(const std::string &tabnames,
                                   const std::map<std::string, MySql::ValueMap> &info) {
    std::string out;
    out += "insert into ";
    out += tabnames;
    out += " (";
    for(auto it = info.begin();
            it != info.end(); ++it) {
        out += (*it).first;
        out += ",";
    }
    out.back() = ')';
    out += " values(";
    for(auto it = info.begin();
            it != info.end();++it) {
        if((*it).second.first == DB_STR) {
            out += "'";
            out += (*it).second.second;
            out += "'";
        } else {
            out += (*it).second.second;
        }
        out += ",";
    }
    out.back() = ')';
    return out;
}

std::string MySql::build_insert_sql(const std::string &tabnames, const std::string &cols,
                                    const std::string &conditions) {
    std::string out;
    out += "insert into ";
    out += tabnames;
    out += cols;
    out += " values";
    out += conditions;
    return out;
}

void SqlRes::prepare() {
        auto num_fields = mysql_num_fields(res_);
        auto row = mysql_fetch_row(res_);
        std::vector<std::string> fields(num_fields);
        auto num_row = mysql_num_rows(res_);
        data_.resize(num_row);
        for(int i = 0;i < num_fields;++i) {
            auto field = mysql_fetch_field(res_);
            fields[i] = std::string(field->name);
        }
        int nrow = 0;
        while(row) {
                RowRes row_res;
		//BOOST_LOG_TRIVIAL(debug) << "[debug]" << row[0];
                for(int i = 0; i < num_fields;++i) {
                        boost::string_view str(row[i],row[i]?strlen(row[i]):0);
                        row_res.insert({fields[i],std::move(str)});
                }
                data_[nrow++] = std::move(row_res);
                row = mysql_fetch_row(res_);
        }
        it_ = data_.begin();
        is_prepare = true;
}

std::string MySql::build_update_sql(const std::string &tabnames,
                                    const std::map<std::string, MySql::ValueMap> &info,
                                    const std::string& conditions) {
    std::string out;
    out += "update ";
    out += tabnames;
    out += " set ";
    for(auto it = info.begin();
            it != info.end();++it) {
        out += (*it).first;
        out += "=";
        if((*it).second.first == DB_STR) {
            out += "'";
            out += (*it).second.second;
            out += "'";
        } else {
            out += (*it).second.second;
        }
        out += ',';
    }
    out.back() = ' ';
    if(!conditions.empty()) {
        out += conditions;
    }
    return out;
}

std::string MySql::build_updae_sql(const std::string &tabnames, const std::string &cols,
                                   const std::string &conditions) {
    std::string out;
    out += "update ";
    out += tabnames;
    out += " set ";
    out += cols;
    out += " ";
    out += " where ";
    out += conditions;
    return out;
}

SqlRes::~SqlRes() {
        mysql_free_result(res_);
}

