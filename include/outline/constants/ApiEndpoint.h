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
  static constexpr std::string_view GetMetricsTransfer = "/metrics/transfer";
  static constexpr std::string_view CreateAccessKey = "/access-keys";

  static constexpr std::string_view GetAccessKeyById = "/access-keys/{key_id}";
  static constexpr std::string_view DeleteAccessKey = "/access-keys/{key_id}";
  static constexpr std::string_view UpdateAccessKey = "/access-keys/{key_id}";
};

}  // namespace api
}  // namespace outline

#endif  // API_ENDPOINTS_H
