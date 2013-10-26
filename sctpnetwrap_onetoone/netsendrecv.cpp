#include "netsendrecv.h"

int Receiver::receiveData(timeval tv)
{
  ofstream logFile;  //log file
  int recvBytes = 0, flags;

  logFile.open( "receive.log", std::ofstream::app);

  unsigned int sizeOfData, nSizeOfData;
  fd_set fdset;
  fd_set dfsetExp;

  FD_ZERO(&fdset);
  FD_SET( m_fd, &fdset);

  FD_ZERO(&dfsetExp);
  FD_SET( m_fd, &dfsetExp);

  int bytesGot = 0;

  while(true)
  {
    //wait until socket has any data
    if ( select ( m_fd+1, &fdset, 0, &dfsetExp, &tv) <= 0)
    {
      if ( errno == EAGAIN || errno== EWOULDBLOCK)
	continue;

      break;
    }
    
    //get size of data received
    bytesGot = recv( m_fd, &nSizeOfData, sizeof(unsigned int), 0);

    if (bytesGot <= 0)
    {
        if ( errno == EAGAIN || errno== EWOULDBLOCK)
	  continue;

	logFile<<getpid()<<" Error occured: "<<bytesGot<< " errno: "<<errno << "  " <<strerror(errno)<<endl;
	logFile.close();
	return -1;
    }
    break;
  }

  sizeOfData = ntohl( nSizeOfData ) ;

  logFile<<endl<<"Size of Data : "<<sizeOfData<<endl;

  if ( m_data != NULL)
  {
    delete[] m_data;
  }

  m_data = new char[sizeOfData ];

  int sizeLeft = (int) sizeOfData;

  while ( sizeLeft  )
  {
    int iSize = MAXBYTES_PERCALL;
    
    if ( ( recvBytes + MAXBYTES_PERCALL ) > sizeOfData)
    {
      iSize = sizeOfData - recvBytes;
    }

    //Wait until socket has got any datax
    if ( select ( m_fd+1, &fdset, 0, &dfsetExp, &tv) <= 0)
    {
      if (  errno== EWOULDBLOCK || errno == EINTR)
	continue;

      break;
    }

    bytesGot = recv( m_fd, &m_data[recvBytes], iSize, 0);

    if (bytesGot <= 0)
    {
      if ( errno== EWOULDBLOCK  || errno == EINTR)
      {
	bytesGot = 0;
	continue;

      }

      logFile<<getpid()<<"Error occured: "<<bytesGot<< " errno: "<<errno << "  "  <<strerror(errno);
      logFile.close();
      return -1;
    }

    if (m_debug )
    {

      logFile<<endl << "Message"<< endl<<"0  1  2  3  4  5  6  7"<<endl;

      int j = 0;
      char hexStr[3];
      for (int i = recvBytes ; i < (recvBytes  + bytesGot) ; i++ )
      {
	stringstream ss;
	ss<< (char)m_data[i];
	sprintf( hexStr, "%x", ss.str().c_str()[0]);

	logFile<< " "  <<hexStr;

	if ( (( j + 1 ) %8) == 0 )
	{
	  logFile<<endl;
	}
	j++;
      }
    }

    recvBytes = recvBytes  + bytesGot;
    sizeLeft =  sizeLeft - bytesGot;    

    if (sizeLeft < 0)
	sizeLeft = 0;
  }
  logFile.close();

  return recvBytes;
}

int Receiver::deSerializeData(void *data)
{
  return 0;
}

int Sender::sendData(  )
{
  ofstream logFile;
  int totalSize = m_dataLen + DATASIZE_LEN ;
  int sentBytes = 0;

  int totalSizeTmp = totalSize; 

  timeval tv;
  tv.tv_sec = 5;
  tv.tv_usec = 0;

  
  logFile.open( "sender.log", std::ofstream::app);

  if (m_data != NULL)
    delete[] m_data;

  m_data = new char[ m_dataLen + DATASIZE_LEN  ];

  //convert data size to network byte order
  unsigned int nLength = htonl( m_dataLen );

  fd_set fdset;
  fd_set dfsetExp;

  FD_ZERO(&fdset);
  FD_SET( m_fd, &fdset);

  FD_ZERO(&dfsetExp);
  FD_SET( m_fd, &dfsetExp);

  //Write data size in send buffer
  memcpy( m_data, &nLength, (size_t)DATASIZE_LEN );

  //Write actual data in send buffer
  memcpy( &m_data[DATASIZE_LEN], m_pData, (size_t)m_dataLen);

  while ( totalSizeTmp )
  {
    int iSize = MAXBYTES_PERCALL;
    
    if ( ( sentBytes + MAXBYTES_PERCALL ) > totalSize)
    {
      iSize = totalSize - sentBytes ;
    }

    //Wait until socket is available for writing
    if ( select ( m_fd+1, 0, &fdset, &dfsetExp, &tv) <= 0)
    {
      if ( errno == EAGAIN || errno== EWOULDBLOCK || errno == EINTR)
	continue;

      break;
    }

    ssize_t bytesSent = send( m_fd, &m_data[sentBytes], iSize, 0);
    
    if (bytesSent <= 0)
    {
      logFile<<"Error occured: "<<bytesSent;
      logFile.close();
      return -1;
    }

    logFile<<endl<< "Bytes Sent: "<< bytesSent<<endl;

    if (m_debug )
    {
      int j = 0;
      char hexStr[3];

      logFile<<endl << "Message"<< endl<<"0  1  2  3  4  5  6  7"<<endl;

      for (int i = sentBytes ; i < ( bytesSent + iSize) ; i++ )
      {
	stringstream ss;
	ss<< (char)m_data[i];
	sprintf( hexStr, "%x", ss.str().c_str()[0]);

	logFile<< " "  <<hexStr;

	if ( (( j + 1) %8) == 0 )
	{
	  logFile<<endl;
	}
	j++;
      }
    }

    sentBytes = sentBytes + iSize;

    totalSizeTmp = totalSizeTmp - (int) bytesSent;

    if (totalSizeTmp < 0)
      totalSizeTmp = 0;

  }
  logFile.close();

  return sentBytes;
}

/* simple implementation of serialize data, 
   since it is virtual method it can be implemented in derived class.*/
int Sender::serializeData(void *pData, unsigned int length)
{

  if (m_pData != NULL )
    delete[] m_pData;

  m_pData = new char[length];

  memcpy( m_pData, pData, length);

  m_dataLen = length;

  return 0;
}
