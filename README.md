# SortNode

[![npm version](https://badge.fury.io/js/%40techainer1t%2Fsort-node.svg)](https://badge.fury.io/js/%40techainer1t%2Fsort-node) [![CI](https://github.com/Techainer/sort-node/actions/workflows/ci.yml/badge.svg)](https://github.com/Techainer/sort-node/actions/workflows/ci.yml) [![publish-prebuild](https://github.com/Techainer/sort-node/actions/workflows/publish-prebuild.yml/badge.svg)](https://github.com/Techainer/sort-node/actions/workflows/publish-prebuild.yml)

SortNode is a JS binding for SORT: Simple, online, and real-time tracking of multiple objects in a video sequence.

This package is maintained by [Techainer](https://techainer.com)


## Install
To install this package, make sure you have the following dependencies installed:
- NodeJS 14.x.x
- cmake 3.9+
- OpenCV 3.x.x (Build from source recommened)
- Eigen 3.x.x (`sudo apt-get install -y libeigen3-dev`)

Noted that we have provide a `Dockerfile` contain all 3rd dependencies. To use it, build and run the image:
```bash
docker build -t sort .
./docker_run.sh
```
Then you can install the package from npm:

```bash
yarn add sort-node@npm:@techainer1t/sort-node
```

## Example

The `sort-node` package contain the object `SortNode` that can be use to track object detected from a single video or camera.

The `SortNode` object can be initialize with 2 arguments:
- `kMinHits`: (int) Minimum number of hits before a bounding box was assigned a new track ID
- `kMinConfidence`: (float between 0 and 1) Bouding boxes with confidence score less than this value will be ignored

With each frame, you will need to call `update` method.

This method except a single arguments that had a the format `List[List[float]]`, which means a list of detected object in that frame. Each object will have the format: `[x_top, y_top, width, height, confidence]`.

The `update` method will return a list of tracked object in the format `[List[List[int]]`, each object will have the format: `[x_top, y_top, width, height, track_id]`

Please noted that the number of returned object might not be the same as the number of inputed object.

```javascript
const sortnode = require("@techainer1t/sort-node");
const kMinHits = 3;
const kMinConfidence = 0.3;
const tracker = sortnode.SortNode(kMinHists, kMinConfidence);
while (true){
    // Call the object detector
    ...

    // Update the tracker
    tracked = tracker.update(detections);
}
```
## References

The C++ implementation of SORT was written by [yasenh](https://github.com/yasenh) from the repo [yasenh/sort-cpp](https://github.com/yasenh/sort-cpp)
