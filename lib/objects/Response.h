//
// Copyright (c) 2014 undefware. All rights reserved.
//


#include <map>
#include "Cbor.h"
#include "../BTree.h"

#ifndef __Response_H_
#define __Response_H_


class Response;

class ResponseWrapper : public CborSerializable {
private:
  unsigned int id;
  Response *response;
public:

    unsigned int getId() const {
        return id;
    }

    void setId(unsigned int id) {
        ResponseWrapper::id = id;
    }

    Response *getResponse() const {
        return response;
    }

    void setResponse(Response *response) {
        ResponseWrapper::response = response;
    }

    virtual void Serialize(CborWriter &writer);
};

class Response : CborSerializable {
public:
  virtual void Serialize(CborWriter &writer) = 0;
};

class ResponseGetOk : public Response {
private:
    std::map<std::string, std::string> values;
public:
    std::map<std::string, std::string> &GetValues();
    void SetValue(const std::string &key, const std::string &value);
  virtual void Serialize(CborWriter &writer);
};

class ResponseGetEmpty : public Response {

public:
    virtual void Serialize(CborWriter &writer);
};


class ResponseAddOk : public Response {
public:
    virtual void Serialize(CborWriter &writer);
};

class ResponseAddTraverse : public Response {
private:
    unsigned int _initRequestId;
    Ciphertext *_ciphertext;
public:
    ResponseAddTraverse();
    ResponseAddTraverse(unsigned int initRequestId, Ciphertext *ciphertext) : _initRequestId(initRequestId), _ciphertext(ciphertext) {}
    virtual void Serialize(CborWriter &writer);

    unsigned int &initRequestId() { return _initRequestId; }
    Ciphertext *&ciphertext() { return _ciphertext; }


};


#endif //__Response_H_
