#include <signal.h>
#include <stdio.h>
#include <string.h>

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
	printf("%s\n", message->payload);
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

