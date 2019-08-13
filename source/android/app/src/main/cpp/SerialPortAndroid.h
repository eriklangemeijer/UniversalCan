//
// Created by Erik on 8/8/2019.
//

#ifndef ANDROID_SERIALPORTANDROID_H
#define ANDROID_SERIALPORTANDROID_H
#include <string>
#include <sstream>
#include <iostream>
#include <jni.h>
#include "../../../../../generic/ISerialPort.h"
#include "../../../../../generic/CanMessage.h"
#include "../../../../../../../../Users/Erik/AppData/Local/Android/Sdk/ndk-bundle/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/c++/v1/string"

class SerialPortAndroid : public ISerialPort    {
private:
    JavaVM* g_JavaVM;
public:
    SerialPortAndroid(JNIEnv* env);
    ~SerialPortAndroid();
    bool sendMessage(std::vector<char> data);
    bool sendString(std::string data);
    void registerCallback(std::function<void(std::string)> callback);
    void messageReceived(std::string message);

};


#endif //ANDROID_SERIALPORTANDROID_H
