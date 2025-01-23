#include "CanValueTemplate.h"
#include <CanMessage.h>
#include <CanMessageTemplate.h>
#include <ProtocolDefinitionParser.h>
#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>
#include <TestConstants.h>

/* This test shows how a message can be found using its filter function.
   This is neccessary to identify the template matching a message.*/
TEST(ProtocolDefinitionParser, findTemplate) {
    ProtocolDefinitionParser parser = ProtocolDefinitionParser("../../ProtocolDefinitions/J1979.xml");
    std::vector<uint8_t> const msg_data = {mode1_response_id,
                                           mode1_veh_spd_id,
                                           0};
    auto msg_template = parser.findMatch(msg_data);
    GTEST_ASSERT_TRUE(strcmp(msg_template->getName().c_str(), "M01VehicleSpeed") == 0);
}

/* This test shows how an exception is thrown if the file cannot be found.*/
TEST(ProtocolDefinitionParser, CannotFindProtocolFile) {
    const std::string expected_error_string = "Cannot open protocol file";
    EXPECT_THROW({
        try
        {
            ProtocolDefinitionParser parser = ProtocolDefinitionParser("foobar");
        }
        catch( const std::runtime_error& e )
        {
            EXPECT_STREQ(expected_error_string.c_str(), e.what() );
            throw;
        } }, std::runtime_error);
}

/* This test shows how a message with an unknown template results in a nullptr for the template.*/
TEST(ProtocolDefinitionParser, NotFindTemplate) {
    ProtocolDefinitionParser parser = ProtocolDefinitionParser("../../ProtocolDefinitions/J1979.xml");
    std::vector<uint8_t> const msg_data = {0,
                                           0,
                                           0};
    auto msg_template = parser.findMatch(msg_data);
    GTEST_ASSERT_EQ(msg_template, nullptr);
}

/* This test shows how messages can defined in subfiles. This way protocols can include messages from other protocols.*/
TEST(ProtocolDefinitionParser, IncludeRelativeFile) {
    ProtocolDefinitionParser parser = ProtocolDefinitionParser("../../ProtocolDefinitions/bmw_motorrad.xml");
    std::vector<uint8_t> const msg_data = {mode1_response_id,
                                           mode1_veh_spd_id,
                                           0};
    // Message is only defined in J1979.xml which is included by filename in bmw_motorrad.xml
    auto msg_template = parser.findMatch(msg_data);
    GTEST_ASSERT_TRUE(strcmp(msg_template->getName().c_str(), "M01VehicleSpeed") == 0);
}
