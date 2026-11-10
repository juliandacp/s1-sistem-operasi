#include "common.h"

typedef struct {
    char status;
    int received;
} TrafficData;

int main() {
    mqd_t server_q, client_q;
    struct mq_attr attr;
    Message msg;
    TrafficData traffic[4];
    int total_received = 0;
    int high_count = 0, low_count = 0;
    int sensor1_data_count = 0;
    int sensor2_data_count = 0;
    int sensor1_connected = 0, sensor2_connected = 0;
    int running = 1;
    int i;

    for (i = 0; i < 4; i++) {
        traffic[i].received = 0;
        traffic[i].status = '?';
    }

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MSGS;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    mq_unlink(SERVER_QUEUE);
    mq_unlink(CLIENT_QUEUE);

    server_q = mq_open(SERVER_QUEUE, O_CREAT | O_RDONLY, 0666, &attr);
    if (server_q == -1) {
        perror("mq_open server gagal");
        return 1;
    }
    client_q = mq_open(CLIENT_QUEUE, O_CREAT | O_WRONLY, 0666, &attr);
    if (client_q == -1) {
        perror("mq_open client gagal");
        return 1;
    }

    printf("[SERVER] Waiting for sensors...\n");

    while (sensor1_connected == 0 || sensor2_connected == 0) {
        if (mq_receive(server_q, (char*)&msg, sizeof(Message), NULL) != -1) {
            if (msg.mtype == MSG_DATA && msg.location == '\0') {
                if (msg.sensor_id == SENSOR1_ID && !sensor1_connected) {
                    sensor1_connected = 1;
                    printf("[SERVER] Sensor 1 connected\n");
                } else if (msg.sensor_id == SENSOR2_ID && !sensor2_connected) {
                    sensor2_connected = 1;
                    printf("[SERVER] Sensor 2 connected\n");
                }
            }
        }
    }

    printf("[SERVER] System ready!\n");
    printf("[SERVER] Waiting for traffic data...\n\n");

    while (running && total_received<4) {
        if (mq_receive(server_q, (char*)&msg, sizeof(Message), NULL) != -1) {

            if (msg.mtype == MSG_EXIT) {
                printf("[SERVER] Exit signal received from Sensor %d\n", msg.sensor_id);
                printf("[SERVER] Shutting down system...\n");

                Message shutdown_msg;
                shutdown_msg.mtype = MSG_SHUTDOWN;
                for (i = 0; i < 2; i++) {
                    mq_send(client_q, (char*)&shutdown_msg, sizeof(Message), 0);
                    printf("[SERVER] Shutdown sent to sensor %d\n", i+1);
                }
		printf("[SERVER] Cleaning up message queue...\n");

                running = 0;
                break;
            }

            if (msg.mtype == MSG_DATA && msg.location != '\0' && total_received < 4) {
                int index = -1;
                switch (msg.location) {
                    case 'A': index = 0; break;
                    case 'B': index = 1; break;
                    case 'C': index = 2; break;
                    case 'D': index = 3; break;
                    default: continue;
                }

                if (!traffic[index].received) {
                    traffic[index].status = msg.status;
                    traffic[index].received = 1;
                    total_received++;

                    if (msg.sensor_id == SENSOR1_ID) {
                        sensor1_data_count++;
                    } else {
                        sensor2_data_count++;
                    }

                    printf("[SERVER] Received: Sensor %d -> %c : %c\n", msg.sensor_id, msg.location, msg.status);
                } else {
                    printf("[SERVER] Warning: Duplicate data for location %c ignored\n", msg.location);
                }
            }
        }
    }

    if (total_received == 4) {
        printf("\n[SERVER] Received data:\n");
        printf("A: %c\n", traffic[0].status);
        printf("B: %c\n", traffic[1].status);
        printf("C: %c\n", traffic[2].status);
        printf("D: %c\n", traffic[3].status);

        high_count = 0;
        low_count = 0;
        for (i = 0; i < 4; i++) {
            if (traffic[i].status == 'H') {
                high_count++;
            } else if (traffic[i].status == 'L') {
                low_count++;
            }
        }

        printf("\n[SERVER] Summary: \n");
        printf("High Traffic: %d\n", high_count);
        printf("Low Traffic: %d\n", low_count);

        char *status_text;
        char status_code;

        if (high_count >= 3) {
            status_text = "MACET TOTAL";
            status_code = '3';
        } else if (high_count == 2) {
            status_text = "PADAT";
            status_code = '2';
        } else {
            status_text = "LANCAR";
            status_code = '1';
        }
        printf("\n[SERVER] City Status: %s\n", status_text);

        Message status_msg;
        status_msg.mtype = MSG_STATUS;
        status_msg.status = status_code;
        status_msg.sensor_id = 0;

        for (i = 0; i < 2; i++) {
            if (mq_send(client_q, (char*)&status_msg, sizeof(Message), 0) == 0) {
                printf("[SERVER] Status sent to sensor %d\n", i+1);
            } else {
                perror("mq_send status gagal");
            }
        }
    }
    printf("[SERVER] Done. \n");
    mq_close(server_q);
    mq_close(client_q);
    mq_unlink(SERVER_QUEUE);
    mq_unlink(CLIENT_QUEUE);

    return 0;
}
