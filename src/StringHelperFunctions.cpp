#include "StringHelperFunctions.h"

using namespace std;

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

bool isWhitespace(char c)
{
    return ( c == ' ' || c == '\t' || c == '\r' ||
             c == '\v' || c == '\f' );
}

bool isCarriageReturn(char c)
{
    return ( c== '\r' || c == '\n' );
}
