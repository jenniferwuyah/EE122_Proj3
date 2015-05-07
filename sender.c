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
#define TIMEOUT_SEC 5
int main(int argc, char** argv)
{
    int sd, buflen, r, packet_no;
    char *sender_id;
    double packet_delay;
    unsigned short port, sender_port;
    char *router_address;
    char junk[PACKET_SIZE-6];
    struct sockaddr_in router, sender;
    char packet[PACKET_SIZE];
    struct timeval start, end;

    if((argc != 6) || !strcmp(argv[1], "-h")) {
        /* incorrect number of arguments given or help flag given.
         * Print usage */
        printf("%i\n",argc);
        printf(" Usage:\n\n"
               "\t%s <router_address>\n\n"
               "\t <router_port>\n\n"
               "\t <sender_port>\n\n"
               "\t <R>\n\n"
               "\t <sender_id>: 1 or 2\n\n"
               " This client will do as the hw instruction.\n\n",argv[0]);
       return 1; /* failure */
    }

    /* Parse args */
    port = atoi(argv[2]);
    sender_port = atoi(argv[3]);
    r = atoi(argv[4]);
    router_address = argv[1];
    sender_id = argv[5];
    
    if (sender_id[0]== '1') {
        r = r*2;
    }

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

    struct timeval timeout;      
    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = 0;

    if (setsockopt (sd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
        puts("setsockopt failed\n");

    sender.sin_family = AF_INET;
    sender.sin_addr.s_addr = htons(INADDR_ANY);
    sender.sin_port = htons(sender_port);

    inet_pton(AF_INET, router_address, &(router.sin_addr));
    router.sin_port = htons(port);
    router.sin_family = AF_INET;

    if (bind(sd, (struct sockaddr*) &sender, sizeof(sender))== -1) {
        printf("[router]\tError: Socket binding failed.\n");
        close(sd);
        exit(1);
     }

    /* Connecting to the router */

    //Talk to router to begin. 
    char buf[10];
    buflen = 10;
    int char_rec;


    memset(junk, '5', PACKET_SIZE-6);

	printf("\n[sender]\tConnected to router!\n");
    packet_no = 1;
    //for (int i=0; i<10; i++) {
	while (packet_no <= 500) { // max of 9999 packets can be sent
		packet_delay = (rand() / (double)(RAND_MAX/10))+(r-5);

		if (packet_delay > 0) {
			usleep((int)(packet_delay * 1000));
		}

        sprintf(packet, "%s%04d%s", sender_id, packet_no, junk);
        puts("[sender]\tSending to Router.\n");
        gettimeofday(&start, NULL);
		if (sendto(sd, packet, PACKET_SIZE, 0, (struct sockaddr *) &router, sizeof(router)) < 0) {
			printf("[sender]\tError: Failed sending packet.\n");
			perror("sendto");
		}
        if (sender_id[0] == '2') {
            while ((char_rec = recvfrom(sd, buf, buflen, 0, NULL, NULL)) <= 0) {
                if (sendto(sd, packet, PACKET_SIZE, 0, (struct sockaddr *) &router, sizeof(router)) < 0) {
                    printf("[sender]\tError: Failed sending packet.\n");
                    perror("sendto");
                }
            }
            gettimeofday(&end, NULL);
            printf("%s\n", buf);
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
