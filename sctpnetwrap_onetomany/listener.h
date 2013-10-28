#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <netinet/sctp.h>
#include <exception>
#include <sstream>
#include <string>
#include <fcntl.h>
#include "netexceptions.h"

#ifndef LISTENER_H_
#define LISTENER_H_

#define BACKLOG 10

using namespace std;

/*
This class registers a server and listens on desired port.
*/
class Listener
{
public:
 Listener() :  m_serverFD(0), m_port(8888)
  {
  }

 
  //This method listens on desired port.
  void listenOnPort();

  //This method accepts new connections
  int  acceptConnections(sockaddr* pRemoteAddr);

  //Set listening port
  bool setPort(int port);

  //contain server FD
  int m_serverFD;

 private:

  static void sigchld_handler(int s)
  {
    while(waitpid(-1, NULL, WNOHANG) > 0);
  }

  int m_port;
  

};

#endif
