#include "outline/OutlineClient.h"
#include "outline/constants/ApiEndpoint.h"
#include "outline/utils/UrlUtils.h"
#include "outline/exceptions/OutlineExceptions.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/asio.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/json.hpp>
#include <boost/system/error_code.hpp>
#include <boost/url.hpp>

#include <coroutine>
#include <future>

namespace outline {
using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;
namespace ssl = boost::asio::ssl;

OutlineClient::OutlineClient(std::string_view apiUrl, std::string_view cert,
                             int timeout)
    : m_cert(cert),
      m_timeout(timeout),
      m_sslContext(ssl::context::sslv23_client),
      m_workGuard(boost::asio::make_work_guard(m_ioContext)) {
  try {
    m_apiUrl = boost::urls::parse_uri(apiUrl).value();
  } catch (const std::exception& e) {
    throw OutlineParseException(std::string("Unable to parse API URL: ") +
                                e.what());
  }
  m_sslContext.set_verify_mode(ssl::verify_none);
  m_sslContext.set_default_verify_paths();
  m_ioThread = std::thread([this]() { m_ioContext.run(); });
}

OutlineClient::~OutlineClient() {
  m_workGuard.reset();
  m_ioContext.stop();
  if (m_ioThread.joinable())
    m_ioThread.join();
}
}  // namespace outline
