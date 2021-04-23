const { readFileSync, existsSync, writeFileSync, mkdirSync, rmdirSync, appendFileSync } = require("fs")
const sortnode = require("../lib");
const assert = require("assert");

assert(sortnode.SortNode, "The expected module is undefined");

function testBasic() {
    console.log("Running testBasic");
    const kMinHits = 3;
    const kMinConfidence = 0.3;
    const instance = new sortnode.SortNode(kMinHits, kMinConfidence);
    assert(instance.update, "The expected method is not defined");
}

function ProcessLabel(dataFile) {
    // Process labels - group bounding boxes by frame index
    let bbox = []
    let bbox_per_frame = []
    // Label index starts from 1
    let current_frame_index = 1;
    for (const line of dataFile.toString().split('\n')) {
        const label = line.split(',')
        if (parseInt(label[0]) != current_frame_index) {
            current_frame_index = parseInt(label[0]);
            bbox.push(bbox_per_frame);
            bbox_per_frame = []
        }
        const object = [parseFloat(label[2]), parseFloat(label[3]), parseFloat(label[4]), parseFloat(label[5]), parseFloat(label[6])]
        bbox_per_frame.push(object)
    }
    // Add bounding boxes from last frame
    bbox.push(bbox_per_frame);
    return bbox;
}

function getDataset() {
    const dataset_name = "ADL-Rundle-6";
    // Open label file and load detections from MOT dataset
    // Note that it can also be replaced by detections from you own detector

    const input_path = "./data/" + dataset_name + "/det.txt";
    if (!existsSync(input_path)) {
        throw new Error("Could not open or find the input test data!!!")
    }
    const dataFile = readFileSync(input_path)
    const all_detections = ProcessLabel(dataFile);

    const gt_path = "./test/" + dataset_name + ".txt";
    const gtFile = readFileSync(gt_path)
    const groundTruth = gtFile.toString().split('\n')
    for (let i = 0; i < groundTruth.length; i++) {
        groundTruth[i] = groundTruth[i].trim();
    }
    groundTruth.splice(-1, 1);
    return [all_detections, groundTruth];
}

function testAccuracyWithoutLandmark() {
    console.log("Running testAccuracyWithoutLandmark");
    const data = getDataset()
    const all_detections = data[0];
    const groundTruth = data[1];

    const total_frames = all_detections.length;

    const tracker = new sortnode.SortNode(3, 0.6);
    let frame_index = 0
    let predicted = [];
    const t1 = Date.now()

    for (let i = 0; i < total_frames; i++) {
        /*** Run SORT tracker ***/
        const detections = all_detections[i];
        const res = tracker.update(detections);
        frame_index++;
        /*** Tracker update done ***/

        for (const object of res) {
            // Export to text file for metrics evaluation
            const bbox = object.bbox;
            const track_id = object.track_id;
            const landmarks = object.landmarks;
            const line = frame_index + "," + track_id + "," + bbox[0] + "," + bbox[1]
                + "," + bbox[2] + "," + bbox[3] + ",1,-1,-1,-1"
            predicted.push(line);
        }
    } // end of iterating all frames
    const t2 = Date.now()
    const time_span = (t2 - t1) / 1000

    console.log("********************************")
    console.log(`Total tracking took: ${time_span}s for ${total_frames} frames`)
    console.log(`FPS = ${total_frames / time_span}`)
    console.log("********************************")

    assert.deepStrictEqual(predicted, groundTruth, "Unexpected value returned");
}

function testKeepLandmark(){
    console.log("Running testKeepLandmark")
    const tracker = new sortnode.SortNode(3, 0.3);

    let input = [
        [120, 240, 50, 70, 0.9, 23, 24, 25, 26, 27, 28, 29, 30],
        [220, 340, 60, 80, 0.92, 33, 34, 35, 36, 37, 38, 39, 40, 51, 52],
        [320, 340, 70, 90, 0.91, 23, 24, 25, 26, 27, 28, 29, 50],
    ]

    let input_2 = [
        [121, 242, 52, 71, 0.91, 123, 124, 125, 126, 127, 128, 129, 130],
        [222, 342, 61, 81, 0.93, 133, 134, 135, 136, 137, 138, 139, 140],
        [321, 344, 73, 91, 0.94, 123, 124, 125, 126, 127, 128, 129, 150, 31, 32],
    ]

    let t1 = Date.now()
    for (let i = 0; i < 500; i++) {
        let res = tracker.update(input)
        for (let i = 0; i < res.length; i++) {
            assert.deepStrictEqual(res[i].landmarks, input[i].slice(5, input[i].length), "Unexpected value returned")
        }

        let res_2 = tracker.update(input_2)
        for (let i = 0; i < res_2.length; i++) {
            assert.deepStrictEqual(res_2[i].landmarks, input_2[i].slice(5, input_2[i].length), "Unexpected value returned")
        }
    }
    let t2 = Date.now()
    let time_span = (t2 - t1) / 1000

    console.log("********************************")
    console.log(`Total tracking with landmarks took: ${time_span}s for ${1000} frames`)
    console.log(`FPS = ${1000 / time_span}`)
    console.log("********************************")
}

function testInvalidParams() {
    console.log("Running testInvalidParams");
    const instance = new SortNode("lol_hehe");
}

assert.doesNotThrow(testBasic, undefined, "testBasic threw an expection");
assert.doesNotThrow(testAccuracyWithoutLandmark, undefined, "testAccuracyWithoutLandmark threw an expection");
assert.doesNotThrow(testKeepLandmark, undefined, "testKeepLandmark threw an expection");
assert.throws(testInvalidParams, undefined, "testInvalidParams didn't throw");

console.log("Tests passed- everything looks OK!");