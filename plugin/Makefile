all:
		g++ -ozql.so zql.cpp ZqlDaemon.cpp -I/home/wolong/repo/mysql-5.6.14 -I/home/wolong/repo/mysql-5.6.14/build/include -I/home/wolong/repo/mysql-5.6.14/sql -I/home/wolong/repo/mysql-5.6.14/include -O0 -ggdb3 -shared -fPIC -fno-exceptions -fno-rtti -DMYSQL_DYNAMIC_PLUGIN -Wno-format-security
		cp zql.so /tmp/mysql/lib/plugin/zql.so
