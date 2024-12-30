#include <boost/algorithm/string/replace.hpp>
#include <boost/url.hpp>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

namespace outline {
namespace utils {

/**
 * @brief Replaces named placeholders in an endpoint string with corresponding values.
 *
 * @param endpoint The endpoint string containing placeholders in the format {key}.
 * @param placeholders A map of key-value pairs where keys correspond to placeholder names.
 * @return A new string with all placeholders replaced by their corresponding values.
 *
 * @throws std::invalid_argument if any placeholders remain unresolved after replacement.
 */
std::string replacePlaceholders(
      std::string_view endpoint,
      const std::map<std::string, std::string>& placeholders) {
    std::string resolvedEndpoint = std::string(endpoint);
    for (const auto& [key, value] : placeholders) {
      std::string placeholder = "{" + key + "}";
      boost::algorithm::replace_all(resolvedEndpoint, placeholder, value);
    }

    if (resolvedEndpoint.find('{') != std::string::npos) {
      throw std::invalid_argument("Unresolved placeholders in endpoint: " + resolvedEndpoint);
    }

    return resolvedEndpoint;
  }

/**
 * @brief Appends a path segment to a base URL.
 *
 * @param baseUrl The base boost::urls::url object.
 * @param endpoint The endpoint to append.
 * @return A new boost::urls::url object with the appended endpoint.
 *
 * @throws std::invalid_argument if the endpoint is invalid.
 */
boost::urls::url appendUrl(const boost::urls::url& baseUrl, const std::string& endpoint) {
        // Create a copy of the base URL
        boost::urls::url url = baseUrl;

        // Remove leading '/' to prevent empty segments
        std::string sanitizedEndpoint = endpoint;
        if (!sanitizedEndpoint.empty() && sanitizedEndpoint[0] == '/') {
            sanitizedEndpoint.erase(0, 1);
        }

        // Split the endpoint into segments and append
        boost::urls::result<boost::urls::segments_view> parseResult = boost::urls::parse_path(sanitizedEndpoint);
        if (!parseResult) {
            throw std::invalid_argument("Invalid endpoint path: " + endpoint);
        }

        for (const auto& segment : parseResult.value()) {
            url.segments().push_back(segment);
        }

        return url;
    }
}  // namespace utils
}