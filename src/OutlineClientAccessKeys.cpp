#include "outline/OutlineClient.h"
#include "outline/constants/ApiEndpoint.h"
#include "outline/exceptions/OutlineExceptions.h"
#include "outline/utils/UrlUtils.h"

#include <boost/json.hpp>
#include <future>
#include <map>
#include <string>

namespace outline {

std::future<std::string> OutlineClient::getAccessKeysAsync() {
  return boost::asio::co_spawn(
      m_ioContext,
      [this]() -> boost::asio::awaitable<std::string> {
        auto url = utils::appendUrl(m_apiUrl,
                                    std::string(api::Endpoints::GetAccessKeys));
        auto [status, body] = co_await doGetAsync(url);
        if (status != 200) {
          throw OutlineServerErrorException(
              "Unable to get access keys (status=" + std::to_string(status) +
              ")");
        }
        boost::json::value keysVal;
        try {
          keysVal = boost::json::parse(body);
        } catch (const std::exception& e) {
          throw OutlineParseException(
              std::string("JSON parse error for access keys: ") + e.what());
        }
        co_return boost::json::serialize(keysVal);
      },
      boost::asio::use_future);
}

std::future<std::string> OutlineClient::getAccessKeyAsync(
    const std::string& accessKeyId) {
  return boost::asio::co_spawn(
      m_ioContext,
      [this, accessKeyId]() -> boost::asio::awaitable<std::string> {
        std::map<std::string, std::string> placeholders{{"keyId", accessKeyId}};
        auto url = utils::appendUrl(
            m_apiUrl,
            utils::replacePlaceholders(
                std::string(api::Endpoints::GetAccessKeyById), placeholders));
        auto [status, body] = co_await doGetAsync(url);
        if (status != 200) {
          throw OutlineServerErrorException(
              "Unable to get access key (status=" + std::to_string(status) +
              ")");
        }
        boost::json::value keyVal;
        try {
          keyVal = boost::json::parse(body);
        } catch (const std::exception& e) {
          throw OutlineParseException(
              std::string("JSON parse error for access key: ") + e.what());
        }
        co_return boost::json::serialize(keyVal);
      },
      boost::asio::use_future);
}

std::future<std::string> OutlineClient::createAccessKeyAsync(
    const CreateAccessKeyParams& params) {
  return boost::asio::co_spawn(
      m_ioContext,
      [this, params]() -> boost::asio::awaitable<std::string> {
        auto url = utils::appendUrl(
            m_apiUrl, std::string(api::Endpoints::CreateAccessKey));
        boost::json::object keyObj;
        if (params.name)
          keyObj["name"] = params.name.value();
        if (params.password)
          keyObj["password"] = params.password.value();
        if (params.method)
          keyObj["method"] = params.method.value();
        if (params.data_limit_bytes) {
          boost::json::object dataLimitObj{
              {"bytes", params.data_limit_bytes.value()}};
          keyObj["limit"] = dataLimitObj;
        }
        auto [status, responseBody] =
            co_await doPostAsync(url, boost::json::serialize(keyObj));
        if (status != 201) {
          throw OutlineServerErrorException(
              "Unable to create access key (status=" + std::to_string(status) +
              ")");
        }
        boost::json::value keyVal;
        try {
          keyVal = boost::json::parse(responseBody);
        } catch (const std::exception& e) {
          throw OutlineParseException(
              std::string("JSON parse error for access key creation: ") +
              e.what());
        }
        co_return boost::json::serialize(keyVal);
      },
      boost::asio::use_future);
}

std::future<std::string> OutlineClient::updateAccessKeyAsync(
    const std::string& accessKeyId, const UpdateAccessKeyParams& params) {
  return boost::asio::co_spawn(
      m_ioContext,
      [this, accessKeyId, params]() -> boost::asio::awaitable<std::string> {
        std::map<std::string, std::string> placeholders{{"keyId", accessKeyId}};
        auto url = utils::appendUrl(
            m_apiUrl,
            utils::replacePlaceholders(
                std::string(api::Endpoints::UpdateAccessKey), placeholders));
        boost::json::object keyObj;
        if (params.name)
          keyObj["name"] = params.name.value();
        if (params.password)
          keyObj["password"] = params.password.value();
        if (params.method)
          keyObj["method"] = params.method.value();
        if (params.data_limit_bytes) {
          boost::json::object dataLimitObj{
              {"bytes", params.data_limit_bytes.value()}};
          keyObj["limit"] = dataLimitObj;
        }
        auto [status, responseBody] =
            co_await doPutAsync(url, boost::json::serialize(keyObj));
        if (status != 201) {
          throw OutlineServerErrorException(
              "Unable to update access key (status=" + std::to_string(status) +
              ")");
        }
        boost::json::value keyVal;
        try {
          keyVal = boost::json::parse(responseBody);
        } catch (const std::exception& e) {
          throw OutlineParseException(
              std::string("JSON parse error for access key update: ") +
              e.what());
        }
        co_return boost::json::serialize(keyVal);
      },
      boost::asio::use_future);
}

std::future<void> OutlineClient::deleteAccessKeyAsync(
    const std::string& accessKeyId) {
  return boost::asio::co_spawn(
      m_ioContext,
      [this, accessKeyId]() -> boost::asio::awaitable<void> {
        std::map<std::string, std::string> placeholders{{"keyId", accessKeyId}};
        auto url = utils::appendUrl(
            m_apiUrl,
            utils::replacePlaceholders(
                std::string(api::Endpoints::DeleteAccessKey), placeholders));
        auto [status, responseBody] = co_await doDeleteAsync(url);
        if (status != 204) {
          throw OutlineServerErrorException(
              "Unable to delete access key (status=" + std::to_string(status) +
              ")");
        }
        co_return;
      },
      boost::asio::use_future);
}

std::future<void> OutlineClient::renameAccessKeyAsync(
    const std::string& accessKeyId, const std::string& newName) {
  return boost::asio::co_spawn(
      m_ioContext,
      [this, accessKeyId, newName]() -> boost::asio::awaitable<void> {
        std::map<std::string, std::string> placeholders{{"keyId", accessKeyId}};
        auto url = utils::appendUrl(
            m_apiUrl,
            utils::replacePlaceholders(
                std::string(api::Endpoints::RenameAccessKey), placeholders));
        boost::json::object keyObj{{"name", newName}};
        auto [status, responseBody] =
            co_await doPutAsync(url, boost::json::serialize(keyObj));
        if (status != 204) {
          throw OutlineServerErrorException(
              "Unable to rename access key (status=" + std::to_string(status) +
              ")");
        }
        co_return;
      },
      boost::asio::use_future);
}

std::future<void> OutlineClient::addDataLimitAsync(
    const std::string& accessKeyId, int dataLimitBytes) {
  return boost::asio::co_spawn(
      m_ioContext,
      [this, accessKeyId, dataLimitBytes]() -> boost::asio::awaitable<void> {
        std::map<std::string, std::string> placeholders{{"keyId", accessKeyId}};
        auto url = utils::appendUrl(
            m_apiUrl,
            utils::replacePlaceholders(
                std::string(api::Endpoints::AddDataLimit), placeholders));
        boost::json::object dataLimitObj{{"bytes", dataLimitBytes}};
        auto [status, responseBody] =
            co_await doPutAsync(url, boost::json::serialize(dataLimitObj));
        if (status != 204) {
          throw OutlineServerErrorException(
              "Unable to add data limit (status=" + std::to_string(status) +
              ")");
        }
        co_return;
      },
      boost::asio::use_future);
}

std::future<void> OutlineClient::deleteDataLimitAsync(
    const std::string& accessKeyId) {
  return boost::asio::co_spawn(
      m_ioContext,
      [this, accessKeyId]() -> boost::asio::awaitable<void> {
        std::map<std::string, std::string> placeholders{{"keyId", accessKeyId}};
        auto url = utils::appendUrl(
            m_apiUrl,
            utils::replacePlaceholders(
                std::string(api::Endpoints::DeleteDataLimit), placeholders));
        auto [status, responseBody] = co_await doDeleteAsync(url);
        if (status != 204) {
          throw OutlineServerErrorException(
              "Unable to delete data limit (status=" + std::to_string(status) +
              ")");
        }
        co_return;
      },
      boost::asio::use_future);
}

std::string OutlineClient::getAccessKeys() {
    return getAccessKeysAsync().get();
}

std::string OutlineClient::getAccessKey(const std::string& accessKeyId) {
    return getAccessKeyAsync(accessKeyId).get();
}

std::string OutlineClient::createAccessKey(const CreateAccessKeyParams& params) {
    return createAccessKeyAsync(params).get();
}

std::string OutlineClient::updateAccessKey(const std::string& accessKeyId, const UpdateAccessKeyParams& params) {
    return updateAccessKeyAsync(accessKeyId, params).get();
}

void OutlineClient::deleteAccessKey(const std::string& accessKeyId) {
    deleteAccessKeyAsync(accessKeyId).get();
}

void OutlineClient::renameAccessKey(const std::string& accessKeyId, const std::string& newName) {
    renameAccessKeyAsync(accessKeyId, newName).get();
}

void OutlineClient::addDataLimit(const std::string& accessKeyId, int dataLimitBytes) {
    addDataLimitAsync(accessKeyId, dataLimitBytes).get();
}

void OutlineClient::deleteDataLimit(const std::string& accessKeyId) {
    deleteDataLimitAsync(accessKeyId).get();
}

}  // namespace outline
