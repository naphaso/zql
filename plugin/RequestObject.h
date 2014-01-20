#ifndef REQUEST_OBJECT_H
#define REQUEST_OBJECT_H

#define REQUEST_TYPE_GET 1
#define REQUEST_TYPE_SELECT 2
#define REQUEST_TYPE_UPDATE 3
#define REQUEST_TYPE_INSERT 4
#define REQUEST_TYPE_DELETE 5

#include "../lib/Cbor.h"

class RequestObject {
public:
	RequestObject(void *data, size_t size);
	~RequestObject();

private:
	int _type;
	
	char *_token_data;
	size_t token_size;
};

#endif
