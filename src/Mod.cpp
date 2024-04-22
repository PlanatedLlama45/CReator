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

std::string getFilenameFromPath(strRef path) {
    size_t delim = path.find_last_of('\\') + 1;
    if (delim == 0)
        return path;
    std::string name = "";
    for (size_t i = delim; i < path.length(); i++)
        name += path[i];
    return name;
}

std::string Block::toString(Mod *mod) const {
    return
        "{\n"
        "  \"stringId\": \"" + mod->getId() + ':' + id + "\",\n"
        "  \"blockStates\": {\n"
        "    \"default\": {\n"
        "      \"modelName\": \"" + mod->getId() + '_' + id + "\",\n"
        "      \"blockEventsId\": \"base:block_events_default\",\n"
        "      \"isOpaque\": true,\n"
        "      \"generateSlabs\": " + (slabs ? "true" : "false") + "\n"
        "    }\n"
        "  }\n"
        "}\n";
}

std::string Block::modelToString(Mod *mod) {
    std::string result =
        "{\n"
        "  \"parent\": \"cube\",\n"
        "  \"textures\": {\n";
    for (auto [face, path] : faces) {
        result +=
            "    \"" + getTextureFaceStr(face) + "\": {\n"
            "      \"fileName\": \"" + mod->getId() + '/' + getFilenameFromPath(path) + "\"\n"
            "    },\n";
    }
    if (!faces.empty())
        result.erase(result.length() - 2, 1);
    result +=
        "  }\n"
        "}\n";
    return result;
}

Mod::Mod(strRef id, strRef name) {
    this->id = id;
    this->name = name;
}

Mod::~Mod() {
    for (Block *b : blocks)
        delete b;
}

void Mod::addBlock(Block *block) {
    blocks.push_back(block);
}
