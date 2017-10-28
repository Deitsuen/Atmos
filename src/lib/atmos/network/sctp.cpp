//SCTPServer.C To compile - gcc sctpserver.c - o server - lsctp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#define MAX_BUFFER 1024
#define MY_PORT_NUM 62324 /* This can be changed to suit the need and should be same in server and client */
 
int sctp_server ()
{
          int listenSock, connSock, ret, in, flags, i;
          struct sockaddr_in servaddr;
          struct sctp_initmsg initmsg;
          struct sctp_event_subscribe events;
          struct sctp_sndrcvinfo sndrcvinfo;
          char buffer[MAX_BUFFER + 1];
         
                  listenSock = socket (AF_INET, SOCK_STREAM, IPPROTO_SCTP);
          if(listenSock == -1)
          {
                      printf("Failed to create socket\n");
                      perror("socket()");
                      exit(1);
                  
        }
         
                  bzero ((void *) &servaddr, sizeof (servaddr));
          servaddr.sin_family = AF_INET;
          servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
          servaddr.sin_port = htons (MY_PORT_NUM);
         
                  ret = bind (listenSock, (struct sockaddr *) &servaddr, sizeof (servaddr));
         
          if(ret == -1 )
           {
                      printf("Bind failed \n");
                      perror("bind()");
                      close(listenSock);
                      exit(1);
                  
        }
         
          /* Specify that a maximum of 5 streams will be available per socket */
                  memset (&initmsg, 0, sizeof (initmsg));
          initmsg.sinit_num_ostreams = 5;
          initmsg.sinit_max_instreams = 5;
          initmsg.sinit_max_attempts = 4;
          ret = setsockopt (listenSock, IPPROTO_SCTP, SCTP_INITMSG,
                                  &initmsg, sizeof (initmsg));
         
          if(ret == -1 )
           {
                      printf("setsockopt() failed \n");
                      perror("setsockopt()");
                      close(listenSock);
                      exit(1);
                  
        }
         
                  ret = listen (listenSock, 5);
          if(ret == -1 )
          {
                      printf("listen() failed \n");
                      perror("listen()");
                      close(listenSock);
                      exit(1);
                  
        }
         
          while (1)
             {
                 
                      char buffer[MAX_BUFFER + 1];
                      int len;
                 
                      //Clear the buffer
                              bzero (buffer, MAX_BUFFER + 1);
                 
                              printf ("Awaiting a new connection\n");
                 
                              connSock = accept (listenSock, (struct sockaddr *) NULL, (int *) NULL);
                      if (connSock == -1)
                      {
                         printf("accept() failed\n");
                                perror("accept()");
                                close(connSock);
                                continue;
                              
                }
                      else
                         printf ("New client connected....\n");
                 
                              in = sctp_recvmsg (connSock, buffer, sizeof (buffer),
                                                     (struct sockaddr *) NULL, 0, &sndrcvinfo, &flags);
                 
                      if( in == -1)
                       {
                                  printf("Error in sctp_recvmsg\n");
                                  perror("sctp_recvmsg()");
                                  close(connSock);
                                  continue;
                              
                }
                      else
                      {
                                  //Add '\0' in case of text data
                                          buffer[in] = '\0';
                         
                                          printf (" Length of Data received: %d\n", in);
                                  printf (" Data : %s\n", (char *) buffer);
                              
                }
                      close (connSock);
                    
        }
         
                  return 0;
}


int sctp_client (int argc, char* argv[])
{
          int connSock, in, i, ret, flags;
          struct sockaddr_in servaddr;
          struct sctp_status status;
          char buffer[MAX_BUFFER + 1];
          int datalen = 0;
         
          /*Get the input from user*/
                  printf("Enter data to send: ");
          fgets(buffer, MAX_BUFFER, stdin);
          /* Clear the newline or carriage return from the end*/
                  buffer[strcspn(buffer, "\r\n")] = 0;
          /* Sample input */
          //strncpy (buffer, "Hello Server", 12);
          //buffer[12] = '\0';
                  datalen = strlen(buffer);
         
                  connSock = socket (AF_INET, SOCK_STREAM, IPPROTO_SCTP);
         
          if (connSock == -1)
           {
                      printf("Socket creation failed\n");
                      perror("socket()");
                      exit(1);
                  
        }
         
                  bzero ((void *) &servaddr, sizeof (servaddr));
          servaddr.sin_family = AF_INET;
          servaddr.sin_port = htons (MY_PORT_NUM);
          servaddr.sin_addr.s_addr = inet_addr ("127.0.0.1");
         
                  ret = connect (connSock, (struct sockaddr *) &servaddr, sizeof (servaddr));
         
          if (ret == -1)
           {
                      printf("Connection failed\n");
                      perror("connect()");
                      close(connSock);
                      exit(1);
                  
        }
         
                  ret = sctp_sendmsg (connSock, (void *) buffer, (size_t) datalen,
                                              NULL, 0, 0, 0, 0, 0, 0);
          if(ret == -1 )
          {
                    printf("Error in sctp_sendmsg\n");
                    perror("sctp_sendmsg()");
                  
        }
          else
                      printf("Successfully sent %d bytes data to server\n", ret);
         
                  close (connSock);
         
                  return 0;
}
