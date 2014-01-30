#include "Worker.h"

#include "Database.h"
#include "Cbor.h"
#include "ObjectParser.h"
#include "log.h"
#include "BTree.h"

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

        loggerf("worker %d received message", _number);

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

bool hasEnding(std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

string formatPath(unsigned long long path) {
    char buf[25];
    sprintf(buf, "%llu", path);
    return string(buf);
}

void Worker::OnRequestAdd(unsigned int requestId, RequestAdd *request) {
    ResponseWrapper wrapper;
    wrapper.setId(requestId);



    /*
    loggerf("request add %u, database %s, table %s", requestId, request->database().c_str(), request->table().c_str());
    for(map<string, string>::iterator it = request->row().begin(); it != request->row().end(); ++it) {
        loggerf("key '%s', value '%s'", it->first.c_str(), it->second.c_str());
    }
    logger("end request");
*/

    bool is_ope = false;
    string opeField;
    string opeValue;
    for(map<string, string>::iterator it = request->row().begin(); it != request->row().end(); ++it) {
        if(hasEnding(it->first, "_ope")) {
            is_ope = true;
            opeField = it->first;
            opeValue = it->second;
            break;
        }
    }

    ResponseAddOk responseAddOk;
    ResponseAddTraverse responseAddTraverse;

    if(!is_ope) {
        logger("add request is not ope, just insert");
        if(_database->add(request->database(), request->table(), request->row())) {
        }

        wrapper.setResponse(&responseAddOk);
    } else {
        logger("add ope request");

        BTreeNode *tree = BTreeForest::instance()->getTree(request->database(), request->table());
        Ciphertext *ciphertext = new Ciphertext((unsigned char *) opeValue.c_str(), opeValue.size());
        if(tree == NULL) {
            logger("tree not found, creating new...");
            BTreeForest::instance()->createTree(request->database(), request->table(), ciphertext);

            request->row()[opeField] = formatPath(9223372036854775808ull); // 1 << 63

            logger("insert new btree root ope value to table");

            if(_database->add(request->database(), request->table(), request->row())) {

            }

            wrapper.setResponse(&responseAddOk);
        } else {
            logger("tree exists, starting tree traverse");

            BTreeTraverseHolder::instance()->createTraverse(requestId, tree, ciphertext)->requestAdd() = request;

            responseAddTraverse.initRequestId() = requestId;
            responseAddTraverse.ciphertext() = ciphertext;
            wrapper.setResponse(&responseAddTraverse);
        }
    }

    logger("serializing and send response to client...");

    CborOutput output(9000);
    CborWriter writer(output);
    wrapper.Serialize(writer);

    zmq_send(_socket, output.getData(), (size_t) output.getSize(), 0);

    logger("request add processing done");
}


void Worker::OnResponseAddOk(unsigned int requestId) {
    loggerf("unknown response on worker: %u", requestId);
}

void Worker::OnResponseAddTraverse(unsigned int requestId, ResponseAddTraverse *response) {
    loggerf("unknown response on worker: %u", requestId);
    delete response;
}

void Worker::OnRequestAddContinue(unsigned int requestId, RequestAddContinue *request) {
    logger("add continue request start processing...");

    ResponseWrapper wrapper;
    wrapper.setId(requestId);

    logger("wrapper created");

    ResponseAddOk responseAddOk;
    ResponseAddTraverse responseAddTraverse;

    BTreeTraverse *traverse = BTreeTraverseHolder::instance()->getTraverse(request->initRequestId());
    if(traverse == NULL) {
        logger("traverse not found... need send error to client");
        // some actions;
    }

    int result;
    if(request->compareResult() == -1) {
        logger("compare result: -1, go to left...");
        result = (int)traverse->goToLeft();
    } else if(request->compareResult() == 1) {
        logger("compare result: 1, go to right...");
        result = (int)traverse->goToRight();
    } else {
        // equals, add to database, increase counter

        logger("equals element, ignore");
        result = 2;
    }

    if(result == 1) { // step
        logger("result = 1, step in depth...");
        responseAddTraverse.initRequestId() = request->initRequestId();
        responseAddTraverse.ciphertext() = traverse->value();
        wrapper.setResponse(&responseAddTraverse);
    } else if(result == 0) { // end
        logger("tree traverse completed, add row to table...");

        string opeFieldName;
        map<string, string> &row = traverse->requestAdd()->row();
        for(map<string, string>::iterator it = row.begin(); it != row.end(); ++it) {
            if(hasEnding(it->first, "_ope")) {
                opeFieldName = it->first;
            }
        }

        char buf[100];
        snprintf(buf, 100, "%llu", traverse->path());
        row[opeFieldName] = buf;

        if(_database->add(traverse->requestAdd()->database(), traverse->requestAdd()->table(), row)) {

        }

        logger("removing traverse...");

        BTreeTraverseHolder::instance()->removeTraverse(requestId);
        wrapper.setResponse(&responseAddOk);
    } else { // equals
        wrapper.setResponse(&responseAddOk);
    }

    logger("serializing response...");

    CborOutput output(10000);
    CborWriter writer(output);
    wrapper.Serialize(writer);

    loggerf("sending response to client size = %d...", output.getSize());
    zmq_send(_socket, output.getData(), (size_t) output.getSize(), 0);

    logger("request add continue processing completed successfully done");
}
