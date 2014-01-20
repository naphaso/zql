//
// Copyright (c) 2014 undefware. All rights reserved.
//


#include "Cbor.h"

#ifndef __Response_H_
#define __Response_H_


class Response;

class ResponseWrapper : public CborSerializable {
private:
  unsigned int id;
  ResponseGet *response;
public:
  virtual void Serialize(CborWriter &writer);
};

class Response : CborSerializable {
public:
  virtual void Serialize(CborWriter &writer) = 0;
};

class ResponseGet : public Response {
public:
  virtual void Serialize(CborWriter &writer);
};



#endif //__Response_H_
