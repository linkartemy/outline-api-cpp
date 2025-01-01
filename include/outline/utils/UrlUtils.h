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
    const std::map<std::string, std::string>& placeholders);

/**
 * @brief Appends a path segment to a base URL.
 *
 * @param baseUrl The base boost::urls::url object.
 * @param endpoint The endpoint to append.
 * @return A new boost::urls::url object with the appended endpoint.
 *
 * @throws std::invalid_argument if the endpoint is invalid.
 */
boost::urls::url appendUrl(const boost::urls::url& baseUrl,
                           const std::string& endpoint);
}  // namespace utils
}  // namespace outline