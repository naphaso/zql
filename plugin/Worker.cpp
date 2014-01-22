#include "Worker.h"

#include "Database.h"
#include "Cbor.h"
#include "ObjectParser.h"
#include "log.h"

using namespace std;

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
    _disposed = false;

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

	while(!_disposed) {
		zmq_msg_t message;
		zmq_msg_init(&message);
		zmq_msg_recv(&message, _socket, 0);

        fprintf(stderr, "worker %d received message", _number);

        CborInput input(zmq_msg_data(&message), zmq_msg_size(&message));
        ObjectParser parser;
        parser.SetInput(input);
        parser.SetListener(*this);
        parser.Run();

		zmq_msg_close(&message);
		//_database->execute("testdatabase", "testtable", false, "0", 3);
		//zmq_send(_socket, "response!", 9, 0);
	}

	zmq_close(_socket);
}

void Worker::OnRequestGet(unsigned int requestId, RequestGet *request) {
    ResponseWrapper wrapper;
    wrapper.setId(requestId);
    ResponseGetOk responseOk;
    ResponseGetEmpty responseEmpty;
    if(_database->get(request->database(), request->table(), request->pk(), responseOk.GetValues())) {
        wrapper.setResponse(&responseOk);
    } else {
        wrapper.setResponse(&responseEmpty);
    }

    CborOutput output(9000);
    CborWriter writer(output);
    wrapper.Serialize(writer);

    zmq_send(_socket, output.getData(), (size_t) output.getSize(), 0);
}

void Worker::OnResponseGetEmpty(unsigned int requestId) {
    loggerf("%s", "invalid request: response empty");
}

void Worker::OnResponseGetOk(unsigned int requestId, ResponseGetOk *response) {
    loggerf("%s", "invalid request: response ok");
}

void Worker::OnError(const char *error) {
    loggerf("error occured: %s", error);
}

void Worker::OnRequestAdd(unsigned int requestId, RequestAdd *request) {
    ResponseWrapper wrapper;
    wrapper.setId(requestId);

    if(_database->add(request->database(), request->table(), request->row())) {
        //wrapper.setResponse();
    }

    /*
    CborOutput output(9000);
    CborWriter writer(output);
    wrapper.Serialize(writer);

    zmq_send(_socket, output.getData(), (size_t) output.getSize(), 0);
    */

    const string response = "hello";
    zmq_send(_socket, response.c_str(), response.size(), 0);
}
