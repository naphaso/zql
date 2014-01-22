#ifndef REQUEST_H_
#define REQUEST_H_

#include "../Cbor.h"
#include <string>
#include <map>


class Request;

class RequestWrapper : public CborSerializable {
private:
	unsigned int id;
	Request *request;
public:

    Request *getRequest() const {
        return request;
    }

    void setRequest(Request *request) {
        RequestWrapper::request = request;
    }

    unsigned int getId() const {
        return id;
    }

    void setId(unsigned int id) {
        RequestWrapper::id = id;
    }

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

class RequestAdd : public Request {
private:
    std::string _database;
    std::string _table;
    std::map<std::string, std::string> _row;
public:
    virtual void Serialize(CborWriter & writer);

    std::string &database();
    std::string &table();
    std::map<std::string, std::string> &row();
};


#endif
