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
	//pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setstacksize(&attr, 256 * 1024);

	_daemon = daemon;
	_number = number;
	_database = new Database();

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
	delete _database;
}

struct thr_init {
	thr_init(Database* database, volatile int& shutdown_flag) : _database(database) {
		_database->initThread(this, shutdown_flag);
	}

	~thr_init() {
		_database->deinitThread();
	}

	Database *_database;
};


void Worker::run() {
	thr_init initobj(_database, _number);

	_socket = zmq_socket(_daemon->getContext(), ZMQ_REP);
	
	zmq_connect(_socket, "inproc://zql");

	while(1) {
		zmq_msg_t message;
		zmq_msg_init(&message);
		zmq_msg_recv(&message, _socket, 0);

		fprintf(stderr, "worker %d received message: %.*s\n",			\
					_number, zmq_msg_size(&message), zmq_msg_data(&message));

		zmq_msg_close(&message);
		
		_database->execute("testdatabase", "testtable", false, 0, 3);


		zmq_send(_socket, "response!", 9, 0); 
	}

	zmq_close(_socket);
}
