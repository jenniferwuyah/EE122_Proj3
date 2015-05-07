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
    int sd, buflen, delay;
    double random_delay;
    unsigned short port, sender_port;
    char *router_address, *sender_address;
    struct sockaddr_in router, sender;

    char *temp = "Initial";
    char ack[10];

    if((argc != 5 && argc != 6) || !strcmp(argv[1], "-h")) {
        /* incorrect number of arguments given or help flag given.
         * Print usage */
        printf("%i\n",argc);
        printf(" Usage:\n\n"
               "\t%s <router_address>\n\n"
               "\t <router_port>\n\n"
               "\t <sender_address>\n\n"
               "\t <sender_port>\n\n"
               "\t [-d]: optional flag for random delay\n\n"
               " This client will do as the hw instruction.\n\n",argv[0]);
       return 1; /* failure */
    }

    /* Parse args */
    port = atoi(argv[2]);
    router_address = argv[1];
    sender_port = atoi(argv[4]);
    sender_address = argv[3];

    if (argc == 6) {
        delay = 5;
    } else {
        delay = -1;
    }
    
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

    inet_pton(AF_INET, sender_address, &(sender.sin_addr));
    sender.sin_port = htons(sender_port);
    sender.sin_family = AF_INET;
    /* Connecting to the router */

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

    while (1) {
        if (delay!= -1) {
            random_delay = (rand() / (double)(RAND_MAX/delay));
            usleep((int)(random_delay*1000));
            delay = (delay +10) % 20; //alternate
        }

        char_rec = recvfrom(sd, buf, buflen, 0, NULL, NULL);
        sprintf(ack, "ACK%c", buf[0]);
        if (char_rec > 0) {
            printf("[receiver]\t Received: %s\n", buf);
            if (delay != -1) {
                if (sendto(sd, ack, strlen(ack), 0, (struct sockaddr *) &sender, sizeof(sender)) == -1) {
                    fprintf(stderr, "[receiver]\tError: Couldn't ACK to the sender.");
                    close(sd);
                    exit(1);
                }
            }
        } else {
            break;
        }
	  
		bzero(buf, buflen);
    } 

    close(sd);
    return 0;
}
