#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT_22GSE 22123
#define BUFFER_SIZE 1024

//functie pt crearea unui socket
int open_server_socket(){
	int server_socket;
	int true =1;

	//creare socket
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket == -1) {
		perror("socket ul nu a fost deschis!");
		return 0;
		
}
puts("socket ul a fost deschis");

	//setarea optiunii pt reutilizarea codului
     if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int))==-1){
       perror("Eroare la setarea optiunii");
       return 0;
     
     }
return server_socket;
}

	//functie pt asocierea socket ului la portul specificat
   void bind_server_socket(int sock, int port){
     struct sockaddr_in serv_addr;
	
     //config. structurii adresei serverului
     serv_addr.sin_family=AF_INET;
     serv_addr.sin_port=htons(port); //conversie port in retea
     serv_addr.sin_addr.s_addr=INADDR_ANY; //acceptare conexiune de pe orice adresa

     //asocierea socketului la adresa si port
     if(bind(sock,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1){
     perror("Eroare bind socket");
     close(sock);
     exit(EXIT_FAILURE);
     }else{
     printf("Bind OK", port);
     }
   }
    
	//functie pt setarea serverului in modul ascultare
     void start_listen(int sock, int pending_con){
     if(listen(sock, pending_con) == -1){
	perror("Eroarea asteptare client");
        close(sock);
        exit(EXIT_FAILURE);	
 }else{
 puts("Asteptare client...");
 }
 }
	 
	//fucntie pt acceptarea conexiunilor de la clienti
    int accept_connections(int sock){
    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof(their_addr);
	
    //acceptare conexiuni
    int accept_socket = accept(sock, (struct sockaddr *)&their_addr,&addr_size);
    if(accept_socket==-1){
    perror("Conexiunea nu a fost acceptata");
    }else{
    puts("cONEXIUNE ACCEPTATA");
    return accept_socket;
    }
    }
	//functie pt verificarea unei comenzi
    int CheckCommand(char *buffer, char *command){
	  if(strncmp(buffer, command, strlen(command))==0){ //compara comanda citita cu cea asteptata
	    return 1; //Comanda este corecta
    }else{
   	     return 0; //comanda este incorecta
    }
    }
   
	//prototipuri pt functiile utilizate
int open_server_socket();
void bind_server_socket(int sock, int port);
void start_listen(int sock, int pending_con);
int accept_connections(int sock);


int main(){
	//crearea socketului serverlui
	int server_socket = open_server_socket();
	if(server_socket == 0){
		exit(EXIT_FAILURE);
	}
	//asocierea socketului la portul definit
	bind_server_socket(server_socket, PORT_22GSE);
	start_listen(server_socket, 5); //serverului incepe sa asculte conexiuni

	//bucla principala a serverului
	while(1){
		//acceptarea unei conexiuni de la client
		int client_socket = accept_connections(server_socket);
		if(client_socket !=0){
			char buffer[BUFFER_SIZE]; //buffer pt mesajele primite
			memset(buffer, 0, BUFFER_SIZE);
			//procesarea mesajelor primite de la client
		       while(1){	
			       //primirea mesajelor
			int bytes_received = recv(client_socket, buffer, BUFFER_SIZE ,0);
			if(bytes_received <=0)
			{
			perror("eroare la primirea mesajului! \n");
			close(client_socket);
			break;
  			}
		printf("Mesaj primit: %s\n", buffer);
		if(CheckCommand(buffer, "exit")){ //inchide coneciunea la primirea comenzii exit
			send(client_socket, "conexiune inchisa\n",18,0);
			close(client_socket);
			break;
		}
			//verifica comanda si trimite rasp 
	if(CheckCommand (buffer," send cmd")){
	send(client_socket, "Comanda acceptata\n", 18, 0);
	} else if(CheckCommand(buffer, "06#")){
		send(client_socket, "Comanda 06# recunoscuta\n" , 24,0);
	}
	else {
	send(client_socket, "Comanda neacceptata\n", 21,0);
	}
	}
	}
	}
	//inchiderea socketului serverului
	close(server_socket);
	return 0;	
}

