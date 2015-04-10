#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

/* this defines the size of our buffer */
#define PACKET_SIZE 128

 int main(int argc, char** argv)
 {
 	//set up random number generator
 	time_t t;
 	srand((unsigned) time(&t));
    char *server_address;
 	int listen_fd, comm_fd, client_len, port, sd;
 	double packet_delay;
 	struct sockaddr_in router, client, server;
 	char buf[7];
 	char buffer[PACKET_SIZE];
 	char storage[64][PACKET_SIZE];
 	char *temp = "Initial";

 	if(argc != 3 || !strcmp(argv[1], "-h"))
 	{
		/* incorrect number of arguments given or help flag given.
		 * Print usage */
		 printf(" Usage:\n\n"
		 	"\t%s <router_address>\n\n"
		 	"\t <port>\n\n"
		 	" This router will do as the hw instruction.\n\n",argv[0]);
		return 1; /* failure */
	}

	/* Parse args */
	port = atoi(argv[2]);
	server_address = argv[1];

	if(port < 1024)
	{
		fprintf(stderr, "[router]\tError: Invalid port number <%d>.\n", port);
		return 1; /* failure */
	}

	/*router set up*/
	//connectionless sockets

	if ((listen_fd = socket(AF_INET, SOCK_DGRAM,0)) == -1) {
	 	printf("[router]\tError: Couldn't make a socket.\n");
	 	exit(1);
	}


    inet_pton(AF_INET, server_address, &(server.sin_addr));
    server.sin_port = htons(port);
    server.sin_family = AF_INET;

	/*Prepare router socket*/
	router.sin_family = AF_INET;
	router.sin_addr.s_addr = htons(INADDR_ANY);
	router.sin_port = htons(port);

	if (bind(listen_fd, (struct sockaddr*) &router, sizeof(router))== -1) {
	 	printf("[router]\tError: Socket binding failed.\n");
	 	close(listen_fd);
	 	exit(1);
	 }


	printf("[router]\tWaiting for clients at port <%d>.\n", port);

    int first_pkt = 1;
    int count =0;
    int n =0;

    //Talk to server to begin. 

    if (sendto(sd, temp, strlen(temp) , 0, (struct sockaddr *) &server, sizeof(server)) == -1) {
        fprintf(stderr, "[router]\tError: Couldn't send to the server.");
        close(sd);
        exit(1);
    }

	while(1)
	{
		memset(&client, 0, sizeof(struct sockaddr_in));
		client_len = sizeof(client);
	 	if ((comm_fd = recvfrom(listen_fd, buf, 8 , 0,(struct sockaddr *) &client, (socklen_t *)&client_len)) == -1) {
		 	printf("[router]\tError: Cannot receive client.\n");
		 	exit(1);
		}
	 	client.sin_family = AF_INET;


		printf("\n[router]\tGot a new client!\n");

		while (1) { // send 10 packets total
	
			usleep(10000000); //10 ms delay between processing packets

			if (sendto(listen_fd, buffer, PACKET_SIZE, 0, (struct sockaddr *) &client, client_len) < 0) {
				printf("[router]\tError: Failed sending packet.\n");
				perror("sendto");
			}
 	
			
		}

		/*Send last empty packet for connectless to finish*/
		char *done = "";
		sendto(listen_fd, done, strlen(done), 0, (struct sockaddr *) &client, client_len);	

		puts("[router]\tClient left.");
	}
	close(sd);
	return 0;
}
