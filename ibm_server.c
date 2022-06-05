
/*** IBMCOPYR ********************************************************/
/*                                                                   */
/* Component Name: MTCSRVR (alias EZAEC047)                          */
/*                                                                   */
/*                                                                   */
/* Copyright:    Licensed Materials - Property of IBM                */
/*                                                                   */
/*               "Restricted Materials of IBM"                       */
/*                                                                   */
/*               5647-A01                                            */
/*                                                                   */
/*               (C) Copyright IBM Corp. 1977, 1998                  */
/*                                                                   */
/*               US Government Users Restricted Rights -             */
/*               Use, duplication or disclosure restricted by        */
/*               GSA ADP Schedule Contract with IBM Corp.            */
/*                                                                   */
/* Status:       CSV2R6                                              */
/*                                                                   */
/*  SMP/E Distribution Name: EZAEC049                                */
/*                                                                   */
/*                                                                   */
/*** IBMCOPYR ********************************************************/

/*********************************************************************/
/* C socket Server Program                                           */
/*                                                                   */
/* This code performs the server functions for multitasking, which   */
/* include                                                           */
/*      . creating subtasks                                          */
/*      . socket(), bind(), listen(), accept()                       */
/*      . getclientid                                                */
/*      . givesocket() to TCP/IP in preparation for the subtask      */
/*                      to do a takesocket()                         */
/*      . select()                                                   */
/*                                                                   */
/* There are three test tasks running:                               */
/*      . server master                                              */
/*      . server subtask - separate TCB within server address space  */
/*      . client                                                     */
/*                                                                   */
/*********************************************************************/

static char ibmcopyr[] =
   "MTCSRVR - Licensed Materials - Property of IBM. "
   "This module is \"Restricted Materials of IBM\" "
   "5647-A01 (C) Copyright IBM Corp. 1994, 1996. "
   "See IBM Copyright Instructions.";

// #include <manifest.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
// #include <inet.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
// #include <tcperrno.h>
// #include <bsdtime.h>
// #include <mtf.h>
#include <stdio.h>

int  dotinit(void);
void getsock(int *s);
int  dobind(int *s, unsigned short port);
int  dolisten(int *s);
int  getname(char *myname, char *mysname);
int  doaccept(int *s);
int  testgive(int *s);
int  dogive(int *clsocket, char *myname);

/*
 * Server Main.
 */
main(argc, argv)
int argc;
char **argv;
{
    unsigned short port;       /* port server for bind                */
    int s;                     /* socket for accepting connections    */
    int rc;                    /* return code                         */
    int count;                 /* counter for number of sockets       */
    int clsocket;              /* client socket                       */
    char myname[8];            /* 8 char name of this addres space    */
    char mysname[8];           /* my subtask name                     */

    /*
     * Check arguments. Should be only one: the port number to bind to.
     */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(1);
    }

    /*
     * First argument should be the port.
     */
    port = (unsigned short) atoi(argv[1]);
    fprintf(stdout, "Server: port = %d \n", port);

    /*
     * Create subtasks
     */
    rc = dotinit();
    if (rc < 0)
        perror("Srvr: error for tinit");
    printf("rc from tinit is %d\n", rc);

    getsock(&s);
    printf("Srvr: socket = %d\n", s);

    rc = dobind(&s, port);
    if (rc < 0)
        tcperror("Srvr: error for bind");
    printf("Srvr: rc from bind is %d\n", rc);

    rc = dolisten(&s);
    if (rc < 0)
        tcperror("Srvr: error for listen");
    printf("Srvr: rc from listen is %d\n", rc);

    /***************************************
     * To do nonblocking mode,
     *  uncomment out this code.
     *
    rc = fcntl(s, F_SETFL, FNDELAY);
    if (rc != 0)
        tcperror("Error for fcntl");
    printf("rc from fcntl is %d\n", rc);

    ***************************************/

    rc = getname(myname, mysname);
    if (rc < 0)
        tcperror("Srvr: error for getclientid");
    printf("Srvr: rc from getclientid is %d\n", rc);

    /*----------------------------------------------------------------*/
    /* . issue accept(), waiting for client connection                */
    /* . issue givesocket() to pass client's socket to TCP/IP         */
    /* . issue select(), waiting for subtask to complete takesocket() */
    /* . close our local socket associated with client's socket       */
    /* . loop on accept(), waiting for another client connection      */
    /*----------------------------------------------------------------*/
    rc    = 0;
    count = 0;          /* number of sockets */
    while (rc == 0) {
        clsocket = doaccept(&s);
        printf("Srvr: clsocket from accept is %d\n", clsocket);
        count = count + 1;
        printf("Srvr: ###number of sockets is %d\n", count);
        if (clsocket != 0) {
            rc = dogive(&clsocket, myname);
            if (rc < 0)
                tcperror("Srvr: error for dogive");
            printf("Srvr: rc from dogive is %d\n", rc);
            if (rc == 0) {
                rc = tsched(MTF_ANY,"csub", &clsocket,
                                myname, mysname);
                if (rc < 0)
                    perror("error for tsched");
                printf("Srvr: rc from tsched is %d\n", rc);

                rc = testgive(&clsocket);
                printf("Srvr: rc from testgive is %d\n", rc);

                sleep(60); /*** do simplified situation first ***/

                printf("Srvr: closing client socket %d\n", clsocket);
                rc = close(clsocket);   /* give back this socket */
                if (rc < 0)
                    tcperror("error for close of clsocket");
                printf("Srvr: rc from close of clsocket is %d\n", rc);
                /****************************************************/
                exit(0); /*** do  this simplified situation first ***/
                /****************************************************/
            } /** end of if (rc == 0)       ****/
        }   /**** end of if (clsocket != 0) ****/
    }   /******** end of while (rc == 0)    ****/
}   /************ end of main           ********/

/*--------------------------------------------------------------------*/
/*    dotinit()                                                       */
/*    Call tinit() to ATTACH subtask and fetch() subtask load module  */
/*--------------------------------------------------------------------*/
int dotinit(void)
{
    int rc;
    int numsubs = 1;
    printf("Srvr: calling __tinit\n");
    rc = __tinit("mtccsub", numsubs);
    return rc;
}

/*--------------------------------------------------------------------*/
/*    getsock()                                                       */
/*    Get a socket                                                    */
/*--------------------------------------------------------------------*/
void getsock(int *s)
{
    int temp;
    temp = socket(AF_INET, SOCK_STREAM, 0);
    *s = temp;
    return;
}

/*--------------------------------------------------------------------*/
/*    dobind()                                                        */
/*    Bind to all interfaces                                          */
/*--------------------------------------------------------------------*/
int dobind(int *s, unsigned short port)
{
    int rc;
    int temps;
    struct sockaddr_in tsock;
    memset(&tsock, 0, sizeof(tsock));   /* clear tsock to 0's */
    tsock.sin_family      = AF_INET;
    tsock.sin_addr.s_addr = INADDR_ANY; /* bind to all interfaces */
    tsock.sin_port        = htons(port);

    temps = *s;
    rc = bind(temps, (struct sockaddr *)&tsock, sizeof(tsock));
    return rc;
}

/*--------------------------------------------------------------------*/
/*    dolisten()                                                      */
/*    Listen to prepare for client connections.                       */
/*--------------------------------------------------------------------*/
int dolisten(int *s)
{
    int rc;
    int temps;
    temps = *s;
    rc = listen(temps, 10);     /* backlog of 10 */
    return rc;
}

/*--------------------------------------------------------------------*/
/*    getname()                                                       */
/*    Get the identifiers by which TCP/IP knows this server.          */
/*--------------------------------------------------------------------*/
int getname(char *myname, char *mysname)
{
    int rc;
    struct clientid cid;
    memset(&cid, 0, sizeof(cid));
    rc = getclientid(AF_INET, &cid);
    memcpy(myname,  cid.name,        8);
    memcpy(mysname, cid.subtaskname, 8);
    return rc;
}

/*--------------------------------------------------------------------*/
/*    doaccept()                                                      */
/*    Select() on this socket, waiting for another client connection. */
/*    If connection is pending, issue accept() to get client's socket */
/*--------------------------------------------------------------------*/
int doaccept(int *s)
{
    int temps;
    int clsocket;
    struct sockaddr clientaddress;
    int addrlen;
    int maxfdpl;
    struct fd_set readmask;
    struct fd_set writmask;
    struct fd_set excpmask;
    int rc;
    struct timeval time;

    temps = *s;
    time.tv_sec  = 1000;
    time.tv_usec = 0;
    maxfdpl = temps + 1;

    FD_ZERO(&readmask);
    FD_ZERO(&writmask);
    FD_ZERO(&excpmask);

    FD_SET(temps, &readmask);

    rc = select(maxfdpl, &readmask, &writmask, &excpmask, &time);
    printf("Srvr: rc from select is %d\n", rc);
    if (rc < 0) {
        tcperror("error from select");
        return rc;
    }
    else if (rc == 0) {  /* time limit expired */
        return rc;
    }
    else {              /* this socket is ready */
        addrlen = sizeof(clientaddress);
        clsocket = accept(temps, &clientaddress, &addrlen);
        return clsocket;
    }
}

/*--------------------------------------------------------------------*/
/*    testgive()                                                      */
/*    Issue select(), checking for an exception condition, which      */
/*    indicates that takesocket() by the subtask was successful.      */
/*--------------------------------------------------------------------*/
int testgive(int *s)
{
    int temps;
    struct sockaddr clientaddress;
    int addrlen;
    int maxfdpl;
    struct fd_set readmask;
    struct fd_set writmask;
    struct fd_set excpmask;
    int rc;
    struct timeval time;

    temps = *s;
    time.tv_sec  = 1000;
    time.tv_usec = 0;
    maxfdpl = temps + 1;

    FD_ZERO(&readmask);
    FD_ZERO(&writmask);
    FD_ZERO(&excpmask);

 /* FD_SET(temps, &readmask); */
 /* FD_SET(temps, &writmask); */
    FD_SET(temps, &excpmask);

    rc = select(maxfdpl, &readmask, &writmask, &excpmask, &time);
    printf("Srvr: rc from select for testgive is %d\n", rc);
    if (rc < 0) {
        tcperror("Srvr: error from testgive");
    }
    else
        rc = 0;

    return rc;
}

/*--------------------------------------------------------------------*/
/*    dogive()                                                        */
/*    Issue givesocket() for giving client's socket to subtask.       */
/*--------------------------------------------------------------------*/
int dogive(int *clsocket, char *myname)
{
    int rc;
    struct clientid cid;
    int temps;

    temps = *clsocket;
    memset(&cid, 0, sizeof(cid));
    cid.domain = AF_INET;

    memcpy(cid.name,       myname,     8);
    memcpy(cid.subtaskname,"        ", 8);
    printf("Srvr: givesocket socket is %d\n", temps);
    printf("Srvr: givesocket name is %s\n", cid.name);

    rc = givesocket(temps, &cid);
    return rc;
}