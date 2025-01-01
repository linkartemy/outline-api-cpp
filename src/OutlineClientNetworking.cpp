#include "outline/OutlineClient.h"
#include "outline/constants/ApiEndpoint.h"
#include "outline/exceptions/OutlineExceptions.h"
#include "outline/utils/UrlUtils.h"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <iostream>

namespace outline {

namespace ssl = boost::asio::ssl;
namespace http = boost::beast::http;
using tcp = boost::asio::ip::tcp;

boost::asio::awaitable<std::pair<int, std::string>> OutlineClient::doGetAsync(
    const boost::urls::url& url) {
  ssl::stream<tcp::socket> sslStream(m_ioContext, m_sslContext);
  tcp::resolver resolver(m_ioContext);
  auto results = co_await resolver.async_resolve(url.host(), url.port(),
                                                 boost::asio::use_awaitable);
  co_await boost::asio::async_connect(sslStream.next_layer(), results,
                                      boost::asio::use_awaitable);
  co_await sslStream.async_handshake(ssl::stream_base::client,
                                     boost::asio::use_awaitable);

  std::string host = url.host();
  std::string target = url.encoded_path().data();
  if (!url.encoded_query().empty()) {
    target += "?" + std::string(url.encoded_query());
  }

  http::request<http::string_body> req{http::verb::get, target, 11};
  req.set(http::field::host, host);
  req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  co_await http::async_write(sslStream, req, boost::asio::use_awaitable);

  boost::beast::flat_buffer buffer;
  http::response<http::dynamic_body> res;
  co_await http::async_read(sslStream, buffer, res, boost::asio::use_awaitable);

  boost::system::error_code ec;
  co_await sslStream.async_shutdown(
      boost::asio::redirect_error(boost::asio::use_awaitable, ec));
  if (ec && ec != boost::asio::error::eof)
    throw boost::system::system_error(ec);

  co_return std::make_pair(static_cast<int>(res.result_int()),
                           boost::beast::buffers_to_string(res.body().data()));
}

boost::asio::awaitable<std::pair<int, std::string>> OutlineClient::doPostAsync(
    const boost::urls::url& url, const std::string& body) {
  ssl::stream<tcp::socket> sslStream(m_ioContext, m_sslContext);
  tcp::resolver resolver(m_ioContext);
  auto results = co_await resolver.async_resolve(url.host(), url.port(),
                                                 boost::asio::use_awaitable);
  co_await boost::asio::async_connect(sslStream.next_layer(), results,
                                      boost::asio::use_awaitable);
  co_await sslStream.async_handshake(ssl::stream_base::client,
                                     boost::asio::use_awaitable);

  std::string host = url.host();
  std::string target = url.encoded_path().data();
  if (!url.encoded_query().empty()) {
    target += "?" + std::string(url.encoded_query());
  }

  http::request<http::string_body> req{http::verb::post, target, 11};
  req.set(http::field::host, host);
  req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  req.set(http::field::content_type, "application/json");
  req.body() = body;
  req.prepare_payload();

  co_await http::async_write(sslStream, req, boost::asio::use_awaitable);

  boost::beast::flat_buffer buffer;
  http::response<http::dynamic_body> res;
  co_await http::async_read(sslStream, buffer, res, boost::asio::use_awaitable);

  boost::system::error_code ec;
  co_await sslStream.async_shutdown(
      boost::asio::redirect_error(boost::asio::use_awaitable, ec));
  if (ec && ec != boost::asio::error::eof)
    throw boost::system::system_error(ec);

  co_return std::make_pair(static_cast<int>(res.result_int()),
                           boost::beast::buffers_to_string(res.body().data()));
}

boost::asio::awaitable<std::pair<int, std::string>> OutlineClient::doPutAsync(
    const boost::urls::url& url, const std::string& body) {
  ssl::stream<tcp::socket> sslStream(m_ioContext, m_sslContext);
  tcp::resolver resolver(m_ioContext);
  auto results = co_await resolver.async_resolve(url.host(), url.port(),
                                                 boost::asio::use_awaitable);
  co_await boost::asio::async_connect(sslStream.next_layer(), results,
                                      boost::asio::use_awaitable);
  co_await sslStream.async_handshake(ssl::stream_base::client,
                                     boost::asio::use_awaitable);

  std::string host = url.host();
  std::string target = url.encoded_path().data();
  if (!url.encoded_query().empty()) {
    target += "?" + std::string(url.encoded_query());
  }

  http::request<http::string_body> req{http::verb::put, target, 11};
  req.set(http::field::host, host);
  req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  req.set(http::field::content_type, "application/json");
  req.body() = body;
  req.prepare_payload();

  co_await http::async_write(sslStream, req, boost::asio::use_awaitable);

  boost::beast::flat_buffer buffer;
  http::response<http::dynamic_body> res;
  co_await http::async_read(sslStream, buffer, res, boost::asio::use_awaitable);

  boost::system::error_code ec;
  co_await sslStream.async_shutdown(
      boost::asio::redirect_error(boost::asio::use_awaitable, ec));
  if (ec && ec != boost::asio::error::eof)
    throw boost::system::system_error(ec);

  co_return std::make_pair(static_cast<int>(res.result_int()),
                           boost::beast::buffers_to_string(res.body().data()));
}

boost::asio::awaitable<std::pair<int, std::string>>
OutlineClient::doDeleteAsync(const boost::urls::url& url) {
  ssl::stream<tcp::socket> sslStream(m_ioContext, m_sslContext);
  tcp::resolver resolver(m_ioContext);
  auto results = co_await resolver.async_resolve(url.host(), url.port(),
                                                 boost::asio::use_awaitable);
  co_await boost::asio::async_connect(sslStream.next_layer(), results,
                                      boost::asio::use_awaitable);
  co_await sslStream.async_handshake(ssl::stream_base::client,
                                     boost::asio::use_awaitable);

  std::string host = url.host();
  std::string target = url.encoded_path().data();
  if (!url.encoded_query().empty()) {
    target += "?" + std::string(url.encoded_query());
  }

  http::request<http::string_body> req{http::verb::delete_, target, 11};
  req.set(http::field::host, host);
  req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  co_await http::async_write(sslStream, req, boost::asio::use_awaitable);

  boost::beast::flat_buffer buffer;
  http::response<http::dynamic_body> res;
  co_await http::async_read(sslStream, buffer, res, boost::asio::use_awaitable);

  boost::system::error_code ec;
  co_await sslStream.async_shutdown(
      boost::asio::redirect_error(boost::asio::use_awaitable, ec));
  if (ec && ec != boost::asio::error::eof)
    throw boost::system::system_error(ec);

  co_return std::make_pair(static_cast<int>(res.result_int()),
                           boost::beast::buffers_to_string(res.body().data()));
}

}  // namespace outline
