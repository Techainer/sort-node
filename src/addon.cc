#include <napi.h>
#include "sort_node.h"

using namespace sortnode;

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  SortNode::Init(env, exports);
  return exports;
}

NODE_API_MODULE(sortnode, Init)
