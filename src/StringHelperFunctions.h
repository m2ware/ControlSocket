#ifndef STRING_HELPER_FUNCTIONS_INCLUDE
#define STRING_HELPER_FUNCTIONS_INCLUDE

string upperCase(string input);
string lowerCase(string input);

void *threadEntryPoint(void *requestVoid);

template <class myType> string toString(myType item);


#endif
