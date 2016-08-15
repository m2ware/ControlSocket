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

#include "ControlSocket.h"
#include "Command.h"
#include "StringHelperFunctions.h"
#include <ctime>
//#include <sstream>
#include <sstream>
#include <string>
#include <iostream>
#include <stdlib.h>
//#include <unistd.h>
#include <sys/wait.h>
//#include <fcntl.h>
#include <pthread.h>

using namespace std;
using namespace tinyxml2;

SocketHandler::SocketHandler(const int socketNumber,
                             const map<const string, const Command> &commands) :
    connection(socketNumber), commands(commands)
{}

ControlSocket::ControlSocket()
{
    InitDefaults();
    configFileName=CONTROL_SOCKET_CONFIG;
    configXml();
}

ControlSocket::ControlSocket(const string &config)
{
    InitDefaults();
    configFileName=config;
    configXml();
}

ControlSocket::~ControlSocket()
{
}

void ControlSocket::InitDefaults()
{
    // Initialize with default settings
    ivySoxMutex = PTHREAD_MUTEX_INITIALIZER;
    portNumber = DEFAULT_PORT_NUMBER;
    logLevel = medium;
}

int ControlSocket::run()
{
    // Open the designated port
    writeLog("Opening port " + toString(portNumber), true, low );
    ivySox.openServerOnPort(portNumber);
    // Listen on port
    writeLog("Listening on port " + toString(portNumber) + "...", true, low);
    if (ivySox.listenToPort() < 0)
    {
        perror("Listen: ");
        writeLog("Error opening port!");
        return -1;
    }

    writeLog("Listening for inbound connections...", true, high);

    while (1)
    {
        // Listen for requests
        handleInboundRequest();
    }
}

void ControlSocket::handleInboundRequest()
{
    // Accept inbound & Log request
    int socketNumber = ivySox.acceptInbound();
    SocketHandler *handler = new SocketHandler(socketNumber, commands);

    // Create a detached thread to handle inbound request
    pthread_t aThread;
    pthread_attr_t threadAttribute;
    pthread_attr_init(&threadAttribute);
    pthread_attr_setdetachstate(&threadAttribute, PTHREAD_CREATE_DETACHED);
    int result = pthread_create( &aThread, &threadAttribute,
                                 threadEntryPoint, (void *) handler);
}

void *threadEntryPoint(void *requestVoid)
{
    SocketHandler *handler = (SocketHandler *) requestVoid;
    handler->connectAndRun();
    delete handler;
    pthread_exit(NULL);
}

void SocketHandler::connectAndRun()
{
    writeLog("Binding inbound connection from " + connection.getIpAddress(), true, low );
    shouldQuit=false;

    while (!shouldQuit)
    {
        int rxBytes = connection.receive(buffer, INBOUND_BUFFER_SIZE);
        if (rxBytes > 0) processMessage(rxBytes);
    }
}

// Process everything in the message queue so far, executing messages as they appear.
void SocketHandler::processMessage(const int rxBytes)
{
    int bufferHead = bufferTail+rxBytes;
    for (int i = 0; i < rxBytes; i++)
    {
        char c = buffer[i];

        if (c == '"')
        {
            if (openQuotes)
            {
                addWord();
                openQuotes=false;
            } else
            {
                openQuotes=true;
            }
            continue;
        }
        if (!openQuotes)
        {
            if (isCarriageReturn(c) || c==';')
            {
                addWord();
                execute();
                openQuotes=false;
                currentWord = "";
                continue;
            }
            if (isWhitespace(c))
            {
                addWord();
                continue;
            }
        }
        currentWord += c;
    }
}

void SocketHandler::addWord()
{
    if (currentWord.length() > 0) words.push_back(currentWord);
    currentWord = "";
}

void SocketHandler::execute()
{
    if (words.size() == 0) return;
    writeLog("Executing command:");
    if (lowerCase(words[0]) == "quit")
    {
        // Quit command from client results in closing connection.
        shouldQuit=true;
        return;
    }

    if ( commands.find(words[0]) == commands.end() )
    {
        writeLog("No action for command " + words[0]);
    } else {
        int result=forkAndRun();
        if (result)
        {
            int err = errno;
            cout << "Errno = " << result << "\n";
        }
    }
    words.clear();
}

int SocketHandler::forkAndRun()
{
    int returnValue=0;
    pid_t processId = fork();
    if (processId < 0)
    {
        perror("Fork");
        cout << "Could not fork!!!";
        return -1;
    } else if (processId > 0)
    {
        // Parent process - wait.
        int waitStatus;
        waitpid(processId, &waitStatus,0);
    } else
    {
        // Child process - execute
        // setenv("ENV_VAR", value->c_str(),1);
        returnValue = commands.find(words[0])->second.run(words);
    }
    return returnValue;
}

void SocketHandler::bufferReset()
{
    // Reset buffer state for new command
    bufferTail = 0;
    words.clear();
    currentWord="";
}

void SocketHandler::handleCommandBufferOverrun()
{
    writeLog("Overran command buffer!  Flushing...");
    bufferReset();
}

void writeLog(const string &logMessage, const bool timestamp,
              const LogLevel msgLogLevel)
{
    // Log level enforcement from configuration
    //if (msgLogLevel > logLevel) return;

    //  Time stamp, add string, write to log.
    ostringstream message("");

    if (timestamp)
    {
        addTimestamp(message);
    }
    message << logMessage << endl;

    cout << message.str();
}

void addTimestamp(ostringstream &message)
{
        time_t currentTime = time(0);
        struct tm *timeStruct = localtime(&currentTime);

        message << "[" << (timeStruct->tm_year + 1900) << '-'
                       << (timeStruct->tm_mon + 1) << '-'
                       << (timeStruct->tm_mday) << " "
                       << (timeStruct->tm_hour) << ":";
        if (timeStruct->tm_min < 10) message << "0";
        message        << (timeStruct->tm_min) << ":";
        if (timeStruct->tm_sec < 10) message << "0";
        message        << (timeStruct->tm_sec) << "]  ";
}

int ControlSocket::configXml()
{
    string msg = "Opening ";
    msg += configFileName;
    writeLog(msg);
    config.LoadFile(configFileName.c_str());

    //  Pick apart the XML for config options...
    XMLElement *root = config.RootElement();
    if ( root == NULL )
    {
        writeLog("Error parsing file - no root element found.", true, error);
        return -1;
    }

    XMLElement *parserElement = root->FirstChildElement("Configuration");
    if (parserElement != NULL)
    {
        writeLog("Found config block.");
        XMLElement *configElement = parserElement->FirstChildElement();
        while (configElement != NULL)
        {
            string settingName = configElement->Name();
            string value = configElement->Attribute("value");
            writeLog(settingName + " = " + value,false, low);
            if ( settingName == "PortNumber")
            {
                portNumber = configElement->IntAttribute("value");
                writeLog("PortNumber = " + portNumber);
            }
            configElement = configElement->NextSiblingElement();
        }
    }

    //  Configuration Options
    parserElement = root->FirstChildElement("Commands");
    if (parserElement != NULL)
    {
        XMLElement *configElement = parserElement->FirstChildElement("Command");

        while (configElement != NULL)
        {
            string keyword = configElement->Attribute("keyword");
            string binary = configElement->Attribute("binary");
            Command command(keyword, binary);
            XMLElement *argElement = configElement->FirstChildElement("arg");

            while (argElement != NULL)
            {
                XMLError error = XML_NO_ERROR;
                int order = argElement->IntAttribute("order");
                int bindOrder = -1;
                bool bind=false;
                error = argElement->QueryIntAttribute("bindOrder", &bindOrder);
                if (error == XML_NO_ERROR) bind=true;
                string argName = argElement->Attribute("name");
                string argString = argElement->Attribute("argString");
                Argument arg(argName, argString, order, bind, bindOrder);
                command.addArgument(arg);
            }
            commands.insert(std::pair<const string, const Command>(keyword,command));
            writeLog("Added command [" + keyword + "]");
            configElement = configElement->NextSiblingElement("Command");
        }
    }
    return 0; // Success
}
