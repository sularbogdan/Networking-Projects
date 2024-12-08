#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>


int  main( ){
	
 	printf("it s open!\n");
	int server_sock = 0;  //declaratie variabila pt socketul clientului
	server_sock = socket(AF_INET6,SOCK_STREAM , 0); //creare socket ipv6
	printf("Socket Initializat\n");
	//verificare daca socketul a fost creat cu succes
	if(server_sock==-1)
	{
		printf("eroare la crearea socket!\n");
		return -1;
	}
	else{
		printf("socket creat\n");
	struct sockaddr_in6 server_address;
	server_address.sin6_family = AF_INET6;	//protocol ipv6
	server_address.sin6_port=htons(80);    //port standard HTTP
	inet_pton(AF_INET6, "::1", &server_address.sin6_addr); //adresa ipv6 (localhost)

	//conectare la server
	if(connect(server_sock,(struct sockaddr*)&server_address, sizeof(server_address))==-1)
	{
	 	perror("eroare la conectare\n\n");
		close(server_sock);
		exit(EXIT_FAILURE);
	} else {
		printf("Conectare reusita!\n");
			
		char request[]="GET / HTTP/1.0\r\n\r\n"; //construirea cererii HTTP
		
		//Trimiterea cererii HTTP catre server
		if(send(server_sock, request, strlen(request),0) == -1){
		  printf("Eroare la trimiterea cererii\n");
		  close(server_sock);
		  exit(EXIT_FAILURE);
		
		}else{
		printf("Cererea a fost trimisa: %s\n", request);

		// deschidere  de fisier pt salvare rasp
		FILE *file = fopen("response.html","w");
		if(file == NULL){
			perror("eroare la deschiderea fisierului");
			exit(EXIT_FAILURE);
		}

		int bytes_received; //variabila pt stocarea numarului de octeti primiti
		char response[1024]; //buffer pt raspunsul serverului
		printf("RASPUNS PRIMIT\n");
		
		while((bytes_received = recv(server_sock, response, sizeof(response) -1 ,0))>0){
		   
	               	response[bytes_received] = '\0';  //terminator pt sirul de caractere
		    	 fprintf(file, "%s", response);
		}

		//verificare eroare la primirea datelor
		if(bytes_received==-1){
		printf("Eroare la primirea raspunsului\n");
		}	
		fclose(file);
	
              }
      close(server_sock);
      printf("\nConexiunea a fost inchisa.\n");
  
	}
}
     return 0;
}

