//
// Created by Igor Glotov on 19/01/14.
// Copyright (c) 2014 undefware. All rights reserved.
//

#include "Response.h"
#include "Tags.h"

using namespace std;

void ResponseWrapper::Serialize(CborWriter &writer) {
  writer.writeTag(TAG_RESPONSE_WRAPPER);
  writer.writeArray(2);
  writer.writeInt(id);

  response->Serialize(writer);
}


void ResponseGetOk::SetValue(const std::string &key, const std::string &value) {
    values[key] = value;
}

void ResponseGetOk::Serialize(CborWriter &writer) {
    writer.writeTag(2100);
    writer.writeMap(values.size());

    for(map<string, string>::iterator it = values.begin(); it != values.end(); ++it) {
        writer.writeString(it->first);
        writer.writeString(it->second);
    }
}

void ResponseGetEmpty::Serialize(CborWriter &writer) {
    writer.writeTag(2101);
    writer.writeMap(0);
}

std::map<std::string, std::string> &ResponseGetOk::GetValues() {
    return values;
}

void ResponseAddOk::Serialize(CborWriter &writer) {
    writer.writeTag(TAG_RESPONSE_ADD_OK);
    writer.writeMap(0);
}

void ResponseAddTraverse::Serialize(CborWriter &writer) {
    writer.writeTag(TAG_RESPONSE_ADD_TRAVERSE);
    writer.writeArray(2);
    writer.writeInt(_initRequestId);
    writer.writeBytes(_ciphertext->data(), _ciphertext->size());
}

ResponseAddTraverse::ResponseAddTraverse() {

}
