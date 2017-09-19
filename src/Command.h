#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <vector>
#include <map>

using namespace std;

class Argument
{
    public:
        const int order;
        const int bindOrder;
        const bool bind;
        const string name;
        const string argString;
        Argument(const string name, const string argString,
                 const int order, const bool bind, const int bindOrder = -1);
        Argument(const Argument &arg); // Copy constructor
        string getString(const vector<string> &bindings) const;
    private:
        Argument();
};


class Command
{
    public:
        const string keyword;
        const string binary;
        Command(const string keyword, const string binary);
        Command(const Command &cmd); // Copy constructor
        void addArgument(const Argument &arg);
        int run(const vector<string> &bindings) const;
    private:
        map<int, Argument> arguments;
        Command();
};

#endif
