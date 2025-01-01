#ifndef OUTLINECLIENT_H
#define OUTLINECLIENT_H

#include <future>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include <boost/asio.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/ssl.hpp>
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
class OutlineClient : public std::enable_shared_from_this<OutlineClient> {
 public:
  /**
     * apiUrl - url for server API
     * cert - certificate after apiUrl
     * timeout - request timeout
     */
  OutlineClient(std::string_view apiUrl, std::string_view cert,
                int timeout = 5);

  /**
     * @brief Destructor. Stops the io_context and joins the io_thread.
     */
  ~OutlineClient();

  /**
    * @brief Creates a shared_ptr for handling the lifetime issues
   */
  static std::shared_ptr<OutlineClient> create(std::string_view apiUrl,
                                               std::string_view cert,
                                               int timeout = 5) {
    return std::shared_ptr<OutlineClient>(
        new OutlineClient(apiUrl, cert, timeout));
  }

  /**
     * @brief Returns the access keys.
     * @return the access keys.
     */
  std::future<std::string> getAccessKeysAsync();
  /**
   * @brief Returns the access key by id.
   * @param accessKeyId - the access key id.
   * @return the access key by id.
   */
  std::future<std::string> getAccessKeyAsync(const std::string& accessKeyId);
  /**
   * @brief Creates the access key.
   * @param params - the parameters for the access key.
   */
  std::future<std::string> createAccessKeyAsync(
      const CreateAccessKeyParams& params);
  /**
   * @brief Updates the access key.
   * @param accessKeyId - the access key id.
   * @param params - the parameters for the access key.
   */
  std::future<std::string> updateAccessKeyAsync(
      const std::string& accessKeyId, const UpdateAccessKeyParams& params);
  /**
   * @brief Deletes the access key.
   * @param accessKeyId - the access key id.
   */
  std::future<void> deleteAccessKeyAsync(const std::string& accessKeyId);
  /**
   * @brief Renames the access key.
   * @param accessKeyId - the access key id.
   * @param newName - the new name for the access key.
   */
  std::future<void> renameAccessKeyAsync(const std::string& accessKeyId,
                                         const std::string& newName);
  /**
   * @brief Adds the data limit for the access key.
   * @param accessKeyId - the access key id.
   * @param dataLimitBytes - the data limit in bytes.
   */
  std::future<void> addDataLimitAsync(const std::string& accessKeyId,
                                      int dataLimitBytes);
  /**
   * @brief Deletes the data limit for the access key.
   * @param accessKeyId - the access key id.
   */
  std::future<void> deleteDataLimitAsync(const std::string& accessKeyId);
  /**
   * @brief Returns the metrics of the server.
   * @return the metrics of the server.
   */
  std::future<std::string> getMetricsAsync();
  /**
   * @details Example: name, serverId, metricsEnabled, createdTimestampMs, version, accessKeyDataLimit, portForNewAccessKeys, hostnameForAccessKeys
   * @brief Returns the information about the server.
   * @return the information about the server.
   */
  std::future<std::string> getServerInformationAsync();
  /**
   * @brief Sets the server name.
   */
  std::future<void> setServerNameAsync(const std::string& serverName);
  /**
   * @brief Sets the host name for the server.
   * @param hostName - the host name, which will be used for the server.
   */
  std::future<void> setHostNameAsync(const std::string& hostName);
  /**
     * @brief Returns the status of metrics.
     * @return true if metrics are enabled, false otherwise.
     */
  std::future<bool> getMetricsStatusAsync();
  /**
     * @brief Sets the status of metrics.
     * @param status - true if metrics are enabled, false otherwise.
     */
  std::future<void> setMetricsStatusAsync(bool status);
  /**
     * @brief Sets the default port for new access keys.
     * @param port - the default port, which will be used for new access keys.
     */
  std::future<void> setDefaultPortAsync(int port);
  /**
     * @brief Sets the data limit for all access keys.
     * @param dataLimitBytes - the data limit in bytes.
     */
  std::future<void> setDataLimitForAllAccessKeysAsync(int dataLimitBytes);
  /**
     * @brief Deletes the data limit for all access keys.
     */
  std::future<void> deleteDataLimitForAllAccessKeysAsync();

  std::string getAccessKeys();
  std::string getAccessKey(const std::string& accessKeyId);
  std::string createAccessKey(const CreateAccessKeyParams& params);
  std::string updateAccessKey(const std::string& accessKeyId,
                              const UpdateAccessKeyParams& params);
  void deleteAccessKey(const std::string& accessKeyId);
  void renameAccessKey(const std::string& accessKeyId,
                       const std::string& newName);
  void addDataLimit(const std::string& accessKeyId, int dataLimitBytes);
  void deleteDataLimit(const std::string& accessKeyId);
  std::string getMetrics();
  std::string getServerInformation();
  bool getMetricsStatus();
  void setMetricsStatus(bool status);
  void setServerName(const std::string& serverName);
  void setHostName(const std::string& hostName);
  void setDefaultPort(int port);
  void setDataLimitForAllAccessKeys(int dataLimitBytes);
  void deleteDataLimitForAllAccessKeys();

 private:
  boost::urls::url m_apiUrl;
  std::string m_cert;
  int m_timeout;

  boost::asio::ssl::context m_sslContext;
  boost::asio::io_context m_ioContext;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
      m_workGuard;
  std::thread m_ioThread;

  boost::asio::awaitable<std::pair<int, std::string>> doGetAsync(
      const boost::urls::url& url);
  boost::asio::awaitable<std::pair<int, std::string>> doPostAsync(
      const boost::urls::url& url, const std::string& body);
  boost::asio::awaitable<std::pair<int, std::string>> doPutAsync(
      const boost::urls::url& url, const std::string& body);
  boost::asio::awaitable<std::pair<int, std::string>> doDeleteAsync(
      const boost::urls::url& url);
};

}  // namespace outline

#endif  // OUTLINECLIENT_H
