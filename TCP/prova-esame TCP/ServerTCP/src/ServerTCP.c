/*
 ============================================================================
 Course      : Computer networks
 Authors     : Volpicella S.D. & Valentino F.
 Description : Program for the course Computers networks, exercise about TCP
 ============================================================================
 */
#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#define QLEN 5

#include <stdio.h>
#include <stdlib.h>

#include "Protocol.h"

void clearwinsock();
float add(float n1,float n2); //functions that perform arithmetic operations
float sub(float n1,float n2);
float mult(float n1,float n2);
float division(float n1,float n2,Bool* flag);


/*
 * The processOperation function is a function that calls the functions that
 * perform arithmetic operations based on the operator sent by the user
 * this function returns a variable of type resultMessage, in which are stored:
 * the result, an error message, a Boolean variable whose value indicates whether
 * the operation sent by the client is valid or not.
 */
resultMessage processOperation(operationString opString);

int main(void) {

	setbuf(stdout, NULL);

	 #if defined WIN32
		// Initialize Winsock
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
			printf("Error at WSAStartup()\n");
			return 0;
		}
	#endif

	//Create socket to accept connection
	int my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	//assigning an address to the socket
	struct sockaddr_in sad;
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = inet_addr("127.0.0.1");
	sad.sin_port = htons(N_PORT);

	if (bind(my_socket, (struct sockaddr*) &sad, sizeof(sad)) <0) {
		printf("bind failed.\n");
		closesocket(my_socket);
		return -1;
	}

	//set the socket to listening
	if (listen (my_socket, 1) < 0) {
		printf("listen failed.\n");
		closesocket(my_socket);
		return -1;
	}

	//accept a new connection
	struct sockaddr_in client_address;
	int client_socket = 0;
	int client_lenght;

	while (1) {

		printf( "Waiting for a client to connect...\n");

		client_lenght = sizeof(client_address);

		if ( (client_socket=accept(my_socket, (struct sockaddr *) &client_address, &client_lenght)) < 0 ) {
		  printf("accept failed.\n");
		  closesocket(my_socket);
		  clearwinsock();
		  return -1;
		}

		printf("Connection established with %s:%d\n",inet_ntoa(client_address.sin_addr),client_address.sin_port);

		Bool exit = false;
		while(exit == false){

			int bytes_rcvd = 0;

			bytes_rcvd = recv(client_socket, (void*)&exit, sizeof(exit), 0);
			if(bytes_rcvd <= 0){
				exit = true;
			}

			if(exit == false){
				operationString operationStr;

				bytes_rcvd = recv(client_socket, (void*)&operationStr, sizeof(operationStr), 0);
				if(bytes_rcvd <= 0){
					printf("Message not receved!\n");
				}

				else{
					resultMessage rm = processOperation(operationStr);
					if((send(client_socket, (void*)&rm, sizeof(rm), 0)) <= 0){
						printf("Message not sended!\n");
					}
				}

			}
		}
		printf("Connection terminated with %s:%d\n\n",inet_ntoa(client_address.sin_addr),client_address.sin_port);

	}

	closesocket(my_socket);
	clearwinsock();
	return EXIT_SUCCESS;
}

void clearwinsock(){
#if defined WIN32
WSACleanup();
#endif
}

float add(float n1,float n2){
	return n1+n2;
}

float sub(float n1,float n2){
	return n1-n2;
}

float mult(float n1,float n2){
	return n1*n2;
}

float division(float n1,float n2,Bool* flag){
	if(n2 == 0 || n2== 0.00 || n2 == 0.0){
		*flag = false;
		return 0.0;
	}
	return n1/n2;
}

resultMessage processOperation(operationString os){
	resultMessage mex;

	os.n1 = ntohl(os.n1);
	os.n2 = ntohl(os.n2);
	os.d1 = ntohs(os.d1);
	os.d2 = ntohs(os.d2);

	float number1 = 0;
	float number2 = 0;

	float dec1 = 0.00;
	float dec2 = 0.00;

	float result = 0;

	//conversion of the decimal part of the numbers sent by the client
	if(os.d1 >= 10){
		dec1 = (float)os.d1 / 100;
	}else{
		dec1 = (float)os.d1 / 10;
	}

	if(os.d2 >= 10){
		dec2 = (float)os.d2 / 100;
	}else{
		dec2 = (float)os.d2 / 10;
	}

	//union of the integer and decimal part in a single float variable
	if(os.n1 >= 0){
		number1 = (float)os.n1 + dec1;
	}else{
		number1 = (float)os.n1 - dec1;
	}

	if(os.n2 >=0){
		number2 = (float)os.n2 + dec2;
	}else{
		number2 = (float)os.n2 - dec2;
	}

	Bool validOp = true;

	switch(os.operator){

		case '+': result = add(number1,number2);
				  sprintf(mex.resultString,"%f",result);
				  mex.validOp = validOp;
				  strcpy(mex.errorString," ");
				  break;

		case '-': result = sub(number1,number2);
				  sprintf(mex.resultString,"%f",result);
				  mex.validOp = validOp;
				  strcpy(mex.errorString," ");
				  break;

		case 'x': result = mult(number1,number2);
				  sprintf(mex.resultString,"%f",result);
				  mex.validOp = validOp;
				  strcpy(mex.errorString," ");
				  break;

		case '/': result = division(number1,number2, &validOp);
				  sprintf(mex.resultString,"%f",result);
				  mex.validOp = validOp;
				  if(mex.validOp == false){
					  strcpy(mex.errorString,"You can't divide for 0!");
				  }
				  break;
			  }

	return mex;
}
