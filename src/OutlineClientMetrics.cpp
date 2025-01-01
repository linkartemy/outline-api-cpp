#include "outline/OutlineClient.h"
#include "outline/constants/ApiEndpoint.h"
#include "outline/exceptions/OutlineExceptions.h"
#include "outline/utils/UrlUtils.h"

#include <boost/json.hpp>
#include <future>
#include <map>
#include <string>

namespace outline {
std::future<std::string> OutlineClient::getMetricsAsync() {
  return boost::asio::co_spawn(
      m_ioContext,
      [this]() -> boost::asio::awaitable<std::string> {
        auto url =
            utils::appendUrl(m_apiUrl, std::string(api::Endpoints::GetMetrics));
        auto [status, body] = co_await doGetAsync(url);
        if (status >= 400 ||
            body.find("bytesTransferredByUserId") == std::string::npos) {
          throw OutlineServerErrorException(
              "Unable to get metrics (status=" + std::to_string(status) + ")");
        }
        boost::json::value metricsVal;
        try {
          metricsVal = boost::json::parse(body);
        } catch (const std::exception& e) {
          throw OutlineParseException(
              std::string("JSON parse error for metrics: ") + e.what());
        }
        co_return boost::json::serialize(metricsVal);
      },
      boost::asio::use_future);
}

std::future<bool> OutlineClient::getMetricsStatusAsync() {
  return boost::asio::co_spawn(
      m_ioContext,
      [this]() -> boost::asio::awaitable<bool> {
        auto url = utils::appendUrl(
            m_apiUrl, std::string(api::Endpoints::GetMetricsStatus));
        auto [status, body] = co_await doGetAsync(url);
        if (status != 200) {
          throw OutlineServerErrorException(
              "Unable to get metrics status (status=" + std::to_string(status) +
              ")");
        }
        boost::json::value metricsVal;
        try {
          metricsVal = boost::json::parse(body);
        } catch (const std::exception& e) {
          throw OutlineParseException(
              std::string("JSON parse error for metrics status: ") + e.what());
        }
        if (!metricsVal.is_object() ||
            !metricsVal.as_object().contains("metricsEnabled")) {
          throw OutlineParseException(
              "Invalid JSON structure for metrics status.");
        }
        co_return metricsVal.as_object()["metricsEnabled"].as_bool();
      },
      boost::asio::use_future);
}

std::future<void> OutlineClient::setMetricsStatusAsync(bool status) {
  return boost::asio::co_spawn(
      m_ioContext,
      [this, status]() -> boost::asio::awaitable<void> {
        auto url = utils::appendUrl(
            m_apiUrl, std::string(api::Endpoints::SetMetricsStatus));
        boost::json::object metricsObj{{"metricsEnabled", status}};
        auto [statusCode, responseBody] =
            co_await doPutAsync(url, boost::json::serialize(metricsObj));
        if (statusCode != 204) {
          throw OutlineServerErrorException(
              "Unable to set metrics status (status=" +
              std::to_string(statusCode) + ")");
        }
        co_return;
      },
      boost::asio::use_future);
}

std::string OutlineClient::getMetrics() {
    return getMetricsAsync().get();
}

std::string OutlineClient::getServerInformation() {
    return getServerInformationAsync().get();
}

bool OutlineClient::getMetricsStatus() {
    return getMetricsStatusAsync().get();
}

void OutlineClient::setMetricsStatus(bool status) {
    setMetricsStatusAsync(status).get();
}
}  // namespace outline