#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <mosquitto.h>

#define broker_ip "127.0.0.1"
#define broker_port 1883

int main(int argc, char **argv){
	struct mosquitto *mosq = NULL;
	mosq = mosquitto_new (NULL, true, NULL);
	
	if(argc < 3){
		printf("Not valid input\n");
		return -1;
	}

	printf("Topic = %s\n", argv[1]);
	printf("Content = %s\n", argv[2]);

	if (!mosq) {
		printf("Can't initialize Mosquitto library\n");
		return -1;
	}

	if(mosquitto_connect (mosq, broker_ip, broker_port, 0) != MOSQ_ERR_SUCCESS){
		printf("Can't connect to Mosquitto server: %s\n", broker_ip);
		return -1;
	}

	if(mosquitto_publish (mosq , NULL, argv[1], strlen(argv[2]), argv[2], 0, false)){
		printf("Can't publish to broker\n");
		return -1;
	}

	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
}
