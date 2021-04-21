#ifndef SORTNODE_H
#define SORTNODE_H

#include <iostream>
#include <vector>
#include <opencv2/core.hpp>
#include "tracker.h"
#include <napi.h>

namespace sortnode
{
    class SortNode : public Napi::ObjectWrap<SortNode>
    {
    public:
        int kMinHits = 3;
        int kMaxCoastCycles = 1;
        float kkMinConfidence = 0.6;
        int frame_index = 0;
        Tracker tracker;

        static Napi::Object Init(Napi::Env env, Napi::Object exports);
        SortNode(const Napi::CallbackInfo& info);

        Napi::Value update(const Napi::CallbackInfo& info);
    };
} // namespace sortnode
#endif