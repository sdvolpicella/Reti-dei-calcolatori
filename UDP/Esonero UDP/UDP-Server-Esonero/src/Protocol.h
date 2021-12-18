/*
 * Protocol.h
 *
 * Authors: Volpicella S.D. & Valentino F.
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#define PORT 48000

typedef enum { false, true } Bool;

/*
 * operationString is the struct where we store the numbers and the operator inserted by the user.
 * When the string is inserted, the function safeInsert implemented in the client controls that the input is correct.
 * If the string is correct the values are stored inside an operationString variable and then the variable is sent to the server.
 */
typedef struct {
    char operator;
    int n1;
    int n2;
    int d1; //decimal part of the first number
    int d2;    //decimal part of the second number
    Bool n1Positive;
    Bool n2Positive;
} operationString;

/*
 * The struct resultMessage is a struct that is sent to the client by the server
 * In a resultMessage variable is stored the result of the arithmetic operation, a boolean flag that tells
 * if the operation sent by the client is valid, and an error message.
 */

typedef struct {
    char resultString[100];
    Bool validOp;
    char errorString[100];
} resultMessage;

#endif /* PROTOCOL_H_ */
