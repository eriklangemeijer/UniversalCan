#include <functional>
#include <vector>
#include <string>
class ISerialPort
{
    public:
        virtual bool sendMessage(std::vector<char> data) = 0;
        virtual bool sendString(std::string data) = 0;
        virtual void registerCallback(std::function<void(std::string)> callback) = 0;
};