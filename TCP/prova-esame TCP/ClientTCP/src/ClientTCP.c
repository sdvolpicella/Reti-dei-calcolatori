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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "Protocol.h"

#define MAX_LENGTH_NUM 9 //max length of numbers
#define MAX_LENGTH_DEC 10
#define PRECISION 2 //decimal part that we will store

void clearwinsock();

/*
 * The function safeInsert is a function that ask to the user to enter an arithmetic operations,
 * then it checks the input. If the operation inserted is not valid then the program will ask to the user
 * to enter the operation again. If the user's input is equal to '=' the program will terminate.
 * If the user's input is equal to 'h', the program will print instructions.
*/
void safeInsert(operationString* op);

int main() {

	setbuf(stdout, NULL);

	char ipAddress[15];
	int port;

	printf("This program allows you to ask to the server arithmetic operations,\n");
	printf("you must connect to the server before sending operations.\n\n");


	#if defined WIN32
		// Initialize Winsock
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
			printf("Error at WSAStartup()\n");
			return -1;
		}
	#endif

		int mySocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); //creation of the socket

		if(mySocket <= 0){
			printf("Error to create socket!");
			closesocket(mySocket);
			clearwinsock();
			return -1;
		}

		int es = -1;

		while(es == -1){

			printf("Address: ");
			fflush(stdin);
			scanf("%[^\n]s", ipAddress);
			printf("Port: ");
			scanf("%d", &port);


			struct sockaddr_in sad; //construction of the server address
			memset(&sad, 0, sizeof(sad));
			sad.sin_family = AF_INET;
			sad.sin_addr.s_addr = inet_addr(ipAddress);
			sad.sin_port = htons(port);

			es = connect(mySocket, (struct sockaddr *) &sad, sizeof(sad)); //connection to the server

			if(es < 0){
				printf("\n");
				printf("Connection failed!\n");
				printf("Address or port are not valid or you're offline.\n\n");
				printf("Reinsert address and port.\n\n");
			}
		}

		system("cls");


		printf("\nThis program allows you to ask to the server arithmetic operations.\n");
		printf("The operations must follow the following syntax: (operator number number)\n");
		printf("Available operations are: + - / x\n");
		printf("If you need help press: h\n");
		printf("If you want to close the comunication with the server and close the program press: =\n\n");

		operationString operation;

		/*
		 operationString is the struct where se save the numbers and the operator inserted by the user,
		 the client communicates with the server by sending the variable operation.
		*/

		while(1){

			safeInsert(&operation);

			if(operation.operator == '='){ //the connection ends if the users prints =
				Bool flag = true;
				if(send(mySocket,(void*) &flag, sizeof(Bool), 0) <= 0){
					printf("Error... flag not sended!");
				}
				printf("\nProgram terminated.\n\n");
				system("pause");
				closesocket(mySocket);
				clearwinsock();
				return 0;
			}
			else{
				Bool flag = false;
				if(send(mySocket,(void*) &flag, sizeof(Bool), 0) <= 0){
					printf("Error... flag not sended!");
					return 0;
				}
			}

			if((send(mySocket,(void*) &operation, sizeof(operation), 0)) <= 0){
				printf("Error... message non sended!");
				closesocket(mySocket);
				clearwinsock();
				return 0;
			}

			resultMessage resultMex;
			if((recv(mySocket,(void*) &resultMex, sizeof(resultMex), 0)) <= 0){ //resultMessage
				printf("Errore di ricezione del messaggio!");
				closesocket(mySocket);
				clearwinsock();
				return 0;
			}

			/*
			 * The server sends to the client a struct called resultMessage
			 * If the validOp variable is equal to true then it will print the result,
			 * otherwise an error message will appear.
			 *
			 */

			if(resultMex.validOp == false){
				printf("%s\n\n", resultMex.errorString);
			}
			else{
				//printf("Result : %s\n\n", resultMex.resultString);
				float result = atof(resultMex.resultString);
				printf("Result: %.2f\n\n", result);
			}

		}
		system("pause");
		closesocket(mySocket);
		clearwinsock();

		return EXIT_SUCCESS;
	}

	void clearwinsock() {
	#if defined WIN32
		WSACleanup();
	#endif
	}


	void safeInsert(operationString* op){

		char operation[100];

		memset(operation,0,strlen(operation));

		char sub_n1[MAX_LENGTH_NUM];  //strings where we will save the numbers.
		memset(sub_n1,0,strlen(sub_n1));
		char sub_n2[MAX_LENGTH_NUM];
		memset(sub_n2,0,strlen(sub_n2));

		char sub_d1[MAX_LENGTH_DEC];
		char sub_d2[MAX_LENGTH_DEC];
		memset(sub_d1,0,strlen(sub_d1));
		memset(sub_d2,0,strlen(sub_d2));

		int length_n1 = 0; //variables that will save the number of digits of the numbers.
		int length_n2 = 0;
		int length_d1 = 0;
		int length_d2 = 0;

		Bool valid_input = false; //flags that will store which conditions have occurred.
		Bool white_space1 = false;
		Bool white_space2 = false;
		Bool num_correct1 = true;
		Bool num_correct2 = true;
		Bool end_correct = true;
		Bool num1_neg = false;
		Bool num2_neg = false;
		Bool flag_dot1 = false;
		Bool flag_dot2 = false;

		printf("Enter the arithmetic operation to be performed by the server: (operator integer integer)\n");
		printf(">>_ ");

		int i;

		//this loop will repeat until the user enters valid input
		do{
			i = 0;
			valid_input = false;
			white_space1 = false;
			white_space2 = false;
			num_correct1 = true;
			num_correct2 = true;
			end_correct = true;
			flag_dot1 = false;
			flag_dot2 = false;
			length_n1 = 0;
			length_n2 = 0;
			length_d1 = 0;
			length_d2 = 0;
			memset(sub_n1,0,strlen(sub_n1));
			memset(sub_n2,0,strlen(sub_n2));
			memset(operation,0,strlen(operation));


			fflush(stdin);
			scanf(" %[^\n]s",operation);

			if(strlen(operation) <= (MAX_LENGTH_NUM*2)+5){ //check if the string size inserted by the user is correct

				//the first char inserted must be an operator
				if(operation[i] == '+' || operation[i] == '-' || operation[i] == 'x' || operation[i] == '/'){

					i++;

					if(operation[i] == ' '){ //we check if there is a white space in the i=1 position
						white_space1 = true;
					}

					/* At this point the control of the first number starts.
					 * We check if the number starts with a sign (+/-) or with a number.
					 * If it starts with a sign then we check if the sign is -, if so then we save that knowledge in a boolean variable.
					 * Then we cycle the number until we encounter a non-digit.
					 * We save the index of the start of the number and the number of digits.
					 * If there arent problems, we store the number inside a string called sub_n1.
					 * Saving the number in the string is done by cycling the string starting from the index of the first number
					 * up to the value of the variable n; in this cycle we store char by char the digits of the number inside
					 * a string (sub_n1) using the function strncat.
					 *
					 */
					if(white_space1 == true && (isdigit(operation[i+1]) != 0 || operation[i+1] == '+' || operation[i+1] == '-')){ //controllo primo numero

						if(operation[i+1] == '+' || operation[i+1] == '-'){
							i++;
							if(operation[i] == '-'){ //if it is negative then we save that knowledge
								num1_neg = true;
							}
						}

						i++; //we are on the first number
						int init_num = i; //save the position of the first number
						int n = 0; //variable that will store the number of digits of the numbers
						int d = 0; //variable that will store the number of digits after the point



						while((isdigit(operation[i]) != 0) || (operation[i] == '.' && flag_dot1 == false && n>=1)){ //we cycle the number


							if(isdigit(operation[i])==0){
								num_correct1 = false; //if we encounter a non-digit then we save that knowledge
							}else{
								n++;
							}

							//check if we encounter a point, the point cant be the first digit and after the point we need to have atleast one number.
							if(operation[i] == '.' && isdigit(operation[i-1])!=0 && flag_dot1 == false && isdigit(operation[i+1])!=0){
								num_correct1 = true;
								flag_dot1 = true;
								n++;
							}


							if(flag_dot1 == true){
								d++;
							}


							i++;
						}
						length_n1 = n; //save length of the string
						length_d2 = d;


						if(length_n1 > MAX_LENGTH_NUM){
							num_correct1 = false;
						}

						if(operation[i] == ' ' && num_correct1 == true){ //if there arent problem we store the first number inside a string

							int j = 0;
							while(j<n){
								strncat(sub_n1,&operation[init_num],1);
								init_num++;
								j++;
							}

							//After the first number, in the next position we must have a white space.

							if(operation[i] == ' '){
								white_space2 = true;
							}

							//From here on the control of the second number starts, the procedure is the same as the first number
							//with the difference that we expect that after the first number there is the end-of-string symbol.

							if(white_space2 == true && (isdigit(operation[i+1]) != 0 || operation[i+1] == '-' || operation[i+1] == '+')){ //controllo secondo numero

								if(operation[i+1] == '+' || operation[i+1] == '-'){
									i++;
									if(operation[i] == '-'){
										num2_neg = true;
									}
								}

								i++;
								init_num = i;
								n = 0;
								d = 0;
								while(isdigit(operation[i]) != 0 || (operation[i] == '.' && flag_dot2 == false && n>=1)){

									if(isdigit(operation[i])==0){
										num_correct2 = false;
									}else{
										n++;
									}

									if(operation[i] == '.' && isdigit(operation[i-1])!=0 && flag_dot2 == false && isdigit(operation[i+1])!=0){
										num_correct2 = true;
										flag_dot2 = true;
										n++;
									}
									if(flag_dot2 == true){
										d++;
									}
									i++;

								}
								length_n2 = n;
								length_d2 = d;

								if(length_n2 > MAX_LENGTH_NUM){
									num_correct2 = false;
								}

								if(operation[i] == '\0' && num_correct2 == true){

									j = 0;
									while(j<n){
										strncat(sub_n2,&operation[init_num],1);
										init_num++;
										j++;
									}

								}else{
									end_correct = false;
								}

							}else{
								num_correct2 = false;
							}


						}else{
							num_correct1 = false;
						}


					}else{
						num_correct1 = false;
					}


					//if every condition is met then the values ​are saved in the struct.
					if( (num_correct1 == true) && (num_correct2 == true) && (white_space1 == true) &&
							(white_space2 == true) && (end_correct == true) && (length_n1 < MAX_LENGTH_NUM) &&
							(length_n2 < MAX_LENGTH_NUM) && (length_d1 <= MAX_LENGTH_DEC) && (length_d2 <= MAX_LENGTH_DEC)){

						valid_input = true;
						op->n1 = atoi(sub_n1); //conversion of string into integer
						op->n2 = atoi(sub_n2);

						if(num1_neg == true){ //if the number is negative, then we do 0 - number
							op->n1 = 0 - op->n1;
						}

						if(num2_neg == true){
							op->n2 = 0 - op->n2;
						}

						/* if in the number there was a dot, then we have to save the decimal part
						 * we find the position of the point in the string, then we create a substring where we save only
						 * the decimal part.
						 */
						if(flag_dot1 == true){

							int point_position;
							for(int k=0; k<strlen(sub_n1); k++){

								if(sub_n1[k]=='.'){
									point_position = k;
								}
							}
							point_position++;
							for(int k=0; k<PRECISION; k++){
								strncat(sub_d1,&sub_n1[point_position],1);
								point_position++;
							}

						}

						if(flag_dot2 == true){

							int point_position;
							for(int k=0; k<strlen(sub_n2); k++){

								if(sub_n2[k]=='.'){
									point_position = k;
								}
							}
							point_position++;
							for(int k=0; k<PRECISION; k++){
								strncat(sub_d2,&sub_n2[point_position],1);
								point_position++;
							}
						}

						op->d1 = atoi(sub_d1); //decimal parts are saved inside the struct
						op->d2 = atoi(sub_d2);

						op->n1 = htonl(op->n1); //we convert the numbers into the network format
						op->n2 = htonl(op->n2);
						op->d1 = htons(op->d1);
						op->d2 = htons(op->d2);
						op->operator = operation[0];

					}else{

						valid_input = false;
						printf("\nThe inserted operation is not valid, press h if you need help.\n");
						printf("Enter the arithmetic operation to be performed by the server: \n");
						printf(">>_ ");

					}

				}else if(operation[i] == '='){ //if the user inserts = then we save the operator into the struct

					i++;
					if(operation[i] == '\0'){
						valid_input = true;
						op->n1 = 0;
						op->n2 = 0;
						op->d1 = 0;
						op->d2 = 0;
						op->operator = '=';
					}else{

						printf("\nThe inserted operation is not valid, if you want to close the program press just: =\n");
						printf("If you need help press h\n");
						printf("Enter the arithmetic operation to be performed by the server: \n");
						printf(">>_ ");

					}

				}else if(operation[i] == 'h' && operation[i+1] == '\0'){

					printf("\nThis program allows you to ask the server to perform arithmetic operations.\n");
					printf("The arithmetic operations must follow the following syntax: (operator number number)\n");
					printf("You can do operations with negative numbers by inserting the sign (+/-) before the number.\n");
					printf("The maximum lenght of an integer number must be <= 8 (7 if it is float).\n");
					printf("Examples of a valid operations are:\n");
					printf("+ 55.16 78\n/ 120.5 7\nx -67 -5.5\n- +55.12 -48\n");
					printf("If you want to terminate the connession with the server and close the application digit: =\n\n");
					printf("Enter the arithmetic operation to be performed by the server: (operator integer integer)\n");
					printf(">>_ ");

				}else{

					printf("\nThe inserted operation is not valid, if you need help press h.\n");
					printf("Enter the arithmetic operation to be performed by the server: \n");
					printf(">>_ ");

				}

			}else{
				printf("\nThe inserted operation is not valid, the digits of the entered operation are too many.\n");
				printf("If you need help press: h\n");
				printf("Enter the arithmetic operation to be performed by the server: \n");
				printf(">>_ ");
			}

		}while(valid_input == false);
	}
