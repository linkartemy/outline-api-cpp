# Outline VPN API C++ Client

Welcome to the **Outline VPN API C++ Client**! This library provides a convenient interface for interacting with the Outline VPN server API, enabling you to manage access keys, configure server settings, and monitor server metrics programmatically. All the functionality that is available on the 1st of January, 2025, is supported by the library. 

## Table of Contents

- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
  - [Creating an Access Key](#creating-an-access-key)
  - [Deleting an Access Key](#deleting-an-access-key)
  - [Adding a Data Limit](#adding-a-data-limit)
  - [Retrieving Access Keys](#retrieving-access-keys)
  - [Managing Server Metrics](#managing-server-metrics)
  - [Configuring Server Settings](#configuring-server-settings)
- [Examples](#examples)
  - [Basic Operations](#basic-operations)
  - [Advanced Metrics Handling](#advanced-metrics-handling)
- [API Reference](#api-reference)
- [Building the Project](#building-the-project)
- [Contributing](#contributing)
- [License](#license)

## Features

- **Access Key Management**: Create, get, update, delete, and rename access keys.
- **Data Limit Control**: Set and remove data usage limits for individual or all access keys.
- **Server Configuration**: Configure server settings such as server name, hostname and default port.
- **Metrics Monitoring**: Enable, disable, and get server metrics to monitor usage of trafic.
- **Asynchronous Operations**: Perform all operations asynchronously for non-blocking execution.
- **Error Handling**: Comprehensive exception handling for robust and reliable integrations.

## Installation

### Prerequisites

Ensure you have the following dependencies installed:

- **C++ Compiler**: `g++` (version 13 recommended)
- **Boost Libraries**: System, Asio, JSON, URL components
- **OpenSSL**
- **CMake** (optional, if using CMake instead of Makefile)
- **CURL**

### Building with Makefile

1. **Clone the Repository**

   ```bash
   git clone https://github.com/linkartemy/outline-api-cpp.git
   cd outline-api-cpp
   ```

2. **Build the Project**

   Use the provided Makefile to compile the library and example executable.

   ```bash
   make all
   ```

3. **Run the Example**

   ```bash
   make run
   ```

4. **Clean Build Artifacts**

   To clean the build directory:

   ```bash
   make clean
   ```

### Building with CMake (Optional)

If you prefer using CMake:

1. **Create a Build Directory**

   ```bash
   mkdir build && cd build
   ```

2. **Configure the Project**

   ```bash
   cmake ..
   ```

3. **Build the Project**

   ```bash
   make
   ```

4. **Run the Example**

   ```bash
   ./example
   ```

## Usage

### Creating an Access Key

To create a new access key with specific parameters:

```cpp
#include "outline/OutlineClient.h"
#include "outline/utils/UrlUtils.h"

#include <iostream>
#include <memory>

int main() {
    std::string apiUrl = "https://your-outline-server.com/api";
    std::string cert = ""; // Add your server certificate if required
    int timeout = 10;

    auto client = outline::OutlineClient::create(apiUrl, cert, timeout);

    outline::CreateAccessKeyParams params;
    params.name = "NewAccessKey";
    params.method = "AES-256-GCM";
    params.password = "securepassword";
    params.data_limit_bytes = 1000000; // 1 MB

    try {
        std::future<std::string> createKeyFuture = client->createAccessKeyAsync(params);
        std::string createdKey = createKeyFuture.get();
        std::cout << "Created Access Key: " << createdKey << std::endl;
    } catch (const outline::OutlineServerErrorException& e) {
        std::cerr << "Server Error: " << e.what() << std::endl;
    } catch (const outline::OutlineParseException& e) {
        std::cerr << "Parse Error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected Error: " << e.what() << std::endl;
    }

    return 0;
}
```

### Deleting an Access Key

To delete an existing access key by its ID:

```cpp
std::string accessKeyId = "your-access-key-id";

try {
    std::future<void> deleteKeyFuture = client->deleteAccessKeyAsync(accessKeyId);
    deleteKeyFuture.get();
    std::cout << "Access Key deleted successfully." << std::endl;
} catch (const outline::OutlineServerErrorException& e) {
    std::cerr << "Server Error: " << e.what() << std::endl;
} catch (const std::exception& e) {
    std::cerr << "Unexpected Error: " << e.what() << std::endl;
}
```

### Adding a Data Limit

To set a data usage limit for a specific access key:

```cpp
std::string accessKeyId = "your-access-key-id";
int dataLimitBytes = 5000000; // 5 MB

try {
    std::future<void> addLimitFuture = client->addDataLimitAsync(accessKeyId, dataLimitBytes);
    addLimitFuture.get();
    std::cout << "Data limit added successfully." << std::endl;
} catch (const outline::OutlineServerErrorException& e) {
    std::cerr << "Server Error: " << e.what() << std::endl;
} catch (const std::exception& e) {
    std::cerr << "Unexpected Error: " << e.what() << std::endl;
}
```

### Retrieving Access Keys

To retrieve all access keys:

```cpp
try {
    std::future<std::string> accessKeysFuture = client->getAccessKeysAsync();
    std::string accessKeys = accessKeysFuture.get();
    std::cout << "Access Keys: " << accessKeys << std::endl;
} catch (const outline::OutlineServerErrorException& e) {
    std::cerr << "Server Error: " << e.what() << std::endl;
} catch (const outline::OutlineParseException& e) {
    std::cerr << "Parse Error: " << e.what() << std::endl;
} catch (const std::exception& e) {
    std::cerr << "Unexpected Error: " << e.what() << std::endl;
}
```

### Managing Server Metrics

#### Enabling Metrics

Metrics provide insights into server performance and usage statistics. To enable metrics:

```cpp
bool enable = true;

try {
    std::future<void> setMetricsFuture = client->setMetricsStatusAsync(enable);
    setMetricsFuture.get();
    std::cout << "Metrics enabled successfully." << std::endl;
} catch (const outline::OutlineServerErrorException& e) {
    std::cerr << "Server Error: " << e.what() << std::endl;
} catch (const std::exception& e) {
    std::cerr << "Unexpected Error: " << e.what() << std::endl;
}
```

#### Retrieving Metrics

To retrieve current server metrics:

```cpp
try {
    std::future<std::string> metricsFuture = client->getMetricsAsync();
    std::string metrics = metricsFuture.get();
    std::cout << "Server Metrics: " << metrics << std::endl;
} catch (const outline::OutlineServerErrorException& e) {
    std::cerr << "Server Error: " << e.what() << std::endl;
} catch (const outline::OutlineParseException& e) {
    std::cerr << "Parse Error: " << e.what() << std::endl;
} catch (const std::exception& e) {
    std::cerr << "Unexpected Error: " << e.what() << std::endl;
}
```

To enable the posibility for getting metrics and retrieve them:

```cpp
#include "outline/OutlineClient.h"
#include "outline/utils/UrlUtils.h"

#include <iostream>
#include <memory>

int main() {
    std::string apiUrl = "https://your-outline-server.com/api";
    std::string cert = ""; // Add your server certificate if required
    int timeout = 10;

    auto client = outline::OutlineClient::create(apiUrl, cert, timeout);

    try {
        // Enable server metrics
        bool enableMetrics = true;
        std::future<void> enableFuture = client->setMetricsStatusAsync(enableMetrics);
        enableFuture.get();
        std::cout << "Server metrics enabled." << std::endl;

        // Retrieve server metrics
        std::future<std::string> metricsFuture = client->getMetricsAsync();
        std::string metrics = metricsFuture.get();
        std::cout << "Server Metrics: " << metrics << std::endl;

        // Interpret metrics (example output)
        /*
        {
            "activeConnections": 5,
            "totalDataTransferred": 1500000,
            "serverUptime": 86400,
            "errorRate": 0.01
        }
        */

    } catch (const outline::OutlineServerErrorException& e) {
        std::cerr << "Server Error: " << e.what() << std::endl;
    } catch (const outline::OutlineParseException& e) {
        std::cerr << "Parse Error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected Error: " << e.what() << std::endl;
    }

    return 0;
}
```

**Understanding Metrics:**

- **Usage Statistics**: Data on bandwidth consumption.

Enabling metrics makes it possible to retrieve them from server via API like it is shown in the previous example.

### Configuring Server Settings

#### Setting Server Name

To set or update the server's display name:

```cpp
std::string serverName = "My Outline Server";

try {
    std::future<void> setNameFuture = client->setServerNameAsync(serverName);
    setNameFuture.get();
    std::cout << "Server name set successfully." << std::endl;
} catch (const outline::OutlineServerErrorException& e) {
    std::cerr << "Server Error: " << e.what() << std::endl;
} catch (const std::exception& e) {
    std::cerr << "Unexpected Error: " << e.what() << std::endl;
}
```

#### Setting Hostname

To configure the server's hostname:

```cpp
std::string hostName = "vpn.example.com";

try {
    std::future<void> setHostFuture = client->setHostNameAsync(hostName);
    setHostFuture.get();
    std::cout << "Hostname set successfully." << std::endl;
} catch (const outline::OutlineServerErrorException& e) {
    std::cerr << "Server Error: " << e.what() << std::endl;
} catch (const std::exception& e) {
    std::cerr << "Unexpected Error: " << e.what() << std::endl;
}
```

#### Setting Default Port

To set the default port for new access keys:

```cpp
int defaultPort = 443;

try {
    std::future<void> setPortFuture = client->setDefaultPortAsync(defaultPort);
    setPortFuture.get();
    std::cout << "Default port set successfully." << std::endl;
} catch (const outline::OutlineServerErrorException& e) {
    std::cerr << "Server Error: " << e.what() << std::endl;
} catch (const std::exception& e) {
    std::cerr << "Unexpected Error: " << e.what() << std::endl;
}
```

## API Reference

### `OutlineClient` Class

#### Constructor

```cpp
static std::shared_ptr<OutlineClient> create(std::string_view apiUrl, std::string_view cert, int timeout = 5);
```

- **Parameters**:
  - `apiUrl`: The URL for the Outline server API.
  - `cert`: Server certificate for SSL/TLS verification.
  - `timeout`: Request timeout in seconds (default is 5 seconds).

#### Synchronous Methods

For convenience, the library also provides synchronous versions of the asynchronous methods. These methods block until the operation completes.

## Contributing

Contributions are welcome! If you'd like to improve the library, please follow these steps:

1. **Fork the Repository**

2. **Create a New Branch**

   ```bash
   git checkout -b feature/YourFeatureName
   ```

3. **Make Your Changes**

4. **Commit Your Changes**

   ```bash
   git commit -m "Add your commit message"
   ```

5. **Push to Your Fork**

   ```bash
   git push origin feature/YourFeatureName
   ```

6. **Create a Pull Request**

   Describe your changes and why they are beneficial.

## License

This project is licensed under the [MIT License](LICENSE).
