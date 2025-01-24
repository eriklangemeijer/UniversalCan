#include <CanMessage.h>

#include <functional>
#include <string>
#include <vector>

class ITransciever {
  public:
    virtual void start() = 0;
    virtual bool sendMessages(std::vector<CanMessage> messages) = 0;
};