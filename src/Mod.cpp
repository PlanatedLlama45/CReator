#include "Mod.h"

std::string getTextureFaceStr(const TextureFace &face) {
    switch (face) {
    case TextureFace::all: return "all";
    case TextureFace::top: return "top";
    case TextureFace::down: return "down";
    case TextureFace::side: return "side";
    }
    return "";
}

std::string Block::toString(const Mod &mod) const {
    return
        "{"
        "    \"stringId\": \"" + mod.getId() + ':' + id + "\","
        "    \"blockStates\": {"
        "        \"default\": {"
        "            \"modelName\": \"" + mod.getId() + '_' + id + "\","
        "            \"blockEventsId\": \"base:block_events_default\","
        "            \"isOpaque\": true,"
        "            \"generateSlabs\": " + (slabs ? "true" : "false") +
        "        }"
        "    }"
        "}";
}

Mod::Mod(strRef id, strRef name) {
    this->id = id;
    this->name = name;
}

Mod::~Mod() { }

void Mod::addBlock(Block *block) {
    blocks.push_back(block);
}
