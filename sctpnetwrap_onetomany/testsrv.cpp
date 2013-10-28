#include "listener.h"
#include "netsendrecv.h"

int main()
{
  Listener *listenerAtPort = new  Listener();
  listenerAtPort->listenOnPort();

  Receiver recvObj(1024);
  Sender sendObj;
  timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 0;

  sendObj.setSockFD( listenerAtPort->m_serverFD  );

  recvObj.setSockFD( listenerAtPort->m_serverFD );

  sendObj.setDebug( true );
  recvObj.setDebug( true );

  while (true)
  {
   int len = recvObj.receiveData(tv);
   if ( len <= 0 )
   {
     break;
   }

   sockaddr tmpaddr = recvObj.getRemoteAddr();
   sendObj.setRemoteAddr( &tmpaddr);

   sendObj.serializeData( (void*)&len, sizeof (len) );
   sendObj.sendData( );
  }

  return 0;
}

