//
// Created by Igor Glotov on 19/01/14.
// Copyright (c) 2014 undefware. All rights reserved.
//

#include "Response.h"
#include "Tags.h"

void ResponseWrapper::Serialize(CborWriter &writer) {
  writer.writeTag(TAG_RESPONSE_WRAPPER);
  writer.writeArray(2);
  writer.writeInt(id);

  response->Serialize(writer);
}

void ResponseGet::Serialize(CborWriter &writer) {

}
