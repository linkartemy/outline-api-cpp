#include "outline/OutlineClient.h"
#include "outline/constants/ApiEndpoint.h"
#include "outline/utils/UrlUtils.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/json.hpp>
#include <boost/system/error_code.hpp>
#include <boost/url.hpp>

namespace outline {

using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;

OutlineClient::OutlineClient(std::string_view apiUrl, std::string_view cert,
                             int timeout)
    : m_cert(cert),
      m_timeout(timeout),
      m_sslContext(boost::asio::ssl::context::sslv23_client),
      m_sslStream(m_ioContext, m_sslContext) {
  try {
    m_apiUrl = boost::urls::parse_uri(apiUrl).value();
  } catch (const std::exception& e) {
    throw OutlineParseException(std::string("Unable to parse API URL: ") +
                                e.what());
  }

  m_sslContext.set_verify_mode(boost::asio::ssl::verify_none);
  m_sslContext.set_default_verify_paths();
  m_socket = std::make_unique<boost::asio::ip::tcp::socket>(m_ioContext);
}

OutlineClient::~OutlineClient() {
  boost::system::error_code ec;
  m_sslStream.shutdown(ec);
  if (ec && ec != boost::asio::error::not_connected) {
    std::cerr << "Shutdown error: " << ec.message() << std::endl;
  }
}

std::string OutlineClient::getAccessKeys() {
  m_apiUrl =
      utils::appendUrl(m_apiUrl, std::string(api::Endpoints::GetAccessKeys));
  auto [statusKeys, bodyKeys] = doGet(m_apiUrl);

  if (statusKeys != 200) {
    throw OutlineServerErrorException(
        "Unable to get keys (status=" + std::to_string(statusKeys) + ")");
  }

  boost::json::value keysVal;
  try {
    keysVal = boost::json::parse(bodyKeys);
  } catch (const std::exception& e) {
    throw OutlineParseException(std::string("JSON parse error for keys: ") +
                                e.what());
  }

  return boost::json::serialize(keysVal);
}

std::string OutlineClient::getAccessKey(std::string& accessKeyId) {
  std::map<std::string, std::string> placeholders;
  placeholders[std::string(api::UrlParams::KeyId)] = accessKeyId;
  std::string endpoint = utils::replacePlaceholders(
      std::string(api::Endpoints::GetAccessKeyById), placeholders);
  m_apiUrl = utils::appendUrl(m_apiUrl, endpoint);
  auto [statusKey, bodyKey] = doGet(m_apiUrl);

  if (statusKey != 200) {
    throw OutlineServerErrorException(
        "Unable to get key (status=" + std::to_string(statusKey) + ")");
  }

  boost::json::value keyVal;
  try {
    keyVal = boost::json::parse(bodyKey);
  } catch (const std::exception& e) {
    throw OutlineParseException(std::string("JSON parse error for key: ") +
                                e.what());
  }

  return boost::json::serialize(keyVal);
}

std::string OutlineClient::createAccessKey(CreateAccessKeyParams& params) {
  m_apiUrl =
      utils::appendUrl(m_apiUrl, std::string(api::Endpoints::CreateAccessKey));

  boost::json::object keyObj;
  if (params.name.has_value()) {
    keyObj["name"] = params.name.value();
  }
  if (params.password.has_value()) {
    keyObj["password"] = params.password.value();
  }
  if (params.method.has_value()) {
    keyObj["method"] = params.method.value();
  }
  if (params.data_limit_bytes.has_value()) {
    boost::json::object dataLimitObj;
    dataLimitObj["bytes"] = params.data_limit_bytes.value();
    keyObj["limit"] = dataLimitObj;
  }

  std::string keyStr = boost::json::serialize(keyObj);
  auto [statusKey, bodyKey] = doPost(m_apiUrl, keyStr);

  if (statusKey != 201) {
    throw OutlineServerErrorException(
        "Unable to create key (status=" + std::to_string(statusKey) + ")");
  }

  boost::json::value keyVal;
  try {
    keyVal = boost::json::parse(bodyKey);
  } catch (const std::exception& e) {
    throw OutlineParseException(std::string("JSON parse error for key: ") +
                                e.what());
  }

  return boost::json::serialize(keyVal);
}

std::string OutlineClient::updateAccessKey(int accessKeyId,
                                           UpdateAccessKeyParams& params) {
  std::map<std::string, std::string> placeholders;
  placeholders[std::string(api::UrlParams::KeyId)] = accessKeyId;
  std::string endpoint = utils::replacePlaceholders(
      std::string(api::Endpoints::UpdateAccessKey), placeholders);
  m_apiUrl = utils::appendUrl(m_apiUrl, endpoint);

  boost::json::object keyObj;
  if (params.name.has_value()) {
    keyObj["name"] = params.name.value();
  }
  if (params.password.has_value()) {
    keyObj["password"] = params.password.value();
  }
  if (params.method.has_value()) {
    keyObj["method"] = params.method.value();
  }
  if (params.data_limit_bytes.has_value()) {
    boost::json::object dataLimitObj;
    dataLimitObj["bytes"] = params.data_limit_bytes.value();
    keyObj["limit"] = dataLimitObj;
  }

  std::string keyStr = boost::json::serialize(keyObj);
  auto [statusKey, bodyKey] = doPut(m_apiUrl, keyStr);

  if (statusKey != 201) {
    throw OutlineServerErrorException(
        "Unable to update key (status=" + std::to_string(statusKey) + ")");
  }

  boost::json::value keyVal;
  try {
    keyVal = boost::json::parse(bodyKey);
  } catch (const std::exception& e) {
    throw OutlineParseException(std::string("JSON parse error for key: ") +
                                e.what());
  }

  return boost::json::serialize(keyVal);
}

void OutlineClient::deleteAccessKey(std::string& accessKeyId) {
  std::map<std::string, std::string> placeholders;
  placeholders[std::string(api::UrlParams::KeyId)] = accessKeyId;
  std::string endpoint = utils::replacePlaceholders(
      std::string(api::Endpoints::DeleteAccessKey), placeholders);
  m_apiUrl = utils::appendUrl(m_apiUrl, endpoint);
  auto [statusKey, bodyKey] = doDelete(m_apiUrl);

  if (statusKey != 204) {
    throw OutlineServerErrorException(
        "Unable to delete key (status=" + std::to_string(statusKey) + ")");
  }
}

void OutlineClient::renameAccessKey(std::string& accessKeyId,
                                    std::string& newName) {
  std::map<std::string, std::string> placeholders;
  placeholders[std::string(api::UrlParams::KeyId)] = accessKeyId;
  std::string endpoint = utils::replacePlaceholders(
      std::string(api::Endpoints::RenameAccessKey), placeholders);
  m_apiUrl = utils::appendUrl(m_apiUrl, endpoint);

  boost::json::object keyObj;
  keyObj["name"] = newName;
  std::string keyStr = boost::json::serialize(keyObj);
  auto [statusKey, bodyKey] = doPut(m_apiUrl, keyStr);

  if (statusKey != 204) {
    throw OutlineServerErrorException(
        "Unable to rename key (status=" + std::to_string(statusKey) + ")");
  }
}

void OutlineClient::addDataLimit(std::string& accessKeyId, int dataLimitBytes) {
  std::map<std::string, std::string> placeholders;
  placeholders[std::string(api::UrlParams::KeyId)] = accessKeyId;
  std::string endpoint = utils::replacePlaceholders(
      std::string(api::Endpoints::AddDataLimit), placeholders);
  m_apiUrl = utils::appendUrl(m_apiUrl, endpoint);

  boost::json::object dataLimitObj;
  dataLimitObj["bytes"] = dataLimitBytes;
  std::string dataLimitStr = boost::json::serialize(dataLimitObj);
  auto [statusKey, bodyKey] = doPut(m_apiUrl, dataLimitStr);

  if (statusKey != 204) {
    throw OutlineServerErrorException(
        "Unable to add data limit (status=" + std::to_string(statusKey) + ")");
  }
}

void OutlineClient::deleteDataLimit(std::string& accessKeyId) {
  std::map<std::string, std::string> placeholders;
  placeholders[std::string(api::UrlParams::KeyId)] = accessKeyId;
  std::string endpoint = utils::replacePlaceholders(
      std::string(api::Endpoints::DeleteDataLimit), placeholders);
  m_apiUrl = utils::appendUrl(m_apiUrl, endpoint);
  auto [statusKey, bodyKey] = doDelete(m_apiUrl);

  if (statusKey != 204) {
    throw OutlineServerErrorException(
        "Unable to delete data limit (status=" + std::to_string(statusKey) +
        ")");
  }
}

std::string OutlineClient::getMetrics() {
  m_apiUrl =
      utils::appendUrl(m_apiUrl, std::string(api::Endpoints::GetMetrics));
  auto [statusMetrics, bodyMetrics] = doGet(m_apiUrl);

  if (statusMetrics >= 400 || bodyMetrics.find("bytesTransferredByUserId") == std::string::npos) {
    throw OutlineServerErrorException(
        "Unable to get metrics (status=" + std::to_string(statusMetrics) + ")");
  }

  boost::json::value metricsVal;
  try {
    metricsVal = boost::json::parse(bodyMetrics);
  } catch (const std::exception& e) {
    throw OutlineParseException(std::string("JSON parse error for metrics: ") +
                                e.what());
  }

  return boost::json::serialize(metricsVal);
}

std::string OutlineClient::getServerInformation() {
  m_apiUrl = utils::appendUrl(m_apiUrl, std::string(api::Endpoints::GetServerInformation));
  auto [statusServer, bodyServer] = doGet(m_apiUrl);

  if (statusServer != 200) {
    throw OutlineServerErrorException(
        "Unable to get server information (status=" + std::to_string(statusServer) + ")");
  }

  boost::json::value serverVal;
  try {
    serverVal = boost::json::parse(bodyServer);
  } catch (const std::exception& e) {
    throw OutlineParseException(std::string("JSON parse error for server: ") +
                                e.what());
  }

  return boost::json::serialize(serverVal);
}

void OutlineClient::setServerName(std::string& serverName) {
  m_apiUrl = utils::appendUrl(m_apiUrl, std::string(api::Endpoints::SetServerName));

  boost::json::object serverObj;
  serverObj["name"] = serverName;
  std::string serverStr = boost::json::serialize(serverObj);
  auto [statusServer, bodyServer] = doPut(m_apiUrl, serverStr);

  if (statusServer != 204) {
    throw OutlineServerErrorException(
        "Unable to set server name (status=" + std::to_string(statusServer) + ")");
  }
}

void OutlineClient::setHostName(std::string& hostName) {
  m_apiUrl = utils::appendUrl(m_apiUrl, std::string(api::Endpoints::SetHostName));

  boost::json::object serverObj;
  serverObj["hostname"] = hostName;
  std::string serverStr = boost::json::serialize(serverObj);
  auto [statusServer, bodyServer] = doPut(m_apiUrl, serverStr);

  if (statusServer != 204) {
    throw OutlineServerErrorException(
        "Unable to set host name (status=" + std::to_string(statusServer) + ")");
  }
}

bool OutlineClient::getMetricsStatus() {
  m_apiUrl = utils::appendUrl(m_apiUrl, std::string(api::Endpoints::GetMetricsStatus));
  auto [statusMetrics, bodyMetrics] = doGet(m_apiUrl);

  if (statusMetrics != 200) {
    throw OutlineServerErrorException(
        "Unable to get metrics status (status=" + std::to_string(statusMetrics) + ")");
  }

  boost::json::value metricsVal;
  try {
    metricsVal = boost::json::parse(bodyMetrics);
  } catch (const std::exception& e) {
    throw OutlineParseException(std::string("JSON parse error for metrics: ") +
                                e.what());
  }

  return metricsVal.as_object()["metricsEnabled"].as_bool();
}

void OutlineClient::setMetricsStatus(bool status) {
  m_apiUrl = utils::appendUrl(m_apiUrl, std::string(api::Endpoints::SetMetricsStatus));

  boost::json::object metricsObj;
  metricsObj["metricsEnabled"] = status;
  std::string metricsStr = boost::json::serialize(metricsObj);
  auto [statusMetrics, bodyMetrics] = doPut(m_apiUrl, metricsStr);

  if (statusMetrics != 204) {
    throw OutlineServerErrorException(
        "Unable to set metrics status (status=" + std::to_string(statusMetrics) + ")");
  }
}

void OutlineClient::setDefaultPort(int port) {
  m_apiUrl = utils::appendUrl(m_apiUrl, std::string(api::Endpoints::SetDefaultPort));

  boost::json::object portObj;
  portObj["port"] = port;
  std::string portStr = boost::json::serialize(portObj);
  auto [statusPort, bodyPort] = doPut(m_apiUrl, portStr);

  if (statusPort == 400) {
    throw OutlineServerErrorException(
        "The requested port isn't an integer from 1 through 65535, or the request had no port parameter.");
  }
  if (statusPort == 409) {
    throw OutlineServerErrorException(
        "The requested port is already in use.");
  }
  if (statusPort != 204) {
    throw OutlineServerErrorException(
        "Unable to set default port (status=" + std::to_string(statusPort) + ")");
  }
}

void OutlineClient::setDataLimitForAllAccessKeys(int dataLimitBytes) {
  m_apiUrl = utils::appendUrl(m_apiUrl, std::string(api::Endpoints::SetDataLimitForAllAccessKeys));

  boost::json::object dataLimitObj;
  dataLimitObj["bytes"] = dataLimitBytes;
  std::string dataLimitStr = boost::json::serialize(dataLimitObj);
  auto [statusDataLimit, bodyDataLimit] = doPut(m_apiUrl, dataLimitStr);

  if (statusDataLimit != 204) {
    throw OutlineServerErrorException(
        "Unable to set data limit for all access keys (status=" + std::to_string(statusDataLimit) + ")");
  }
}

void OutlineClient::deleteDataLimitForAllAccessKeys() {
  m_apiUrl = utils::appendUrl(m_apiUrl, std::string(api::Endpoints::deleteDataLimitForAllAccessKeys));
  auto [statusDataLimit, bodyDataLimit] = doDelete(m_apiUrl);

  if (statusDataLimit != 204) {
    throw OutlineServerErrorException(
        "Unable to delete data limit for all access keys (status=" + std::to_string(statusDataLimit) + ")");
  }
}

std::pair<int, std::string> OutlineClient::doGet(const boost::urls::url& url) {
  return doHttpsGet(url);
}

std::pair<int, std::string> OutlineClient::doHttpsGet(
    const boost::urls::url& url) {
  tcp::resolver resolver(m_ioContext);
  auto const results_resolve = resolver.resolve(url.host(), url.port());

  boost::asio::connect(m_sslStream.next_layer(), results_resolve);

  m_sslStream.handshake(boost::asio::ssl::stream_base::client);

  std::string host = url.host();
  std::string target = url.encoded_path().data();
  if (!url.encoded_query().empty()) {
    target += "?";
    target += url.encoded_query();
  }

  http::request<http::string_body> req{http::verb::get, target, 11};
  req.set(http::field::host, host);
  req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

  http::write(m_sslStream, req);

  boost::beast::flat_buffer buffer;
  http::response<http::dynamic_body> res;
  http::read(m_sslStream, buffer, res);

  boost::system::error_code ec;
  m_sslStream.shutdown(ec);
  if (ec && ec != boost::asio::error::eof) {
    throw boost::system::system_error(ec);
  }

  int statusCode = static_cast<int>(res.result_int());
  std::string bodyStr = boost::beast::buffers_to_string(res.body().data());

  return {statusCode, bodyStr};
}

std::pair<int, std::string> OutlineClient::doPut(const boost::urls::url& url,
                                                 const std::string& body) {
  return doHttpsPut(url, body);
}

std::pair<int, std::string> OutlineClient::doHttpsPut(
    const boost::urls::url& url, const std::string& body) {
  tcp::resolver resolver(m_ioContext);
  auto const results_resolve = resolver.resolve(url.host(), url.port());

  boost::asio::connect(m_sslStream.next_layer(), results_resolve);

  m_sslStream.handshake(boost::asio::ssl::stream_base::client);

  std::string host = url.host();
  std::string target = url.encoded_path().data();
  if (!url.encoded_query().empty()) {
    target += "?";
    target += url.encoded_query();
  }

  http::request<http::string_body> req{http::verb::put, target, 11};
  req.set(http::field::host, host);
  req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  req.set(http::field::content_type, "application/json");
  req.body() = body;
  req.prepare_payload();

  http::write(m_sslStream, req);

  boost::beast::flat_buffer buffer;
  http::response<http::dynamic_body> res;
  http::read(m_sslStream, buffer, res);

  boost::system::error_code ec;
  m_sslStream.shutdown(ec);
  if (ec && ec != boost::asio::error::eof) {
    throw boost::system::system_error(ec);
  }

  int statusCode = static_cast<int>(res.result_int());
  std::string bodyStr = boost::beast::buffers_to_string(res.body().data());

  return {statusCode, bodyStr};
}

std::pair<int, std::string> OutlineClient::doPost(const boost::urls::url& url,
                                                  const std::string& body) {
  return doHttpsPost(url, body);
}

std::pair<int, std::string> OutlineClient::doHttpsPost(
    const boost::urls::url& url, const std::string& body) {
  boost::system::error_code ec;

  tcp::resolver resolver(m_ioContext);
  auto const results = resolver.resolve(url.host(), url.port(), ec);
  if (ec) {
    throw boost::system::system_error(ec);
  }

  boost::asio::connect(m_sslStream.next_layer(), results, ec);
  if (ec) {
    throw boost::system::system_error(ec);
  }

  m_sslStream.handshake(boost::asio::ssl::stream_base::client, ec);
  if (ec) {
    throw boost::system::system_error(ec);
  }

  std::string target = url.encoded_path().data();
  if (!url.encoded_query().empty()) {
    target += "?" + std::string(url.encoded_query());
  }

  boost::beast::http::request<boost::beast::http::string_body> req{
      boost::beast::http::verb::post, target, 11};
  req.set(boost::beast::http::field::host, url.host());
  req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  req.set(boost::beast::http::field::content_type, "application/json");
  req.body() = body;
  req.prepare_payload();

  boost::beast::http::write(m_sslStream, req, ec);
  if (ec) {
    throw boost::system::system_error(ec);
  }

  boost::beast::flat_buffer buffer;
  boost::beast::http::response<boost::beast::http::dynamic_body> res;

  boost::beast::http::read(m_sslStream, buffer, res, ec);
  if (ec && ec != boost::beast::http::error::end_of_stream) {
    throw boost::system::system_error(ec);
  }

  m_sslStream.shutdown(ec);
  if (ec && ec != boost::asio::error::eof) {
    throw boost::system::system_error(ec);
  }

  int statusCode = static_cast<int>(res.result_int());
  std::string bodyStr = boost::beast::buffers_to_string(res.body().data());

  return {statusCode, bodyStr};
}

std::pair<int, std::string> OutlineClient::doDelete(
    const boost::urls::url& url) {
  return doHttpsDelete(url);
}

std::pair<int, std::string> OutlineClient::doHttpsDelete(
    const boost::urls::url& url) {
  tcp::resolver resolver(m_ioContext);
  auto const results_resolve = resolver.resolve(url.host(), url.port());

  boost::asio::connect(m_sslStream.next_layer(), results_resolve);

  m_sslStream.handshake(boost::asio::ssl::stream_base::client);

  std::string host = url.host();
  std::string target = url.encoded_path().data();
  if (!url.encoded_query().empty()) {
    target += "?";
    target += url.encoded_query();
  }

  http::request<http::string_body> req{http::verb::delete_, target, 11};
  req.set(http::field::host, host);
  req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

  http::write(m_sslStream, req);

  boost::beast::flat_buffer buffer;
  http::response<http::dynamic_body> res;
  http::read(m_sslStream, buffer, res);

  boost::system::error_code ec;
  m_sslStream.shutdown(ec);
  if (ec && ec != boost::asio::error::eof) {
    throw boost::system::system_error(ec);
  }

  int statusCode = static_cast<int>(res.result_int());
  std::string bodyStr = boost::beast::buffers_to_string(res.body().data());

  return {statusCode, bodyStr};
}

}  // namespace outline