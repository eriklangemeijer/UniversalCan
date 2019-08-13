#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include "../../libraries/tinyxml2.h"

#include "CanMessage.h"

class xmlReader
{
private:
    /* data */
    std::vector<CanMessage> messages;
public:
    xmlReader(std::string file);
    ~xmlReader();
};

xmlReader::xmlReader(std::string file)
{
    // std::cout<<"Parsing document\n";
    // tinyxml2::XMLDocument doc;
    // doc.LoadFile(file.c_str());
    // auto foo = doc.FirstChildElement("Vehicle/Module/Message/Value/Description")->GetText();
    // std::cout<<foo;
    // for(auto root = doc.FirstChildElement(); root != NULL; root=root->FirstChildElement())
    // {
    //     auto foo = root->GetText();
    //     std::cout<<foo;
    // }
    using namespace std;
   std::string tmp;
    std::stringstream str_strm(file.c_str());
   vector<string> words;
   char delim = ','; // Ddefine the delimiter to split by

   while (std::getline(str_strm, tmp, delim)) {
      // Provide proper checks here for tmp like if empty
      // Also strip down symbols like !, ., ?, etc.
      // Finally push it.
      words.push_back(tmp);
   }

   for(auto it = words.begin(); it != words.end(); it++) {
      std::cout << *it << std::endl;
   }
}
xmlReader::~xmlReader()
{
}
