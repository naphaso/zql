#include "ObjectParser.h"
#include "log.h"

ObjectParser::ObjectParser(CborInput &input) : CborReader(input) {
	state = OBJECT_PARSER_STATE_INIT;
}

void ObjectParser::Run() {
	run();
}

void ObjectParser::onInteger(int value) {
	
}

void ObjectParser::onBytes(unsigned char *data, int size) {

}

void ObjectParser::onString(std::string &str) {

}

void ObjectParser::onArray(int size) {

}

void ObjectParser::onMap(int size) {

}

void ObjectParser::onTag(int tag) {

}

void ObjectParser::onSpecial(int code) {

}

void ObjectParser::onError(const char *error) {

}


void ObjectParser::onRequestGet(unsigned int requestId, RequestGet &request) {

}

void ObjectParser::onErrorOccured(const char *error) {

}


int main(int argc, char **argv) {
	logger("hello world!");
	return 0;
}
