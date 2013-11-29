#include <memory>
#include <string>
#include <stdio.h>

/*
#include "config.hpp"
#include "mysql_incl.hpp"
#include "string_util.hpp"
*/

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

//using namespace zql;

static char *zql_address = 0;
static char *zql_port = 0;
static unsigned int zql_threads = 1;

static int daemon_zql_init(void *p) {
	//std::auto_ptr<ZqlDaemon> ap(new ZqlDaemon());
	// TODO: create daemon, configure and start listen server
	st_plugin_int *const plugin = static_cast<st_plugin_int *>(p);
	plugin->data = NULL;//ap.release();
	return 0;
}

static int daemon_zql_deinit(void *p) {
	st_plugin_int *const plugin = static_cast<st_plugin_int *>(p);
	//ZqlDaemon *instance = static_cast<ZqlDaemon>(plugin->data);
	//delete instance;
	return 0;
}

static struct st_mysql_daemon daemon_zql_plugin = {
  MYSQL_DAEMON_INTERFACE_VERSION
};

static struct st_mysql_sys_var *daemon_zql_system_variables[] = {
  0
};

static SHOW_VAR zql_status_variables[] = {
	/*  {"table_open", (char*) &open_tables_count, SHOW_LONGLONG},
  {"table_close", (char*) &close_tables_count, SHOW_LONGLONG},
  {"table_lock", (char*) &lock_tables_count, SHOW_LONGLONG},
  {"table_unlock", (char*) &unlock_tables_count, SHOW_LONGLONG},*/
  {NullS, NullS, SHOW_LONG}
};

static int show_zql_vars(THD *thd, SHOW_VAR *var, char *buff) {
  var->type= SHOW_ARRAY;
  var->value= (char *) &zql_status_variables;
  return 0;
}

static SHOW_VAR daemon_zql_status_variables[] = {
	//  {"Hs", (char*) show_hs_vars, SHOW_FUNC},
  {NullS, NullS, SHOW_LONG}
};

mysql_declare_plugin(handlersocket)
{
  MYSQL_DAEMON_PLUGIN,
  &daemon_zql_plugin,
  "ZQL",
  "undefware inc",
  "",
  PLUGIN_LICENSE_BSD,
  daemon_zql_init,
  daemon_zql_deinit,
  0x0100 /* 1.0 */,
  daemon_zql_status_variables,
  daemon_zql_system_variables,
  0
}
mysql_declare_plugin_end;
