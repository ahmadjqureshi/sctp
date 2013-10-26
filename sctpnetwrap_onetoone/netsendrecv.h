#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <netinet/sctp.h>
#include <errno.h>
#include <sstream>

#ifndef NETSENDRECV_H_
#define NETSENDRECV_H_

using namespace std;

/*
  Message Format

  ------------------------------------------------
  | 32 bits size of message | Actual message     |
  ------------------------------------------------
*/

#define DATASIZE_LEN 4
#define MAXBYTES_PERCALL 100
/*
This class receives data from sctp socket
*/
class Receiver
{
 public:
  Receiver()
  {
  }

  Receiver(int dataSize) : m_fd(0), m_debug(false)
  {
    m_data = new char[dataSize];
    m_dataLen = dataSize;
  }
  
  //set fd of target fd
  void setSockFD(int fd) { m_fd = fd;  }

  //set/unset (true/false ) if you want detail data log
  void setDebug(bool debug) { m_debug = debug; }

  //This method receives data on target fd
  int receiveData(timeval tv);

  //Virtual methood to deserialize data after sending
  virtual int deSerializeData(void *data);

 protected:
  char* m_data;
  int m_dataLen;
  int m_fd;
  bool m_debug;

 private:

};


/*
This class receivessends data on sctp socket
*/
class Sender
{
 public:
 Sender(): m_data(NULL),  m_pData(NULL), m_dataLen(0), m_debug(false)
  {
    
  }

  //set fd of target fd
  void setSockFD(int fd) { m_fd = fd;  }

  //set/unset (true/false ) if you want detail data log
  void setDebug(bool debug) { m_debug = debug; }

  //Virtual methood to serialize data before sending
  virtual int serializeData(void *pData, unsigned int length);

  //This method sends data over network
  int sendData(  );

 protected:
  char* m_data;
  char *m_pData;
  int m_dataLen;
  int m_fd;
  bool m_debug;

};

#endif
