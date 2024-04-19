#pragma once

#include "config.h"
#include "Map.h"

enum class TextureFace { all, top, down, side };

extern std::string getTextureFaceStr(const TextureFace &face);
extern std::string getFilenameFromPath(strRef path);

class Mod;

struct Block {
    std::string id;
    std::string name;
    Map<TextureFace, std::string> faces;
    bool slabs;

    std::string toString(Mod *mod) const;
    std::string modelToString(Mod *mod);
};

class Mod {
public:
    Mod(strRef id, strRef name);
    ~Mod();

    getter(id, strRef, Id)
    getter(name, strRef, Name)

    setter(id, strRef, Id)
    setter(name, strRef, Name)

    getter_norm(blocks, std::vector<Block *> &, Blocks)

    void addBlock(Block *block);

private:
    std::string id;
    std::string name;
    std::vector<Block *> blocks;
};
