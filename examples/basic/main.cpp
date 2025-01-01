#include "outline/OutlineClient.h"
#include "outline/exceptions/OutlineExceptions.h"
#include <iostream>

int main() {
    std::string host = "https://IP:Port/pemgierjg";
    std::string cert = "";
    int timeout = 10;

    auto client = outline::OutlineClient::create(host, cert, timeout);

    try {
        auto response = client->getServerInformation();
        /*
        Prints the server information:
        Server info: {"name":"Server Outline",
        "serverId":"gb236f3c-523f-23ss-926f-f23572382f71",
        "metricsEnabled":true,
        "createdTimestampMs":1729449481512,
        "version":"1.11.0",
        "portForNewAccessKeys":Port,
        "hostnameForAccessKeys":"IP"}
        */
        std::cout << "Server info: " << response << std::endl;
    } catch(const outline::OutlineServerErrorException& e) {
        std::cerr << "Server Error: " << e.what() << std::endl;
    } catch(const outline::OutlineParseException& e) {
        std::cerr << "Parse Error: " << e.what() << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Unexpected Error: " << e.what() << std::endl;
    }

    return 0;
}
