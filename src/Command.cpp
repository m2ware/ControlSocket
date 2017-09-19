#include "Command.h"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <stdio.h>
#include <cstring>

using namespace std;

Argument::Argument(const string name, const string argString, const int order,
                   const bool bind, const int bindOrder) :
    name(name), argString(argString), order(order), bind(bind), bindOrder(bindOrder) {}

Argument::Argument(const Argument &arg) :
    Argument(arg.name, arg.argString, arg.order, arg.bind, arg.bindOrder)
{}

string Argument::getString(const vector<string> &bindings) const
{
    // Unbound parameters just pass argString
    if (!bind)
    {
        return argString;
    }
    if (bindOrder >= bindings.size()) return "";
    string str = argString;
    if (bind) str += bindings[bindOrder];
    return str;
}

Command::Command(const string keyword, const string binary) :
    keyword(keyword), binary(binary)
{
}

Command::Command(const Command &cmd) :
    Command(cmd.keyword, cmd.binary)
{
    arguments.insert(cmd.arguments.begin(), cmd.arguments.end());
}

void Command::addArgument(const Argument &arg)
{
    cout << "[" << arg.name << "]";
    arguments.insert(std::pair<int, Argument> (arg.order, arg) );
}

int Command::run(const vector<string> &bindings) const
{
    //const char ** args = new const char * [arguments.size()+2];
    char ** args = new char * [arguments.size()+2];
    char **argCopy = new char *[arguments.size()+2];
    for (int i = 0; i < arguments.size()+2; i++)
    {
        argCopy[i] = new char[512];
    }

    strcpy(argCopy[0], binary.c_str());
    //args[0] = binary.c_str();
    args[0] = argCopy[0];

    cout << "[" << args[0] << "]";
    int idx = 1;
    // We're making use of the sorting property of the map by
    // specified from the XML config.  We use an indexing int
    // so order needn't be zero-based or sequential.
    for (map<int, Argument>::const_iterator it = arguments.begin();
         it != arguments.end(); ++it)
    {
        string argString = it->second.getString(bindings);
        if (argString.length() > 0)
        {
            strcpy(argCopy[idx], argString.c_str());
            //args[idx] = argString.c_str();
            args[idx] = argCopy[idx];
            cout << idx;
            cout << "[" << args[idx] << "]";
            idx++;
        }
    }
    args[idx] = NULL; // In case we have fewer args than expected.
    args[arguments.size()+2-1] = NULL; // Null terminate
    cout << endl << flush;
    for (int i = 0; i < arguments.size() + 2; i++)
    {
      if ( args[i] == NULL ) continue;
      cout << "[" << args[i] << "]";
    }
    cout << endl << flush;
    cout << "GO!" << endl << flush;

    //int returnValue = execvp(binary.c_str(), (char * const *)args);
    int returnValue = execvp(binary.c_str(), args);
    if (returnValue) perror("Error running command: ");
    cout << "Done" << endl;
    delete[] args;
    for (int i = 0; i < arguments.size()+2; i++)
    {
        delete [] argCopy[i];
    }
    delete[] argCopy;
    return returnValue;
}
