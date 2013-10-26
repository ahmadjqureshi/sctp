#include "listener.h"

bool Listener::setPort(int port)
{
  if ( port < 0)
    return false;

  m_port = port;

  return true;
}

void Listener::listenOnPort()
{
  struct addrinfo hints, *servinfo, *p;
  struct sigaction sa;
  int yes=1;
  char s[INET6_ADDRSTRLEN];
  int rv;
  ostringstream port;   // stream used for port the conversion
  struct sctp_initmsg initmsg;

  port << m_port;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM; //One to one socket type
  //hints.ai_socktype = SOCK_SEQPACKET; //One to many socket type
  hints.ai_flags = AI_PASSIVE; // use my IP

  //Get local IPv4 interface addresses
  if ((rv = getaddrinfo(NULL, port.str().c_str(), &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return;
  }
  
  // loop through all the results and bind to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if (( m_serverFD = socket(p->ai_family, p->ai_socktype,
			      IPPROTO_SCTP /*IPPROTO_TCP*/ )) == -1) {
      perror("server: socket");
      continue;
    }
    
    linger lin;
    
    lin.l_onoff = 1;
    lin.l_linger = 30;

    //Socket option to avoid CLOSE_WAIT condition.
    if (setsockopt( m_serverFD, IPPROTO_SCTP, SO_LINGER, &lin, sizeof(linger)) == -1) {
      perror("setsockopt 1");
      exit(1);
      }

    if (bind( m_serverFD, p->ai_addr, p->ai_addrlen) == -1) {
      close( m_serverFD );
      perror("server: bind");
      continue;
    }

    /* 1 stream will be available per socket but streams can be increased*/
    memset( &initmsg, 0, sizeof(initmsg) );
    initmsg.sinit_num_ostreams = 1;
    initmsg.sinit_max_instreams = 1;
    initmsg.sinit_max_attempts = 4;
  
    //Set socket options for multiple streams but just now we have one for receving and one for sending
    if ( setsockopt( m_serverFD, IPPROTO_SCTP, SCTP_INITMSG, 
		   &initmsg, sizeof(initmsg)) == -1) {
      perror("setsockopt 1");
      exit(1);
      }
      break;
  }

  if (p == NULL) {
    fprintf(stderr, "server: failed to bind\n");
    return;
  }

  // all done with this structure
  freeaddrinfo(servinfo); 

  if (listen( m_serverFD, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }

  // reap all dead processes
  sa.sa_handler = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }
}

//Accept new connections
int Listener::acceptConnections(sockaddr* pRemoteAddr )
{
  socklen_t addrSize;
  int clientFD, flags;

  printf("server: waiting for connections...\n");
  
  addrSize = sizeof( *pRemoteAddr);
  clientFD = accept( m_serverFD, (struct sockaddr *)pRemoteAddr, &addrSize);
  if ( clientFD == -1)
  {
    throw AcceptException();
  }

  //make the socket Non blocking
  flags = fcntl( clientFD, F_GETFL, 0);
  fcntl( clientFD, F_SETFL, flags | O_NONBLOCK);

  linger lin;
    
  lin.l_onoff = 1;
  lin.l_linger = 30;

  //avoid CLOSE_WAIT situation
  if (setsockopt( clientFD, SOL_SCTP, SO_LINGER, &lin, sizeof(linger)) == -1) {
      perror("setsockopt 1");
      exit(1);
  }

  int noDelay = 1;
  
  //Send data as soon as possible
  if (setsockopt( clientFD, SOL_SCTP, SCTP_NODELAY, &noDelay, sizeof(noDelay)) == -1) {
      perror("setsockopt 1");
      exit(1);
  }

  return clientFD;
}
