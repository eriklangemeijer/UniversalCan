#include <gtest/gtest.h>
#include <memory>
#include <CanMessage.h>
#include <CanMessageTemplate.h>
#include <ProtocolDefinitionParser.h>

const uint8_t mode1_response_id = 0x41;
/* Mode 1 messages can be used to request live data. 
   This test verifies how responses to these messages are parsed by the template
   In this test vehicle speed response is checked which is stored directly in the message in kph */
TEST(M01EngSpdResponse, add)
{
    const uint8_t mode1_veh_spd_id = 0x0D;
    std::string mode1_veh_spd_template_str = "<Message Name=\"M01VehicleSpeed\" Description=\"Mode $01 response for VehicleSpeed\">\
                                                <Values>\
                                                    <Value name=\"VehicleSpeed\" DataType=\"uint8\">\
                                                        <Operations> \
                                                            <BYTE_SELECT arg1=\"2\" arg2=\"3\"/>\
                                                        </Operations>\
                                                    </Value>\
                                                </Values>\
                                            </Message>";
                                            
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(mode1_veh_spd_template_str.c_str());
    pugi::xml_node msg = doc.child("Message");
    std::shared_ptr<CanMessageTemplate> mode1_veh_spd_template = std::make_shared<CanMessageTemplate>(msg);
    CanMessage response_0kph = CanMessage({mode1_response_id, mode1_veh_spd_id, 0}, mode1_veh_spd_template);
    GTEST_ASSERT_EQ(response_0kph.values[0], 0);
    CanMessage response_60kph = CanMessage({mode1_response_id, mode1_veh_spd_id, 60}, mode1_veh_spd_template);
    GTEST_ASSERT_EQ(response_60kph.values[0], 60);
    CanMessage response_120kph = CanMessage({mode1_response_id, mode1_veh_spd_id, 120}, mode1_veh_spd_template);
    GTEST_ASSERT_EQ(response_120kph.values[0], 120);
}
TEST(M01PIDSupportResponse, add)
{
    const uint8_t mode1_supp_pid = 0x00;
    ProtocolDefinitionParser parser = ProtocolDefinitionParser("../../../ProtocolDefinitions/J1979.xml");
    std::shared_ptr<CanMessageTemplate> template_ptr = std::make_shared<CanMessageTemplate> (*parser.getMessageList().begin());
    CanMessage response_supp_pid = CanMessage({mode1_response_id, mode1_supp_pid, 0xBE, 0x1F, 0xA8, 0x13}, template_ptr);
    // From wikipedia table: https://en.wikipedia.org/wiki/OBD-II_PIDs#Service_01_PID_00
    std::vector<bool> expected_response = {true, false, true, true, true, true, true, false,
                                           false, false, false, true, true, true, true, true,
                                           true, false, true, false, true, false, false, false,
                                           false, false, false, true, false, false, true, true};
    for(uint8_t ii = 0; ii < expected_response.size(); ii++) {
        
        GTEST_ASSERT_EQ((bool)response_supp_pid.values[ii], (bool)expected_response[ii]);
    }
    
}

TEST(IncludeRelativeFile, add)
{
    const uint8_t mode1_supp_pid = 0x00;
    ProtocolDefinitionParser parser = ProtocolDefinitionParser("../../../ProtocolDefinitions/bmw_motorrad.xml");    
}

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}