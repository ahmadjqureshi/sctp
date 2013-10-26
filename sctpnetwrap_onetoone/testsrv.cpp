#include "listener.h"
#include "netsendrecv.h"

int main()
{
  sockaddr remoteAddr;
  Listener *listenerAtPort = new  Listener();
  listenerAtPort->listenOnPort();
  int childFD;
  pid_t pid;

  while (true)
  {
    childFD = listenerAtPort->acceptConnections( &remoteAddr );

    pid = fork();

    if ( pid == 0 )
    {
      break;
    }

    close(childFD);
  }

  close( listenerAtPort->m_serverFD);

  Receiver recvObj(1024);
  Sender sendObj;
  timeval tv;
  tv.tv_sec = 5;
  tv.tv_usec = 0;

  sendObj.setSockFD( childFD );

  recvObj.setSockFD( childFD );

  sendObj.setDebug( true );
  recvObj.setDebug( true );

  while (true)
  {
   int len = recvObj.receiveData(tv);
   if ( len <= 0 )
   {
     break;
   }

   //sleep(1);

   sendObj.serializeData( (void*)&len, sizeof (len) );
   sendObj.sendData( );
  }

  return 0;
}

