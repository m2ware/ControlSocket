# ControlSocket
Native code socket listener providing programmable remote control interface for Linux projects

Developed for Raspberry Pi, but will work on most BSD/Linux systems.

This program provides a socket interface with a simple command interface to allow remote control of projects.  A configuration file allows commands to be defined along with binding information for parameters.  When a remote process connects to the listener, it will execute the commands as configured and bind parameters in the message to command line arguments for execution.

This interface allows for control of projects requiring lower latency or higher message volume than would be practical for web service interfaces (for example, responsive control of robotics projects).  

The client can be any remote program that can connect to the socket and supply simple commands.  Commands may be separated by semicolons or carriage returns.  The socket will stay connected until the connection is terminated by the client, or until a "quit" command is sent.




