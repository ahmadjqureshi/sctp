#include <exception>

using namespace std;

#ifndef NETEXCEPTION_H_
#define NETEXCEPTION_H_

class ListenerException : public exception
{
 public:
  ListenerException()
  {
    
  }

  const char* what() const throw()
  {
    return "Listener Exception.";
  }
};

class AcceptException : public exception
{
 public:
  AcceptException()
  {
  }

  const char* what() const throw()
  {
    
    return "Unable to accept connection.";
  }
};

class PortNotAvailable : ListenerException
{
 public:
  PortNotAvailable()
  {
  }

  const char* what() const throw()
  {
    return "Port not available.";
  }
};

#endif
