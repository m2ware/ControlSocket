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
#include <sstream>
#include <stdlib.h>
//#include <unistd.h>
//#include <sys/wait.h>
//#include <fcntl.h>
#include <pthread.h>

using namespace std;
using namespace tinyxml2;

SocketHandler::SocketHandler(const int socketNumber, const IvySox &ivySox) :
    socketNumber(socketNumber), ivySox(ivySox)
{
}

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
    InboundConnection *connection = new InboundRequest();
    request->ControlSocketPtr = (void *)this;

    request->socketNumber = ivySox.acceptInbound(&request->inbound);

    // Create a detached thread to handle inbound request
    pthread_t aThread;
    pthread_attr_t threadAttribute;
    pthread_attr_init(&threadAttribute);
    pthread_attr_setdetachstate(&threadAttribute, PTHREAD_CREATE_DETACHED);
    int result = pthread_create( &aThread, &threadAttribute, threadEntryPoint, (void *) request);

}

/*
void *threadEntryPoint(void *requestVoid)
{
    InboundRequest *request = (InboundRequest *) requestVoid;
    ControlSocket *ControlSocket = (ControlSocket *)request->ControlSocketPtr;
    ControlSocket->threadRequestHandler(request);
    delete request;
    pthread_exit(NULL);
}*/

//  Except for logging, this can be moved to request class
void ControlSocket::threadRequestHandler(SocketHandler &handler)
{
    writeLog("Binding inbound connection from " + request->inbound.getIpAddress(), true, low );

    while (1)
    {

    }

    request->receivedBytes = request->inbound.receive(request->inboundBuffer, INBOUND_BUFFER_SIZE);
    request->requestString = ivySox.messageToString(request->inboundBuffer, request->receivedBytes);
    writeLog(request->requestString, false, high);
    writeLog("(" + toString(request->receivedBytes) + " bytes)",false, medium);
    // Parse the request string to get the file or script being requested
    // request->requestedFile = parseRequest(request->requestString);
    // request->queryString = extractQueryArgs(request->requestedFile);
    request->parse(defaultFileName);

}

void ControlSocket::writeLog(const string &logMessage, const bool timestamp,
                      const LogLevel msgLogLevel)
{
    // Log level enforcement from configuration
    if (msgLogLevel > logLevel) return;

    //  Time stamp, add string, write to log.
    ostringstream message("");

    if (timestamp)
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
    message << logMessage << endl;

    cout << message.str();
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

    XMLElement *configElement = root->FirstChildElement("Configuration");
    if (parserElement != NULL)
    {
        XMLElement *configElement = parserElement->FirstChildElement();
        while (configElement != NULL)
        {
            string settingName = configElement->Name();
            string value = configElement->Attribute("value");
            writeLog(settingName + " = " + value,false, low);
            if ( settingName == "PortNumber")
                portNumber = configElement->IntAttribute("value");
            configElement = configElement->NextSiblingElement();
    }

    //  Configuration Options
    XMLElement *parserElement = root->FirstChildElement("Commands");
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
                if (error == XML_NO_ERROR)
                {
                    bind=true;
                }
                string argName = argElement->Attribute("name");
                string argString = argElement->Attribute("argString");
                Argument arg(argName, argString, order, bind, bindOrder);
                command.addArgument(arg);
            }
            commands[keyword]=command;
            configElement = configElement->NextSiblingElement("Command");
        }
    }
    return 0; // Success
}
