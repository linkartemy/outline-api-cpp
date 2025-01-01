#include "outline/OutlineClient.h"
#include "outline/constants/ApiEndpoint.h"
#include "outline/exceptions/OutlineExceptions.h"
#include "outline/utils/UrlUtils.h"

#include <boost/json.hpp>
#include <future>
#include <map>
#include <string>

namespace outline {
std::future<std::string> OutlineClient::getServerInformationAsync() {
  return boost::asio::co_spawn(
      m_ioContext,
      [this]() -> boost::asio::awaitable<std::string> {
        auto url = utils::appendUrl(
            m_apiUrl, std::string(api::Endpoints::GetServerInformation));
        auto [status, body] = co_await doGetAsync(url);
        if (status != 200) {
          throw OutlineServerErrorException(
              "Unable to get server information (status=" +
              std::to_string(status) + ")");
        }
        boost::json::value serverVal;
        try {
          serverVal = boost::json::parse(body);
        } catch (const std::exception& e) {
          throw OutlineParseException(
              std::string("JSON parse error for server: ") + e.what());
        }
        co_return boost::json::serialize(serverVal);
      },
      boost::asio::use_future);
}

std::future<void> OutlineClient::setServerNameAsync(
    const std::string& serverName) {
  return boost::asio::co_spawn(
      m_ioContext,
      [this, serverName]() -> boost::asio::awaitable<void> {
        auto url = utils::appendUrl(m_apiUrl,
                                    std::string(api::Endpoints::SetServerName));
        boost::json::object serverObj{{"name", serverName}};
        auto [status, responseBody] =
            co_await doPutAsync(url, boost::json::serialize(serverObj));
        if (status != 204) {
          throw OutlineServerErrorException(
              "Unable to set server name (status=" + std::to_string(status) +
              ")");
        }
        co_return;
      },
      boost::asio::use_future);
}

std::future<void> OutlineClient::setHostNameAsync(const std::string& hostName) {
  return boost::asio::co_spawn(
      m_ioContext,
      [this, hostName]() -> boost::asio::awaitable<void> {
        auto url = utils::appendUrl(m_apiUrl,
                                    std::string(api::Endpoints::SetHostName));
        boost::json::object hostObj{{"hostname", hostName}};
        auto [status, responseBody] =
            co_await doPutAsync(url, boost::json::serialize(hostObj));
        if (status != 204) {
          throw OutlineServerErrorException("Unable to set host name (status=" +
                                            std::to_string(status) + ")");
        }
        co_return;
      },
      boost::asio::use_future);
}

std::future<void> OutlineClient::setDefaultPortAsync(int port) {
  return boost::asio::co_spawn(
      m_ioContext,
      [this, port]() -> boost::asio::awaitable<void> {
        auto url = utils::appendUrl(
            m_apiUrl, std::string(api::Endpoints::SetDefaultPort));
        boost::json::object portObj{{"port", port}};
        auto [status, responseBody] =
            co_await doPutAsync(url, boost::json::serialize(portObj));
        if (status == 400) {
          throw OutlineServerErrorException(
              "The requested port isn't valid or missing.");
        }
        if (status == 409) {
          throw OutlineServerErrorException(
              "The requested port is already in use.");
        }
        if (status != 204) {
          throw OutlineServerErrorException(
              "Unable to set default port (status=" + std::to_string(status) +
              ")");
        }
        co_return;
      },
      boost::asio::use_future);
}

std::future<void> OutlineClient::setDataLimitForAllAccessKeysAsync(
    int dataLimitBytes) {
  return boost::asio::co_spawn(
      m_ioContext,
      [this, dataLimitBytes]() -> boost::asio::awaitable<void> {
        auto url = utils::appendUrl(
            m_apiUrl,
            std::string(api::Endpoints::SetDataLimitForAllAccessKeys));
        boost::json::object dataLimitObj{{"bytes", dataLimitBytes}};
        auto [status, responseBody] =
            co_await doPutAsync(url, boost::json::serialize(dataLimitObj));
        if (status != 204) {
          throw OutlineServerErrorException(
              "Unable to set data limit for all (status=" +
              std::to_string(status) + ")");
        }
        co_return;
      },
      boost::asio::use_future);
}

std::future<void> OutlineClient::deleteDataLimitForAllAccessKeysAsync() {
  return boost::asio::co_spawn(
      m_ioContext,
      [this]() -> boost::asio::awaitable<void> {
        auto url = utils::appendUrl(
            m_apiUrl,
            std::string(api::Endpoints::DeleteDataLimitForAllAccessKeys));
        auto [status, responseBody] = co_await doDeleteAsync(url);
        if (status != 204) {
          throw OutlineServerErrorException(
              "Unable to delete data limit for all (status=" +
              std::to_string(status) + ")");
        }
        co_return;
      },
      boost::asio::use_future);
}

void OutlineClient::setServerName(const std::string& serverName) {
    setServerNameAsync(serverName).get();
}

void OutlineClient::setHostName(const std::string& hostName) {
    setHostNameAsync(hostName).get();
}

void OutlineClient::setDefaultPort(int port) {
    setDefaultPortAsync(port).get();
}

void OutlineClient::setDataLimitForAllAccessKeys(int dataLimitBytes) {
    setDataLimitForAllAccessKeysAsync(dataLimitBytes).get();
}

void OutlineClient::deleteDataLimitForAllAccessKeys() {
    deleteDataLimitForAllAccessKeysAsync().get();
}
}  // namespace outline
