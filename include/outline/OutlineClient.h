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

  std::string getAccessKeys();
  std::string getAccessKey(std::string& accessKeyId);
  std::string createAccessKey(CreateAccessKeyParams& params);
  std::string updateAccessKey(int accessKeyId, UpdateAccessKeyParams& params);
  void deleteAccessKey(std::string& accessKeyId);

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
