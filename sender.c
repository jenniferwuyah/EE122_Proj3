#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#define PACKET_SIZE 128
int main(int argc, char** argv)
{
    int sd, buflen, r, packet_no;
    char *sender_id;
    double packet_delay;
    unsigned short port;
    char *router_address;
    char junk[PACKET_SIZE-6];
    struct sockaddr_in router;
    char packet[PACKET_SIZE];
    // char pno_str[4];
    // char time_str[100];
    // struct timeval start, end, conn_start, conn_end;
    // float sec_delay;
    // int bFlag = 0;

    char *temp = "Initial";

    if((argc != 5) || !strcmp(argv[1], "-h")) {
        /* incorrect number of arguments given or help flag given.
         * Print usage */
        printf("%i\n",argc);
        printf(" Usage:\n\n"
               "\t%s <router_address>\n\n"
               "\t <port>\n\n"
               "\t <R>\n\n"
               "\t <sender_id>\n\n"
               " This client will do as the hw instruction.\n\n",argv[0]);
       return 1; /* failure */
    }

    /* Parse args */
    port = atoi(argv[2]);
    r = atoi(argv[3]);
    router_address = argv[1];
    sender_id = argv[4];
    
    if(port < 1024) {
        fprintf(stderr, "[sender]\tError: Invalid port number <%d>.\n", port);
        fprintf(stderr, "\t\t(Only accepts ports over 1000)\n");
        return 1; /* failure */
    }

    //create socket
    if ( (sd = socket(AF_INET, SOCK_DGRAM,0)) == -1 ) {
        fprintf(stderr, "[sender]\tError: Can't create a socket.\n");
        exit(1); 
    }

    inet_pton(AF_INET, router_address, &(router.sin_addr));
    router.sin_port = htons(port);
    router.sin_family = AF_INET;

    /* Connecting to the router */

    //Talk to router to begin. 

    // if (sendto(sd, temp, strlen(temp) , 0, (struct sockaddr *) &router, sizeof(router)) == -1) {
    //     fprintf(stderr, "[sender]\tError: Couldn't send to the router.");
    //     close(sd);
    //     exit(1);
    // }

    memset(junk, '5', PACKET_SIZE-6);

	printf("\n[sender]\tConnected to router!\n");
    packet_no = 1;
    for (int i=0; i<10; i++) {
	//while (packet_no <= 9999) { // max of 9999 packets can be sent
		packet_delay = (rand() / (double)(RAND_MAX/10))+(r-5);

		if (packet_delay > 0) {
			usleep((int)(packet_delay * 100000));
		}

        sprintf(packet, "%s%04d%s", sender_id, packet_no, junk);

		if (sendto(sd, packet, PACKET_SIZE, 0, (struct sockaddr *) &router, sizeof(router)) < 0) {
			printf("[sender]\tError: Failed sending packet.\n");
			perror("sendto");
		}
        packet_no++;
		/* delay */
		printf("[sender]\tdelay for %f ms\n", packet_delay);	
		
	}

	/*Send last empty packet for connectless to finish*/
	char *done = "";
	sendto(sd, done, strlen(done), 0, (struct sockaddr *) &router, sizeof(router));	

	puts("[sender]\tFinish sending packets.");
	return 0;
}
