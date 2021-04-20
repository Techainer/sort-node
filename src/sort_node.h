#pragma once

#include <iostream>
#include <vector>
#include <opencv2/core.hpp>
#include "tracker.h"
#include <napi.h>

class SortNode : public Napi::ObjectWrap<SortNode>
{
public:
    int kMinHits = 3;
    int kMaxCoastCycles = 1;
    float kkMinConfidence = 0.6;
    int frame_index = 0;
    Tracker tracker;

    SortNode(const Napi::CallbackInfo&);
    Napi::Value update(const Napi::CallbackInfo&);

    static Napi::Function GetClass(Napi::Env);
};
