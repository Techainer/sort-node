const sort_node = require("bindings")("sort-cpp.node");

// const bindings = require("bindings");
// const path = require("path");

// const moduleRoot = bindings.getRoot(bindings.getFileName());
// console.log(moduleRoot)
// const buildFolder = path.join(moduleRoot, "build", "Release");
// sort_node.initenv(`${buildFolder};${process.env.path}`);

module.exports = sort_node;
