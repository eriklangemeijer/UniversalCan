#include <CanMessage.h>

#include <functional>
#include <vector>
#include <string>


class ITransciever
{
    public:
        virtual void start() = 0;
		virtual bool sendMessage(std::vector<CanMessage> messages) = 0;
		virtual void registerCallback(std::function<void(CanMessage)> callback) = 0;
};