#include "ZqlDaemon.h"


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

ZqlDaemon::ZqlDaemon() {

}

ZqlDaemon::~ZqlDaemon() {

}
