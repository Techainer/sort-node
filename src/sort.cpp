#include <iostream>
#include <vector>
#include <opencv2/core.hpp>
#include "tracker.h"

class SortTracker
{
public:
    int kMinHits = 3;
    int kMaxCoastCycles = 1;
    float kkMinConfidence = 0.6;
    int frame_index = 0;
    Tracker tracker;

    SortTracker()
    {
        std::cout << "Initailized a new tracker" << std::endl;
    }

    SortTracker(int kMinHits, float kkMinConfidence)
    {
        std::cout << "Initailized a new tracker" << std::endl;
        this->kMinHits = kMinHits;
        this->kkMinConfidence = kkMinConfidence;
    }

    std::vector<std::vector<int>> update(std::vector<std::vector<float>> dets)
    {
        // dets have format [[x1, y1, w, h, conf], ...]

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
        return res;
    }
};