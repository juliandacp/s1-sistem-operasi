#include "common.h"

mqd_t q_tx, q_rx, q_pub;
int my_id;
char my_board[4][4], enemy_board[4][4];
pthread_mutex_t board_mutex = PTHREAD_MUTEX_INITIALIZER;
volatile int is_my_turn = 0;
volatile int game_running = 1;

void init_boards() {
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) { my_board[i][j] = ' '; enemy_board[i][j] = '?'; }
    }
}

void print_boards() {
    pthread_mutex_lock(&board_mutex);
    printf("\n    Papan Lawan\n  A B C D\n");
    for(int i=0; i<4; i++) {
        printf("%d|", i);
        for(int j=0; j<4; j++) printf("%c|", enemy_board[i][j]);
        printf("\n");
    }
    printf("\n    Papan Anda\n  A B C D\n");
    for(int i=0; i<4; i++) {
        printf("%d|", i);
        for(int j=0; j<4; j++) printf("%c|", my_board[i][j]);
        printf("\n");
    }
    pthread_mutex_unlock(&board_mutex);
}

void* listener_thread(void* arg) {
    Message msg;
    while(game_running) {
        if (mq_receive(q_rx, (char*)&msg, sizeof(Message), NULL) > 0) {
            switch(msg.type) {
                case MSG_TURN_START:
                    is_my_turn = 1;
                    printf("========================================\n[GILIRAN ANDA]\n");
                    print_boards(); 
                    printf("\nTarget: "); fflush(stdout);
                    break;
                case MSG_SHOOT_RES_ME:
                    pthread_mutex_lock(&board_mutex);
                    enemy_board[msg.r][msg.c] = strstr(msg.text, "KENA KAPAL") ? 'X' : ' ';
                    pthread_mutex_unlock(&board_mutex);
                    printf("\n[HASIL TEMBAKAN]\n%s\n========================================\n\n", msg.text);
                    break;
                case MSG_SHOOT_RES_ENEMY:
                    pthread_mutex_lock(&board_mutex);
                    if(strstr(msg.text, "KENA KAPAL")) my_board[msg.r][msg.c] = 'X';
                    pthread_mutex_unlock(&board_mutex);
                    printf("\n[INFO] %s\n", msg.text);
                    break;
                case MSG_GAME_OVER_WIN:
                    printf("========================================\n[HASIL] Semua kapal musuh telah tenggelam!\n[HASIL] ANDA MENANG!\n========================================\n");
                    game_running = 0; break;
                case MSG_GAME_OVER_LOSE:
                    printf("========================================\n[HASIL] Semua kapal Anda telah tenggelam.\n[HASIL] ANDA KALAH.\n========================================\n");
                    game_running = 0; break;
                default: break;
            }
        }
    }
    return NULL;
}

int main() {
    init_boards();
    struct mq_attr attr = {.mq_flags = 0, .mq_maxmsg = 10, .mq_msgsize = sizeof(Message)};
    
    q_pub = mq_open(PUB_Q, O_WRONLY | O_CREAT, 0666, &attr);
    if(q_pub == (mqd_t)-1) { printf("Gagal konek ke server.\n"); return 1; }

    printf("[PEMAIN] Menghubungkan ke server...\n");
    
    // Buka semua kemungkinan queue dengan NONBLOCK untuk nyari ID
    mqd_t q_p1 = mq_open(P1_RX, O_RDONLY | O_CREAT | O_NONBLOCK, 0666, &attr);
    mqd_t q_p2 = mq_open(P2_RX, O_RDONLY | O_CREAT | O_NONBLOCK, 0666, &attr);

    Message msg; msg.type = MSG_CONNECT; msg.pid = getpid();
    mq_send(q_pub, (char*)&msg, sizeof(Message), 0);

    // Loop nunggu Server bales ke queue mana yg sesuai sama PID kita
    while(1) {
        if(mq_receive(q_p1, (char*)&msg, sizeof(Message), NULL) > 0) {
            if(msg.pid == getpid()) { my_id = 1; break; } 
            else { mq_send(q_p1, (char*)&msg, sizeof(Message), 0); }
        }
        if(mq_receive(q_p2, (char*)&msg, sizeof(Message), NULL) > 0) {
            if(msg.pid == getpid()) { my_id = 2; break; } 
            else { mq_send(q_p2, (char*)&msg, sizeof(Message), 0); }
        }
        usleep(10000); 
    }

    if(my_id == 1) {
        q_tx = mq_open(P1_TX, O_WRONLY | O_CREAT, 0666, &attr);
        q_rx = mq_open(P1_RX, O_RDONLY | O_CREAT, 0666, &attr);
        mq_close(q_p1); mq_close(q_p2);
        printf("[PEMAIN 1] Berhasil terhubung!\n");
        mq_receive(q_rx, (char*)&msg, sizeof(Message), NULL); 
        if(msg.type == MSG_WAIT) printf("[SERVER] Menunggu Pemain 2 untuk bergabung...\n");
    } else {
        q_tx = mq_open(P2_TX, O_WRONLY | O_CREAT, 0666, &attr);
        q_rx = mq_open(P2_RX, O_RDONLY | O_CREAT, 0666, &attr);
        mq_close(q_p1); mq_close(q_p2);
        printf("[PEMAIN 2] Berhasil terhubung!\n");
    }

    mq_receive(q_rx, (char*)&msg, sizeof(Message), NULL);
    if(msg.type == MSG_START_SETUP) {
        printf("[SERVER] Permainan dimulai (4x4 Sederhana)!\n");
        print_boards();
        
        printf("\nAnda akan menempatkan %d kapal (masing-masing 1 petak).\n\n", SHIPS_PER_PLAYER);
        for(int i = 1; i <= SHIPS_PER_PLAYER; i++) {
            char input[256]; int r, c;
            while(1) {
                printf("Tempatkan Kapal %d:\n> ", i);
                scanf("%s", input);
                
                r = input[0] - '0'; 
                c = toupper(input[1]) - 'A';
                
                if(r < 0 || r > 3 || c < 0 || c > 3) {
                    printf("Koordinat di luar batas!\n"); continue;
                }
                
                msg.type = MSG_PLACE_REQ; msg.r = r; msg.c = c;
                mq_send(q_tx, (char*)&msg, sizeof(Message), 0);
                
                mq_receive(q_rx, (char*)&msg, sizeof(Message), NULL);
                if(msg.type == MSG_PLACE_OK) {
                    pthread_mutex_lock(&board_mutex);
                    my_board[r][c] = 'S';
                    pthread_mutex_unlock(&board_mutex);
                    printf("Kapal %d ditempatkan.\n", i);
                    break;
                } else {
                    printf("Petak sudah ditempati!\n");
                }
            }
        }
    }

    printf("\n[INFO] Menunggu lawan menyelesaikan penempatan...\n");
    mq_receive(q_rx, (char*)&msg, sizeof(Message), NULL); // nunggu setup wait
    mq_receive(q_rx, (char*)&msg, sizeof(Message), NULL); // nunggu game start
    printf("[INFO] Semua pemain siap.\n\n");

    pthread_t tid;
    pthread_create(&tid, NULL, listener_thread, NULL);

    char input[256]; int r, c;
    while(game_running) {
        if(is_my_turn) {
            scanf("%s", input);
            r = input[0] - '0'; c = toupper(input[1]) - 'A';
            if(r < 0 || r > 3 || c < 0 || c > 3) {
                printf("Koordinat di luar batas!\nTarget: "); continue;
            }
            msg.type = MSG_SHOOT_REQ; msg.r = r; msg.c = c;
            mq_send(q_tx, (char*)&msg, sizeof(Message), 0);
            is_my_turn = 0; 
        } else {
            usleep(100000); 
        }
    }

    pthread_join(tid, NULL);
    mq_close(q_tx); mq_close(q_rx);
    return 0;
}