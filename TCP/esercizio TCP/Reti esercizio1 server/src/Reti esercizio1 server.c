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
#define PROTOPORT 27015
#define QLEN 6
//
#define BUFFERSIZE 512

#include <ctype.h>
#include <string.h>

void errorhandler(char *errorMessage){
	printf("%s",errorMessage);
}

void clearwinsock(){
	#if defined WIN32
	WSACleanup();
	#endif
}

int main(int argc, char *argv[]) {

	setvbuf (stdout, NULL, _IONBF, 0);
	int port;
	printf("[--- SERVER ---]\n\n");

	if(argc>1){
		port = atoi(argv[1]);
	}else{
		port = PROTOPORT;
	}

	if(port<0){
		printf("[-] Numero di porta non valido %s \n", argv[1]);
		return 0;
	}

	#if defined WIN32
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2),&wsa_data);
	if(result != 0){
		errorhandler("[-] Error at WSAStartup()\n");
		return 0;
	}
	#endif

	int my_socket;
	my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(my_socket<0){
		errorhandler("[-] Creazione della socket fallita.\n");
		clearwinsock();
		return -1;
	}else{
		printf("[+] Creazione della socket avvenuta con successo.\n");
	}

	struct sockaddr_in sad;
	memset(&sad,0,sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = inet_addr("127.0.0.1");
	sad.sin_port = htons(port);
	if(bind(my_socket, (struct sockaddr*) &sad, sizeof(sad)) < 0){
		errorhandler("[-] bind() fallita.\n");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}else{
		printf("[+] bind() riuscita.\n");
	}

	if(listen(my_socket,QLEN)<0){
			errorhandler("[-] listen() fallita.\n");
			closesocket(my_socket);
			clearwinsock();
			return -1;
	}else{
		printf("[+] listen() riuscita.\n");
	}

	struct sockaddr_in cad;
	int client_socket;
	int client_len;
	printf("[ ] In attesa che un client si connetta...\n");
	while(1){
		client_len = sizeof(cad);
		if((client_socket = accept(my_socket, (struct sockaddr*)&cad, &client_len))<0){
			errorhandler("[-] accept() fallita.\n");

			closesocket(client_socket);
			clearwinsock();
			return 0;
		}else{
			printf("[+] accept() riuscita.\n");
		}
		printf("[ ] Connesso con il client %s\n",inet_ntoa(cad.sin_addr));

		/////-------


		char buf[BUFFERSIZE];
		printf("[ ] Dati ricevuti dal client: ");

		memset(buf,0,BUFFERSIZE);
		recv(client_socket,buf,BUFFERSIZE-1,0);
		printf("%s",buf);


		char* input_string = "Ciao client, io sono il server";
		int string_len = strlen(input_string);

		printf("\n[ ] Stringa che il server vuole inviare al client: %s\n",input_string);

		if(send(client_socket,input_string,string_len,0) != string_len){
			errorhandler("\n[-] Invio del messaggio non riuscito, numero di byte non rispettati.\n");
			closesocket(client_socket);
			clearwinsock();
			return -1;
		}else{
			printf("[+] Invio del messaggio avvenuto con successo.\n");
		}

		//RICEZIONE DELLE DUE STRINGHE DEL SERVER DA PARTE DEL CLIENT

		msgStruct msg;

		memset(&msg,0,sizeof(msg));
		recv(client_socket,&msg,sizeof(msg),0);

		printf("[ ] Stringhe ricevute dal client: %s %s", msg.string1, msg.string2);

		//RENDO LA PRIMA STRINGA TUTTA MAIUSCOLA E LA SECONDA MINUSCOLA


		for(int i=0;i<strlen(msg.string1);i++){
		        msg.string1[i] = toupper(msg.string1[i]);
		}
		for(int i=0;i<strlen(msg.string2);i++){
				msg.string2[i] = tolower(msg.string2[i]);
		}

		printf("\n[ ] Stringhe che il server vuole inviare al client: %s %s",msg.string1,msg.string2);

		if(send(client_socket,&msg,sizeof(msgStruct),0) != sizeof(msgStruct)){
			errorhandler("\n[-] Invio del messaggio non riuscito, numero di byte non rispettati.\n");
			closesocket(client_socket);
			clearwinsock();
			return -1;
		}else{
			printf("\n[+] Invio del messaggio avvenuto con successo.");
		}


	}

	//return EXIT_SUCCESS;
}
