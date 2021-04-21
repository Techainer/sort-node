const { readFileSync, existsSync, writeFileSync, mkdirSync, rmdirSync, appendFileSync } = require("fs")
const sortnode = require("../lib");
const assert = require("assert");

assert(sortnode.SortNode, "The expected module is undefined");

function testBasic()
{
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

function testAccuracy()
{
    const dataset_name = "ADL-Rundle-6";
    const tracker = new sortnode.SortNode(3, 0.6);
    // Open label file and load detections from MOT dataset
    // Note that it can also be replaced by detections from you own detector

    const input_path = "./data/" + dataset_name + "/det.txt";
    if (!existsSync(input_path)) {
        throw new Error("Could not open or find the input test data!!!")
    }
    const dataFile = readFileSync(input_path)
    const all_detections = ProcessLabel(dataFile);

    const gt_path = "./output/" + dataset_name + ".txt";
    const gtFile = readFileSync(gt_path)
    const groundTruth = gtFile.toString().split('\n')
    groundTruth.splice(-1, 1);

    const total_frames = all_detections.length;

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
            const line = frame_index + "," + object[4] + "," + object[0] + "," + object[1]
                + "," + object[2] + "," + object[3] + ",1,-1,-1,-1"
            predicted.push(line);
        }
    } // end of iterating all frames
    const t2 = Date.now()
    const time_span = (t2 - t1) / 1000

    console.log("********************************")
    console.log(`Total tracking took: ${time_span}s for ${total_frames} frames`)
    console.log(`FPS = ${total_frames / time_span}`)
    console.log("********************************")

    assert.deepEqual(predicted, groundTruth, "Unexpected value returned");

}

function testInvalidParams()
{
    const instance = new SortNode("lol_hehe");
}

assert.doesNotThrow(testBasic, undefined, "testBasic threw an expection");
assert.doesNotThrow(testAccuracy, undefined, "testAccuracy threw an expection");
assert.throws(testInvalidParams, undefined, "testInvalidParams didn't throw");

console.log("Tests passed- everything looks OK!");