#include "CanMessage.h"

#include <functional>
#include <vector>


class ITransciever
{
    public:
        virtual bool open(const std::string& port);
        virtual void close();
        virtual bool write(const std::vector<uint8_t>& data);
        virtual bool read(std::vector<uint8_t>& buffer, size_t size);
        virtual void registerCallback(std::function<void(std::vector<uint8_t>)> callback);
};