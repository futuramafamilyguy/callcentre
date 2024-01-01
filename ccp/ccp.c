#include "ccp.h"

char* concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1) + strlen(s2) + 2); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, " ");
    strcat(result, s2);
    return result;
}

void parse_ccp(char *ccp_msg, char *name, char *payload)
{
    *name = strtok(ccp_msg, " ");
    *payload = strtok(NULL, "");
}

void build_ccp(char *ccp_msg, char *name, char *payload)
{
    *ccp_msg = concat(name, payload);
}