#ifndef STRING_HELPER_FUNCTIONS_INCLUDE
#define STRING_HELPER_FUNCTIONS_INCLUDE

#include <string>
#include <sstream>

using namespace std;

string upperCase(string input);
string lowerCase(string input);
bool isWhitespace(char c);
bool isCarriageReturn(char c);


template <class myType> string toString(myType item)
{
    ostringstream oss;
    oss << item;
    return (oss.str());
}
#endif
