#include <stdio.h>

#ifndef CCP_H
#define CCP_H

void parse_ccp(char *ccp_msg, char *name, char *payload);
void build_ccp(char *ccp_msg, char *name, char *payload);

#endif