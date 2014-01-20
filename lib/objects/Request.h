#ifndef REQUEST_H_
#define REQUEST_H_

#include "../Cbor.h"
#include <string>


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
