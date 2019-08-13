#include <jni.h>
#include <string>
#include "C:\dev\CanScan\source\generic\ElmBt.h"
#include "C:\dev\CanScan\source\generic\xmlReader.h"
#include "SerialPortAndroid.h"

#include <memory>
std::unique_ptr<SerialPortAndroid> commHandler;
extern "C" JNIEXPORT jstring JNICALL
Java_com_erik_canscan_MainActivity_nativeMain(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    xmlReader reader("./Databases/BMW_GS.csv");
    commHandler = std::make_unique<SerialPortAndroid>(env);
    ElmBT foo(std::unique_ptr<ISerialPort>(commHandler));

    return env->NewStringUTF(hello.c_str());
}


extern "C" JNIEXPORT jstring JNICALL
Java_com_erik_canscan_BluetoothHandler_serialReceiveCallback( 
    JNIEnv* env,
    jobject /* this */,
    jstring message)
    {
        std::cout<<message;
        
        commHandler->messageReceived(message);
        return message;
    }
