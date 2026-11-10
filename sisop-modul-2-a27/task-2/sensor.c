#include <pthread.h>
#include "common.h"
#include <unistd.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
int running = 1;
int sensor_id;
pthread_t main_thread;

void safe_print(const char *str){
pthread_mutex_lock(&print_mutex);
printf("%s", str);
fflush(stdout);
pthread_mutex_unlock(&print_mutex);
}

void *listener_thread(void *arg){
mqd_t client_q = *(mqd_t*)arg;
Message msg;
struct timespec ts;
while(running){
clock_gettime(CLOCK_REALTIME, &ts);
ts.tv_sec += 1;
int ret = mq_timedreceive(client_q, (char*)&msg, sizeof(Message), NULL, &ts);
if(ret != -1){
if(msg.mtype == MSG_STATUS){
char buffer[200];
const char *status_str;

if(msg.status == '3') status_str = "MACET TOTAL";
else if(msg.status == '2') status_str = "PADAT";
else status_str = "LANCAR";

snprintf(buffer, sizeof(buffer), "\n[INFO] Current city status: %s\n", status_str);
safe_print(buffer);
}
else if(msg.mtype == MSG_SHUTDOWN){
safe_print("\n[INFO] Another sensor has exited\n");
safe_print("[INFO] Cancelling current input...\n");
safe_print("[INFO] System shutting down...\n");
running=0;
pthread_kill(main_thread, SIGINT);
break;
}
} 
}
return NULL;
}


int main(){
mqd_t server_q, client_q;
Message msg;
static int counter=0;


main_thread= pthread_self();

server_q = mq_open(SERVER_QUEUE, O_WRONLY);
client_q = mq_open(CLIENT_QUEUE, O_RDONLY);

char temp[100];
char success[100];
char prompt[50];
FILE *f= fopen("/tmp/sensor_counter.txt", "r+");
if(f){
fscanf(f, "%d", &counter);
rewind(f);
counter++;
fprintf(f, "%d", counter);
fclose(f);
}
else{
f = fopen("/tmp/sensor_counter.txt", "w");
counter = 1;
fprintf(f, "%d", counter);
fclose(f);
}
sensor_id=counter;

sprintf(temp,"[SENSOR] Starting as Sensor %d\n", sensor_id);
safe_print(temp);

msg.mtype= MSG_DATA;
msg.sensor_id = sensor_id;
msg.location = '\0';
msg.status = '\0';
msg.is_exit = 0;

mq_send(server_q, (char*)&msg, sizeof(Message), 0);
safe_print("[SENSOR] Connection message sent to server\n");
safe_print("[SENSOR] Connected to server\n");

char input[100];
char location, status;

safe_print("\n[SENSOR] Format input: <LOKASI> <STATUS> (contoh: A H)\n");
safe_print("[SENSOR] Ketik 'exit' untuk berhenti\n\n");

pthread_t listener;
pthread_create(&listener, NULL, listener_thread, &client_q);

while(running){
char prompt[50];
sprintf(prompt,"[SENSOR %d] Input: ", sensor_id);
safe_print(prompt);

if(fgets(input, sizeof(input), stdin)==NULL){
if(!running) break;
continue;
}

input[strcspn(input,"\n")]= '\0';

if(strcmp(input, "exit")==0){
safe_print("[SENSOR] Sending exit signal...\n");
msg.mtype = MSG_EXIT;
msg.sensor_id = sensor_id;
msg.is_exit = 1;
mq_send(server_q, (char*)&msg, sizeof(Message), 0);
running = 0;
Message dummy;
dummy.mtype = MSG_SHUTDOWN;
mq_send(client_q, (char*)&dummy, sizeof(Message), 0);
safe_print("[SENSOR] Shutting down...\n");
break;
}
if(sscanf(input, "%c %c", &location, &status)==2){
int valid=0;
if(sensor_id==SENSOR1_ID){
if(location == 'A' || location == 'B') valid =1;
else safe_print("[ERROR] Sensor 1 hanya bisa A atau B!\n");
}
else if(sensor_id == SENSOR2_ID){
if(location=='C' || location == 'D') valid = 1;
else safe_print("[ERROR] Sensor 2 hanya bisa C atau D!\n");
}

if(valid && status != 'L' && status != 'H') {
safe_print("[ERROR] Status harus L atau H!\n");
valid = 0;
}

if(valid){
msg.mtype = MSG_DATA;
msg.sensor_id = sensor_id;
msg.location = location;
msg.status = status;
msg.is_exit = 0;
mq_send(server_q, (char*)&msg, sizeof(Message), 0);
sprintf(success,"[SENSOR] Data sent: %c %c\n", location, status);
safe_print(success);
}
}
else {
safe_print("[ERROR] Format salah! Gunakan: A H\n");
}
}

pthread_cancel(listener);
pthread_join(listener, NULL);

sleep(2);
mq_close(server_q);
mq_close(client_q);
return 0;
}
