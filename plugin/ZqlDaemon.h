#ifndef ZQL_DAEMON_H
#define ZQL_DAEMON_H

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

#include <zmq.h>


class ZqlDaemon {
public:
	ZqlDaemon();
	~ZqlDaemon();

	void run();
private:
	pthread_t _thread;
	void *_context;
	void *_frontend_socket;
	void *_backend_socket;
};

#endif
