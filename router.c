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
#define STORAGE_SIZE 8192
#define PACKET_SIZE 128

 int main(int argc, char** argv)
 {
 	//set up random number generator
 	time_t t;
 	srand((unsigned) time(&t));

 	int listen_fd, comm_fd, sender_len, receiver_len, port, char_rec;
 	struct sockaddr_in router, sender, receiver;
 	char buf[7];
 	char buffer[STORAGE_SIZE];

 	if(argc != 3 || !strcmp(argv[1], "-h"))
 	{
		/* incorrect number of arguments given or help flag given.
		 * Print usage */
		 printf(" Usage:\n\n"
		 	"\t%s <port>\n\n"
		 	" This router will do as the hw instruction.\n\n",argv[0]);
		return 1; /* failure */
	}

	/* Parse args */
	port = atoi(argv[1]);
	//r = atoi(argv[2]);

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

	/*Prepare router socket*/
	router.sin_family = AF_INET;
	router.sin_addr.s_addr = htons(INADDR_ANY);
	router.sin_port = htons(port);

	if (bind(listen_fd, (struct sockaddr*) &router, sizeof(router))== -1) {
	 	printf("[router]\tError: Socket binding failed.\n");
	 	close(listen_fd);
	 	exit(1);
	 }


	printf("[router]\tWaiting for senders at port <%d>.\n", port);


	while(1)
	{
		memset(&receiver, 0, sizeof(struct sockaddr_in));
		receiver_len = sizeof(receiver);
	 	if ((comm_fd = recvfrom(listen_fd, buf, 8 , 0,(struct sockaddr *) &receiver, (socklen_t *)&receiver_len)) == -1) {
		 	printf("[router]\tError: Cannot receive receiver.\n");
		 	exit(1);
		}
	 	receiver.sin_family = AF_INET;
		printf("\n[router]\tGot a new receiver!\n");


		memset(&sender, 0, sizeof(struct sockaddr_in));
		sender_len = sizeof(sender);
	 	if ((comm_fd = recvfrom(listen_fd, buf, 8 , 0,(struct sockaddr *) &sender, (socklen_t *)&sender_len)) == -1) {
		 	printf("[router]\tError: Cannot receive sender.\n");
		 	exit(1);
		}
	 	sender.sin_family = AF_INET;
		printf("\n[router]\tGot a new sender!\n");


    	int char_rec;

		while ((char_rec = recvfrom(listen_fd, buffer, PACKET_SIZE, 0, (struct sockaddr *) &sender, (socklen_t *)&sender_len)) > 0) {
		//while (1) { // send 10 packets total
			//packet_delay = (rand() / (double)(RAND_MAX/r)) ;

			// if (packet_delay > 0) {
			// 	usleep((int)(packet_delay * 1000000));
			// }

			if (sendto(listen_fd, buffer, PACKET_SIZE, 0, (struct sockaddr *) &receiver, receiver_len) < 0) {
				printf("[router]\tError: Failed sending packet.\n");
				perror("sendto");
			}
 
			/* delay */
			// printf("[router]\tdelay for %f sec\n", packet_delay);	
			
		}

		/*Send last empty packet for connectless to finish*/
		char *done = "";
		sendto(listen_fd, done, strlen(done), 0, (struct sockaddr *) &receiver, receiver_len);	

		puts("[router]\tsender left, closing with receiver.");
	}
	return 0;
}
