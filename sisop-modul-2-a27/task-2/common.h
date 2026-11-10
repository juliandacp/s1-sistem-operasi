#ifndef COMMON_H
#define COMMON_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <mqueue.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

#define SERVER_QUEUE "/stc_server"
#define CLIENT_QUEUE "/stc_client"
#define MAX_MSG_SIZE sizeof(Message)
#define MAX_MSGS 10

#define MSG_DATA 1
#define MSG_EXIT 2
#define MSG_STATUS 3
#define MSG_SHUTDOWN 4

#define SENSOR1_ID 1
#define SENSOR2_ID 2

typedef struct{
long mtype; 
int sensor_id;
char location;
char status;
int is_exit;
} Message;

#endif

