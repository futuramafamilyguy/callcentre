# callcentre

all msgs sent by clients are received by the server

server can only send msgs to a single client at a time

on the server side, clients are identified by their fd

to specify which client to send msgs to, just start the msg with the target client fd followed by a single space and append the rest of the msg to it

eg. 5 hello there

*will send "hello there" to the client whose sock fd is 5

## ccp

server and clients communicate using the callcentre protocol (ccp, unfortunate naming i know)

ccp is simply the chat name of the server and the entire msg separated by a single space

this is so that clients can have a name displayed for the server during a chat session

server chat name is set via cmd arguments when starting the program

just to clarify the difference between this and prefixing the input with the client fd above:
- the former is handled by the program, ie. program automatically prefixes the msg with the server chat name and sends the entire thing over the socket. client program will know how to parse the msg to extract the name
- the latter is handled by the operator of the server program, ie. user explicitly prefixes the msg with the target client fd. it does not get sent over the socket, just a way to identify which client to send the msg to on the server side

eg. allen hi

*client will display msg as "allen: hi"
