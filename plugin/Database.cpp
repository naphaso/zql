#include "Database.h"

Database::Database() {

}

int Database::execute(const char* databaseName, const char *tableName, bool forWrite, const char *index, int value) {
	int r = 0;
	TABLE_LIST tables;
	TABLE *table = NULL;

	const thr_lock_type lock_type = forWrite ? TL_WRITE : TL_READ;

	tables.init_one_table(databaseName, strlen(databaseName), tableName, strlen(tableName), tableName, lock_type);

	tables.mdl_request.init(MDL_key::TABLE, databaseName, tableName,
      forWrite ? MDL_SHARED_WRITE : MDL_SHARED_READ, MDL_TRANSACTION);
    Open_table_context ot_act(thd, 0);

	if (!open_table(thd, &tables, &ot_act)) {
		table = tables.table;
	}

	if(table == NULL) {
		fprintf(stderr, "failed to open table\n");
		return -1;
	}

	//statistic_increment(open_tables_count, &LOCK_status);
	
	table->reginfo.lock_type = lock_type;
    table->use_all_columns();
	
	// TODO: find symbolic index
	int indexNumber = atoi(index);

	// TODO: specific fields
	
	// TODO: other find types
	ha_rkey_function find_flag = HA_READ_KEY_EXACT;

	lockTables();

	KEY& kinfo = table->key_info[indexNumber];

	uchar *const key_buf = new uchar[kinfo.key_length];
	size_t kplen_sum = 0;

	// START prepare keybuf
	prepareKeyBuf(key_buf, table, kinfo, value);
	// END prepare keybuf

	table->read_set = &table->s->all_set;
	
	handler *const hnd = table->file;

	if (!forWrite) {
		hnd->init_table_handle_for_HANDLER();
	}

	hnd->ha_index_or_rnd_end();
	hnd->ha_index_init(indexNumber, 1);

	// start response array

	const key_part_map keyPartMap = (1U << 1) - 1; // use all key parts

	r = hnd->ha_index_read_map(table->record[0], key_buf, keyPartMap, find_flag);

	while(true) {
		r = hnd->ha_index_next_same(table->record[0], key_buf, kplen_sum);

		if(r == 0 /* && filter */  /* && no skip */ ) {
			// send row
		} else {
			break;
		}
	}

	hnd->ha_index_or_rnd_end();
	// send end of array

	// closing table
	unlockTables();
	close_thread_tables(thd);
	thd->mdl_context.release_transactional_locks();

	return 0;
}

void Database::lockTables(TABLE *table) {
	// TODO: check relock and errors
	thd->lock = mysql_lock_tables(thd, &tables[0], num_open, 0);
	// statistic_increment(lock_tables_count, &LOCK_status);
	// thd_proc_info(thd, &info_message_buf[0]);
	// check for write
	
}

void Database::unlockTables() {
	// TODO: check relock and errors
	//bool suc = (trans_commit_stmt(thd) == 0);
	mysql_unlock_tables(thd, lock);
	thd->lock = 0;
	// statistic_increment(unlock_tables_count, &LOCK_status);
}

int Database::prepareKeybuf(uchar *key_buf, TABLE *table, KEY &kinfo, int value) {
	unsigned int kplen_sum = 0;
	char valueStr[100];
	const KEY_PART_INFO &keyPartInfo = kinfo.key_part[0]; // only forst keypart
	keyPartInfo->set_notnull(); // or null :)

	sprintf(valueStr, "%d", value);
	kpt.field->store(valueStr, strlen(valueStr), &my_charset_bin);
    kplen_sum += kpt.store_length;

	key_copy(key_buf, table->record[0], &kinfo, kplen_sum);

	return kplen_sum;
}


void Database::initThread(const void *stack_bottom, volatile int &shutdown_flag) {
	my_thread_init();
	thd = new THD;
	thd->thread_stack = (char *)stack_bottom;
	thd->store_globals();
	thd->system_thread = static_cast<enum_thread_type>(1<<30UL);
	const NET v = { 0 };
	thd->net = v;
	// TODO: for write check
	my_pthread_setspecific_ptr(THR_THD, thd);

	// atomically increment integer
	pthread_mutex_lock(&LOCK_thread_count);
	thd->thread_id = thread_id++;
	add_global_thread(thd);
	pthread_mutex_unlock(&LOCK_thread_count);
	// end

	thd_proc_info(thd, &info_message_buf[0]);
 	lex_start(thd); // ???

}

void Database::deinitThread() {
	unlockTables();	
	my_pthread_setspecific_ptr(THR_THD, 0);
	
	// atomically integer decrement
	pthread_mutex_lock(&LOCK_thread_count);
	remove_global_thread(thd);
	delete thd;
	thd = 0;
	pthread_mutex_unlock(&LOCK_thread_count);
	// end

	my_thread_end();
}
