const sortnode = require("../lib");
console.log("Hehe loaded")
const assert = require("assert");

assert(sortnode.SortNode, "The expected module is undefined");

function testBasic()
{
    const kMinHits = 3;
    const kMinConfidence = 0.3;
    const instance = new sortnode.SortNode(kMinHits, kMinConfidence);
    assert(instance.update, "The expected method is not defined");

    // assert.strictEqual(instance.update("kermit"), "mr-yeoman", "Unexpected value returned");
}

// function testInvalidParams()
// {
//     const instance = new SortNode("lol_hehe");
// }

assert.doesNotThrow(testBasic, undefined, "testBasic threw an expection");
// assert.throws(testInvalidParams, undefined, "testInvalidParams didn't throw");

console.log("Tests passed- everything looks OK!");