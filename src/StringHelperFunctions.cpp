#include "StringHelperFunctions.h"

template <class myType> string toString(myType item)
{
    ostringstream oss;
    oss << item;
    return (oss.str());
}

string upperCase(string input)
{
    for (string::iterator it = input.begin(); it != input.end(); ++ it)
        { *it = toupper(*it); }
    return input;
}

string lowerCase(string input)
{
    for (string::iterator it = input.begin(); it != input.end(); ++ it)
          *it = tolower(*it);
    return input;
}
