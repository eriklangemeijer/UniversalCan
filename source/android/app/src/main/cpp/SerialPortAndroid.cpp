//
// Created by Erik on 8/8/2019.
//

#include "SerialPortAndroid.h"
#include "../../../../../../../../Users/Erik/AppData/Local/Android/Sdk/ndk-bundle/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/jni.h"



SerialPortAndroid::SerialPortAndroid(JNIEnv* env)
{

}

SerialPortAndroid::~SerialPortAndroid()
{

}
bool SerialPortAndroid::sendMessage(std::vector<char> data)
{
    return false;
}
bool SerialPortAndroid::sendString(std::string data)
{
    return false;
}
void SerialPortAndroid::registerCallback(std::function<void(std::string)> callback)
{

}
void SerialPortAndroid::messageReceived(std::string message)
{

}
    