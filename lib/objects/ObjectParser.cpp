#include "ObjectParser.h"
#include "log.h"

ObjectParser::ObjectParser() {
}

void ObjectParser::Run() {
    CborReader reader(*input);
    reader.SetListener(*this);
    reader.Run();
}

void ObjectParser::OnInteger(int value) {
	if(state == OBJECT_PARSER_STATE_REQUEST_WRAPPER_ID) {
        currentRequestId = (unsigned int) value; // TODO: fix type cast
        state = OBJECT_PARSER_STATE_REQUEST_WRAPPER_BODY;
    } else if(state == OBJECT_PARSER_STATE_REQUEST_GET_PK) {
        ((RequestGet *)currentObject)->pk() = (unsigned long long) value; // TODO: fix type cast
        state = OBJECT_PARSER_STATE_INIT;
        listener->OnRequestGet(currentRequestId, (RequestGet*)currentObject);
    } else listener->OnError("unknown integer");
}

void ObjectParser::OnBytes(unsigned char *data, int size) {
    listener->OnError("unknown bytes");
}

void ObjectParser::OnString(std::string &str) {
    if(state == OBJECT_PARSER_STATE_REQUEST_GET_DATABASE) {
        ((RequestGet *)currentObject)->database() = str;
        state = OBJECT_PARSER_STATE_REQUEST_GET_TABLE;
    } else if(state == OBJECT_PARSER_STATE_REQUEST_GET_TABLE) {
        ((RequestGet *)currentObject)->table() = str;
        state = OBJECT_PARSER_STATE_REQUEST_GET_PK;
    } else listener->OnError("unknown string");
}

void ObjectParser::OnArray(int size) {
    if(state == OBJECT_PARSER_STATE_REQUEST_WRAPPER_ARRAY) {
        if(size == 2) {
            state = OBJECT_PARSER_STATE_REQUEST_WRAPPER_ID;
        } else listener->OnError("unknown wrapper size");
    } else if(state == OBJECT_PARSER_STATE_REQUEST_GET_ARRAY) {
        if(size == 3) {
            state = OBJECT_PARSER_STATE_REQUEST_GET_DATABASE;
        } else listener->OnError("unknown wrapper size");
    } else listener->OnError("unknown array");
}

void ObjectParser::OnMap(int size) {
    listener->OnError("unknown map");
}

void ObjectParser::OnTag(unsigned int tag) {
    if(state == OBJECT_PARSER_STATE_INIT) {
        if(tag == 1000) { // request wrapper
            state = OBJECT_PARSER_STATE_REQUEST_WRAPPER_ARRAY;
        } else listener->OnError("unknown tag");
    } else if(state == OBJECT_PARSER_STATE_REQUEST_WRAPPER_BODY) {
        if(tag == 1100) {
            state = OBJECT_PARSER_STATE_REQUEST_GET_ARRAY;
            currentObject = new RequestGet();
        } else listener->OnError("unknown tag");
    } else listener->OnError("unknown tag");
}

void ObjectParser::OnSpecial(int code) {
    listener->OnError("unknown special");
}

void ObjectParser::OnError(const char *error) {
    listener->OnError(error);
}



void ObjectParser::SetInput(CborInput &input) {
    this->input = &input;
}

void ObjectParser::SetListener(ObjectListener &listener) {
    this->listener = &listener;
}


void DebugObjectListener::OnRequestGet(unsigned int requestId, RequestGet *request) {
    loggerf("request get id %u, database '%s', table '%s', pk %d", requestId, request->database().c_str(), request->table().c_str(), (int) request->pk());
    delete request;
}

void DebugObjectListener::OnError(const char *error) {
    loggerf("parsing error: %s", error);
}
