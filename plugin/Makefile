zql_lib := /root/zeromq-4.0.3
mysql_src := /root/mariadb-10.0.6
mysql_deployment := /tmp/maria
sodium_path := /root/libsodium

all: test module

test:
		g++ -I$(zql_lib)/include -I../lib -I../lib/objects -ggdb3 -lrt -lpthread test.cpp ../lib/Cbor.cpp ../lib/objects/Request.cpp ../lib/objects/Response.cpp ../lib/objects/ObjectParser.cpp $(zql_lib)/src/.libs/libzmq.a $(sodium_path)/src/libsodium/.libs/libsodium.a -o test

module:
		g++ -I$(zql_lib)/include -I../lib -I../lib/objects -O0 -ggdb3 -lrt -ozql.so zql.cpp ZqlDaemon.cpp Worker.cpp Database.cpp ../lib/Cbor.cpp ../lib/BTree.cpp ../lib/objects/Request.cpp ../lib/objects/Response.cpp ../lib/objects/ObjectParser.cpp $(zql_lib)/src/.libs/libzmq.a $(sodium_path)/src/libsodium/.libs/libsodium.a -I$(mysql_src) -I$(mysql_src)/build/include -I$(mysql_src)/sql -I$(mysql_src)/regex -I$(mysql_src)/include -shared -fPIC -DPIC -fno-exceptions -fno-rtti -Wno-format-security -fabi-version=2 -fno-omit-frame-pointer -fno-strict-aliasing -MD -MP
		cp zql.so $(mysql_deployment)/lib/plugin/zql.so
