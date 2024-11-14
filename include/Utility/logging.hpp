#ifndef INCLUDED_UTILTIY_LOGGING
#define INCLUDED_UTILTIY_LOGGING

#define BOOST_LOG_DYN_LINK 1

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <fstream>
#include <iomanip>
#include <ios>

namespace utility::logging
{

namespace log   = boost::log;
namespace src   = boost::log::sources;
namespace expr  = boost::log::expressions;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;

#define LOGGING_UTILITY_SCOPED_LOG_TIMELINE                                              \
    BOOST_LOG_SCOPED_THREAD_ATTR("Timeline", attrs::timer());

#define LOGGING_UTILITY_SCOPED_ADD_TAG(tag)                                              \
    BOOST_LOG_SCOPED_THREAD_ATTR("Tag", attrs::constant<std::string>(tag));

// Define our own severity levels
enum severity_level
{
    trace,
    debug,
    info,
    important_info,
    warning,
    error,
    fatal,
};

// Enable streaming of severity_level enum
auto operator<<(std::ostream& strm, severity_level level) -> std::ostream&
{
    static const std::string_view strings[] = { "trace",          "debug",   "info",
                                                "important info", "warning", "error",
                                                "fatal" };
    if (static_cast<std::size_t>(level) < sizeof(strings) / sizeof(*strings))
    {
        const auto s = strings[level];
        strm << "<" << s << std::setw(static_cast<int>(9uz - s.size()))
             << std::setfill(' ') << ">";
    }
    else
    {
        strm << "<" << static_cast<int>(level) << ">";
    }
    return strm;
}

// Define attribute keywords
BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(tag_attr, "Tag", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(scope, "Scope", attrs::named_scope::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(timeline, "Timeline", attrs::timer::value_type)

//[ example_tutorial_formatters_stream_date_time
auto init() -> void
{
    using text_sink   = sinks::synchronous_sink<sinks::text_ostream_backend>;
    auto general_sink = boost::make_shared<text_sink>();

    general_sink->locked_backend()->add_stream(
        boost::make_shared<std::ofstream>("logs/general.log")
    );

    general_sink->set_formatter(
        expr::stream << std::setw(5) << std::left << std::dec << std::setfill(' ')
                     << expr::attr<unsigned int>("LineID") << " "
                     << expr::format_date_time<boost::posix_time::ptime>(
                            "TimeStamp", "  %Y-%m-%d %H:%M:%S    "
                        )
                     << severity
                     << expr::if_(expr::has_attr(tag_attr)
                        )[expr::stream << "  [" << tag_attr << "]"]
                     << expr::if_(expr::has_attr(timeline)
                        )[expr::stream << "  [" << timeline << "]"]
                     << "  " << expr::smessage
    );

    auto errors_sink = boost::make_shared<text_sink>();
    errors_sink->locked_backend()->add_stream(
        boost::make_shared<std::ofstream>("logs/errors.log")
    );

    errors_sink->set_formatter(
        expr::stream << std::setw(5) << std::left << std::dec << std::setfill(' ')
                     << expr::attr<unsigned int>("LineID") << " "
                     << expr::format_date_time<boost::posix_time::ptime>(
                            "TimeStamp", "  %Y-%m-%d %H:%M:%S    "
                        )
                     << severity
                     << expr::if_(expr::has_attr(tag_attr)
                        )[expr::stream << "  [" << tag_attr << "]"]
                     << expr::if_(expr::has_attr(timeline)
                        )[expr::stream << "  [" << timeline << "]"]
                     << "  " << expr::smessage
    );
    errors_sink->set_filter(severity >= severity_level::warning);

    log::core::get()->add_sink(general_sink);
    log::core::get()->add_sink(errors_sink);

    log::add_common_attributes();
}

class default_source
{
public:
    inline static auto log(severity_level sev, std::string_view message) -> void
    {
        static src::severity_logger<severity_level> lg;
        BOOST_LOG_SEV(lg, sev) << message;
    }
};

} // namespace utility::logging

#endif // INCLUDED_UTILTIY_LOGGING
