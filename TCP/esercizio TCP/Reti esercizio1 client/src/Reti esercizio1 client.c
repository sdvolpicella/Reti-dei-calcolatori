/*
 ============================================================================
 Name        : Reti.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "struct.h"
#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#define BUFFERSIZE 512
#define PROTOPORT 27015

void errorhandler(char *error_message){
	printf("%s",error_message);
}

void clearwinsock(){
	#if defined WIN32
	WSACleanup();
	#endif
}

int main(void) {

	setvbuf (stdout, NULL, _IONBF, 0);
	printf("[--- CLIENT ---]\n\n");

	#if defined WIN32
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2),&wsa_data);
	if(result != 0){
		printf("error at WSAStartup\n");
		return -1;
	}
	#endif

	int c_socket;
	c_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(c_socket < 0){
		errorhandler("[-] Creazione della socket non riuscita.\n");
		closesocket(c_socket);
		clearwinsock();
		return -1;
	}else{
		printf("[+] Socket creata con successo.\n");
	}

	struct sockaddr_in sad;
	memset(&sad,0,sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = inet_addr("127.0.0.1");
	sad.sin_port = htons(27015);

	if(connect(c_socket,(struct sockaddr *) &sad,sizeof(sad))<0){

		errorhandler("[-] Tentativo di connessione fallito.\n");
		closesocket(c_socket);
		clearwinsock();
		return -1;
	}else{
		printf("[+] Connessione con il server riuscita.\n");
	}

	//INVIARE DATI AL SERVER

	char* input_string = "Ciao, sono il client.";
	int string_len = strlen(input_string);

	printf("[ ] Stringa che il client vuole mandare al server: %s",input_string);

	if(send(c_socket,input_string,string_len,0) != string_len){
		errorhandler("\n[-] Invio del messaggio non riuscito, numero di byte non rispettati.\n");
		closesocket(c_socket);
		clearwinsock();
		return -1;
	}else{
		printf("\n[+] Invio del messaggio avvenuto con successo.");
	}

	//RICEVERE DATI DAL SERVER
	int bytes_rcvd;
	int total_bytes_rcvd = 0;
	char buf[BUFFERSIZE];
	printf("\n[ ] Dati ricevuti dal server: ");
	while(total_bytes_rcvd < string_len){
		if((bytes_rcvd = recv(c_socket,buf,BUFFERSIZE-1,0)) <=0){
			errorhandler("[-] La ricezione del messaggio dal server è fallita.\n");
			closesocket(c_socket);
			clearwinsock();
			return -1;
		}
		total_bytes_rcvd += bytes_rcvd;
		buf[bytes_rcvd] = '\0';
		printf("%s",buf);

	}

	//INVIO DELLE DUE STRINGHE AL SERVER

	msgStruct msg;

	//msg.string1 = (char*)malloc(sizeof(char*));
	printf("\nInserisci la prima stringa: ");
	scanf("%s",msg.string1);

	//msg.string2 = (char*)malloc(sizeof(char*));
	printf("Inserisci la seconda stringa: ");
	scanf("%s",msg.string2);


	printf("[ ] Il client vuole inviare al server: %s %s",msg.string1, msg.string2);

	if(send(c_socket,&msg,sizeof(msgStruct),0) != sizeof(msgStruct)){
		errorhandler("\n[-] Invio del messaggio non riuscito, numero di byte non rispettati.\n");
		closesocket(c_socket);
		clearwinsock();
		return -1;
	}else{
		printf("\n[+] Invio del messaggio avvenuto con successo.");
	}


	memset(&msg,0,sizeof(msg));
	recv(c_socket,&msg,sizeof(msg),0);
	printf("\n[ ] Stringhe ricevute dal server: %s %s", msg.string1, msg.string2);


	//CHIUSURA DELLA CONNESSIONE

	closesocket(c_socket);
	clearwinsock();
	printf("\n\n");
	system("pause");
	return(0);

}
