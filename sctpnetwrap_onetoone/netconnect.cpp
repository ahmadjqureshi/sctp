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
  hints.ai_socktype = SOCK_STREAM;

  //One to many socket
  //hints.ai_socktype = SOCK_SEQPACKET;

  getaddrinfo( m_hostname.c_str(), port.str().c_str(), &hints, &res);
  
  // make a socket:
  m_fd = socket(res->ai_family, res->ai_socktype, IPPROTO_SCTP /* IPPROTO_TCP*/);

  //set socket as non blocking
  flags = fcntl( m_fd, F_GETFL, 0);
  fcntl( m_fd, F_SETFL, flags | O_NONBLOCK);

  // connect!
  ret = connect( m_fd, res->ai_addr, res->ai_addrlen);
  
  if ( ret == 0)
    return m_fd;

  if (errno != EINPROGRESS )
    return -1;

  FD_ZERO(&fdset);
  FD_SET( m_fd, &fdset);

  gettimeofday( &currentTime, NULL);

  until.tv_sec = currentTime.tv_sec + timeout;
  until.tv_usec = currentTime.tv_usec;

  timeoutTV.tv_sec = timeout;
  timeoutTV.tv_usec = 0;

  while (1)
  {
    //Wait for status from socket
    ret = select( m_fd + 1, 0, &fdset, 0, &timeoutTV);
    if ( errno == EINTR || errno == EWOULDBLOCK)
      continue;

    gettimeofday( &currentTime, NULL);

    /*calculate timeout*/
    if ( ( currentTime.tv_sec >= until.tv_sec) && ( currentTime.tv_usec > until.tv_usec) )
    {
      /*timeout */
      return -1;
    }

    break;
  }

  /*flags = fcntl( m_fd, F_GETFL, 0);
    fcntl( m_fd, F_SETFL, flags & ~O_NONBLOCK);*/

  int noDelay = 1;

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
