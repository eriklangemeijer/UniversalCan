#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <ModifierFunction.h>

/* Function used to simplify creation of a ModifierFunction object based on an xml string. */
std::shared_ptr<ModifierFunction> InitModifierFunction(const std::string &xml_str) {
    pugi::xml_document doc;
    doc.load_string(xml_str.c_str());
    pugi::xml_node function_xml = doc.first_child();
    return std::make_shared<ModifierFunction>(function_xml);
}

/* Function used to simplify testing function calls that should result in runtime_errors */
void FunctionCallExpectedError(std::shared_ptr<ModifierFunction> function, const std::string expected_error_string, const std::vector<uint8_t> &input = {0, 0, 0}) {
    EXPECT_THROW({
        try
        {
            function->call(input);
        }
        catch( const std::runtime_error& e )
        {
            EXPECT_STREQ(expected_error_string.c_str(), e.what() );
            throw;
        } }, std::runtime_error);
}

/* Test to verify behaviour when a ModifierFunction is created with an unsupported name*/
TEST(ModifierFunction, UnsupportedFunction) {
    const uint8_t test_value = 0xFA;
    std::string const template_str =
        "<FOOBAR></FOOBAR>";

    pugi::xml_document doc;
    doc.load_string(template_str.c_str());
    pugi::xml_node function_xml = doc.first_child();
    EXPECT_THROW({
        try
        {
            auto function = ModifierFunction(function_xml);
        }
        catch( const std::runtime_error& e )
        {
            EXPECT_STREQ("Unsupported function FOOBAR\n", e.what() );
            throw;
        } }, std::runtime_error);
}

TEST(ModifierFunctionConstant, HexConstantOneByte) {
    const uint8_t test_value = 0xFA;
    std::string const template_str =
        "<CONSTANT value=\"0xFA\"/>";

    std::shared_ptr<ModifierFunction> function = InitModifierFunction(template_str);
    std::vector<uint8_t> ret_value = function->call({0});
    uint8_t ret_int = *((uint8_t *)ret_value.data());
    // GTEST_ASSERT_EQ(ret_value.size(), 1);
    GTEST_ASSERT_EQ(ret_int, test_value);
}

TEST(ModifierFunctionConstant, HexConstantTwoBytes) {
    const uint16_t test_value = 0xFABE;
    std::string const template_str =
        "<CONSTANT value=\"0xFABE\"/>";

    std::shared_ptr<ModifierFunction> function = InitModifierFunction(template_str);
    std::vector<uint8_t> ret_value = function->call({0});
    // GTEST_ASSERT_EQ(ret_value.size(), 2);
    uint16_t ret_int = *((uint16_t *)ret_value.data());
    // GTEST_ASSERT_EQ(ret_value.size(), 1);
    GTEST_ASSERT_EQ(ret_int, test_value);
}

/* Test to verify the BYTE_SELECT function using constant values.
    A single byte is selected.*/
TEST(ModifierFunctionByteSelect, OneByteOk) {
    const uint8_t test_value = 0xCE;
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

/* Test to verify the BYTE_SELECT function using constant values
    An array of bytes is selected.*/
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

TEST(ModifierFunctionMultiply, TwoConstants) {
    const uint8_t test_value = 6;
    std::string const template_str =
        "<MULTIPLY>\
            <CONSTANT value=\"3\"/>\
            <CONSTANT value=\"2\"/>\
        </MULTIPLY>";

    std::shared_ptr<ModifierFunction> function = InitModifierFunction(template_str);
    std::vector<uint8_t> ret_value = function->call({0});
    GTEST_ASSERT_EQ(ret_value[0], test_value);
}

TEST(ModifierFunctionMultiply, TwoLargeConstants) {
    const uint32_t test_value = 9999 * 2222;
    std::string const template_str =
        "<MULTIPLY>\
            <CONSTANT value=\"9999\"/>\
            <CONSTANT value=\"2222\"/>\
        </MULTIPLY>";

    std::shared_ptr<ModifierFunction> function = InitModifierFunction(template_str);
    std::vector<uint8_t> ret_value = function->call({0});
    uint32_t ret_int = *((uint32_t *)ret_value.data());
    GTEST_ASSERT_EQ(ret_int, test_value);
}

TEST(ModifierFunctionMultiply, OneConstantOneByteSelect) {
    const uint8_t test_value = 6;
    std::string const template_str =
        "<MULTIPLY>\
            <CONSTANT value=\"3\"/>\
            <BYTE_SELECT>\
                <CONSTANT value=\"1\"/>\
                <CONSTANT value=\"2\"/>\
            </BYTE_SELECT>\
        </MULTIPLY>";

    std::shared_ptr<ModifierFunction> function = InitModifierFunction(template_str);
    std::vector<uint8_t> ret_value = function->call({0, test_value, 0});
    GTEST_ASSERT_EQ(ret_value[0], test_value * 3);
}

TEST(ModifierFunctionMultiply, InlineConstant) {
    const uint32_t test_value = 9999 * 2222;
    std::string const template_str =
        "<MULTIPLY arg1=\"9999\">\
            <CONSTANT value=\"2222\"/>\
        </MULTIPLY>";

    std::shared_ptr<ModifierFunction> function = InitModifierFunction(template_str);
    std::vector<uint8_t> ret_value = function->call({0});
    uint32_t ret_int = *((uint32_t *)ret_value.data());
    GTEST_ASSERT_EQ(ret_int, test_value);
}

TEST(ModifierFunctionMultiply, TwoInlineConstant) {
    const uint32_t test_value = 9999 * 2222;
    std::string const template_str =
        "<MULTIPLY arg1=\"9999\" arg2=\"2222\">\
        </MULTIPLY>";

    std::shared_ptr<ModifierFunction> function = InitModifierFunction(template_str);
    std::vector<uint8_t> ret_value = function->call({0});
    uint32_t ret_int = *((uint32_t *)ret_value.data());
    GTEST_ASSERT_EQ(ret_int, test_value);
}

TEST(ModifierFunctionMultiply, TwoInlineConstantShortNotation) {
    const uint32_t test_value = 9999 * 2222;
    std::string const template_str =
        "<MULTIPLY arg1=\"9999\" arg2=\"2222\"/>";

    std::shared_ptr<ModifierFunction> function = InitModifierFunction(template_str);
    std::vector<uint8_t> ret_value = function->call({0});
    uint32_t ret_int = *((uint32_t *)ret_value.data());
    GTEST_ASSERT_EQ(ret_int, test_value);
}

TEST(ModifierFunctionByteCompare, TwoByteHex) {
    std::string const template_str =
        "<INT_COMPARE arg2=\"0x0D41\">\
            <BYTE_SELECT arg1=\"0\" arg2=\"2\"/>\
        </INT_COMPARE>";

    std::shared_ptr<ModifierFunction> function = InitModifierFunction(template_str);
    std::vector<uint8_t> ret_value = function->call({0x41, 0x0D}); //data becomes flipped due to little endian OSes
    bool ret_bool = *((bool *)ret_value.data());
    GTEST_ASSERT_TRUE(ret_bool);
    
    ret_value = function->call({0x0D, 0x41});
    ret_bool = *((bool *)ret_value.data());
    GTEST_ASSERT_FALSE(ret_bool);
}