/*
 ===================================================================================
 Course      : Computer networks
 Authors     : Volpicella S.D. & Valentino F.
 Description : Program for the course Computers networks, exercise about UDP: Server
 ===================================================================================
 */

#if defined WIN32
#include <winsock.h>
#else
#include <netdb.h>         //DNS no Windows
#define closesocket close
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Protocol.h"

float add(float n1,float n2); //functions that perform arithmetic operations
float sub(float n1,float n2);
float mult(float n1,float n2);
float division(float n1,float n2,Bool* flag);


/*The formatNumber function is used to insert commas within the string that represents the result every three digits.
 * For example if the result of an arithmetic operation is 1123.00, then with this function the result will be 1,123.00*/
void formatNumber(char numberString[], char res[]);


/*the formatString function is a function that serves to create the string formed by the operands,
 * the operator and the result of the operation. For example if the operation sent from the client is '+ 5 3',
 * then this function will create the following string: '5 + 3 = 8'*/
void formatString(char finalString[], char numberString[], operationString operString);


/*
 * The processOperation function is a function that calls the functions that
 * perform arithmetic operations based on the operator sent by the user
 * this function returns a variable of type resultMessage, in which are stored:
 * the result, an error message, a Boolean variable whose value indicates whether
 * the operation sent by the client is valid or not.
 */
resultMessage processOperation(operationString opString);


void ErrorHandler(char *errorMessage){
	printf(errorMessage);
}

void ClearWinSock(){
	#if defined WIN32
	WSACleanup();
	#endif
}

int main(void) {

	setbuf(stdout, NULL);

	#if defined WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(iResult != 0){
		printf("error at WSAStrartup\n");
		return EXIT_FAILURE;
	}
	#endif

	int sock;
	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;
	unsigned int clientAddrLen;
	int recvMsgSize;

	//creation of the socket
	if((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP))<0){
		ErrorHandler("socket() failed\n");
	}

	//building the address of the server
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//bind of the socket
	if((bind(sock,(struct sockaddr *)&serverAddr, sizeof(serverAddr)))<0){
		ErrorHandler("bind() failed\n");
	}


	operationString op;

	while(1){
		clientAddrLen = sizeof(clientAddr);
		//the server receives the struct from the client
		recvMsgSize = recvfrom(sock, (void*)&op, sizeof(op), 0, (struct sockaddr*)&clientAddr, &clientAddrLen);
		if(recvMsgSize <= 0){
			printf("Error\n");
		}

		//gethostbyaddress
		struct hostent *host;
		struct in_addr addr;
		addr.s_addr = inet_addr(inet_ntoa(clientAddr.sin_addr));
		host = gethostbyaddr((char*)&addr,4,AF_INET);

		char* canonical_name = host->h_name;

		op.n1 = ntohl(op.n1);
		op.n2 = ntohl(op.n2);
		op.d1 = ntohs(op.d1);
		op.d2 = ntohs(op.d2);

		char temp1[10] = "";
		char temp2[10] = "";
		char num1[10] = "";
		char num2[10] = "";

		itoa(op.n1,temp1,10);
		itoa(op.n2,temp2,10);

		formatNumber(num1,temp1);
		formatNumber(num2,temp2);

		printf("Operation request '%c %s.%d %s.%d' from client %s, ip %s\n",op.operator,num1,op.d1,num2,op.d2,canonical_name,inet_ntoa(clientAddr.sin_addr));

		resultMessage rm = processOperation(op);
		char formattedNumber[100] = "";
		char result[100] = "";

		strcpy(result,rm.resultString);
		strcpy(rm.resultString,"");

		formatNumber(formattedNumber,result);

		formatString(rm.resultString,formattedNumber,op);

		//the result is sent to the client
		if(sendto(sock, (void*)&rm, sizeof(rm), 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr)) !=
				sizeof(rm)){
				ErrorHandler("sendto() sent different number of bytes than expected");
		}


	}

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

	float number1 = 0;
	float number2 = 0;

	float dec1 = 0.00;
	float dec2 = 0.00;

	float result = 0;

	//conversion of the decimal part of the numbers sent by the client
	if(os.d1 >= 10){
		dec1 = (float)os.d1 / 100;
	}else if(os.d1 >= 100){
		dec1 = (float)os.d1 / 1000;
	}else if(os.d1 >= 1000){
		dec1 = (float)os.d1 / 10000;
	}else if(os.d1 >= 10000){
		dec1 = (float)os.d1 / 100000;
	}else if(os.d1 >= 100000){
		dec1 = (float)os.d1 / 1000000;
	}else if(os.d1 > 0 && os.d1<10){
		dec1 = (float)os.d1 / 10;
	}else if(os.d1 == 0){

		dec1 = 0.00;
	}

	if(os.d2 >= 10){
		dec2 = (float)os.d2 / 100;
	}else if(os.d2 >= 100){
		dec2 = (float)os.d2 / 1000;
	}else if(os.d2 >= 10000){
		dec2 = (float)os.d2 / 100000;
	}else if(os.d2 >= 100000){
		dec2 = (float)os.d2 / 1000000;
	}else if(os.d2 > 0 && os.d2<10){
			dec2 = (float)os.d2 / 10;
		}else if(os.d2 == 0){

			dec2 = 0.00;
		}


	//union of the integer and decimal part in a single float variable
	if(os.n1 > 0){
		number1 = (float)os.n1 + dec1;
	}else if(os.n1 == 0){
		if(os.n1Positive == true){
				number1 =  (float)os.n1 + dec1;
		}else{
			number1 =  (float)os.n1 + dec1;
			number1 = number1 * (-1);
		}

	}else{
		number1 =  (float)os.n1 - dec1;
	}

	if(os.n2 >0){
		number2 =  (float)os.n2 + dec2;
	}else if(os.n2 == 0){
		if(os.n2Positive == true){
			number2 =  (float)os.n2 + dec2;
		}else{
			number2 =  (float)os.n2 + dec2;
			number2 = number2 * (-1);
		}
	}
	else{
		number2 =  (float)os.n2 - dec2;
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

void formatNumber(char numberString[], char res[]){

    char temp[100] = "";
    int start = strlen(res);
    int count = 0;
    int j = 0;
    Bool flag_point = false;
    Bool isDecimal = false;

    for(int i=0;i<start;i++){
    	if(res[i] == '.'){
    		isDecimal = true;
    		i = start;
    	}
    }

    for(int i=start-1; i>=0; i--){
    	if(res[i] == '.'){
    		temp[j] = res[i];
            flag_point = true;
            j++;
        }else if(flag_point == false && isDecimal == true){
        	temp[j] = res[i];
        	j++;
        }else{
            count++;
            if(count == 3 && (i-1>=0) && (res[i-1] != '-')){
            	count = 0;
            	temp[j] = res[i];
            	temp[j+1] = ',';
            	j++;
                j++;
            }else{
            	temp[j] = res[i];
            	j++;
            }
        }
    }

    int pointIndex = 0;
    start = strlen(temp);
    j = 0;
    for(int i = start-1;i>=0;i--){
    	if(temp[i] == '.'){
    		pointIndex = j;
    	}
    	numberString[j] = temp[i];
    	j++;
    }

    if(flag_point == true){
    	numberString[pointIndex+3] = '\0';
    }

}

void formatString(char finalString[], char numberString[], operationString operString){

	 char dec1[10] = "";
	 char num1[10] = "";
	 char dec2[10] = "";
	 char num2[10] = "";
	 char temp1[10] = "";
	 char temp2[10] = "";

	 char equal = '=';

	 itoa(operString.d1,dec1,10);
	 itoa(operString.d2,dec2,10);
	 itoa(operString.n1,temp1,10);
	 itoa(operString.n2,temp2,10);

	 formatNumber(num1,temp1);
	 formatNumber(num2,temp2);

	 strncat(finalString,num1,10);
	 strncat(finalString,".",10);
	 strncat(finalString,dec1,10);

	 strncat(finalString," ",10);

	 strncat(finalString, &operString.operator,1);

	 strncat(finalString," ",10);

	 strncat(finalString,num2,10);
	 strncat(finalString,".",10);
	 strncat(finalString,dec2,10);

	 strncat(finalString," ",10);

	 strncat(finalString,&equal,1);

	 strncat(finalString," ",10);

	 strncat(finalString,numberString,50);

}
