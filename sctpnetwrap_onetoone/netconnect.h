#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/sctp.h>
#include <sstream>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef NETCONNECT_H_
#define NETCONNECT_H_

using namespace std;

//This class connects to a server over sctp
class Connection
{
public:
  Connection( char* hostName, int port ) : m_fd(0)
  {
    m_hostname.append(hostName);
    m_port = port;
  }

  //Set target host/server name
  void setHostName(char* hostName) { 
    m_hostname.clear();
    m_hostname.append(hostName);
  }

  //Set host/server port no
  void setPortNo(int port) { m_port = port; }

  //Get file descriptor after connect
  int getConnectedFD() { return m_fd; }

  //Connect server
  int connectServer();

  //Disconnect from server
  void disconnect();

private:
  string m_hostname;
  int  m_port;
  int  m_fd;
};

#endif
