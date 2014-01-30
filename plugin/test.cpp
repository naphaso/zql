#include <stdio.h>
#include <string.h>
#include <zmq.h>
#include "Request.h"
#include "ObjectParser.h"
#include <stdlib.h>
#include <string>
#include <map>
#include <pthread.h>

using namespace std;


class InsertResultListener {
public:
    virtual void OnSuccess() = 0;
    virtual void OnError() = 0;
};

class RequestExecutor {
public:
    virtual void Run(void *socket, const string &identity) = 0;
};

static map<unsigned int, ObjectListener *> requestListeners;

class GlobalResponseListener : public ObjectListener {
public:
    virtual void OnRequestGet(unsigned int requestId, RequestGet *request) {
        ObjectListener *listener = requestListeners[requestId];
        if(listener != NULL) {
            listener->OnRequestGet(requestId, request);
        }
    }

    virtual void OnResponseGetEmpty(unsigned int requestId) {
        ObjectListener *listener = requestListeners[requestId];
        if(listener != NULL) {
            listener->OnResponseGetEmpty(requestId);
        }
    }

    virtual void OnResponseGetOk(unsigned int requestId, ResponseGetOk *response) {
        ObjectListener *listener = requestListeners[requestId];
        if(listener != NULL) {
            listener->OnResponseGetOk(requestId, response);
        }
    }

    virtual void OnRequestAdd(unsigned int requestId, RequestAdd *request) {
        ObjectListener *listener = requestListeners[requestId];
        if(listener != NULL) {
            listener->OnRequestAdd(requestId, request);
        }
    }

    virtual void OnResponseAddOk(unsigned int requestId) {
        ObjectListener *listener = requestListeners[requestId];
        if(listener != NULL) {
            listener->OnResponseAddOk(requestId);
        }
    }

    virtual void OnResponseAddTraverse(unsigned int requestId, ResponseAddTraverse *response) {
        ObjectListener *listener = requestListeners[requestId];
        if(listener != NULL) {
            listener->OnResponseAddTraverse(requestId, response);
        }
    }

    virtual void OnRequestAddContinue(unsigned int requestId, RequestAddContinue *request) {
        ObjectListener *listener = requestListeners[requestId];
        if(listener != NULL) {
            listener->OnRequestAddContinue(requestId, request);
        }
    }

    virtual void OnError(const char *error) {
        printf("error: %s\n", error);
    }
};

bool hasEnding(std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

string encrypt(string &data) {
    return data;
}

string decrypt(const string &data) {
    return data;
}

int compare(const string &a, const string &b) {
    return a.compare(b);
}

class InsertExecutor : public ObjectListener, public RequestExecutor {
private:
    string _database;
    string _table;
    map<string, string> _row;
    string opeValue;
    InsertResultListener *_listener;
    string _identity;
    void *_socket;
public:
    InsertExecutor(InsertResultListener *listener, const string &database, const string &table, const map<string, string> &row) : _listener(listener) {
        _database = database;
        _table = table;
        _row = row;

        for(map<string, string>::iterator it = _row.begin(); it != _row.end(); ++it) {
            if(hasEnding(it->first, "_ope")) {
                opeValue = it->second;
                it->second = encrypt(it->second);
            }
        }
    }

    virtual void Run(void *socket, const string &identity) {
        printf("running insert executor...\n");
        _socket = socket;
        _identity = identity;

        RequestAdd request;
        request.database() = _database;
        request.table() = _table;
        request.row() = _row;

        RequestWrapper wrapper;
        unsigned int requestId = (unsigned int) rand();
        wrapper.setId(requestId);
        wrapper.setRequest(&request);

        CborOutput output(10000);
        CborWriter writer(output);
        wrapper.Serialize(writer);

        requestListeners[requestId] = this;

        //zmq_send(socket, identity.c_str(), identity.size(), ZMQ_MORE);
        //zmq_send(socket, identity.c_str(), 0, ZMQ_MORE);
        zmq_send(socket, output.getData(), (size_t) output.getSize(), 0);

        printf("insert executor execution done\n");
    }

    virtual void OnRequestGet(unsigned int requestId, RequestGet *request) {}
    virtual void OnResponseGetEmpty(unsigned int requestId) {}
    virtual void OnResponseGetOk(unsigned int requestId, ResponseGetOk *response) {}
    virtual void OnRequestAdd(unsigned int requestId, RequestAdd *request) {}
    virtual void OnResponseAddOk(unsigned int requestId) {
        printf("insert executor response add ok\n");
        requestListeners.erase(requestId);
        _listener->OnSuccess();

        delete this;
    }

    virtual void OnResponseAddTraverse(unsigned int requestId, ResponseAddTraverse *response) {
        printf("insert executor response add traverse\n");
        requestListeners.erase(requestId);

        RequestAddContinue request;
        request.initRequestId() = response->initRequestId();
        string ciphertext((char*)response->ciphertext()->data(), response->ciphertext()->size());
        request.compareResult() = compare(decrypt(ciphertext), opeValue);

        unsigned int addContinueRequestId = (unsigned int) rand();

        RequestWrapper wrapper;
        wrapper.setId(addContinueRequestId);
        wrapper.setRequest(&request);

        CborOutput output(10000);
        CborWriter writer(output);
        wrapper.Serialize(writer);

        requestListeners[addContinueRequestId] = this;

        delete response;

        //zmq_send(_socket, _identity.c_str(), _identity.size(), ZMQ_MORE);
        //zmq_send(_socket, _identity.c_str(), 0, ZMQ_MORE);
        zmq_send(_socket, output.getData(), (size_t) output.getSize(), 0);
        printf("insert executor response add traverse processing done.\n");
    }


    virtual void OnRequestAddContinue(unsigned int requestId, RequestAddContinue *request) {}
    virtual void OnError(const char *error) {}
};


/*
class InsertListener : public ObjectListener {

public:
    virtual void OnRequestGet(unsigned int requestId, RequestGet *request) {
        printf("request get\n");
    }

    virtual void OnResponseGetEmpty(unsigned int requestId) {
        printf("response get empty\n");
    }

    virtual void OnResponseGetOk(unsigned int requestId, ResponseGetOk *response) {
        printf("response get ok\n");
    }

    virtual void OnRequestAdd(unsigned int requestId, RequestAdd *request) {
        printf("request add\n");
    }


    virtual void OnResponseAddOk(unsigned int requestId) {
        printf("response add ok: %u\n", requestId);
    }

    virtual void OnResponseAddTraverse(unsigned int requestId, ResponseAddTraverse *response) {
        printf("response add traverse, request id %u init request id %u, ciphertext size %u\n", requestId, response->initRequestId(), response->ciphertext()->size());

        RequestAddContinue request;
        request.initRequestId() = response->initRequestId();
        request.compareResult() = 1;
	
	printf("request add continue composed\n");
	
        RequestWrapper wrapper;
        wrapper.setId((unsigned int) rand());
        wrapper.setRequest(&request);

	printf("wrapper composed\n");

        CborOutput output(100000);
        CborWriter writer(output);
        wrapper.Serialize(writer);

	printf("serialized size %d\n", output.getSize());

        zmq_send(socket, output.getData(), (size_t) output.getSize(), 0);

	printf("sended\n");

        zmq_msg_t message;
        zmq_msg_init(&message);
        zmq_msg_recv(&message, socket, 0);

	printf("received response with size: %lu\n", zmq_msg_size(&message));

        CborInput input(zmq_msg_data(&message), zmq_msg_size(&message));
        ObjectParser parser;
        parser.SetInput(input);
        parser.SetListener(*this);
        parser.Run();

	printf("response parsed\n");

        delete response;
    }


    virtual void OnRequestAddContinue(unsigned int requestId, RequestAddContinue *request) {
        printf("request add continue\n");
    }

    virtual void OnError(const char *error) {
        printf("error: %s\n", error);
    }
};*/

/*
int main(int argc, char **argv) {

	context = zmq_ctx_new();
	socket = zmq_socket(context, ZMQ_REQ);


	zmq_connect(socket, "tcp://127.0.0.1:9990");
	printf("zmq connect ok\n");



    RequestAdd request;
    request.database() = argv[1];
    request.table() = argv[2];
    request.row()[argv[3]] = argv[4];
    request.row()[argv[5]] = argv[6];

	printf("request add ok\n");

    RequestWrapper wrapper;
    wrapper.setId(123);
    wrapper.setRequest(&request);

	printf("request wrapper ok\n");

    CborOutput output(100000);
    CborWriter writer(output);
    wrapper.Serialize(writer);

	printf("serialize ok\n");

    zmq_send(socket, output.getData(), output.getSize(), 0);

printf("send ok\n");

    zmq_msg_t message;
    zmq_msg_init(&message);
    zmq_msg_recv(&message, socket, 0);

printf("recv ok\n");

    CborInput input(zmq_msg_data(&message), zmq_msg_size(&message));
    ObjectParser parser;
    InsertListener listener;
    parser.SetInput(input);
    parser.SetListener(listener);
    parser.Run();

printf("parse ok\n");

	zmq_close(socket);
	zmq_ctx_destroy(context);
	return 0;
}


*/

static pthread_mutex_t send_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static RequestExecutor **executorQueue = new RequestExecutor*[1000];
static int executorQueueOffset = 0;

void add_executor(RequestExecutor *executor) {
    pthread_mutex_lock(&send_queue_mutex);

    executorQueue[executorQueueOffset++] = executor;

    pthread_mutex_unlock(&send_queue_mutex);
}

class DebugInsertResultListener : public InsertResultListener {

public:
    virtual void OnSuccess() {
        printf("insert success\n");
    }

    virtual void OnError() {
        printf("insert error\n");
    }
};

void *client_thread(void *data) {
    struct timespec t;
    t.tv_sec = 10;
    t.tv_nsec = 0;
    nanosleep(&t, NULL);

    map<string, string> row;
    row["id"] = "1001";
    row["data_ope"] = "hello";
    add_executor(new InsertExecutor(new DebugInsertResultListener(), "testdatabase", "testtable2", row));
    return NULL;
}

int main(int argc, char **argv) {
    void *context = zmq_ctx_new();
    void *socket = zmq_socket(context, ZMQ_REQ);

    zmq_connect(socket, "tcp://127.0.0.1:9990");

    bool disposed = false;


    // start client thread
    pthread_attr_t attr;
    pthread_t clientThread;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&clientThread, &attr, &client_thread, NULL);

    zmq_msg_t message;
    GlobalResponseListener globalResponseListener;

    struct timespec t;
    t.tv_sec = 0;
    t.tv_nsec = 5 * 1000000;

    srand((unsigned int) time(NULL));
    int randIdentity = rand();
    char idBuf[64];
    snprintf(idBuf, 64, "%d", randIdentity);
    string identity(idBuf);

    while(!disposed) {
        RequestExecutor **executors = NULL;
        int executorsCount = 0;

        pthread_mutex_lock(&send_queue_mutex);

        if(executorQueueOffset != 0) {
            printf("executor queue not null, copy to local...\n");
            executors = new RequestExecutor*[executorQueueOffset];
            executorsCount = executorQueueOffset;

            for(int i = 0; i < executorsCount; i++) {
                executors[i] = executorQueue[i];
            }

            executorQueueOffset = 0;
        }

        pthread_mutex_unlock(&send_queue_mutex);

        for(int i = 0; i < executorsCount; i++) {
            printf("run executor...\n");
            executors[i]->Run(socket, identity);
        }

        zmq_msg_init(&message);
        int rc = zmq_msg_recv(&message, socket, ZMQ_DONTWAIT);
        if(rc > 0) {
            printf("received message...\n");
            if(zmq_msg_more(&message)) {
                zmq_msg_t delimiter;
                zmq_msg_t dataMessage;
                zmq_msg_init(&delimiter);
                zmq_msg_init(&dataMessage);
                zmq_msg_recv(&delimiter, socket, 0);
                zmq_msg_recv(&dataMessage, socket, 0);

                printf("parse and process message...\n");
                CborInput input(zmq_msg_data(&dataMessage), zmq_msg_size(&dataMessage));
                ObjectParser parser;
                parser.SetInput(input);
                parser.SetListener(globalResponseListener);
                parser.Run();
                printf("parse and process message completed\n");

                zmq_msg_close(&delimiter);
                zmq_msg_close(&dataMessage);
            }
        }
        zmq_msg_close(&message);

        nanosleep(&t, NULL);
    }

    return 0;
}