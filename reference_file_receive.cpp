#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <signal.h>
#include <ctype.h>          
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#define PORT 9516
#define BACKLOG 5
#define LENGTH 1024*1024

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main ()
{
	/* Defining Variables */
	int sockfd; 
	int nsockfd; 
	int num;
	socklen_t sin_size; 
	struct sockaddr_in addr_local; /* client addr */
	struct sockaddr_in addr_remote; /* server addr */
	char revbuf[LENGTH]; // Receiver buffer

	/* Get the Socket file descriptor */
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
	{
		error("ERROR: Failed to obtain Socket Descriptor.\n");
	}
	else 
		printf("[Server] Obtaining socket descriptor successfully.\n");

	/* Fill the client socket address struct */
	addr_local.sin_family = AF_INET; // Protocol Family
	addr_local.sin_port = htons(PORT); // Port number
	addr_local.sin_addr.s_addr = INADDR_ANY; // AutoFill local address
	bzero(&(addr_local.sin_zero), 8); // Flush the rest of struct

	/* Bind a special Port */
	if( bind(sockfd, (struct sockaddr*)&addr_local, sizeof(struct sockaddr)) == -1 )
	{
		error("ERROR: Failed to bind Port.\n");
	}
	else 
		printf("[Server] Binded tcp port %d in addr 127.0.0.1 sucessfully.\n",PORT);

	/* Listen remote connect/calling */
	if(listen(sockfd,BACKLOG) == -1)
	{
		error("ERROR: Failed to listen Port.\n");
	}
	else
		printf ("[Server] Listening the port %d successfully.\n", PORT);

	for(;;)
	{
		sin_size = sizeof(struct sockaddr_in);

		/* Wait a connection, and obtain a new socket file despriptor for single connection */
		if ((nsockfd = accept(sockfd, (struct sockaddr *)&addr_remote, &sin_size)) == -1) 
		{
			error("ERROR: Obtaining new Socket Despcritor.\n");
		}
		else 
		printf("[Server] Master has connected %s.\n", inet_ntoa(addr_remote.sin_addr));

		/*Receive File from Client */
		const char* fr_name = "/home/ubuntu/JP/hackpes/spark.tgz";
		// FILE *fr = fopen(fr_name, "a");
		int fd = open(fr_name, 'r');
		if(fd < 0)
			printf("File %s Cannot be opened file on server.\n", fr_name);
		else
		{
			memset(revbuf,0, LENGTH); 
			int fr_block_sz = 0;
			while((fr_block_sz = read(nsockfd, revbuf, LENGTH))!=0) //could it be sockfd?
			{
				printf("%d bytes received\n",fr_block_sz);
				if(fr_block_sz < 0)
				{
					error("Error receiving file from client to server.\n");
				}
				// int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fr);
				int write_sz = write(fd, revbuf, fr_block_sz);
				if(write_sz < fr_block_sz)
				{
					error("File write failed on server.\n");
					break;
				}
				// else if(fr_block_sz)
				// {
				// 	break;
				// }
				memset(revbuf,0, LENGTH);
			}
			printf("Ok received from client!\n");
			// fclose(fr);
			close(fd);
			//}
		}

	}
}