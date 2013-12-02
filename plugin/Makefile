all: test module

test:
		g++ -I/home/wolong/repo/zeromq-4.0.1/include -lsodium -lpthread test.cpp /home/wolong/repo/zeromq-4.0.1/src/.libs/libzmq.a -o test

module:
		g++ -I/home/wolong/repo/zeromq-4.0.1/include -lsodium -ozql.so zql.cpp ZqlDaemon.cpp Worker.cpp Database.cpp /home/wolong/repo/zeromq-4.0.1/src/.libs/libzmq.a -I/home/wolong/repo/mysql-5.6.14 -I/home/wolong/repo/mysql-5.6.14/build/include -I/home/wolong/repo/mysql-5.6.14/sql -I/home/wolong/repo/mysql-5.6.14/regex -I/home/wolong/repo/mysql-5.6.14/include -O0 -ggdb3 -shared -fPIC -fno-exceptions -fno-rtti -Wno-format-security
		cp zql.so /tmp/mysql/lib/plugin/zql.so
