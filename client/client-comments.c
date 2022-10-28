// Cwk2: client.c - message length headers with variable sized payloads
//  also use of readn() and writen() implemented in separate code module

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <arpa/inet.h>
#include "rdwrn.h"
#include <sys/time.h>
#include <ctype.h>
#include <sys/utsname.h>






//DEFINING FUNCTIONS FOR THE MAIN FUNCTION

int input_check();
void send_msg(int, char *, size_t);
void menu_opts(int);
void signal_handler(int);
void copy_contents(int, char *);
void getsystem_info(int, char *);
void handler_client_server(int, char *);



//DEFINING STRUCTS

typedef struct
{
        char sysname[100];
        char release[100];
        char version[100];
}
uts;
struct timeval tv;
struct timeval tv1;


// DECLARING GLOBAL VARIABLES
int connfd;


/*
DISPLAYING THE STUDENT DETAILS BEFORE DIPLAYING THE MENU
*/

void get_studentDetails()
{
        printf("\e[1;1H\e[2J");
        printf("\e[96m");
        printf("\e[1m");
        printf(" ------------------------------------------------------- \n");
        printf(" *                STUDENT DETAILS                   *\n");
        printf(" ------------------------------------------------------- ");
        printf("\e[96m");
        printf("\n\e[1m STUDENT NAME:           \e[0m\e[96mMAINA MARGARET\n");
        printf("\e[1m STUDENT ID:             \e[0m\e[96mS1906597\n");
        printf("\e[1m MODULE NAME:            \e[0m\e[96mSYSTEMS PROGRAMMING\n");
        printf("\n MENU LOADING...\n");
}

/*DISPLAYING THE MENU OPTIONS*/

void show_menu()
{
        printf("\e[96m");
        printf("\e[1m");
        printf(" ------------------------------------------------------- \n");
        printf(" *                    MENU  OPTIONS                        *\n");
        printf(" ------------------------------------------------------- ");
        printf("\e[0m");
        printf("\n\e[96m 1. DISPLAY STUDENT DETAILS\n");
        printf(" 2. DISPLAY RANDOM ARRAY OF 5 NUMBERS\n");
        printf(" 3. DISPLAY SERVER INFORMATION\n");
        printf(" 4. DISPLAY LIST OF FILES IN THE CURRRENT WORKING DIRECTORY\n");
	printf(" 5. COPY A FILE CONTENTS TO THE CLIENT'S DIRECTORY\n");
        printf(" 6. CLOSE CLIENT CONNECTION\n");
        printf("\n option > ");
}



int main(void)
{


// CHECK THE TOTAL ELAPSED TIME SINCE THE CONNECTION TO THE SERVER WAS MADE
	
        gettimeofday(&tv, NULL);
        struct sigaction sa;

       	// SETTING UP THE SIGNAL HANDLER
        sa.sa_handler = &signal_handler;
        if (sigaction(SIGINT, &sa, NULL) == -1)
        {
                perror(" ERROR: CANNOT HANDLE SIGINT\n");
        }

    

    int sockfd = 0;
    struct sockaddr_in serv_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	perror("Error - could not create socket");
	exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;

// IP address and port of server we want to connect to
    serv_addr.sin_port = htons(50031);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

// try to connect to server...
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)  {
	perror("Error - connect failed");
	exit(1);
    } else
       printf("Connected to server...\n");
	connfd = sockfd;
	
	while(1){
	menu_opts(sockfd);
}
close(sockfd);
exit(EXIT_SUCCESS);

}//END MAIN





//FUNCTION TO DISPLAY MENU TO THE USER.

void menu_opts(int socket)
{
        get_studentDetails();
        sleep(5);
        printf("\e[1;1H\e[2J");
        do {
                show_menu();
                int selection;
                selection = input_check();

                switch (selection)
                {
                        case 1:
                                {
                                        char *selection1 = "1\n";
                                        handler_client_server(socket, selection1);
                                        break;
                                }
                        case 2:
                                {
                                        char *selection2 = "2\n";
                                        handler_client_server(socket, selection2);
                                        break;
                                }
                        case 3:
                                {
                                        char *selection3 = "3\n";
                                        getsystem_info(socket, selection3);
                                        break;
                                }
                        case 4:
                                {
                                        char *selection4 = "4\n";
                                        handler_client_server(socket, selection4);
                                        break;
                                }
                        case 5:
                                {
                                        char *selection5 = "5\n";
                                        copy_contents(socket, selection5);
                                        break;
                                }
                        case 6:
                                {
                                        char msg[10] = "6\n";
                                        size_t size = sizeof(msg) + 1;
                                        send_msg(socket, msg, size);
                                        printf(" DISCONNECTING FROM SERVER. JUST A MOMENT...\n");
                                        sleep(2);
                                        printf(" CLIENT DISCONNECTED.\n");
                                        exit(1);
                                }
                        default:
                                printf(" WRONG INPUT. ENTER A VALID OPTION.\n");
                }
        } while (1);

}//END OF MENU OPTIONS

//CHECKING USER INPUT AND CONVERTING STRINGS TO INT THROUGH SKIPPING WHITE SPACES TO GET TO A CHATACTER

int input_check(){
	int size, k;
        char input[5];

        scanf("%s", input);
        size = strlen(input);
        for (k = 0; k < size; k++)
                if (!isdigit(input[k]))
                {
                        return 0;
                }
        int selection = atoi(input);
        return (selection);


}//END OF INPUT CHECK

//REDING SERVER RESPONSE AND SENDING MESSAGE TO THE CLIENT
void handler_client_server(int socket, char *msg)
{

        size_t size = strlen(msg) + 1;
        send_msg(socket, msg, size);
        char resp[1024] = { 0 };
        size_t size1;
        int error0 =
                readn(socket, (unsigned char *) &size1, sizeof(size_t));
        if (error0 < 0)
        {
                perror(" FAILURE\n");
        }
        int error1 =
                readn(socket, (unsigned char *) resp, size1);
        if (error1 < 0)
        {
                perror(" FAILURE\n");
        }
        printf("\n RECEIVED: %zu BYTES\n\n", size1);
        printf(" RESPONSE:\n%s\n\n", resp);

}//END OF CLIENT SERVER HANDLER


//SENDING A MESSAGE THROUGH THE SOCKET 
void send_msg(int socket, char *msg, size_t size)
{
        int error =
                writen(socket, (unsigned char *) &size, sizeof(size_t));
        if (error < 0)
        {
                perror(" FAILED TO SEND MESSAGE\n");
                exit(EXIT_FAILURE);
        }
        int error1 =
                writen(socket, (unsigned char *) msg, size);
        if (error1 < 0)
        {
                perror(" FAILED TO SEND MESSAGE\n");
                exit(EXIT_FAILURE);
        }
}//END OF SEND MESSAGE

//GETTING SYSTEM INFORMATION
void getsystem_info(int socket, char *msg)
{
        size_t s = strlen(msg) + 1;
        send_msg(socket, msg, s);
        printf(" REQUESTING INFO FROM SERVER...\n");
        uts *structure = (uts*) malloc(sizeof(uts));
        size_t payload_length;
        size_t m = readn(socket, (unsigned char *) &payload_length, sizeof(size_t));
        printf(" RECEIVED: %zu (%zu BYTES)\n\n", payload_length, m);
        m = readn(socket, (unsigned char *) structure, payload_length);

        struct utsname uts;
        if (uname(&uts) == -1)
        {
                perror("UNAME ERROR");
                exit(EXIT_FAILURE);
        }
        printf("  NODE NAME:  %s\n", uts.nodename);
        printf("SYSTEM NAME:  %s\n", uts.sysname);
        printf("    RELEASE:  %s\n", uts.release);
        printf("    VERSION:  %s\n", uts.version);

}//END OF GETTING SYSTEM INFO

//IMPLEMENTING A SIGNAL HANDLER TO GRACEFULLY EXIT, CLEAN AND DISPLAY THE TOTAL SERVER EXECUTION TIME ON RECEIPT OF CTR + C OR SIGINT

void signal_handler(int signal)
{
        if (signal == SIGINT)
        {
                gettimeofday(&tv1, NULL);
                char msg[10] = "0\n";
                size_t i = sizeof(msg) + 1;
                printf("\n\n DISCONNECTING FROM SERVER. JUST A MOMENT... \n");
                sleep(3);
                send_msg(connfd, msg, i);
                printf(" EXECUTION TIME: %.2f SECONDS\n", difftime(tv1.tv_sec, tv.tv_sec));
                close(connfd);
                exit(0);
        }
}//END OF SIGNAL HANDLER

//COPYING THE CONTENTS OF A FILE SPECIFIED BY CLIENT USER FROM THE SERVER "UPLOAD" DIRECTORY TO THE CLIENT'S EXECUTABLE CURRENT WORKING DIRECTORY VIA SOCKET

void copy_contents(int socket, char *msg){


        char response[1024] = { 0 };

        size_t size = strlen(msg) + 1;
        send_msg(socket, msg, size);
        printf("REQUESTING INFORMATION FROM SERVER...\n");

        char fileName[80];
        printf("\nEnter file name:\n");
        scanf("%s", fileName);

        size_t size2 = strlen(fileName) + 1;
        send_msg(socket, fileName, size2);

        size_t size3;
        int err10 =
                readn(socket, (unsigned char *) &size3, sizeof(size_t));
        if (err10 < 0)
        {
                perror(" FAILURE\n");
        }
        int err11 =
                readn(socket, (unsigned char *) response, size3);
        if (err11 < 0)
        {
                perror(" FAILURE\n");
        }
        printf("\n RECEIVED: %zu BYTES\n", size3);

        int request = atoi(response);
        if (request == 1)
        {
                printf(" FILE HAS BEEN FOUND\n");
                char recvBuff[256];
                FILE * fp;
                fp = fopen(fileName, "w");
                if (fp == NULL)
                {
                        printf(" UNABLE TO OPEN FILE");
                }
                int data = 0;
                fseek(fp, 0, SEEK_END);
                int fs = ftell(fp);
                printf(" FILE SIZE IS: %d\n", fs);

                while ((data = recv(socket, recvBuff, 256, 0)))
                {
                        fprintf(fp, "%s", recvBuff);
                        bzero(recvBuff, 256);
                        if (data <= 256)
                        {
                                break;
                        }
                        if (fs == 0)
                        {
                                return;
                        }
                }
                fclose(fp);
                printf(" THE FILE %s WAS COPIED\n", fileName);
        }
        else if (request == 2)
        {
                perror(" THIS FILE DOES NOT EXIST\n");
        }
        else
        {
                FILE * fp;
                fp = fopen(fileName, "w");
                if (fp == NULL)
                {
                        printf(" UNABLE TO OPEN FILE");
                }
                fprintf(fp, " ");
                fclose(fp);
                printf(" FILE %s HAS BEEN CREATED. REQUESTED FILE IS EMPTY.\n", fileName);
        

	}
}//END OF COPY CONTENTS







