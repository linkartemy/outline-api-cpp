#ifndef OUTLINECLIENT_H
#define OUTLINECLIENT_H

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include "outline/exceptions/OutlineExceptions.h"

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/url.hpp>

namespace outline {

struct CreateAccessKeyParams {
  std::optional<std::string> name;
  std::optional<std::string> method;
  std::optional<std::string> password;
  std::optional<int> data_limit_bytes;
};

struct UpdateAccessKeyParams {
  std::optional<std::string> name;
  std::optional<std::string> method;
  std::optional<std::string> password;
  std::optional<int> data_limit_bytes;
};

/**
 * @brief Класс OutlineClient отвечает за подключение к Outline-серверу.
 */
class OutlineClient {
 public:
  /**
     * apiUrl - url for server API
     * cert - certificate after apiUrl
     * timeout - request timeout
     */
  OutlineClient(std::string_view apiUrl, std::string_view cert,
                int timeout = 5);

  /**
     * @brief Деструктор. Гарантирует корректное отключение при уничтожении объекта.
     */
  ~OutlineClient();

  /**
     * @brief Returns the access keys.
     * @return the access keys.
     */
  std::string getAccessKeys();
  /**
   * @brief Returns the access key by id.
   * @param accessKeyId - the access key id.
   * @return the access key by id.
   */
  std::string getAccessKey(std::string& accessKeyId);
  /**
   * @brief Creates the access key.
   * @param params - the parameters for the access key.
   */
  std::string createAccessKey(CreateAccessKeyParams& params);
  /**
   * @brief Updates the access key.
   * @param accessKeyId - the access key id.
   * @param params - the parameters for the access key.
   */
  std::string updateAccessKey(int accessKeyId, UpdateAccessKeyParams& params);
  /**
   * @brief Deletes the access key.
   * @param accessKeyId - the access key id.
   */
  void deleteAccessKey(std::string& accessKeyId);
  /**
   * @brief Renames the access key.
   * @param accessKeyId - the access key id.
   * @param newName - the new name for the access key.
   */
  void renameAccessKey(std::string& accessKeyId, std::string& newName);
  /**
   * @brief Adds the data limit for the access key.
   * @param accessKeyId - the access key id.
   * @param dataLimitBytes - the data limit in bytes.
   */
  void addDataLimit(std::string& accessKeyId, int dataLimitBytes);
  /**
   * @brief Deletes the data limit for the access key.
   * @param accessKeyId - the access key id.
   */
  void deleteDataLimit(std::string& accessKeyId);
  /**
   * @brief Returns the metrics of the server.
   * @return the metrics of the server.
   */
  std::string getMetrics();
  /**
   * @details Example: name, serverId, metricsEnabled, createdTimestampMs, version, accessKeyDataLimit, portForNewAccessKeys, hostnameForAccessKeys
   * @brief Returns the information about the server.
   * @return the information about the server.
   */
  std::string getServerInformation();
  /**
   * @brief Sets the server name.
   */
  void setServerName(std::string& serverName);
  /**
   * @brief Sets the host name for the server.
   * @param hostName - the host name, which will be used for the server.
   */
  void setHostName(std::string& hostName);
  /**
     * @brief Returns the status of metrics.
     * @return true if metrics are enabled, false otherwise.
     */
  bool getMetricsStatus();
  /**
     * @brief Sets the status of metrics.
     * @param status - true if metrics are enabled, false otherwise.
     */
  void setMetricsStatus(bool status);
  /**
     * @brief Sets the default port for new access keys.
     * @param port - the default port, which will be used for new access keys.
     */
  void setDefaultPort(int port);
  /**
     * @brief Sets the data limit for all access keys.
     * @param dataLimitBytes - the data limit in bytes.
     */
  void setDataLimitForAllAccessKeys(int dataLimitBytes);
  /**
     * @brief Deletes the data limit for all access keys.
     */
  void deleteDataLimitForAllAccessKeys();

 private:
  boost::urls::url m_apiUrl;
  std::string m_cert;
  int m_timeout;

  boost::asio::io_context m_ioContext;
  std::unique_ptr<boost::asio::ip::tcp::socket> m_socket;

  std::pair<int, std::string> doGet(const boost::urls::url& url);
  std::pair<int, std::string> doHttpsGet(const boost::urls::url& url);
  std::pair<int, std::string> doPut(const boost::urls::url& url,
                                    const std::string& body);
  std::pair<int, std::string> doHttpsPut(const boost::urls::url& url,
                                         const std::string& body);
  std::pair<int, std::string> doPost(const boost::urls::url& url,
                                     const std::string& body);
  std::pair<int, std::string> doHttpsPost(const boost::urls::url& url,
                                          const std::string& body);
  std::pair<int, std::string> doDelete(const boost::urls::url& url);
  std::pair<int, std::string> doHttpsDelete(const boost::urls::url& url);

  boost::asio::ssl::context m_sslContext;
  boost::asio::ssl::stream<boost::asio::ip::tcp::socket> m_sslStream;
};

}  // namespace outline

#endif  // OUTLINECLIENT_H
