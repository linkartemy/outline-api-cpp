#include <gtest/gtest.h>
#include <boost/json.hpp>
#include <iostream>
#include <memory>
#include <string>
#include "../include/outline/OutlineClient.h"

// Test Fixture for OutlineClient
class AccessKeysTest : public ::testing::Test {
 protected:
  // This function runs before each test
  void SetUp() override {
    // Initialize the API client
    client = std::make_unique<outline::OutlineClient>(
        "https://IP:Port/f4j29jf93jf93fj398fj", "", 10);
  }

  // This function runs after each test
  void TearDown() override {
    // Clean up
    client.reset();
  }

  // Pointer to the API client
  std::unique_ptr<outline::OutlineClient> client;
};

TEST_F(AccessKeysTest, GetAccessKeysSuccess) {
  outline::CreateAccessKeyParams params;
  params.name = "auto_testing";
  params.password = "auto_testing";
  params.data_limit_bytes = 1024 * 1024 * 1024;
  std::string createResponse = client->createAccessKey(params);
  boost::json::object createResponseObj =
      boost::json::parse(createResponse).as_object();
  std::string id = createResponseObj["id"].as_string().c_str();
  client->deleteAccessKey(id);
  EXPECT_EQ(createResponseObj["name"].as_string().c_str(), params.name);
  EXPECT_EQ(createResponseObj["password"].as_string().c_str(), params.password);
  boost::json::object dataLimitObj = createResponseObj["dataLimit"].as_object();
  EXPECT_EQ(dataLimitObj["bytes"].as_int64(), params.data_limit_bytes);
}
