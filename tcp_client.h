#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "project.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/signal.h>
#include <netinet/tcp.h>
//#include <arpa/inet.h>


#define check(expr) if (!(expr)) { perror(#expr); kill(0, SIGTERM); }

void enable_keepalive(int sock) 
{
  int yes = 1;
  check(setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int)) != -1);

  int idle = 1;
  check(setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int)) != -1);

  int interval = 1;
  check(setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int)) != -1);

  int maxpkt = 10;
  check(setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int)) != -1);
}


void * start_client(void * args)
{
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  int sfd, s, j;
  size_t len;
  ssize_t nread;
  char s_op_id[MAXBUF], rx[MAXBUF], request[MAXBUF], output[MAXBUF], hostname[10] = "127.0.0.1", service[9] = "delivery";
    
  /* Obtain addresss(es) matching host/port */
  memset(&hints, 0 ,sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC; /*allow ipv4 or ipv6 */
  hints.ai_socktype = SOCK_STREAM; /* Stream socket (TCP) */
  hints.ai_flags = 0;
  hints.ai_protocol = 0; /* any protocol */

  s = getaddrinfo(hostname, service, &hints, &result);
  if (s != 0)
    {
      printf("getaddrinfo: %s\n", gai_strerror(s));
      //exit(EXIT_FAILURE);
    }
  /* getaddrinfo() returns a list of address structures.
     Try each address until we successfully connect(2).
     if socket(2) (or connect(2)) fails we (close the socket
     and) try the nexst adress. */
  for (rp = result; rp != NULL; rp = rp->ai_next)
    {
      sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
      //enable_keepalive(sfd);
      if (sfd == -1) continue;
      if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) break; /* Success */
      close(sfd);
    }

  if (rp == NULL)
    {
      printf("Could not connect\n");
      return (void *) "failed";
      //exit
    }
  freeaddrinfo(result); /* No longer needed */
    
  /* create the request */
  printf("What operation am i?\n");
  scanf("%s",s_op_id);
  printf("%s\n", s_op_id);
  //snprintf(s_op_id, MAXBUF, "%d", op_id);
  //  strcpy(request, "hi there");
    
  /* send the initial "subscription" request */
  if (write(sfd, s_op_id, strlen(s_op_id)) != strlen(s_op_id))
    {
      perror("partial/failed write\n");
      close(sfd);
      pthread_exit(NULL);
      //exit
    }
   
  while (true)
    {
      /* receive data */
      memset(rx, 0 , MAXBUF);
      int bytes, received = 0;
      bytes = read(sfd, rx, MAXBUF);
      //printf("bytes read %d", bytes);
      if (bytes > 0) 
	{
	  int i = 0;
	  char header[11];
	  memcpy(header, rx, 10);
	  header[10] = '\0';
	  if (!strcmp(header, "file name ")) 
	    {
	      i = 10;
	      while (i <= bytes && rx[i] != ' ') i++;
	      char * filename;
	      filename = (char *) malloc((i-9)*sizeof(char));
	      memcpy(filename, rx+10, (i-10));
	      filename[i-10] = '\0';
	      printf("%s\n", filename);
	      i+=6;
	      while (i <= bytes && rx[i] != '\n') i++;
	      char * filesize;
	      filesize = (char *) malloc((i-6-9-strlen(filename))*sizeof(char));
	      memcpy(filesize, rx+6+10+strlen(filename), (i-6-10-strlen(filename)));
	      filesize[i-6-10-strlen(filename)] = '\0';
	      printf("%s\n", filesize);
	      i++;
	    }
	  for (j = i; j < bytes; j++) 
	    {
	      if (rx[i] == '\n')
		{
		  ;;
		}
	    }
	  received+=bytes;
	  //bytes = read(sfd,response,MAXBUF);
	}
      if (received);;
        //printf("server said %s\n", rx);
    }
  close(sfd);
  return (void *) "success";
}

#endif //TCP_CLIENT_H
