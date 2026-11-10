#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <pthread.h>
#include <stdbool.h>
#include <ctype.h>

#define PUB_Q "/bs_pub_v12"
#define P1_TX "/bs_p1tx_v12"
#define P1_RX "/bs_p1rx_v12"
#define P2_TX "/bs_p2tx_v12"
#define P2_RX "/bs_p2rx_v12"

#define SHIPS_PER_PLAYER 2

typedef enum {
    MSG_CONNECT,
    MSG_ASSIGN_P1,
    MSG_ASSIGN_P2,
    MSG_WAIT,
    MSG_START_SETUP,
    MSG_PLACE_REQ,
    MSG_PLACE_OK,
    MSG_PLACE_FAIL,
    MSG_SETUP_WAIT,
    MSG_GAME_START,
    MSG_TURN_START,
    MSG_SHOOT_REQ,
    MSG_SHOOT_RES_ME,
    MSG_SHOOT_RES_ENEMY,
    MSG_GAME_OVER_WIN,
    MSG_GAME_OVER_LOSE
} MsgType;

typedef struct {
    MsgType type;
    int pid;
    int r, c;
    char text[128];
} Message;

#endif