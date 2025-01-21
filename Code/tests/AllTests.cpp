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

const uint8_t mode1_veh_spd_id = 0x0D;
const uint8_t mode1_response_id = 0x41;

/* Mode 1 messages can be used to request live data. 
   This test verifies how responses to these messages are parsed by the template
   In this test vehicle speed response is checked which is stored directly in the message in kph */
TEST(MessageParsing, M01EngSpdResponse)
{
    std::string const mode1_veh_spd_template_str =
            "<Message Name=\"M01VehicleSpeed\" Description=\"Mode $01 response for VehicleSpeed\">\
                    <FILTER_FUNCTION name=\"filter\" DataType=\"bool\" Unit=\"flag\">\
                        <LOGICAL_AND>\
                            <INT_COMPARE>\
                                <BYTE_SELECT>\
                                        <CONSTANT value=\"0\"/>\
                                        <CONSTANT value=\"1\"/>\
                                    </BYTE_SELECT>\
                                <CONSTANT value=\"0x41\"/>\
                            </INT_COMPARE>\
                            <INT_COMPARE>\
                                <BYTE_SELECT>\
                                        <CONSTANT value=\"1\"/>\
                                        <CONSTANT value=\"2\"/>\
                                    </BYTE_SELECT>\
                                <CONSTANT value=\"0x0D\"/>\
                            </INT_COMPARE>\
                        </LOGICAL_AND>\
                    </FILTER_FUNCTION>\
                    <Values>\
                        <Value name=\"VehicleSpeed\" DataType=\"uint8\" Unit=\"kph\">\
                            <BYTE_SELECT>\
                                <CONSTANT value=\"2\"/>\
                                <CONSTANT value=\"3\"/>\
                            </BYTE_SELECT>\
                        </Value>\
                    </Values>\
            </Message>";

    pugi::xml_document doc;
    doc.load_string(mode1_veh_spd_template_str.c_str());
    pugi::xml_node msg = doc.child("Message");
    std::shared_ptr<CanMessageTemplate> const mode1_veh_spd_template = std::make_shared<CanMessageTemplate>(msg);
    CanMessage response_0kph = CanMessage({mode1_response_id, mode1_veh_spd_id, 0}, mode1_veh_spd_template);
    GTEST_ASSERT_EQ(response_0kph.values[0].getValue<uint8_t>(), 0);
    CanMessage response_60kph = CanMessage({mode1_response_id, mode1_veh_spd_id, 60}, mode1_veh_spd_template);
    GTEST_ASSERT_EQ(response_60kph.values[0].getValue<uint8_t>(), 60);
    CanMessage response_120kph = CanMessage({mode1_response_id, mode1_veh_spd_id, 120}, mode1_veh_spd_template);
    GTEST_ASSERT_EQ(response_120kph.values[0].getValue<uint8_t>(), 120);
}

/*In this test PID Support response is checked which stores a bitmap with supported PID's. 
  Reference values come from wikipedia article */
TEST(MessageParsing, M01PIDSupportResponse)
{
    const uint8_t mode1_supp_pid = 0x00;
    ProtocolDefinitionParser parser = ProtocolDefinitionParser("../../../ProtocolDefinitions/J1979.xml");
    std::shared_ptr<CanMessageTemplate> const template_ptr =
      std::make_shared<CanMessageTemplate>(*parser.getMessageList().begin());
    CanMessage response_supp_pid = CanMessage({mode1_response_id, mode1_supp_pid, 0xBE, 0x1F, 0xA8, 0x13}, template_ptr);
    // From wikipedia table: https://en.wikipedia.org/wiki/OBD-II_PIDs#Service_01_PID_00
    std::vector<bool> expected_response = {true, false, true, true, true, true, true, false,
                                           false, false, false, true, true, true, true, true,
                                           true, false, true, false, true, false, false, false,
                                           false, false, false, true, false, false, true, true};
    for(uint8_t ii = 0; ii < expected_response.size(); ii++) {
        
        GTEST_ASSERT_EQ(response_supp_pid.values[ii].getBoolValue(), expected_response[ii]);
    }
    
}

/* This test shows how a message can be found using its filter function. 
   This is neccessary to identify the template matching a message.*/
TEST(ProtocolDefinitionParser, findTemplate)
{
    ProtocolDefinitionParser parser = ProtocolDefinitionParser("../../../ProtocolDefinitions/J1979.xml");
    std::vector<uint8_t> const msg_data = { mode1_response_id,
                                            mode1_veh_spd_id,
                                            0 };
    auto msg_template = parser.findMatch(msg_data);
    GTEST_ASSERT_TRUE(strcmp(msg_template->getName().c_str(), "M01VehicleSpeed") == 0);
}

/* This test shows how messages can defined in subfiles. This way protocols can include messages from other protocols.*/
TEST(ProtocolDefinitionParser, IncludeRelativeFile)
{
    ProtocolDefinitionParser parser = ProtocolDefinitionParser("../../../ProtocolDefinitions/bmw_motorrad.xml");
    std::vector<uint8_t> const msg_data = { mode1_response_id,
                                            mode1_veh_spd_id,
                                            0 };
    // Message is only defined in J1979.xml which is included by filename in bmw_motorrad.xml
    auto msg_template = parser.findMatch(msg_data);
    GTEST_ASSERT_TRUE(strcmp(msg_template->getName().c_str(), "M01VehicleSpeed") == 0);
}


int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}