#include "netconnect.h"
#include "netsendrecv.h"

int main()
{
  string testMsg("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
  char hostName[] = "masterserver01";
  Connection conn( hostName, 8888);
  Sender sendMsg;
  Receiver recvObj(1024);

  int fd = conn.connectServer();
  int count = 0;

  timeval tv;
  tv.tv_sec = 5;
  tv.tv_usec = 0;

  sendMsg.setSockFD( fd );
  recvObj.setSockFD( fd );

  sendMsg.setDebug( true );
  recvObj.setDebug( true );

  while (true)
  {
      sendMsg.serializeData( ( void*)testMsg.c_str(), testMsg.size());
      sendMsg.sendData(  );

      int len = recvObj.receiveData(tv);
      if ( len <= 0 )
      {
         break;
      }
      count++;

      if ( count == 10 )
	break;
  }

  conn.disconnect();
  return 0;
}
