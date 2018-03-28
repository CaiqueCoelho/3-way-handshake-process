#include <stdio.h>          /* printf */
#include <stdlib.h>         /* exit */
#include <string.h>         /* bzero */
#include <sys/socket.h>     /* struct sockaddr, socket, listen, bind, accept, recv, send */
#include <sys/wait.h>       /* waitpid */
#include <arpa/inet.h>      /* struct sockaddr */
#include <unistd.h>         /* exit, fork */
#include <signal.h>         /* signal */
#include <time.h>           /* TIME, time_t */
#include <pthread.h>        /* pthread_t, pthread_create */
#include <sys/stat.h>       /* lstat() */
#include <sys/types.h>      /* mode_t */
#include <unistd.h>
#include <netdb.h>

/*
maximum size of a printed address -- if not defined, we define it here
*/

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN        46
#endif

/*
INET6_ADDRSTRLEN
*/

#define BUFSIZE 1000

/*
print a system error and exit the program
*/


static void error (char *s)
{
	perror (s);
	exit (1);
}

static void usage (char *program)
{
	printf ("usage: %s hostname [port]\n", program);
	exit (1);
}

static char *build_request (char *hostname, char *caminho)
{

	char header2 [] = "\r\n\r\n";
	char header1[BUFSIZE];

	if(caminho[0] == '0'){
		//char header1 [] = "GET / HTTP/1.1\r\nHost: ";
		strcpy(header1, "GET / HTTP/1.1\r\nHost: ");
		//printf("Entrou aqui.........................................................");
	}

	else{
		strcpy(header1, "GET ");
		strcat(header1, caminho);
		strcat(header1, " HTTP/1.1\r\nHost: ");
		//printf("####################### Concatena: %s\n", header1);
	}

/*
add 1 to the total length, so we have room for the null character --
*the null character is never sent, but is needed to make this a C string
*/

	int tlen =  strlen (header1) + strlen (hostname) + strlen (header2) + 1;

	//printf("CHEGOU AQUI......................");

	char *result = malloc (tlen);

	if(result == NULL)
		return NULL;

	snprintf (result, tlen, "%s%s%s", header1, hostname, header2);

	return result;
}

static void getHostName(char *name, char *hostName, char *caminho){

	int count = 0;
	int countHost = 0;
	int countCaminho = 0;

	if(name[0] == 'h' && name[1] == 't' && name[2] == 't' && name[3] == 'p' && name[4] == 's' &&
		name[5] == ':' && name[6] == '/' && name[7] == '/' && name[8] == 'w' && name[9] == 'w' && name[10] == 'w'
		&& name[11] == '.'){
			count = 12;
			//printf("Entrou no else if\n");
	}

	else if(name[0] == 'h' && name[1] == 't' && name[2] == 't' && name[3] == 'p' &&
		name[4] == ':' && name[5] == '/' && name[6] == '/' && name[7] == 'w' && name[8] == 'w' && name[9] == 'w'
		&& name[10] == '.'){
			count = 11;
			//printf("Entrou no else if\n");
	}

	else if(name[0] == 'h' && name[1] == 't' && name[2] == 't' && name[3] == 'p' && name[4] == 's'
		&& name[5] == ':' && name[6] == '/' && name[7] == '/'){
			count = 8;
			//printf("Entroou no IF\n");
	}

	else if(name[0] == 'h' && name[1] == 't' && name[2] == 't' && name[3] == 'p' &&
		name[4] == ':' && name[5] == '/' && name[6] == '/'){
			count = 7;
			//printf("Entrou no else if\n");
	}

	//printf("name: %c\n", name[count]);

	while(name[count] != '/'){
		//printf("Entrou no while\n");
		hostName[countHost] = name[count];
		countHost++;
		count++;
	}


	while(count < strlen(name)){
		caminho[countCaminho] = name[count];
		count++;
		countCaminho++;
	}

} 



/*
must be executed inline, so must be defined as a macro
*/

int main (int argc, char **argv)
{
	int sockfd;
	struct addrinfo *addrs;
	struct addrinfo hints;
	char *port = "80"; /*default is to connect to the http port, port 80*/

	if((argc != 2) && (argc != 3))
	usage (argv [0]);

	char *hostNameAndCaminho = argv [1];
	char hostname[BUFSIZE] = "";
	char caminho[BUFSIZE] = "0";
	getHostName(argv[1], hostname, caminho);
	printf("############################ Host: %s\n", hostname);
	printf("############################ Host e Caminho: %s\n", hostNameAndCaminho);
	printf("############################ Caminho: %s\n", caminho);

	if(argc == 3)
		port = argv [2];

	bzero (&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if(getaddrinfo (hostname, port, &hints, &addrs) != 0)
		error ("getaddrinfo");

	struct addrinfo *original_addrs = addrs;

		char buf [BUFSIZE];
		char prt [INET6_ADDRSTRLEN] = "unable to print";
		int af = addrs->ai_family;
		struct sockaddr_in *sinp = (struct sockaddr_in *) addrs->ai_addr;
		struct sockaddr_in6 *sin6p = (struct sockaddr_in6 *) addrs->ai_addr;

		if(af == AF_INET)
			inet_ntop (af, &(sinp->sin_addr), prt, sizeof(prt));

		else if(af == AF_INET6)
			inet_ntop (af, &(sin6p->sin6_addr), prt, sizeof(prt));

		else{
			printf ("unable to print address of family %d\n", af);
			//next_loop (addrs, -1);
		}

		if((sockfd = socket (af, addrs->ai_socktype, addrs->ai_protocol)) < 0) {
			perror ("socket");
			//next_loop (addrs, -1);
		}

		printf ("trying to connect to address %s, port %s\n", prt, port);

		if(connect (sockfd, addrs->ai_addr, addrs->ai_addrlen) != 0) {
			perror ("connect");
			//next_loop (addrs, sockfd);
		}

		printf ("connected to %s\n", prt);

		char *request = build_request (hostname, caminho);
		
		sleep(3000000000);

		close(sockfd);

		/*

		if(request == NULL){
			printf ("memory allocation (malloc) failed\n");
			next_loop (addrs, sockfd);
		}

		if(send (sockfd, request, strlen (request), 0) != strlen (request)) {
			perror ("send");
			next_loop (addrs, sockfd);
		}

		free (request); //return the malloc’d memory

		int count = 0;

		while (1) {
			
			//use BUFSIZE - 1 to leave room for a null character
			

			int rcvd = recv (sockfd, buf, BUFSIZE - 1, 0);
			count++;

			if(rcvd <= 0)
				break;

			buf [rcvd] = '\0';
			printf ("%s", buf);
		}

		printf ("data was received in %d recv calls\n", count);
		next_loop (addrs, sockfd);

		*/
		freeaddrinfo (original_addrs);
		return 0;
}