#ifndef OBJECT_PARSER_H_
#define OBJECT_PARSER_H_

typedef enum {
	OBJECT_PARSER_STATE_REQUEST_GET,
	OBJECT_PARSER_STATE_RESPONSE_GET
} ObjectParserState;

class ObjectParser : public CborReader {
public:
	ObjectParser(unsigned char *data, unsigned int size);
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
	virtual void onRequestGet(unsigned int requestId, RequestGet &request) = 0;
	virtual void onErrorOccured(const char *error) = 0;
	//	virtual void OnResponseGet(unsigned int requestId, ResponseGet &response);
};

#endif
