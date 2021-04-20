/**
 * SORT: A Simple, Online and Realtime Tracker
 */

#include <iostream>
#include <fstream>
#include <map>
#include <random>
#include <chrono>

#include <opencv2/core.hpp>
#include <boost/filesystem.hpp>

#include "tracker.h"
#include "utils.h"
#include "sort.cpp"


std::vector<std::vector<std::vector<float>>> ProcessLabel(std::ifstream& label_file) {
    // Process labels - group bounding boxes by frame index
    std::vector<std::vector<std::vector<float>>> bbox;
    std::vector<std::vector<float>> bbox_per_frame;
    // Label index starts from 1
    int current_frame_index = 1;
    std::string line;

    while (std::getline(label_file, line)) {
        std::stringstream ss(line);
        // Label format <frame>, <id>, <bb_left>, <bb_top>, <bb_width>, <bb_height>, <conf>, <x>, <y>, <z>
        std::vector<float> label;
        std::string data;
        while (getline(ss , data, ',')) {
            label.push_back(std::stof(data));
        }

        if (static_cast<int>(label[0]) != current_frame_index) {
            current_frame_index = static_cast<int>(label[0]);
            bbox.push_back(bbox_per_frame);
            bbox_per_frame.clear();
        }

        std::vector<float> object{label[2], label[3], label[4], label[5], label[6]};
        bbox_per_frame.push_back(object);
    }
    // Add bounding boxes from last frame
    bbox.push_back(bbox_per_frame);
    return bbox;
}


int main(int argc, const char *argv[]) {
    // All training dataset in MOT15
    std::vector<std::string> dataset_names{"ADL-Rundle-6"};

    // For each dataset 
    for (const auto& dataset_name : dataset_names) {
        // Open label file and load detections from MOT dataset
        // Note that it can also be replaced by detections from you own detector
        std::string label_path = "../data/" + dataset_name + "/det.txt";
        std::ifstream label_file(label_path);
        if (!label_file.is_open()) {
            std::cerr << "Could not open or find the label!!!" << std::endl;
            return -1;
        }
        std::vector<std::vector<std::vector<float>>> all_detections = ProcessLabel(label_file);
        // Close label file
        label_file.close();

        // Create output folder if it does not exist
        std::string output_folder = "../output/";
        boost::filesystem::path output_folder_path(output_folder);
        if(boost::filesystem::create_directory(output_folder_path)) {
            std::cerr<< "Directory Created: "<< output_folder <<std::endl;
        }

        std::string output_path = output_folder + dataset_name + ".txt";
        std::ofstream output_file(output_path);

        if (output_file.is_open()) {
            std::cout << "Result will be exported to " << output_path << std::endl;
        } else {
            std::cerr << "Unable to open output file" << std::endl;
            return -1;
        }

        size_t total_frames = all_detections.size();

        SortTracker tracker;

        auto t1 = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < total_frames; i++) {
            /*** Run SORT tracker ***/
            auto detections = all_detections[i];
            const auto res = tracker.update(detections);
            /*** Tracker update done ***/

            for (auto &object : res) {
                // Export to text file for metrics evaluation
                output_file << tracker.frame_index << "," << object[4] << "," << object[0] << "," << object[1]
                            << "," << object[2] << "," << object[3] << ",1,-1,-1,-1\n";
            }
        } // end of iterating all frames
        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);

        std::cout << "********************************" << std::endl;
        std::cout << "Total tracking took: " << time_span.count() << " for " << total_frames << " frames" << std::endl;
        std::cout << "FPS = " << total_frames / time_span.count() << std::endl;
        std::cout << "********************************" << std::endl;

        output_file.close();
    } // end of iterating all dataset
    return 0;
}
