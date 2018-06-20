#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <mosquitto.h>

#define mqtt_host "127.0.0.1"
#define mqtt_port 1883

static int run = 1;

void handle_signal(int s)
{
		run = 0;
}

void connect_callback(struct mosquitto *mosq, void *obj, int result)
{
//	printf("Enter connect callback\n");
}

void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
//	printf("Enter message_callback\n");
	time_t current_time = time(NULL);
	struct tm *p_tm;
	p_tm = gmtime(&current_time);
	char time_stamp[100] = {0};
	char filepath[200] = {0};
	sprintf(time_stamp, "%d-%d-%d,%d:%d:%d", (p_tm->tm_year+1900), (p_tm->tm_mon+1), p_tm->tm_mday, p_tm->tm_hour+8, p_tm->tm_min, p_tm->tm_sec);
	sprintf(filepath, "alert_%s.bmp", time_stamp);
	FILE *fp = fopen(filepath, "wb");
	if(!fp){
		printf("Fail to open file %s\n", filepath);
		return;
	}

	fwrite(message->payload, 1, message->payloadlen, fp);
	fclose(fp);
	//printf("%s\n", message->payload);
}

int main(int argc, char *argv[])
{
	struct mosquitto *mosq;
	int rc = 0;
	
	if(argc < 2){
		printf("Invalid input\n");
		return -1;
	}

	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);

	mosquitto_lib_init();
	
	mosq = mosquitto_new(NULL, true, NULL);

	if(mosq){
		mosquitto_connect_callback_set(mosq, connect_callback);
		mosquitto_message_callback_set(mosq, message_callback);
		rc = mosquitto_connect(mosq, mqtt_host, mqtt_port, 60);

		mosquitto_subscribe(mosq, NULL, argv[1], 0);

		while(run){
			rc = mosquitto_loop(mosq, -1, 1);
			if(run && rc){
				sleep(20);
				mosquitto_reconnect(mosq);
			}
		}
		mosquitto_destroy(mosq);
	}

	mosquitto_lib_cleanup();
	printf("Thank you\n");

	return 0;
}

