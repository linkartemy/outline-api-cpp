#include "outline/utils/UrlUtils.h"
#include <boost/algorithm/string/replace.hpp>
#include <boost/url.hpp>
#include <stdexcept>

namespace outline {
namespace utils {

std::string replacePlaceholders(
    std::string_view endpoint,
    const std::map<std::string, std::string>& placeholders) {
  std::string resolvedEndpoint = std::string(endpoint);
  for (const auto& [key, value] : placeholders) {
    std::string placeholder = "{" + key + "}";
    boost::algorithm::replace_all(resolvedEndpoint, placeholder, value);
  }

  if (resolvedEndpoint.find('{') != std::string::npos) {
    throw std::invalid_argument("Unresolved placeholders in endpoint: " +
                                resolvedEndpoint);
  }

  return resolvedEndpoint;
}

boost::urls::url appendUrl(const boost::urls::url& baseUrl,
                           const std::string& endpoint) {
  // Create a copy of the base URL
  boost::urls::url url = baseUrl;

  // Remove leading '/' to prevent empty segments
  std::string sanitizedEndpoint = endpoint;
  if (!sanitizedEndpoint.empty() && sanitizedEndpoint[0] == '/') {
    sanitizedEndpoint.erase(0, 1);
  }

  // Split the endpoint into segments and append
  boost::urls::result<boost::urls::segments_view> parseResult =
      boost::urls::parse_path(sanitizedEndpoint);
  if (!parseResult) {
    throw std::invalid_argument("Invalid endpoint path: " + endpoint);
  }

  for (const auto& segment : parseResult.value()) {
    url.segments().push_back(segment);
  }

  return url;
}

}  // namespace utils
}  // namespace outline