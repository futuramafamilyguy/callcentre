# callcentre
all msgs sent by clients are received by the server

server can only send msgs to a single client at a time

on the server side, clients are identified by their fd

to specify which client to send msgs to, just start the msg with the target client fd followed by a single space and append the rest of the msg to it

eg. 5 hello there

*will send "hello there" to the client whose sock fd is 5
