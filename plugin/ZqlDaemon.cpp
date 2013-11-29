#include "ZqlDaemon.h"

pthread_handler_t daemonize(void *ptr) {
	ZqlDaemon *daemon = static_cast<ZqlDaemon *>(ptr);
	daemon->run();

	return 0;
}

ZqlDaemon::ZqlDaemon() {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	//pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	if(pthread_create(&_thread, &attr, daemonize, static_cast<void *>(this)) != 0) {
		fprintf(stderr, "Could not create zql thread: %s\n", strerror(errno));
		exit(0);
	}

	pthread_attr_destroy(&attr);
}

ZqlDaemon::~ZqlDaemon() {
	void *dummy_retval;
	pthread_cancel(_thread);
	pthread_join(_thread, &dummy_retval);
}

void *ZqlDaemon::getContext() {
	return _context;
}

#define WORKER_COUNT 16

void ZqlDaemon::run() {
	_context = zmq_ctx_new();
	_frontend_socket = zmq_socket(_context, ZMQ_ROUTER);
	_backend_socket = zmq_socket(_context, ZMQ_DEALER);

	zmq_bind(_frontend_socket, "tcp://*:9990");
	zmq_bind(_backend_socket, "inproc://zql");

	Worker *workers[WORKER_COUNT];
	for(int i = 0; i < WORKER_COUNT; i++) {
		workers[i] = new Worker(this, i + 1);
	}

	zmq_proxy(_frontend_socket, _backend_socket, NULL);
	
	zmq_close(_frontend_socket);
	zmq_close(_backend_socket);

	zmq_ctx_destroy(_context);
}
