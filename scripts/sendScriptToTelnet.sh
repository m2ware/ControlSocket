# From the command line on a client machine, you can do the following:
#
#   ./sendScriptToTelnet.sh command.script | telnet 192.168.1.106 8089
#
# This will push the entire contents of command.script over the wire to
# ControlSocket.
#
# The ONLY FUNCTION of this script is to sleep long enough for Telnet
# to connect to the host server before sending the command script.

# Wait for telnet connect, increase sleep time if needed.
sleep 1
# Send script to host
cat command.script
