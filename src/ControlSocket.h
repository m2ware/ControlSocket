/*

Copyright (C) 2016 Jeffrey Moore

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef CONTROL_SOCKET_INCLUDE
#define CONTROL_SOCKET_INCLUDE

#define CONTROL_SOCKET_VERSION "0.1.0"

#include "tinyxml2.h"
#include "IvySox.h"
#include "Command.h"
//#include <pthread.h>
#include <unordered_map>
#include <vector>
#include <map>
#include <string>
//#include <iostream>
//#include <fstream>


#define CONTROL_SOCKET_CONFIG "config.xml"
#define DEFAULT_PORT_NUMBER 8088
#define INBOUND_BUFFER_SIZE 10000
#define OUTBOUND_BUFFER_SIZE 10000

using namespace std;
using namespace tinyxml2;

enum LogLevel
{
    none = 0,
    error = 1,
    low = 2,
    medium = 3,
    high = 4
};

class SocketHandler
{
    public:

    SocketHandler(const map<const string, const Command> &commands);
    void connectAndRun();
    InboundConnection &getConnection();

    private:

    InboundConnection connection;
    const map<const string, const Command> &commands;
    char buffer[INBOUND_BUFFER_SIZE];
    int bufferTail = 0;
    string currentWord;
    vector<string> words;
    bool shouldQuit=false;
    bool openQuotes=false;
    void execute();
    void bufferReset();
    void processMessage(const int rxBytes);
    void addWord();
    void handleCommandBufferOverrun();
    int forkAndRun();
    SocketHandler();
};

class ControlSocket
{
    public:

    ControlSocket();
    ControlSocket(const string &config);
    ~ControlSocket();
    int run();
    //void threadRequestHandler(InboundRequest *request);

    private:

    map<const string, const Command> commands;
    void InitDefaults();
    int configXml();
    void listen();
    int portNumber;
    LogLevel logLevel=high;
    string configFileName;

    IvySox ivySox;
    XMLDocument config;

    pthread_mutex_t ivySoxMutex;
    pthread_t threadPool[INBOUND_CONNECTION_LIMIT];
    unsigned int threadIndex;
};

void *threadEntryPoint(void *requestVoid);

void writeLog(const string &logMessage, const bool timestamp=true,
              const LogLevel msgLogLevel = low);
void addTimestamp(ostringstream &message);

#endif
