#ifndef REQUEST_H_
#define REQUEST_H_

#include "../Cbor.h"
#include <string>

#define TAG_REQUEST_WRAPPER 1000u
#define TAG_RESPONSE_WRAPPER 2000u
#define TAG_REQUEST_GET 1100
#define TAG_RESPONSE_GET_OK 2100u
#define TAG_RESPONSE_GET_NOT_FOUND 2101u
#define TAG_RESPONSE_GET_ERROR 2102u

#define TAG_REQUEST_ADD 1200u
#define TAG_RESPONSE_ADD_OK 2200u
#define TAG_RESPONSE_ADD_NEED_COMPARE 2201u
#define TAG_RESPONSE_ADD_ERROR 2202u

#define TAG_REQUEST_ADD_CONTINUE 3100u

class Request;

class RequestWrapper : public CborSerializable {
private:
	unsigned int id;
	Request *request;
public:
	virtual void Serialize(CborWriter &writer);
};

class Request : public CborSerializable {
public:
	virtual void Serialize(CborWriter &writer) = 0;
};

class RequestGet : public Request {
private:
	std::string _database;
	std::string _table;
	unsigned long long _pk;
public:
	virtual void Serialize(CborWriter &writer);

	std::string &database();
	std::string &table();
	unsigned long long &pk();
};




#endif
