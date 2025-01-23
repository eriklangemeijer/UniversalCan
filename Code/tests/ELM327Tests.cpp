#include "ProtocolDefinitionParser.h"
#include <Transcievers/ELM327.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <vector>

class MockISerial : public ISerial {
  public:
    MOCK_METHOD(bool, open, (const std::string &port), (override));
    MOCK_METHOD(void, close, (), (override));
    MOCK_METHOD(bool, write, (const std::vector<uint8_t> &data), (override));
    MOCK_METHOD(bool, writeString, (std::string data), (override));
    MOCK_METHOD(std::string, read, (), (override));
    MOCK_METHOD(void, registerCallback, (std::function<void(std::vector<uint8_t>)> callback), (override));

    void DelegateToFake() {
        ON_CALL(*this, registerCallback).WillByDefault([this](std::function<void(std::vector<uint8_t>)> callback) {
            this->callbackPtr = callback;
        });

        ON_CALL(*this, writeString).WillByDefault([this](std::string data) {
            if(data == "AT Z\r") {
                
                std::string str = "ELM327 v1.0";
                std::vector<unsigned char> message(str.begin(), str.end());
                this->callbackPtr(message);
            }
            this->callbackPtr({'>'});
            return true;
        });
    }

  private:
    std::function<void(std::vector<uint8_t>)> callbackPtr;
};

TEST(ELM327, SetupCorrectAnswer) {
    auto mock_serial = std::make_unique<::testing::NiceMock<MockISerial>>();

    mock_serial->DelegateToFake();

    // Following expected calls have no action as they use the default defined in the DelegateToFake function
    EXPECT_CALL(*mock_serial, registerCallback(testing::_));
    EXPECT_CALL(*mock_serial, writeString("AT Z\r"));
    EXPECT_CALL(*mock_serial, writeString("AT SP0\r"));
    EXPECT_CALL(*mock_serial, writeString("01 00\r"));
    

    ProtocolDefinitionParser parser = ProtocolDefinitionParser("../../ProtocolDefinitions/bmw_motorrad.xml");

    ELM327 elm327(std::move(mock_serial), std::make_unique<ProtocolDefinitionParser>(parser));
    elm327.start();
}

TEST(ELM327, SetupTimeOut) {
    auto mock_serial = std::make_unique<::testing::NiceMock<MockISerial>>();

    mock_serial->DelegateToFake();

    EXPECT_CALL(*mock_serial, writeString("AT Z\r")).WillOnce(::testing::Return(false));
    EXPECT_CALL(*mock_serial, writeString("AT SP0\r")).WillOnce(::testing::Return(false));
    EXPECT_CALL(*mock_serial, writeString("01 00\r")).WillOnce(::testing::Return(false));
    

    ProtocolDefinitionParser parser = ProtocolDefinitionParser("../../ProtocolDefinitions/bmw_motorrad.xml");

    ELM327 elm327(std::move(mock_serial), std::make_unique<ProtocolDefinitionParser>(parser));
    elm327.start();
}

TEST(ELM327, ParsePIDString) {

    auto mock_serial = std::make_unique<::testing::NiceMock<MockISerial>>();
    ProtocolDefinitionParser parser = ProtocolDefinitionParser("../../ProtocolDefinitions/bmw_motorrad.xml");
    ELM327 elm327(std::move(mock_serial), std::make_unique<ProtocolDefinitionParser>(parser));

    std::string str = "41 00 FC 3E B0 11";
    std::vector<unsigned char> message(str.begin(), str.end());
    elm327.serialReceiveCallback(message);
    GTEST_ASSERT_TRUE(elm327.messageAvailable());
    auto parsed_message = elm327.readMessage();
    ASSERT_THAT(parsed_message->data, ::testing::ElementsAre(0x41, 0x00, 0xFC, 0x3E, 0xB0, 0x11));
}
