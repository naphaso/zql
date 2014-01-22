#ifndef WORKER_H
#define WORKER_H

/*
#include <my_global.h>
#include <sql_priv.h>
#include <stdlib.h>
#include <ctype.h>
#include <mysql_version.h>
#include <mysql/plugin.h>
#include <my_dir.h>
#include "my_pthread.h"                         // pthread_handler_t
#include "my_sys.h"                             // my_write, my_malloc
#include "m_string.h"                           // strlen
#include "sql_plugin.h"                         // st_plugin_int
*/

#include "mysql_include.h"

#include <zmq.h>

class Worker;

#include "ZqlDaemon.h"
#include "Database.h"
#include "ObjectParser.h"

class Worker : public ObjectListener {
public:
	Worker(ZqlDaemon *daemon, int number);
	~Worker();


    virtual void OnRequestGet(unsigned int requestId, RequestGet *request);

    virtual void OnResponseGetEmpty(unsigned int requestId);

    virtual void OnResponseGetOk(unsigned int requestId, ResponseGetOk *response);


    virtual void OnRequestAdd(unsigned int requestId, RequestAdd *request);

    virtual void OnError(const char *error);

    void run();
private:
	Database *_database;

	int _number;
	pthread_t _thread;
	ZqlDaemon *_daemon;
	void *_socket;
    bool _disposed;
};

#endif
