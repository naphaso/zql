#ifndef DATABASE_H
#define DATABASE_H

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

class Database;

// local includes

class Database {
public:
	Database();
	int execute(const char *database, const char *table, bool forWrite, const char *index, int value);
private:
	void prepareKeybuf(uchar *key_buf, TABLE *table, KEY &kinfo, int value);

	void lockTables(TABLE *table);
	void unlockTables();

	void initThread(const void *stack_bottom, volatile int &shutdown_flag);
	void deinitThread();
	THD *thd;
};

#endif
