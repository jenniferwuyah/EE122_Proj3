#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

int main(int argc, char** argv)
{
    int sd, buflen;
    unsigned short port;
    char *router_address;
    struct sockaddr_in router;
    // char time_str[100];
    // struct timeval start, end, conn_start, conn_end;
    // float sec_delay;
    // int bFlag = 0;

    char *temp = "Initial";

    if((argc != 3) || !strcmp(argv[1], "-h")) {
        /* incorrect number of arguments given or help flag given.
         * Print usage */
        printf("%i\n",argc);
        printf(" Usage:\n\n"
               "\t%s <router_address>\n\n"
               "\t <port>\n\n"
               " This client will do as the hw instruction.\n\n",argv[0]);
       return 1; /* failure */
    }

    /* Parse args */
    port = atoi(argv[2]);
    router_address = argv[1];
    
    if(port < 1024) {
        fprintf(stderr, "[receiver]\tError: Invalid port number <%d>.\n", port);
        fprintf(stderr, "\t\t(Only accepts ports over 1000)\n");
        return 1; /* failure */
    }

    //create socket
    if ( (sd = socket(AF_INET, SOCK_DGRAM,0)) == -1 ) {
        fprintf(stderr, "[receiver]\tError: Can't create a socket.\n");
        exit(1); 
    }

    inet_pton(AF_INET, router_address, &(router.sin_addr));
    router.sin_port = htons(port);
    router.sin_family = AF_INET;

    /* Connecting to the router */

    //setup for delay timing
    int first_pkt = 1;
    int count =0;
    int n =0;

    //Talk to router to begin. 

    if (sendto(sd, temp, strlen(temp) , 0, (struct sockaddr *) &router, sizeof(router)) == -1) {
        fprintf(stderr, "[receiver]\tError: Couldn't send to the router.");
        close(sd);
        exit(1);
    }
    printf("\n[receiver]\tConnected to router!\n");

    char buf[4096];
    buflen = 4096;
    int char_rec;

    // if (bFlag == 0) {
    while ((char_rec = recvfrom(sd, buf, buflen, 0, NULL, NULL)) > 0) {
        printf("[receiver]\t Received: %s\n", buf);
		//count+=char_rec;
	  
		bzero(buf, buflen);
    } 

    close(sd);
    return 0;
}
