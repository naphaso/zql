#include <stdio.h>
#include <string.h>
#include <zmq.h>

int main(int argc, char **argv) {
	if(argc != 2) {
		printf("usage: test [data]\n");
		return 1;
	}

	char *data = argv[1];
	int size = strlen(data);

	void *context = zmq_ctx_new();
	void *socket = zmq_socket(context, ZMQ_REQ);
	zmq_connect(socket, "tcp://127.0.0.1:9990");
	
	//	for(int i = 0; i < 1000000; i++) {
		zmq_send(socket, data, size, 0);
	
		zmq_msg_t message;
		zmq_msg_init(&message);
		zmq_msg_recv(&message, socket, 0);
		
		//		printf("request %d response data: %.*s\n", i, zmq_msg_size(&message), zmq_msg_data(&message));

		zmq_msg_close(&message);

		//if(i % 10000 == 0) {
		//	printf("request %d\n", i);
		//}

		//}

	zmq_close(socket);
	zmq_ctx_destroy(context);
	return 0;
}

