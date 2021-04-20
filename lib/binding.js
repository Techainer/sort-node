const addon = require('../build/Release/sort-node-native');

function SortNode(name) {
    this.greet = function(str) {
        return _addonInstance.greet(str);
    }

    var _addonInstance = new addon.SortNode(name);
}

module.exports = SortNode;
