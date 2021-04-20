#pragma once

#include <napi.h>

class SortNode : public Napi::ObjectWrap<SortNode>
{
public:
    SortNode(const Napi::CallbackInfo&);
    Napi::Value Greet(const Napi::CallbackInfo&);

    static Napi::Function GetClass(Napi::Env);

private:
    std::string _greeterName;
};
