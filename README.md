# ControlSocket
Native code socket listener providing programmable remote control interface for Linux projects

Developed for Raspberry Pi, but will work on most BSD/Linux systems.

[NOTE: Now functional and tested.  Still in beta, working out a few kinks.  Please file an issue if it gives you any trouble. ]

Please see the Wiki for detailed setup, configuration, and usage instructions.

This program provides a socket interface with a simple command interface to allow remote control of projects.  A configuration file allows commands to be defined along with binding information for parameters.  When a remote process connects to the listener, it will execute the commands as configured and bind parameters in the message to command line arguments for execution.

This interface allows for control of projects requiring lower latency or higher message volume than would be practical for web service interfaces (for example, responsive control of robotics projects).  You can connect the interface to literally anything on your host machine provided that it can be invoked from the command line.

The client can be any remote program that can connect to the socket and supply simple commands.  The command interface follows a very basic subset of Tcl:

- All commands consist of lists of words, which are space-separated
- The first word in a command is the command to be executed, subsequent words are arguments
- Double-quotes can be used for arguments with spaces in them
- Commands are terminated by either semicolons or carriage returns

For example, here's two hypothetical commands:

print_n_times "Hello, world!" 3;print_backwards "!FooBar"

Might produce the following output on the host machine:

Hello, world!
Hello, world!
Hello, world!
raBooF!




