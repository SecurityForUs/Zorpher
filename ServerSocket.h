// Definition of the ServerSocket class

#ifndef ServerSocket_class
#define ServerSocket_class

#include "Socket.h"


class ServerSocket : private Socket
{
 public:

  ServerSocket ( std::string host, int port );
  ServerSocket (){};
  virtual ~ServerSocket();

  const ServerSocket& operator << ( const std::string& ) const;
  const ServerSocket& operator >> ( std::string& ) const;

  void Send(const std::string &data){ Socket::send(data); }
  void Recv(std::string &data){ Socket::recv(data); }

  void accept ( ServerSocket& );

	char *GetClient(){ return Socket::clienthost; }

  bool is_valid(){ return Socket::is_valid(); }
  int id(){ return Socket::GetID(); }
};


#endif
