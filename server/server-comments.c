// Cwk2: server.c - multi-threaded server using readn() and writen()

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <dirent.h>
#include <sys/utsname.h>
#include "rdwrn.h"
#include <sys/time.h>

// THREAD FUNCTIONS
void *client_handler(void *);
void get_student_details(int);
void random_number(int);
void get_system_details(int);
void upload_list(int);
void copy_contents(int);
void signal_handler(int signal);
int get_random_number(int, int);
void send_msg(int, char *, size_t);


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


//DEFINING GLOBAL VARIABLES
char *address;
int connfd;


/*
DISPLAYING THE STUDENT DETAILS BEFORE STARTING THE SERVER
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
        printf("\n SERVER STARTING...\n");
	printf("\n \e[1mCLIENT: JUST A MOMENT BEFORE CONNECTING.\e[0m\n");

}//END OF STUDENT DETAILS


//

int main(void)
{
        get_studentDetails();
        sleep(5);
        printf("\e[1;1H\e[2J");
        printf("\e[96m");

       	// CHECK THE TOTAL ELAPSED TIME SINCE THE CONNECTION TO THE SERVER WAS MADE
        gettimeofday(&tv, NULL);
        struct sigaction sa;

       	// SETTING UP THE SIGNAL HANDLER
        sa.sa_handler = &signal_handler;
        if (sigaction(SIGINT, &sa, NULL) == -1)
        {
                perror(" Error: CANNOT HANDLE SIGINT");
        }

        int listenfd = 0, connfd = 0;

        struct sockaddr_in serv_addr;
        struct sockaddr_in client_addr;
        socklen_t socksize = sizeof(struct sockaddr_in);
        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        memset(&serv_addr, '0', sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(50031);

        bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

        if (listen(listenfd, 10) == -1)
        {
                perror(" FAILED TO LISTEN");
                exit(EXIT_FAILURE);
        }

       	// ACCEPTING INCOMING CLIENT CONNECTION
        puts(" WAITING FOR INCOMING CONNECTIONS...\n");
        while (1)
        {
                printf(" CLIENT CAN CONNECT...\n");
                connfd = accept(listenfd, (struct sockaddr *) &client_addr, &socksize);
                printf(" CONNECTION ACCEPTED...\n");
               	// CLIENT ADDRESS ASSIGNED TO GLOBAL VARIABLE TO BE ACCESSIBLE
                address = inet_ntoa(client_addr.sin_addr);
                pthread_t sniffer_thread;

                if (pthread_create(&sniffer_thread, NULL, client_handler, (void*) &connfd) < 0)
                {
                        perror(" COULD NOT CREATE THREAD\n");
                        exit(EXIT_FAILURE);
                }
               	
                printf(" HANDLER ASSIGNED\n");
        }
        exit(EXIT_SUCCESS);

}//END OF MAIN

//CLIENTHANDLER 
void *client_handler(void *socket_desc)
{

        int connfd = *(int*) socket_desc;
        do {
                char resp[1024] = { 0 };
                printf("\n WAITING FOR A REQUEST FROM THE CLIENT...\n");
                size_t size;
                int error =
                        readn(connfd, (unsigned char *) &size, sizeof(size_t));
                if (error < 0)
                {
                        perror(" CLIENT READ FAILED\n");
                }
                int error1 =
                        readn(connfd, (unsigned char *) resp, size);
                if (error1 < 0)
                {
                        perror(" CLIENT READ FAILED\n");
                }
                printf("\n RECEIVED: %zu BYTES\n", size);
                printf(" PROCESSING OPTION: %s\n", resp);
                int request = atoi(resp);
                switch (request)
                {
                        case 1:
                                {
                                        get_student_details(connfd);
                                        break;
                                }
                        case 2:
                                {
                                        random_number(connfd);
                                        break;
                                }
                        case 3:
                                {

                                        get_system_details(connfd);
                                        break;
                                }
                        case 4:
                                {
                                        upload_list(connfd);
                                        break;
                                }
                        case 5:
                                {
                                        copy_contents   (connfd);
                                        break;
                                }
                        case 6:
                                {
                                        break;
                                }
                        default:
                                return 0;
                }
        } while (1);
}//END OF CASE HANDLER

//SENDING A MESSAGE THROUGH THE SOCKET

void send_msg(int socket, char *msg, size_t size)
{
        int error =
                writen(socket, (unsigned char *) &size, sizeof(size_t));
        if (error < 0)
        {
                perror(" CLIENT WRITE FAILED\n");
                exit(EXIT_FAILURE);
        }
        int error1 =
                writen(socket, (unsigned char *) msg, size);
        if (error1 < 0)
        {
                perror(" CLIENT WRITE FAILED\n");
                exit(EXIT_FAILURE);
        }

}//END OF SEND MESSAGE

//SENDING A STRING TO CLIENT CONTAINING THE STUDENT DETAILS WHILE ACCESSING THE IP ADDRESS THROUGH THE GLOBAL VARIABLE 'ADDRESS' 	

void get_student_details(int socket)
{
	char msg[100] = "\n NAME           : MAINA MARGARET \n STUDENT_ID     : S1906597 \n IP ADDRESS     : ";
        strcat(msg, address);
        size_t size = strlen(msg) + 1;
        send_msg(socket, msg, size);
        printf(" COMPLETED\n");

}//END OF STUDENT DETAILS


//GETTING 5 RANDOM NUMBERS IN THE RANGE OF 1-1000   
void random_number(int socket)
{
        int num;
        char msg[50];
        char num_str[10];
        strcpy(msg, " 5 RANDOM VALUES ARE: \n");
        int i;
        for (i = 0; i < 5; i++)
        {
                num = rand() % 1001;
                sprintf(num_str, " %d ", num);
                strcat(msg, num_str);
        }

        size_t size = strlen(msg) + 1;
        send_msg(socket, msg, size);
        printf(" REQUEST COMPLETED\n");

}//END OF GETTING RANDOM NUMBERS


//DISPLAYING OF SYSTEM DETAILS BY MAKING USE OF THE UTSNAME STRUCTURE AND DISPLAYING THE STRUCTURE USING THE UNAME FUNCTION

void get_system_details(int socket)
{
	struct utsname systemInfo;
        uname(&systemInfo);
        uts * structure;
        structure = (uts*) malloc(sizeof(uts));
        strcpy(structure->sysname, systemInfo.sysname);
        strcpy(structure->release, systemInfo.release);
        strcpy(structure->version, systemInfo.version);
        size_t payload_length = sizeof(uts);

        int error =
                writen(socket, (unsigned char *) &payload_length, sizeof(size_t));
        if (error < 0)
        {
                perror(" CLIENT FAILED\n");
                exit(EXIT_FAILURE);
        }
        int error1 =
                writen(socket, (unsigned char *) structure, payload_length);
        if (error1 < 0)
        {
                perror(" CLIENT FAILED\n");
                exit(EXIT_FAILURE);
        }
        printf(" REQUEST COMPLETED\n");
        free(structure);

}//END OF GET SYSTEM DETAILS

//THE SIGNAL HANDLER IS TRIGGERED BY CTRL+C WHICH PRINTS THE EXECUTION TIME AND STORES IT.THE SERVER THEN CLEANS UP AND EXITS GRACEFULLY

void signal_handler(int signal)
{
        if (signal == SIGINT)
                gettimeofday(&tv1, NULL);
        printf("\n\n SHUTTING DOWN THE SERVER. PLEASE WAIT... \n");
        sleep(3);
        printf(" EXECUTION TIME: %.2f SECONDS\n", difftime(tv1.tv_sec, tv.tv_sec));
        printf(" SERVER HAS SHUT DOWN SUCCESSFULLY\n");
        shutdown(connfd, SHUT_RDWR);
        close(connfd);
        exit(0);

}//END OF SIGNALHANDLER

/*
CHECKING THE FILES IN THE THE UPLOAD DIRECTORY,A LIST STRUCT IS CREATED, WHICH CHECKS FOR THE DIRECTORY AND CHECKS IF THERE ARE FILES IN THE UPLOAD DIRECTORY
IF IT CONTAINS FILES, STRCAT IS USED TO GET THE FILE NAMES AND THEY ARE SENT AS STRINGS 
*/

void upload_list(int socket){
char msg[1024] = " UPLOAD DIRECTORY FILES: \n";

        struct dirent **list;
        int dir;

        if ((dir = scandir("./upload", &list, NULL, alphasort)) == -1)
        {
                char error[100] = "<UPLOAD> DIRECTORY DOES NOT EXIST.\n DIRECTORY IS BEING CREATED...";
                printf(" DIRECTORY HAS BEEN CREATED..\n");
                size_t size = strlen(error) + 1;
                send_msg(socket, error, size);
                mkdir("upload", 0700);
        }
        else
        {
                if (chdir("./upload") == -1)
                        perror(" FAILED TO LOCATE<UPLOAD> DIRECTORY.");
                while (dir--)
                {
                        struct stat sb;

                        if (stat(list[dir]->d_name, &sb) == -1)
                        {
                                perror("stat");
                                exit(EXIT_FAILURE);
                        }
                        if ((sb.st_mode &S_IFMT) == S_IFREG)
                        {
                                strcat(msg, list[dir]->d_name);
                                strcat(msg, "\n");
                        }

                        free(list[dir]);
                }
                chdir("..");
                free(list);
                size_t m = strlen(msg) + 1;
                send_msg(socket, msg, m);
        }

        printf(" REQUEST COMPLETED\n");

}//END OF UPLOAD FILES

/*
THIS FUNCTION RECEIVES THE FILE NAME TO BE COPIED AND VERIFIES IF THE FILE EXISTS , OPENS THE FILE AND READS THE CONTENTS,THE CONTENT IS SENT TO THE CLIENT WHERE A NEW FILE WITH THE SAME FILE IS CREATED IN THE CLIENT'S DIRECTORY
*/
void copy_contents(int socket)
{
        char response[1024] = { 0 };
        size_t size;
        int error =
                readn(socket, (unsigned char *) &size, sizeof(size_t));
        if (error < 0)
        {
                perror(" CLIENT FAILED\n");
        }
        int error1 =
                readn(socket, (unsigned char *) response, size);
        if (error1 < 0)
        {
                perror(" FAILED\n");
        }
        printf("\n RECEIVED: %zu BYTES\n", size);

        char filename[200] = "./upload/";
        strcat(filename, response);

        FILE * fp;
        fp = fopen(filename, "r");

        if ((fp = fopen(filename, "r")))
        {
                printf(" FILE FOUND\n");
                char *msg = "1\n";
                size_t size1 = strlen(msg) + 1;
                send_msg(socket, msg, size1);

                while (1)
                {
                        unsigned char buff[256] = { 0 };
                        int nread = fread(buff, 1, 256, fp);

                        fseek(fp, 0, SEEK_END);
                        int fs = ftell(fp);
                        printf(" FILE SIZE IS: %d\n", fs);

                        if (nread > 0)
                        {
                                write(socket, buff, nread);
                        }

                        if (nread < 256)
                        {
                                if (feof(fp))
                                        if (ferror(fp))
                                                printf(" ERROR reading file\n");
                                break;
                        }
                }
                printf(" PROCESSING REQUESTED FILE\n");
                fclose(fp);
                printf(" FILE SENT \n");
        }
        else
        {
                printf(" THE FILE<%s > DOES NOT EXIST\n", filename);
                char *msg1 = "2\n";
                size_t size2 = strlen(msg1) + 1;
                send_msg(socket, msg1, size2);
                return;
        }
        fseek(fp, 0, SEEK_END);
        int fs = ftell(fp);
        while (fs == 0)
        {
                if ((fp = fopen(filename, "r")))
                {
                        break;
                }
        }

        char *msg3 = "3\n";
        size_t size3 = strlen(msg3) + 1;
        send_msg(socket, msg3, size3);
}//END OF COPY CONTENTS








