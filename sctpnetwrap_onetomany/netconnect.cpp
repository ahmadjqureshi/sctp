#include "netconnect.h"

int Connection::connectServer()
{
  struct addrinfo hints, *res;
  ostringstream port;   // stream used for port the conversion
  int flags, ret;
  fd_set fdset;
  int timeout = 5;
  timeval until, timeoutTV;
  timeval currentTime;

  port << m_port; 

  // first, load up address structs with getaddrinfo():
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;

  //One to one socket
  //hints.ai_socktype = SOCK_STREAM;

  //One to many socket
  hints.ai_socktype = SOCK_SEQPACKET;

  getaddrinfo( m_hostname.c_str(), port.str().c_str(), &hints, &res);
  
  // make a socket:
  m_fd = socket(res->ai_family, res->ai_socktype, IPPROTO_SCTP );

  int noDelay = 1;
  m_hostAddr = *(res->ai_addr);

  //Send data as soon as possible
  if (setsockopt( m_fd, SOL_SCTP, SCTP_NODELAY, &noDelay, sizeof(noDelay)) == -1) {
      perror("setsockopt 1");
      exit(1);
  }

  return m_fd;
}

void Connection::disconnect()
{
  if ( m_fd != 0 )
  {
    shutdown( m_fd, 2);
    close(m_fd);
  }
}
