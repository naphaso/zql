#include "Request.h"

#include <cstdio>

using namespace std;

void RequestWrapper::Serialize(CborWriter &writer) {
	writer.writeTag(TAG_REQUEST_WRAPPER);
	writer.writeArray(2);
	writer.writeInt(id);
	
	request->Serialize(writer);
}

string &RequestGet::database() {
	return _database;
}

string &RequestGet::table() {
	return _table;
}

unsigned long long &RequestGet::pk() {
	return _pk;
}

void RequestGet::Serialize(CborWriter &writer) {
	writer.writeTag(TAG_REQUEST_GET);
	writer.writeArray(3);
	writer.writeString(_database);
	writer.writeString(_table);
	writer.writeInt(_pk);
}


int main(int argc, char **argv) {


	RequestGet get;
	get.database() = "testdatabase";
	get.table() = "testtable";
	get.pk() = 123;

	CborOutput output(9000);
	CborWriter writer(output);	
	get.Serialize(writer);
	
	//fwrite(output.data(), 1, output.size(), stdout);

	//fwrite(output.getData(), 1, output.getSize(), stdout);

	//return 0;
	unsigned char *data = output.getData();
	int size = output.getSize();

	CborInput input(data, size);
	CborReader reader(input);
	reader.run();

	return 0;
}

