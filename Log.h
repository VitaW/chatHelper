#ifndef CPP_LOG_HPP
#define CPP_LOG_HPP
#ifndef BOOST_ALL_DYN_LINK
#define BOOST_ALL_DYN_LINK
#endif
#include <boost/log/trivial.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

static void LOG_INIT(const std::string& file_name) {

    boost::log::add_common_attributes();
    boost::log::add_file_log(
            boost::log::keywords::auto_flush = true,
            boost::log::keywords::file_name = file_name,
    boost::log::keywords::rotation_size = 10 * 1024 * 1024,
    boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
    boost::log::keywords::format = "[%TimeStamp%]: %Message%"
    );

    boost::log::core::get()->set_filter
            (
                    boost::log::trivial::severity >= boost::log::trivial::info
            );
}




#endif
