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
    int sd, buflen, r;
    char *sender_id;
    double packet_delay;
    unsigned short port;
    char *router_address;
    char all_packets[10][6] = {"00000","11111","22222","33333","44444","55555","66666","77777","88888","99999"};
    struct sockaddr_in router;
    char packet[PACKET_SIZE];
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

    //setup for delay timing
    int first_pkt = 1;
    int count =0;
    int n =0;

    //Talk to router to begin. 

    // if (sendto(sd, temp, strlen(temp) , 0, (struct sockaddr *) &router, sizeof(router)) == -1) {
    //     fprintf(stderr, "[sender]\tError: Couldn't send to the router.");
    //     close(sd);
    //     exit(1);
    // }

	printf("\n[sender]\tConnected to router!\n");

    for (int i=0; i<10; i++) {
	//while (1) { // send 10 packets total
		packet_delay = (rand() / (double)(RAND_MAX/r)) ;

		if (packet_delay > 0) {
			usleep((int)(packet_delay * 1000000));
		}

        strcpy(packet, sender_id);
        strcat(packet, all_packets[i]);

		if (sendto(sd, packet, PACKET_SIZE, 0, (struct sockaddr *) &router, sizeof(router)) < 0) {
			printf("[sender]\tError: Failed sending packet.\n");
			perror("sendto");
		}

		/* delay */
		printf("[sender]\tdelay for %f sec\n", packet_delay);	
		
	}

	/*Send last empty packet for connectless to finish*/
	char *done = "";
	sendto(sd, done, strlen(done), 0, (struct sockaddr *) &router, sizeof(router));	

	puts("[sender]\tFinish sending packets.");
	return 0;
}
