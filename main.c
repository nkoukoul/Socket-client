#include "tcp_client.h"
#include "project.h"


int main()
{
  pthread_t client_thread;
  int s, w;
  void * res;
  s = pthread_create(&client_thread, NULL, start_client, NULL);
  if (s != 0)
    {
      perror("client thread creation failed.. exiting\n");
      exit(EXIT_FAILURE);
    }
  /*  while(true)
    {
      ;;
      }*/
  s = pthread_join(client_thread, &res);
  if (s != 0)
    {
      perror("client thread join failed.. exiting\n");
      exit(EXIT_FAILURE);
    }
  return 0;
}
