#include "sort_node.h"

namespace sortnode
{
    Napi::Object SortNode::Init(Napi::Env env, Napi::Object exports)
    {
        Napi::HandleScope scope(env);
        Napi::Function func = DefineClass(env,
                                          "SortNode",
                                          {InstanceMethod("update", &SortNode::update)});

        Napi::FunctionReference *constructor = new Napi::FunctionReference();
        *constructor = Napi::Persistent(func);
        // env.SetInstanceData(constructor);

        exports.Set("SortNode", func);
        return exports;
    }

    SortNode::SortNode(const Napi::CallbackInfo& info) : Napi::ObjectWrap<SortNode>(info)
    {
        Napi::Env env = info.Env();
        Napi::HandleScope scope(env);

        if (info.Length() < 2 || info.Length() > 2)
        {
            Napi::TypeError::New(env, "SortTracker constructor received wrong number of arguments: kMinHits, kMinConfidence")
                .ThrowAsJavaScriptException();
            return;
        }

        if (!info[0].IsNumber())
        {
            Napi::TypeError::New(env, "kMinHits must be an interger")
                .ThrowAsJavaScriptException();
            return;
        }

        auto kMinHits = info[0].As<Napi::Number>().DoubleValue();
        if (fmod(kMinHits, 1) != 0)
        {
            Napi::TypeError::New(env, "kMinHits must be an interger")
                .ThrowAsJavaScriptException();
            return;
        }

        if (!info[1].IsNumber() || info[1].As<Napi::Number>().DoubleValue() > 1 || info[1].As<Napi::Number>().DoubleValue() < 0)
        {
            Napi::TypeError::New(env, "kMinConfidence must be a float between 0 and 1")
                .ThrowAsJavaScriptException();
            return;
        }

        this->kMinHits = int(kMinHits);
        this->kkMinConfidence = float(info[1].As<Napi::Number>().DoubleValue());
    }

    Napi::Value SortNode::update(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        // Check and convert input
        if (info.Length() < 1 || info.Length() > 1)
        {
            Napi::TypeError::New(env, "SortTracker::update got wrong number of arguments: dets")
                .ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsArray())
        {
            Napi::TypeError::New(env, "dets must have format: List[List[x1, y1, w, h, conf]]")
                .ThrowAsJavaScriptException();
            return env.Null();
        }

        std::vector<std::vector<float> > dets;
        auto jsList = info[0].As<Napi::Array>();
        auto len = jsList.Length();
        for (uint32_t i = 0; i < len; ++i)
        {
            Napi::Value _each_bbox = jsList[i];
            if (!_each_bbox.IsArray())
            {
                Napi::TypeError::New(env, "dets must have format: List[List[x1, y1, w, h, conf]]")
                    .ThrowAsJavaScriptException();
                return env.Null();
            }
            auto each_bbox = _each_bbox.As<Napi::Array>();
            auto each_bbox_len = each_bbox.Length();
            if (each_bbox_len != 5)
            {
                Napi::TypeError::New(env, "each bbox must have format [x1, y1, w, h, conf]")
                    .ThrowAsJavaScriptException();
                return env.Null();
            }

            std::vector<float> current_bbox;
            for (uint32_t j = 0; j < 5; j++)
            {
                Napi::Value _val = each_bbox[j];
                if (!_val.IsNumber())
                {
                    Napi::TypeError::New(env, "each value x1, y1, w, h, conf must be a number")
                        .ThrowAsJavaScriptException();
                    return env.Null();
                }
                current_bbox.push_back((float)_val.As<Napi::Number>().DoubleValue());
            }
            dets.push_back(current_bbox);
        }

        // Actuall processing start here

        this->frame_index++;

        // Convert vector of number to cv::Rect to use
        std::vector<cv::Rect> bbox_per_frame;
        for (auto &each_bbox : dets)
        {
            if (each_bbox[4] >= this->kkMinConfidence)
            {
                bbox_per_frame.emplace_back(each_bbox[0], each_bbox[1], each_bbox[2], each_bbox[3]);
            }
        }

        // Run SORT tracker
        this->tracker.Run(bbox_per_frame);
        const auto tracks = this->tracker.GetTracks();

        // Convert results from cv::Rect to normal int vector
        std::vector<std::vector<int> > res;
        for (auto &trk : tracks)
        {
            const auto &bbox = trk.second.GetStateAsBbox();
            // Note that we will not export coasted tracks
            // If we export coasted tracks, the total number of false negative will decrease (and maybe ID switch)
            // However, the total number of false positive will increase more (from experiments),
            // which leads to MOTA decrease
            // Developer can export coasted cycles if false negative tracks is critical in the system
            if (trk.second.coast_cycles_ < this->kMaxCoastCycles && (trk.second.hit_streak_ >= this->kMinHits || this->frame_index < this->kMinHits))
            {
                std::vector<int> current_object{bbox.tl().x, bbox.tl().y, bbox.width, bbox.height, trk.first};
                // Last value is track id
                res.push_back(current_object);
            }
        }

        // Now return it back as JS array
        auto jsOutputList = Napi::Array::New(env);
        for (uint32_t i = 0; i < res.size(); i++)
        {
            auto bbox = res[i];
            auto jsBbox = Napi::Array::New(env);
            for (uint32_t j = 0; j < bbox.size(); j++)
            {
                jsBbox[j] = Napi::Number::New(env, bbox[j]);
            }
            jsOutputList[i] = jsBbox;
        }
        return jsOutputList;
    }
} // namespace sortnode