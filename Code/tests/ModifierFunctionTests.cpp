#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

#include <ModifierFunction.h>

std::shared_ptr<ModifierFunction> InitModifierFunction(const std::string& xml_str) {
    pugi::xml_document doc;
    doc.load_string(xml_str.c_str());
    pugi::xml_node function_xml = doc.first_child();
    return std::make_unique<ModifierFunction>(function_xml);

}

TEST(ModifierFunctionByteSelect, OneByteOk) {
    const uint8_t test_value = 0xFA;
    std::string const template_str =
        "<BYTE_SELECT>\
            <CONSTANT value=\"1\"/>\
            <CONSTANT value=\"2\"/>\
        </BYTE_SELECT>";

    std::shared_ptr<ModifierFunction> function = InitModifierFunction(template_str);
    std::vector<uint8_t> ret_value = function->call({0, test_value, 0});
    GTEST_ASSERT_EQ(ret_value.size(), 1);
    GTEST_ASSERT_EQ(ret_value[0], test_value);
}

TEST(ModifierFunctionByteSelect, MultiByteOk) {
    const uint8_t test_value = 0xFA;
    const uint8_t nr_values = 4;
    const uint8_t rear_pad = 2;
    std::string const template_str =
        "<BYTE_SELECT>\
            <CONSTANT value=\"1\"/>\
            <CONSTANT value=\"5\"/>\
        </BYTE_SELECT>";

    std::shared_ptr<ModifierFunction> function = InitModifierFunction(template_str);

    std::vector<uint8_t> input_value = {0};
    for (uint8_t ii = 0; ii < nr_values + rear_pad; ii++) {
        input_value.push_back(test_value + ii);
    }
    std::vector<uint8_t> ret_value = function->call(input_value);
    GTEST_ASSERT_EQ(ret_value.size(), nr_values);

    for (uint8_t ii = 0; ii < nr_values; ii++) {
        GTEST_ASSERT_EQ(ret_value[ii], test_value + ii);
    }
}

void FunctionCallExpectedError(std::shared_ptr<ModifierFunction> function, const std::string expected_error_string, const std::vector<uint8_t>& input = {0, 0, 0}) {
    EXPECT_THROW({
        try
        {
            function->call(input);
        }
        catch( const std::runtime_error& e )
        {
            EXPECT_STREQ(expected_error_string.c_str(), e.what() );
            throw;
        }
    }, std::runtime_error);
}

TEST(ModifierFunctionByteSelect, NoArgs) {
    std::string const template_str =
        "<BYTE_SELECT>\
        </BYTE_SELECT>";
    auto function = InitModifierFunction(template_str);
    FunctionCallExpectedError(function, "Incorrect number of arguments provided");
}

TEST(ModifierFunctionByteSelect, OneArgs) {
    std::string const template_str =
        "<BYTE_SELECT>\
            <CONSTANT value=\"1\"/>\
        </BYTE_SELECT>";
    auto function = InitModifierFunction(template_str);
    FunctionCallExpectedError(function, "Incorrect number of arguments provided");
}

TEST(ModifierFunctionByteSelect, ThreeArgs) {
    std::string const template_str =
        "<BYTE_SELECT>\
            <CONSTANT value=\"1\"/>\
            <CONSTANT value=\"2\"/>\
            <CONSTANT value=\"3\"/>\
        </BYTE_SELECT>";
    auto function = InitModifierFunction(template_str);
    FunctionCallExpectedError(function, "Incorrect number of arguments provided");
}

TEST(ModifierFunctionByteSelect, OutOfRange) {
    std::string const template_str =
        "<BYTE_SELECT>\
            <CONSTANT value=\"1\"/>\
            <CONSTANT value=\"2\"/>\
        </BYTE_SELECT>";
    auto function = InitModifierFunction(template_str);
    FunctionCallExpectedError(function, "Selection is out of range", {0});
}



