#ifndef OBJECT_PARSER_H_
#define OBJECT_PARSER_H_

#include "../Cbor.h"
#include "Request.h"

typedef enum {
	OBJECT_PARSER_STATE_INIT,
	OBJECT_PARSER_STATE_REQUEST_GET,
	OBJECT_PARSER_STATE_RESPONSE_GET
} ObjectParserState;

class ObjectParser : public CborReader {
public:
	ObjectParser(CborInput &input);
	void Run();
private:
	ObjectParserState state;
	int fieldNumber;
protected:
	virtual void onInteger(int value);
	virtual void onBytes(unsigned char *data, int size);
	virtual void onString(std::string &str);
	virtual void onArray(int size);
	virtual void onMap(int size);
	virtual void onTag(int tag);
	virtual void onSpecial(int code);
	virtual void onError(const char *error);
protected:
	virtual void onRequestGet(unsigned int requestId, RequestGet &request);
	virtual void onErrorOccured(const char *error);
	//	virtual void OnResponseGet(unsigned int requestId, ResponseGet &response);
};

class ObjectListener {
public:
	virtual void OnRequestGet(unsigned int requestId, RequestGet &request) = 0;
	// TODO: other requests
	virtual void OnError(const char *error);
};

class ObjectFactory : public CborReader {
public:
	ObjectFactory(CborInput &input);
public:
	
}


#endif
