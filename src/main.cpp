#include "main.h"

Scene *currentScene = nullptr;
Map<std::string, Scene *> scenes = { };
Mod *currentMod = nullptr;
Block *currentBlock = nullptr;
std::vector<Mod *> mods = { };
fs::path cosmicReachDir = fs::path(getenv("LOCALAPPDATA")) / "cosmic-reach";
TextureFace currentFace = TextureFace::all;
// std::string openFolderCommand = "explorer " +  + "\\cosmic-reach";

ButtonColorScheme colorScheme_button {
    { WHITE, LIGHTGRAY },
    { WHITE, GRAY },
    { RAYWHITE, GRAY }
};
TextEditColorScheme colorScheme_textEdit {
    colorScheme_button.normal,
    colorScheme_button.hovered,
    colorScheme_button.pressed,
    BLACK
};
TextEditColorScheme colorScheme_textEdit_red {
    colorScheme_button.normal,
    colorScheme_button.hovered,
    colorScheme_button.pressed,
    RED
};
ColorScheme colorScheme_scrollBox { GRAY, DARKGRAY };
TextCheckboxColorScheme colorScheme_textCheckbox {
    { WHITE, LIGHTGRAY },
    { WHITE, GRAY },
    BLACK,
    BLACK
};

int main(int argc, const char **argv) {
    InitWindow(600, 480, "CRreator");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    createUI();
    
    loadMods();

    Map<std::string, UiElement *> &modSelectorScrollBoxElements = currentScene->getUiElement<ScrollBox>("mod-list").getElements();
    Map<std::string, UiElement *> &blockSelectorScrollBoxElements = getScene("mod-edit").getUiElement<ScrollBox>("block-list").getElements();

    while (!WindowShouldClose()) {
        // Mod selector screen
        if (isScene("mod-selector")) {
            if (isButtonPressed("add-mod")) {
                setScene("mod-creator");
            } else if (isButtonPressed("exit")) {
                break;
            } else {
                for (auto [id, elem] : modSelectorScrollBoxElements) {
                    if (((Button *)elem)->isPressed()) {
                        setMod(id);
                        setScene("mod-edit");
                    }
                }
            }
        }
        // Mod creator screen
        else if (isScene("mod-creator")) {
            if (isButtonPressed("back")) {
                setScene("mod-selector");
            } else if (isButtonPressed("done")) {
                std::string id = currentScene->getUiElement<TextEdit>("mod-id").getText();
                createMod(
                    id,
                    currentScene->getUiElement<TextEdit>("mod-name").getText()
                );
                setScene("mod-selector");
            }
        }
        // Mod edit screen
        else if (isScene("mod-edit")) {
            if (isButtonPressed("back")) {
                std::string oldId = currentMod->getId();
                currentMod->setId(currentScene->getUiElement<TextEdit>("id-edit").getText());
                currentMod->setName(currentScene->getUiElement<TextEdit>("name-edit").getText());

                auto modPair = modSelectorScrollBoxElements.find(oldId);
                delete modPair->second;
                *modPair = { currentMod->getId(), new TextButton(modTextButtonArgs_noId(currentMod)) };

                dumpMods();
                setScene("mod-selector");
            } else if (isButtonPressed("delete")) {
                setScene("mod-delete-confirm");
            } else if (isButtonPressed("add-block")) {
                setScene("block-creator");
            } else if (isButtonPressed("export")) {
                exportCurrentMod();
                setScene("export-done");
            } else {
                for (auto [id, elem] : blockSelectorScrollBoxElements) {
                    if (((Button *)elem)->isPressed()) {
                        setBlock(id);
                        setScene("block-edit");
                    }
                }
            }
        }
        // Mod deletion confirmation screen
        else if (isScene("mod-delete-confirm")) {
            if (isButtonPressed("yes")) {
                getScene("mod-selector").getUiElement<ScrollBox>("mod-list").removeElement(currentMod->getId());
                for (auto it = mods.begin(); it != mods.end(); it++) {
                    if (*it == currentMod) {
                        mods.erase(it);
                        break;
                    }
                }
                delete currentMod;
                setScene("mod-selector");
            } else if (isButtonPressed("nuh-uh")) {
                setScene("mod-edit");
            }
        }
        // Block edit screen
        else if (isScene("block-edit")) {
            if (isButtonPressed("back")) {
                std::string oldId = currentBlock->id;
                currentBlock->id = currentScene->getUiElement<TextEdit>("id-edit").getText();
                currentBlock->name = currentScene->getUiElement<TextEdit>("name-edit").getText();
                currentBlock->slabs = currentScene->getUiElement<TextCheckbox>("slabs-checkbox").isChecked();

                auto blockPair = blockSelectorScrollBoxElements.find(oldId);
                delete blockPair->second;
                *blockPair = { currentBlock->id, new TextButton(blockTextButtonArgs_noId(currentBlock)) };

                dumpMods();
                setScene("mod-edit");
            } else if (isButtonPressed("delete")) {
                setScene("block-delete-confirm");
            }
            blockEditFaceChooseLogic("all", TextureFace::all, "All")
            blockEditFaceChooseLogic("top", TextureFace::top, "Top")
            blockEditFaceChooseLogic("side", TextureFace::side, "Side")
            blockEditFaceChooseLogic("down", TextureFace::down, "Down")
        }
        // Faces edit screen
        else if (isScene("face-edit")) {
            if (isButtonPressed("back")) {
                currentScene->getUiElement<ImageBox>("face-image").setImage("");
                currentScene->getUiElement("face-paste").setHidden(false);
                currentScene->getUiElement("face-delete").setHidden(true);
                setScene("block-edit");
            } else if (isButtonPressed("face-paste")) {
                const char *clipboard = GetClipboardText();
                std::string texturePath = (clipboard ? clipboard : "");
                if (isValidTexture(texturePath)) {
                    currentScene->getUiElement<ImageBox>("face-image").setImage(texturePath);
                    currentBlock->faces.insert(currentFace, texturePath);
                    currentScene->getUiElement("face-paste").setHidden(true);
                    currentScene->getUiElement("face-delete").setHidden(false);
                }
            } else if (isButtonPressed("face-delete")) {
                currentScene->getUiElement<ImageBox>("face-image").setImage("");
                currentBlock->faces.erase(currentFace);
                currentScene->getUiElement("face-paste").setHidden(false);
                currentScene->getUiElement("face-delete").setHidden(true);
            }
        }
        // Block deletion confirmation screen
        else if (isScene("block-delete-confirm")) {
            if (isButtonPressed("yes")) {
                std::cout << currentBlock->id << '\n';
                getScene("mod-edit").getUiElement<ScrollBox>("block-list").removeElement(currentBlock->id);
                std::vector<Block *> &blocks = currentMod->getBlocks();
                for (auto it = blocks.begin(); it != blocks.end(); it++) {
                    if (*it == currentBlock) {
                        delete *it;
                        blocks.erase(it);
                        break;
                    }
                }
                setScene("mod-edit");
            } else if (isButtonPressed("nuh-uh")) {
                setScene("block-edit");
            }
        }
        // Block creator screen
        else if (isScene("block-creator")) {
            if (isButtonPressed("back")) {
                setScene("mod-edit");
            } else if (isButtonPressed("done")) {
                createBlock(
                    new Block {
                        currentScene->getUiElement<TextEdit>("block-id").getText(),
                        currentScene->getUiElement<TextEdit>("block-name").getText(),
                        { },
                        false
                    }
                );
                setScene("mod-edit");
            }
        }
        // Export doen screen
        else if (isScene("export-done")) {
            if (isButtonPressed("ok-btn")) {
                setScene("mod-edit");
            }
        }

        SetMouseCursor(MOUSE_CURSOR_DEFAULT);

        currentScene->update();

#ifdef DEBUG
    system("clear");
    std::cout << "Begin Drawing\n";
#endif // DEBUG
        BeginDrawing();
            ClearBackground(RAYWHITE);
            currentScene->draw();
        EndDrawing();
#ifdef DEBUG
    std::cout << "End Drawing\n";
#endif // DEBUG
    }

    CloseWindow();

    dumpMods();
    for (Mod *m : mods)
        delete m;
    for (auto [id, scene] : scenes)
        delete scene;

    return 0;
}

void createUI() {
    addScene("mod-selector")
        .addScrollBox("mod-list", { 395, 240, }, { 410, 480 }, true, 15, 5, "No mods yet", 30, colorScheme_scrollBox, 2)
        .addTextButton("add-mod", { 95, 30 }, { 180, 50 }, true, "Add", 30, true, colorScheme_textEdit, 2)
        .addTextButton("exit", { 95, 450 }, { 180, 50 }, true, "Exit", 30, true, colorScheme_textEdit_red, 2);
    setScene("mod-selector");

    addScene("mod-creator")
        .addTextEdit("mod-id", { 300, 165 }, { 280, 40 }, true, 24, true, colorScheme_textEdit, 2, "Enter mod ID", false)
        .addTextEdit("mod-name", { 300, 215 }, { 280, 40 }, true, 24, true, colorScheme_textEdit, 2, "Enter mod name", false)
        .addTextButton("done", { 300, 265 }, { 180, 40 }, true, "Done", 30, true, colorScheme_textEdit, 2)
        .addTextButton("back", { 300, 315 }, { 180, 40 }, true, "Back", 30, true, colorScheme_textEdit,  2);
    
    addScene("mod-edit")
        .addStaticText("id-label", { 300, 20 }, true, "Mod ID", 24, BLACK)
        .addTextEdit("id-edit", { 300, 55 }, { 280, 40 }, true, 24, true, colorScheme_textEdit, 2, "", false)
        .addStaticText("name-label", { 300, 95 }, true, "Mod Name", 24, BLACK)
        .addTextEdit("name-edit", { 300, 135 }, { 280, 40 }, true, 24, true, colorScheme_textEdit, 2, "", false)
        .addScrollBox("block-list", { 0, 255, }, { 600, 270 }, false, 15, 5, "No blocks yet", 30, colorScheme_scrollBox, 2)
        .addTextButton("add-block", { 300, 185 }, { 590, 40 }, true, "Add Block", 30, true, colorScheme_textEdit, 2)
        .addTextButton("export", { 100, 230 }, { 190, 40 }, true, "Export", 30, true, colorScheme_textEdit, 2)
        .addTextButton("delete", { 300, 230 }, { 190, 40 }, true, "Delete", 30, true, colorScheme_textEdit_red, 2)
        .addTextButton("back", { 500, 230 }, { 190, 40 }, true, "Back", 30, true, colorScheme_textEdit, 2);
    
    addScene("block-edit")
        // Id edit
        .addStaticText("id-label", { 300, 20 }, true, "Block ID", 24, BLACK)
        .addTextEdit("id-edit", { 300, 55 }, { 280, 40 }, true, 24, true, colorScheme_textEdit, 2, "", false)
        // Name edit
        .addStaticText("name-label", { 300, 95 }, true, "Block Name", 24, BLACK)
        .addTextEdit("name-edit", { 300, 135 }, { 280, 40 }, true, 24, true, colorScheme_textEdit, 2, "", false)

        .addTextCheckbox("slabs-checkbox", { 200, 190 }, { 30, 30 }, true, "Generate slabs?", 24, 25, colorScheme_textCheckbox)
        .addTextCheckbox("pre", { 200, 190 }, { 30, 30 }, true, "Generate slabs?", 24, 25, colorScheme_textCheckbox)
        
        .addStaticText("face-label", { 300, 240 }, true, "Face Textures", 30, BLACK)
        .addTextButton("face-all", { 150, 300 }, { 280, 40 }, true, "All", 30, true, colorScheme_textEdit, 2)
        .addTextButton("face-top", { 450, 300 }, { 280, 40 }, true, "Top", 30, true, colorScheme_textEdit, 2)
        .addTextButton("face-side", { 150, 360 }, { 280, 40 }, true, "Side", 30, true, colorScheme_textEdit, 2)
        .addTextButton("face-down", { 450, 360 }, { 280, 40 }, true, "Down", 30, true, colorScheme_textEdit, 2)

        .addTextButton("delete", { 150, 455 }, { 290, 40 }, true, "Delete", 30, true, colorScheme_textEdit_red, 2)
        .addTextButton("back", { 450, 455 }, { 290, 40 }, true, "Back", 30, true, colorScheme_textEdit, 2);
    
    addScene("face-edit")
        .addStaticText("face-label", { 300, 50 }, true, "All", 30, BLACK)
        .addImageBox("face-image", { 300, 160 }, { 160, 160 }, true, "", LIGHTGRAY, 2)
        .addTextButton("face-paste", { 300, 290 }, { 280, 50 }, true, "Paste an image path", 24, true, colorScheme_textEdit, 2)
        .addTextButton("face-delete", { 300, 290 }, { 280, 50 }, true, "Clear image", 24, true, colorScheme_textEdit, 2)

        .addTextButton("back", { 300, 455 }, { 300, 40 }, true, "Back", 30, true, colorScheme_textEdit, 2);
    
    getScene("face-edit").getUiElement("face-delete").setHidden(true);

    addScene("block-creator")
        .addTextEdit("block-id", { 300, 165 }, { 280, 40 }, true, 24, true, colorScheme_textEdit, 2, "Enter block ID", false)
        .addTextEdit("block-name", { 300, 215 }, { 280, 40 }, true, 24, true, colorScheme_textEdit, 2, "Enter block name", false)
        .addTextButton("done", { 300, 265 }, { 180, 40 }, true, "Done", 30, true, colorScheme_textEdit, 2)
        .addTextButton("back", { 300, 315 }, { 180, 40 }, true, "Back", 30, true, colorScheme_textEdit,  2);
    
    addScene("export-done")
        .addStaticText("text", { 300, 200 }, true, "   The mod has been\n\nexported successfully", 30, BLACK)
        .addTextButton("ok-btn", { 300, 260 }, { 180, 50 }, true, "Ok", 30, true, colorScheme_textEdit, 2);
    
    addScene("mod-delete-confirm")
        .addStaticText("question", { 300, 200 }, true, "Are you sure you want to delete this mod?", 24, BLACK)
        .addTextButton("yes", { 205, 260 }, { 180, 50 }, true, "Yes", 30, true, colorScheme_textEdit_red, 2)
        .addTextButton("nuh-uh", { 395, 260 }, { 180, 50}, true, "No", 30, true, colorScheme_textEdit, 2);

    addScene("block-delete-confirm")
        .addStaticText("question", { 300, 200 }, true, "Are you sure you want to delete this block?", 24, BLACK)
        .addTextButton("yes", { 205, 260 }, { 180, 50 }, true, "Yes", 30, true, colorScheme_textEdit_red, 2)
        .addTextButton("nuh-uh", { 395, 260 }, { 180, 50 }, true, "No", 30, true, colorScheme_textEdit, 2);
}

Scene &addScene(strRef name) {
    scenes.insert(name, new Scene(name));
    return *scenes[name];
}

Scene &getScene(strRef name) {
    return *scenes[name];
}

Scene *setScene(strRef name) {
    if (scenes.empty() || !scenes.contains(name))
        return { };
    currentScene = scenes[name];
    return currentScene;
}

void loadMods() {
    std::ifstream file("../mods");

    std::string line;
    std::string id, name;

    std::vector<Block *> blocks;
    std::string blockId, blockName, blockSlabs;
    std::string fullFace;
    std::string faceName, facePath;

    size_t safety = 0;

    while (!file.eof()) {
        std::getline(file, id, '|');
        std::getline(file, name, '|');
        if (id.empty() || name.empty())
            break;
        while (safety < 100) {
            std::getline(file, blockId, '|');
            if (blockId == "==")
                break;
            std::getline(file, blockName, '|');
            std::getline(file, blockSlabs, '|');

            blocks.push_back(new Block{ blockId, blockName, { }, (blockSlabs == "true") });

            for (size_t i = 0; i < 4; i++) {
                std::getline(file, fullFace, '|');
                if (fullFace == "=")
                    break;
                
                size_t delim = fullFace.find(':');
                faceName = fullFace.substr(0, delim);
                facePath = fullFace.substr(delim + 1, fullFace.length());

                if (faceName == "all")
                    blocks.back()->faces.insert(TextureFace::all, facePath);
                else if (faceName == "top")
                    blocks.back()->faces.insert(TextureFace::top, facePath);
                else if (faceName == "down")
                    blocks.back()->faces.insert(TextureFace::down, facePath);
                else if (faceName == "side")
                    blocks.back()->faces.insert(TextureFace::side, facePath);
            }
            safety++;
        }

        mods.push_back(new Mod(id, name));

        for (Block *b : blocks)
            mods.back()->addBlock(b);

        blocks = { };
    }

    file.close();

    ScrollBox &mainMenuScrollBox = getScene("mod-selector").getUiElement<ScrollBox>("mod-list");

    for (Mod *mod : mods) {
        mainMenuScrollBox.addTextButton(
            modTextButtonArgs(mod)
        );
    }
}

bool checkTexture(strRef filename) {
    return IsFileExtension(filename.c_str(), ".png");;
}

void dumpMods() {
    std::ofstream file("../mods");

    std::vector<Block *> blocks;
    for (Mod *mod : mods) {
        blocks = mod->getBlocks();

        file << mod->getId() << '|' << mod->getName() << '|';
        for (Block *b : blocks) {
            file << b->id << '|' << b->name << '|' << (b->slabs ? "true" : "false") << '|';
            for (auto [face, path] : b->faces)
                file << getTextureFaceStr(face) << ':' << path << '|';
            file << "=|";
        }
        file << "==|";
    }

    file.close();
}

bool modExists(strRef id) {
    for (Mod *mod : mods) {
        if (mod->getId() == id)
            return true;
    }
    return false;
}

void createMod(strRef id, strRef name) {
    if (!modExists(id)) {
        mods.push_back(new Mod(id, name));
        currentMod = mods.back();

        getScene("mod-selector").getUiElement<ScrollBox>("mod-list").addTextButton(
            modTextButtonArgs(currentMod)
        );
    }
}

bool blockExists(strRef id) {
    for (Block *b : currentMod->getBlocks()) {
        if (b->id == id)
            return true;
    }
    return false;
}

void createBlock(Block *b) {
    if (!blockExists(b->id)) {
        currentMod->getBlocks().push_back(b);

        getScene("mod-edit").getUiElement<ScrollBox>("block-list").addTextButton(
            blockTextButtonArgs(b)
        );
    }
}

void setMod(strRef id) {
    for (Mod *mod : mods) {
        if (mod->getId() == id) {
            currentMod = mod;
            break;
        }
    }

    Scene &scene = getScene("mod-edit");

    scene.getUiElement<TextEdit>("id-edit").setText(currentMod->getId());
    scene.getUiElement<TextEdit>("name-edit").setText(currentMod->getName());

    ScrollBox &blockList = scene.getUiElement<ScrollBox>("block-list");
    blockList.clearElements();
    for (Block *b : currentMod->getBlocks()) {
        blockList.addTextButton(
            blockTextButtonArgs(b)
        );
    }
}

void setBlock(strRef id) {
    for (Block *b : currentMod->getBlocks()) {
        if (b->id == id) {
            currentBlock = b;
            break;
        }
    }

    Scene &scene = getScene("block-edit");

    scene.getUiElement<TextEdit>("id-edit").setText(currentBlock->id);
    scene.getUiElement<TextEdit>("name-edit").setText(currentBlock->name);
    scene.getUiElement<TextCheckbox>("slabs-checkbox").setChecked(currentBlock->slabs);
}

std::string addEllipsis(std::string text, float width, int fontSize) {
    float textWidth = MeasureText(text.c_str(), fontSize);
    if (textWidth <= width)
        return text;
    std::string textEllipsis;
    do {
        text.erase(--text.end());
        textEllipsis = text + "...";
        textWidth = MeasureText(textEllipsis.c_str(), fontSize);
    } while (textWidth > width);
    return textEllipsis;
}

bool isValidTexture(std::string filename) {
    if (fs::exists(filename) && IsFileExtension(filename.c_str(), ".png")) {
        Image img = LoadImage(filename.c_str());
        if (img.width != 16 || img.height != 16)
            return false;
        UnloadImage(img);
        return true;
    }
    return false;
}

void exportCurrentMod() {
    std::string modId = currentMod->getId();
    fs::create_directories(cosmicReachDir / "mods");
    fs::create_directories(cosmicReachDir / "export_mods");

    fs::path modsPath = cosmicReachDir / "mods" / "assets";
    fs::path exportModsPath = cosmicReachDir / "export_mods" / modId;

    fs::create_directories(exportModsPath);
    // export_mods/blocks/<modId>
    fs::create_directories(exportModsPath / "blocks" / modId);
    // export_mods/models/blocks/<modId>
    fs::create_directories(exportModsPath / "models" / "blocks" / modId);
    // export_mods/textures/blocks/<modId>
    fs::create_directories(exportModsPath / "textures" / "blocks" / modId);

    std::ofstream file;
    
    fs::path texPath, placementDir;
    std::string cloneCmd;
    for (Block *b : currentMod->getBlocks()) {
        // export_mods/blocks/<modId>/<blockId>.json
        file.open(exportModsPath / "blocks" / (modId + '_' + b->id + ".json"));
        file << b->toString(currentMod);
        file.close();

        // export_mods/models/blocks/<modId>/<blockId>.json
        file.open(exportModsPath / "models" / "blocks" / (modId + '_' + b->id + ".json"));
        file << b->modelToString(currentMod);
        file.close();

        for (auto [face, path] : b->faces) {
            // export_mods/textures/<modId>/<texture>.png
            texPath = exportModsPath / "textures" / "blocks" / getFilenameFromPath(path);
            if (!fs::exists(texPath))
                fs::copy_file(path, texPath);
        }
    }
    // * Clone all mod data
    try {
        // * Blocks
        placementDir = modsPath / "blocks";
        fs::create_directories(placementDir);
        // copy export_mods/<modId>/blocks/<modId> mods/blocks/<modId>
        cloneCmd = std::string("copy ") + (exportModsPath / "blocks" / "").string() + ' ' + (placementDir / "").string();
        system(cloneCmd.c_str());

        // * Models
        placementDir = modsPath / "models" / "blocks";
        fs::create_directories(placementDir);
        // copy export_mods/<modId>/models/blocks/<modId> mods/models/blocks/<modId>
        cloneCmd = std::string("copy ") + (exportModsPath / "models" / "blocks" / "").string() + ' ' + (placementDir / "").string();
        system(cloneCmd.c_str());

        // * Textures
        placementDir = modsPath / "textures" / "blocks";
        fs::create_directories(placementDir);
        // copy export_mods/<modId>/textures/blocks/<modId> mods/textures/blocks/<modId>
        cloneCmd = std::string("copy ") + (exportModsPath / "textures" / "blocks" / "").string() + ' ' + (placementDir / "").string();
        system(cloneCmd.c_str());
    } catch (std::exception &ex) {
        std::cout << ex.what() << std::endl;
    }
}
