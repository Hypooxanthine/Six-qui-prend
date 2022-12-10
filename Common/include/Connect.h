#ifndef CONNECT_H
#define CONNECT_H

#include "Types.h"

Socket ConnectToServer(const char* hostName, in_port_t port);

void GetHostName(char* hostName);
in_port_t GetPort();

#endif