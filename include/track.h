#pragma once

#include <opencv2/core.hpp>
#include "kalman_filter.h"

class Track {
public:
    // Constructor
    Track();

    // Destructor
    ~Track() = default;

    void Init(const std::pair<cv::Rect, std::vector<float>> &bbox);
    void Predict();
    void Update(const std::pair<cv::Rect, std::vector<float>> &bbox);
    cv::Rect GetStateAsBbox() const;
    float GetNIS() const;

    int coast_cycles_ = 0, hit_streak_ = 0;
    std::vector<float> landmarks;

private:
    Eigen::VectorXd ConvertBboxToObservation(const cv::Rect& bbox) const;
    cv::Rect ConvertStateToBbox(const Eigen::VectorXd &state) const;

    KalmanFilter kf_;
};
