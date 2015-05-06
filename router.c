#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
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

 	int listen_fd, comm_fd, port, char_rec, sender_len, receiver1_len, receiver2_len;
 	struct sockaddr_in router, sender, receiver1, receiver2;
 	int q1_ptr, q2_ptr;
 	char buf[7];
 	char buffer[PACKET_SIZE], queue1[STORAGE_SIZE], queue2[STORAGE_SIZE];

 	if(argc != 2 || !strcmp(argv[1], "-h"))
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
		memset(&receiver1, 0, sizeof(struct sockaddr_in));
		receiver1_len = sizeof(receiver1);
	 	if ((comm_fd = recvfrom(listen_fd, buf, 8 , 0,(struct sockaddr *) &receiver1, (socklen_t *)&receiver1_len)) == -1) {
		 	printf("[router]\tError: Cannot receive receiver.\n");
		 	exit(1);
		}
	 	receiver1.sin_family = AF_INET;
		printf("\n[router]\tGot a new receiver 1!\n");

		memset(&receiver2, 0, sizeof(struct sockaddr_in));
		receiver2_len = sizeof(receiver2);
	 	if ((comm_fd = recvfrom(listen_fd, buf, 8 , 0,(struct sockaddr *) &receiver2, (socklen_t *)&receiver2_len)) == -1) {
		 	printf("[router]\tError: Cannot receive receiver.\n");
		 	exit(1);
		}
	 	receiver2.sin_family = AF_INET;
		printf("\n[router]\tGot a new receiver 2!\n");

		memset(&sender, 0, sizeof(struct sockaddr_in));
		sender_len = sizeof(sender);
	 // 	if ((comm_fd = recvfrom(listen_fd, buf, 8 , 0,(struct sockaddr *) &sender, (socklen_t *)&sender_len)) == -1) {
		//  	printf("[router]\tError: Cannot receive sender.\n");
		//  	exit(1);
		// }
	 	sender.sin_family = AF_INET;
		// printf("\n[router]\tGot a new sender!\n");
	 	q1_ptr = 0;
	 	q2_ptr = 0;
		int count_done = 0;
		bool should_run_q2 = false;

		//while ((char_rec = recvfrom(listen_fd, buffer, PACKET_SIZE, 0, (struct sockaddr *) &sender, (socklen_t *)&sender_len)) > 0) {
		while (count_done < 2 || q1_ptr != 0 || q2_ptr != 0) { // send 10 packets total
			//packet_delay = (rand() / (double)(RAND_MAX/r)) ;

			// if (packet_delay > 0) {
			bzero(buffer, PACKET_SIZE);
			usleep(10000);
			// }
			if (count_done < 2) {
				char_rec = recvfrom(listen_fd, buffer, PACKET_SIZE, 0, (struct sockaddr *) &sender, (socklen_t *)&sender_len);
				printf("received packet from sender %c on port %hu\n", buffer[0], ntohs(sender.sin_port));
				if (char_rec <= 0) {
					printf("sender %c finished sending.\n", buffer[0]);
					count_done++;
				}

				if(buffer[0] == '1') {
					memcpy(queue1+q1_ptr, &buffer, char_rec);
					q1_ptr+=char_rec;

				} else {
					memcpy(queue2+q2_ptr, &buffer, char_rec);
					q2_ptr+=char_rec;
				}
			}
			// printf("q1_ptr %i, q1: %s\n", q1_ptr, queue1);
			// printf("q2_ptr %i, q2: %s\n", q2_ptr, queue2);

			if(q1_ptr != 0) {
				puts("[router]\tSending to receiver 1.\n");
				memmove(buffer, queue1, PACKET_SIZE);
				// printf("packet is %s\n", buffer);
				memmove(queue1, queue1+PACKET_SIZE, q1_ptr);
				q1_ptr-=PACKET_SIZE;
				// printf("after shifting up q1 %i\n", q1_ptr);
				if (sendto(listen_fd, buffer, PACKET_SIZE, 0, (struct sockaddr *) &receiver1, receiver1_len) < 0) {
					printf("[router]\tError: Failed sending packet.\n");
					perror("sendto");
				}

 			} else if (q2_ptr != 0 && should_run_q2){
				puts("[router]\tSending to receiver 2.\n");
				memmove(buffer, queue2, PACKET_SIZE);
				// printf("packet is %s\n", buffer);
				memmove(queue2, queue2+PACKET_SIZE, q2_ptr);
				q2_ptr-=PACKET_SIZE;
				// printf("after shifting up q2 %i\n", q2_ptr);
				if (sendto(listen_fd, buffer, PACKET_SIZE, 0, (struct sockaddr *) &receiver2, receiver2_len) < 0) {
					printf("[router]\tError: Failed sending packet.\n");
					perror("sendto");
				}
 			}
 			should_run_q2 = (q2_ptr - q1_ptr >= 512) || (count_done>0);
 			// printf("run q2? %i", should_run_q2);
			/* delay */
			// printf("[router]\tdelay for %f sec\n", packet_delay);	
		}

		/*Send last empty packet for connectless to finish*/
		char *done = "";
		sendto(listen_fd, done, strlen(done), 0, (struct sockaddr *) &receiver1, receiver1_len);
		sendto(listen_fd, done, strlen(done), 0, (struct sockaddr *) &receiver2, receiver2_len);
		puts("[router]\tsender left, closing with receiver.");
	}
	return 0;
}
