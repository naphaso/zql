#include "Worker.h"

#include "Database.h"

pthread_handler_t start_worker(void *ptr) {
	Worker *worker = static_cast<Worker *>(ptr);
	worker->run();
	return 0;
}

Worker::Worker(ZqlDaemon *daemon, int number) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	_daemon = daemon;
	_number = number;

	if(pthread_create(&_thread, &attr, start_worker, static_cast<void *>(this)) != 0) {
		fprintf(stderr, "Could not create worker thread: %s\n", strerror(errno));
		exit(0);
	}

	pthread_attr_destroy(&attr);
}

Worker::~Worker() {
	void *dummy_retval;
	pthread_cancel(_thread);
	pthread_join(_thread, &dummy_retval);
}

void Worker::run() {
	_socket = zmq_socket(_daemon->getContext(), ZMQ_REP);
	
	zmq_connect(_socket, "inproc://zql");

	while(1) {
		zmq_msg_t message;
		zmq_msg_init(&message);
		zmq_msg_recv(&message, _socket, 0);

		//fprintf(stderr, "worker %d received message: %.*s\n",			\
			//		_number, zmq_msg_size(&message), zmq_msg_data(&message));

		zmq_msg_close(&message);

		zmq_send(_socket, "response!", 9, 0); 
	}

	zmq_close(_socket);
}
