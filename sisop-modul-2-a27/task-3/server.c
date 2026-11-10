#include "common.h"
#include <time.h>

char board_p1[4][4], board_p2[4][4];
int ships_p1 = 0, ships_p2 = 0;
mqd_t q_pub, q_p1_tx, q_p1_rx, q_p2_tx, q_p2_rx;

void init_boards() {
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) { board_p1[i][j] = ' '; board_p2[i][j] = ' '; }
    }
}

void cleanup() {
    mq_close(q_pub); mq_close(q_p1_tx); mq_close(q_p1_rx);
    mq_close(q_p2_tx); mq_close(q_p2_rx);
    mq_unlink(PUB_Q); mq_unlink(P1_TX); mq_unlink(P1_RX);
    mq_unlink(P2_TX); mq_unlink(P2_RX);
}

int main() {
    cleanup(); 
    init_boards(); 
    srand(time(NULL));
    
    struct mq_attr attr = {.mq_flags = 0, .mq_maxmsg = 10, .mq_msgsize = sizeof(Message), .mq_curmsgs = 0};

    q_pub = mq_open(PUB_Q, O_CREAT | O_RDWR, 0666, &attr);
    q_p1_tx = mq_open(P1_TX, O_CREAT | O_RDWR, 0666, &attr);
    q_p1_rx = mq_open(P1_RX, O_CREAT | O_RDWR, 0666, &attr);
    q_p2_tx = mq_open(P2_TX, O_CREAT | O_RDWR, 0666, &attr);
    q_p2_rx = mq_open(P2_RX, O_CREAT | O_RDWR, 0666, &attr);

    printf("[SERVER] Game Master Battleship dimulai (4x4 Sederhana).\n");
    Message msg;
    
    // -- KONEKSI --
    printf("[SERVER] Menunggu Pemain 1...\n");
    mq_receive(q_pub, (char*)&msg, sizeof(Message), NULL);
    msg.type = MSG_ASSIGN_P1; mq_send(q_p1_rx, (char*)&msg, sizeof(Message), 0);
    msg.type = MSG_WAIT; mq_send(q_p1_rx, (char*)&msg, sizeof(Message), 0);
    printf("[SERVER] Pemain 1 terhubung.\n");

    printf("[SERVER] Menunggu Pemain 2...\n");
    mq_receive(q_pub, (char*)&msg, sizeof(Message), NULL);
    msg.type = MSG_ASSIGN_P2; mq_send(q_p2_rx, (char*)&msg, sizeof(Message), 0);
    printf("[SERVER] Pemain 2 terhubung.\n");
    printf("[SERVER] Kedua pemain terhubung. Permainan dimulai!\n");

    msg.type = MSG_START_SETUP;
    mq_send(q_p1_rx, (char*)&msg, sizeof(Message), 0);
    mq_send(q_p2_rx, (char*)&msg, sizeof(Message), 0);
    
    // -- PENEMPATAN ARMADA --
    printf("[SERVER] Pemain 1 sedang menempatkan armada...\n");
    int p1_ready = 0;
    while(p1_ready < SHIPS_PER_PLAYER) {
        mq_receive(q_p1_tx, (char*)&msg, sizeof(Message), NULL);
        if(board_p1[msg.r][msg.c] == 'S') {
            msg.type = MSG_PLACE_FAIL; mq_send(q_p1_rx, (char*)&msg, sizeof(Message), 0);
        } else {
            board_p1[msg.r][msg.c] = 'S'; p1_ready++; ships_p1++;
            msg.type = MSG_PLACE_OK; mq_send(q_p1_rx, (char*)&msg, sizeof(Message), 0);
        }
    }
    msg.type = MSG_SETUP_WAIT; mq_send(q_p1_rx, (char*)&msg, sizeof(Message), 0);
    
    printf("[SERVER] Pemain 2 sedang menempatkan armada...\n");
    int p2_ready = 0;
    while(p2_ready < SHIPS_PER_PLAYER) {
        mq_receive(q_p2_tx, (char*)&msg, sizeof(Message), NULL);
        if(board_p2[msg.r][msg.c] == 'S') {
            msg.type = MSG_PLACE_FAIL; mq_send(q_p2_rx, (char*)&msg, sizeof(Message), 0);
        } else {
            board_p2[msg.r][msg.c] = 'S'; p2_ready++; ships_p2++;
            msg.type = MSG_PLACE_OK; mq_send(q_p2_rx, (char*)&msg, sizeof(Message), 0);
        }
    }
    msg.type = MSG_SETUP_WAIT; mq_send(q_p2_rx, (char*)&msg, sizeof(Message), 0);
    printf("[SERVER] Penempatan armada selesai.\n");

    // -- GAMEPLAY --
    msg.type = MSG_GAME_START;
    mq_send(q_p1_rx, (char*)&msg, sizeof(Message), 0); 
    mq_send(q_p2_rx, (char*)&msg, sizeof(Message), 0);

    int turn = rand() % 2 + 1;
    printf("[SERVER] Dipilih secara acak: Pemain %d bermain duluan.\n", turn);

    while(ships_p1 > 0 && ships_p2 > 0) {
        mqd_t q_tx_active = (turn == 1) ? q_p1_tx : q_p2_tx;
        mqd_t q_rx_active = (turn == 1) ? q_p1_rx : q_p2_rx;
        mqd_t q_rx_enemy  = (turn == 1) ? q_p2_rx : q_p1_rx;
        char (*enemy_board)[4] = (turn == 1) ? board_p2 : board_p1;
        int *enemy_ships = (turn == 1) ? &ships_p2 : &ships_p1;

        msg.type = MSG_TURN_START; mq_send(q_rx_active, (char*)&msg, sizeof(Message), 0);
        
        mq_receive(q_tx_active, (char*)&msg, sizeof(Message), NULL);
        
        char col_c = msg.c + 'A'; int is_hit = 0;
        if(enemy_board[msg.r][msg.c] == 'S') { 
            enemy_board[msg.r][msg.c] = 'X'; (*enemy_ships)--; is_hit = 1; 
        } else if (enemy_board[msg.r][msg.c] == ' ') { 
            enemy_board[msg.r][msg.c] = 'O'; 
        }

        printf("[GILIRAN] Pemain %d menembak %d%c: %s\n", turn, msg.r, col_c, is_hit ? "KENA" : "MELESET");
        
        msg.type = MSG_SHOOT_RES_ME;
        sprintf(msg.text, "%d%c %s, SISA %d KAPAL LAGI", msg.r, col_c, is_hit ? "KENA KAPAL" : "MELESET", *enemy_ships);
        mq_send(q_rx_active, (char*)&msg, sizeof(Message), 0);
        
        msg.type = MSG_SHOOT_RES_ENEMY;
        sprintf(msg.text, "Lawan menembak %d%c: %s, SISA %d KAPAL LAGI", msg.r, col_c, is_hit ? "KENA KAPAL" : "MELESET", *enemy_ships);
        mq_send(q_rx_enemy, (char*)&msg, sizeof(Message), 0);

        if(*enemy_ships == 0) {
            printf("[TENGGELAM] Pemain %d menenggelamkan Kapal Pemain %d!\n", turn, (turn==1)?2:1);
            printf("[SERVER] Pemain %d menang!\n", turn);
            msg.type = MSG_GAME_OVER_WIN; mq_send(q_rx_active, (char*)&msg, sizeof(Message), 0);
            msg.type = MSG_GAME_OVER_LOSE; mq_send(q_rx_enemy, (char*)&msg, sizeof(Message), 0);
            break;
        }
        turn = (turn == 1) ? 2 : 1;
    }

    printf("[SERVER] Pembersihan selesai. Sampai jumpa.\n");
    cleanup(); 
    return 0;
}