#include <iostream>     // cout, endl
#include <fstream>      // fstream
#include <vector>
#include <string>
#include <algorithm>    // copy
#include <iterator>     // ostream_operator

#include <boost/tokenizer.hpp>
#include "libraries/tinyxml2.h"

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
    using namespace boost;

    string data(file.c_str());

    ifstream in(data.c_str());

    typedef tokenizer< escaped_list_separator<char> > Tokenizer;

    vector< string > vec;
    string line;

    while (getline(in,line))
    {
        Tokenizer tok(line);
        vec.assign(tok.begin(),tok.end());

        if (vec.size() < 3) continue;

        copy(vec.begin(), vec.end(),
             ostream_iterator<string>(cout, "|"));

        cout << "\n----------------------" << endl;
    }
}
xmlReader::~xmlReader()
{
}
