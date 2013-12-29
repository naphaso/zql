#ifndef MYSQL_INCLUDE_H
#define MYSQL_INCLUDE_H


#define MYSQL_DYNAMIC_PLUGIN 1
#define MYSQL_SERVER 0 
#define DBUG_OFF

#include <my_pthread.h>
#include <sql_priv.h>
#include "sql_class.h"
#include "unireg.h"
#include "lock.h"
#include "key.h" // key_copy()
#include <my_global.h>
#include <mysql/plugin.h>
#include <transaction.h>
#include <sql_base.h>
// FIXME FIXME FIXME
#define safeFree(X) my_free(X)
#define pthread_cond_timedwait  mysql_cond_timedwait
#define  pthread_mutex_lock  mysql_mutex_lock
#define  pthread_mutex_unlock  mysql_mutex_unlock
#define current_stmt_binlog_row_based  is_current_stmt_binlog_format_row
#define clear_current_stmt_binlog_row_based  clear_current_stmt_binlog_format_row


//#include <global_threads.h>

#undef min
#undef max


#endif
