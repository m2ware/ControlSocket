#include "Command.h"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <stdio.h>

using namespace std;

Argument::Argument(const string name, const string argString, const int order,
                   const bool bind, const int bindOrder) :
    name(name), argString(argString), order(order), bind(bind), bindOrder(bindOrder) {}

Argument::Argument(const Argument &arg) :
    Argument(arg.name, arg.argString, arg.order, arg.bind, arg.bindOrder)
    //name(arg.name), argString(arg.argString), order(arg.order), bind(arg.bind), bindOrder(arg.bindOrder)
{}

string Argument::getString(const vector<string> &bindings)
{
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
    /*
    for (map<int, Argument>::const_iterator it = cmd.arguments.begin();
         it != cmd.arguments.end(); it++)
    {
        addArgument(it->second);
    }*/
    arguments.insert(cmd.arguments.begin(), cmd.arguments.end());
}

void Command::addArgument(const Argument &arg)
{
    //arguments[arg.order] = arg;
    arguments.insert(std::pair<int, Argument> (arg.order, arg) );
}

int Command::run(const vector<string> &bindings)
{
    const char **args = new const char *[arguments.size()+2];
    args[0] = binary.c_str();
    int idx = 1;
    // We're making use of the sorting property of the map by
    // specified from the XML config.  We use an indexing int
    // so order needn't be zero-based or sequential.
    for (map<int, Argument>::iterator it = arguments.begin();
         it != arguments.end(); ++it)
    {
        args[idx] = it->second.getString(bindings).c_str();
        cout << args[idx];
        idx++;
    }
    args[arguments.size()]=0; // Null terminate

    int returnValue = execv(binary.c_str(), args);
    if (returnValue) perror("Error running command: ");
    delete(args);
    return returnValue;
}
