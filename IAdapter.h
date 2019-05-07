#include "CanMessage.h"

#include <functional>
#include <vector>


class IAdapter
{
    public:
        virtual bool sendMessage(std::vector<CanMessage> messages) = 0;
        virtual void registerCallback(std::function<void(CanMessage)> callback) = 0;

};