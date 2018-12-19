#include "clientsender.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory.h>
#include <stdio.h>
#include <iostream>

using namespace  std;

    ClientSender::ClientSender()
    {

    }

    void ClientSender::Client_connect()
    {
          s = socket( AF_INET, SOCK_STREAM, 0 );
          if(s < 0)
          {
             perror( "Error calling socket" );
          }
          cout<<"triyng to connect"<<endl;

          struct sockaddr_in peer;
          peer.sin_family = AF_INET;
          peer.sin_port = htons( 6666 );
          peer.sin_addr.s_addr = inet_addr( "127.0.0.1" );
          int result = connect( s, ( struct sockaddr * )&peer, sizeof( peer ) );
          if( result )
          {
              perror( "Error calling connect" );
              cout<<"Error calling connect"<<endl;
          }
    }

    void ClientSender::Client_send(string buf)
    {

        int result = send( s, (char *)buf.c_str(), strlen(buf.c_str()), 0);
        cout<<"sending message"<<endl;
        if( result <= 0 )
        {
            perror( "Error calling send" );
        }
    }
