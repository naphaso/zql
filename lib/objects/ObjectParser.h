#ifndef OBJECT_PARSER_H_
#define OBJECT_PARSER_H_

#include "../Cbor.h"
#include "Request.h"
#include "Response.h"

typedef enum {
	OBJECT_PARSER_STATE_INIT,

    OBJECT_PARSER_STATE_REQUEST_WRAPPER_ARRAY,
	OBJECT_PARSER_STATE_REQUEST_WRAPPER_ID,
    OBJECT_PARSER_STATE_REQUEST_WRAPPER_BODY,

    OBJECT_PARSER_STATE_REQUEST_GET_ARRAY,
    OBJECT_PARSER_STATE_REQUEST_GET_DATABASE,
    OBJECT_PARSER_STATE_REQUEST_GET_TABLE,
    OBJECT_PARSER_STATE_REQUEST_GET_PK,

    OBJECT_PARSER_STATE_RESPONSE_WRAPPER_ARRAY,
    OBJECT_PARSER_STATE_RESPONSE_WRAPPER_ID,
    OBJECT_PARSER_STATE_RESPONSE_WRAPPER_BODY,

    OBJECT_PARSER_STATE_RESPONSE_GET_EMPTY_MAP,

    OBJECT_PARSER_STATE_RESPONSE_GET_OK_MAP,
    OBJECT_PARSER_STATE_RESPONSE_GET_OK_MAP_KEY,
    OBJECT_PARSER_STATE_RESPONSE_GET_OK_MAP_VALUE,

    OBJECT_PARSER_STATE_REQUEST_ADD_ARRAY,
    OBJECT_PARSER_STATE_REQUEST_ADD_DATABASE,
    OBJECT_PARSER_STATE_REQUEST_ADD_TABLE,
    OBJECT_PARSER_STATE_REQUEST_ADD_ROW_MAP,
    OBJECT_PARSER_STATE_REQUEST_ADD_ROW_MAP_KEY,
    OBJECT_PARSER_STATE_REQUEST_ADD_ROW_MAP_VALUE

} ObjectParserState;


class ObjectListener {
public:
    virtual void OnRequestGet(unsigned int requestId, RequestGet *request) = 0;
    //	virtual void OnResponseGet(unsigned int requestId, ResponseGet &response);
    virtual void OnResponseGetEmpty(unsigned int requestId) = 0;
    virtual void OnResponseGetOk(unsigned int requestId, ResponseGetOk *response) = 0;
    virtual void OnError(const char *error) = 0;
    virtual void OnRequestAdd(unsigned int requestId, RequestAdd *request) = 0;
};

class DebugObjectListener : public ObjectListener {

public:
    virtual void OnRequestGet(unsigned int requestId, RequestGet *request);
    virtual void OnResponseGetEmpty(unsigned int requestId);
    virtual void OnResponseGetOk(unsigned int requestId, ResponseGetOk *response);
    virtual void OnRequestAdd(unsigned int requestId, RequestAdd *request);
    virtual void OnError(const char *error);
};

class ObjectParser : public CborListener {
public:
	ObjectParser();
	void Run();
    void SetInput(CborInput &input);
    void SetListener(ObjectListener &listener);
private:
    std::string currentKey;
    int elements;
    ObjectListener *listener;
    CborInput *input;
	ObjectParserState state;
    void *currentObject;
    unsigned int currentRequestId;
protected:
	virtual void OnInteger(int value);
	virtual void OnBytes(unsigned char *data, int size);
	virtual void OnString(std::string &str);
	virtual void OnArray(int size);
	virtual void OnMap(int size);
	virtual void OnTag(unsigned int tag);
	virtual void OnSpecial(int code);
	virtual void OnError(const char *error);
};



#endif
