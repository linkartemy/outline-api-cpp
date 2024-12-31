// include/outline/api/ApiEndpoints.h
#ifndef API_ENDPOINTS_H
#define API_ENDPOINTS_H

#include <string>
#include <string_view>

namespace outline {
namespace api {

struct UrlParams {
  static constexpr std::string_view KeyId = "key_id";
};

struct Endpoints {
  static constexpr std::string_view GetAccessKeys = "/access-keys";
  static constexpr std::string_view GetAccessKeyById = "/access-keys/{key_id}";
  static constexpr std::string_view CreateAccessKey = "/access-keys";
  static constexpr std::string_view UpdateAccessKey = "/access-keys/{key_id}";
  static constexpr std::string_view DeleteAccessKey = "/access-keys/{key_id}";
  static constexpr std::string_view RenameAccessKey = "/access-keys/{key_id}/name";
  static constexpr std::string_view AddDataLimit = "/access-keys/{key_id}/data-limit";
  static constexpr std::string_view DeleteDataLimit = "/access-keys/{key_id}/data-limit";
  static constexpr std::string_view GetMetrics = "/metrics/transfer";
  static constexpr std::string_view GetServerInformation = "/server";
  static constexpr std::string_view SetServerName = "/name";
  static constexpr std::string_view SetHostName = "/server/hostname-for-access-keys";
  static constexpr std::string_view GetMetricsStatus = "/metrics/enabled";
  static constexpr std::string_view SetMetricsStatus = "/metrics/enabled";
  static constexpr std::string_view SetDefaultPort = "/server/port-for-new-access-keys";
  static constexpr std::string_view SetDataLimitForAllAccessKeys = "/server/access-key-data-limit";
  static constexpr std::string_view deleteDataLimitForAllAccessKeys = "/server/access-key-data-limit";
};

}  // namespace api
}  // namespace outline

#endif  // API_ENDPOINTS_H
