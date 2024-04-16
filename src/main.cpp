#include "main.h"

Scene *currentScene = nullptr;
Map<std::string, Scene *> scenes = { };
Mod *currentMod = nullptr;
Block *currentBlock = nullptr;
std::vector<Mod *> mods = { };
std::string cosmicReachDir = getenv("LOCALAPPDATA");
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
    // getFromFileDialog();
    // return 0;
    InitWindow(600, 480, "CRreator");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    // Ui monstrosity
    {
        addScene("mod-selector")
            .addScrollBox("mod-list", { 395, 240, }, { 410, 480 }, true, 15, 5, "No mods yet", 30, colorScheme_scrollBox, 2)
            .addTextButton("add-mod", { 95, 30 }, { 180, 50 }, true, "Add", 30, true, colorScheme_textEdit, 2)
            .addTextButton("exit", { 95, 450 }, { 180, 50 }, true, "Exit", 30, true, colorScheme_textEdit_red, 2);

        addScene("mod-creator")
            .addTextEdit("mod-id", { 300, 165 }, { 280, 40 }, true, 24, true, colorScheme_textEdit, 2, "Enter mod ID")
            .addTextEdit("mod-name", { 300, 215 }, { 280, 40 }, true, 24, true, colorScheme_textEdit, 2, "Enter mod name")
            .addTextButton("done", { 300, 265 }, { 180, 40 }, true, "Done", 30, true, colorScheme_textEdit, 2)
            .addTextButton("back", { 300, 315 }, { 180, 40 }, true, "Back", 30, true, colorScheme_textEdit,  2);
        
        addScene("mod-edit")
            .addStaticText("id-label", { 300, 20 }, true, "Mod ID", 24, BLACK)
            .addTextEdit("id-edit", { 300, 55 }, { 280, 40 }, true, 24, true, colorScheme_textEdit, 2, "")
            .addStaticText("name-label", { 300, 95 }, true, "Mod Name", 24, BLACK)
            .addTextEdit("name-edit", { 300, 135 }, { 280, 40 }, true, 24, true, colorScheme_textEdit, 2, "")
            .addScrollBox("block-list", { 0, 255, }, { 600, 270 }, false, 15, 5, "No blocks yet", 30, colorScheme_scrollBox, 2)
            .addTextButton("save", { 75, 230 }, { 140, 40 }, true, "Save", 30, true, colorScheme_textEdit, 2)
            .addTextButton("export", { 225, 230 }, { 140, 40 }, true, "Export", 30, true, colorScheme_textEdit, 2)
            .addTextButton("delete", { 375, 230 }, { 140, 40 }, true, "Delete", 30, true, colorScheme_textEdit_red, 2)
            .addTextButton("back", { 525, 230 }, { 140, 40 }, true, "Back", 30, true, colorScheme_textEdit, 2)
            .addTextButton("add-block", { 300, 185 }, { 590, 40 }, true, "Add Block", 30, true, colorScheme_textEdit, 2);
        
        addScene("mod-delete-confirm")
            .addStaticText("question", { 300, 200 }, true, "Are you sure you want to delete this mod?", 24, BLACK)
            .addTextButton("yes", { 205, 260 }, { 180, 50 }, true, "Yes", 30, true, colorScheme_textEdit_red, 2)
            .addTextButton("nuh-uh", { 395, 260 }, { 180, 50}, true, "No", 30, true, colorScheme_textEdit, 2);
        
        addScene("block-edit")
            // Id edit
            .addStaticText("id-label", { 300, 20 }, true, "Block ID", 24, BLACK)
            .addTextEdit("id-edit", { 300, 55 }, { 280, 40 }, true, 24, true, colorScheme_textEdit, 2, "")
            // Name edit
            .addStaticText("name-label", { 300, 95 }, true, "Block Name", 24, BLACK)
            .addTextEdit("name-edit", { 300, 135 }, { 280, 40 }, true, 24, true, colorScheme_textEdit, 2, "")

            .addTextCheckbox("slabs-checkbox", { 200, 190 }, { 30, 30 }, true, "Generate slabs?", 24, 25, colorScheme_textCheckbox)
            // Faces
            .addStaticText("faces-label", { 300, 230 }, true, "Block Faces", 30, BLACK)
            // All faces
            .addStaticText("face-all-label", { 300, 240 }, true, "All", 24, BLACK)
            .addTextEdit("face-all-edit", { 300, 280 }, { 280, 40 }, true, 24, true, colorScheme_textEdit, 2, "")
            // Top face
            .addStaticText("face-top-label", { 300, 95 }, true, "Top", 24, BLACK)
            .addTextEdit("face-top-edit", { 300, 135 }, { 280, 40 }, true, 24, true, colorScheme_textEdit, 2, "")
            // Down face
            .addStaticText("face-down-label", { 300, 95 }, true, "Down", 24, BLACK)
            .addTextEdit("face-down-edit", { 300, 135 }, { 280, 40 }, true, 24, true, colorScheme_textEdit, 2, "")
            // Side face
            .addStaticText("face-side-label", { 300, 95 }, true, "Side", 24, BLACK)
            .addTextEdit("face-side-edit", { 300, 135 }, { 280, 40 }, true, 24, true, colorScheme_textEdit, 2, "")

            .addTextButton("delete", { 150, 455 }, { 290, 40 }, true, "Delete", 30, true, colorScheme_textEdit_red, 2)
            .addTextButton("back", { 450, 455 }, { 290, 40 }, true, "Back", 30, true, colorScheme_textEdit, 2);
        
        addScene("block-delete-confirm")
            .addStaticText("question", { 300, 200 }, true, "Are you sure you want to delete this block?", 24, BLACK)
            .addTextButton("yes", { 205, 260 }, { 180, 50 }, true, "Yes", 30, true, colorScheme_textEdit_red, 2)
            .addTextButton("nuh-uh", { 395, 260 }, { 180, 50}, true, "No", 30, true, colorScheme_textEdit, 2);

        addScene("block-creator")
            .addTextEdit("block-id", { 300, 165 }, { 280, 40 }, true, 24, true, colorScheme_textEdit, 2, "Enter block ID")
            .addTextEdit("block-name", { 300, 215 }, { 280, 40 }, true, 24, true, colorScheme_textEdit, 2, "Enter block name")
            .addTextButton("done", { 300, 265 }, { 180, 40 }, true, "Done", 30, true, colorScheme_textEdit, 2)
            .addTextButton("back", { 300, 315 }, { 180, 40 }, true, "Back", 30, true, colorScheme_textEdit,  2);
    }
    
    setScene("mod-selector");
    
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
                currentMod->setId(currentScene->getUiElement<TextEdit>("id-edit").getText());
                currentMod->setName(currentScene->getUiElement<TextEdit>("name-edit").getText());
                dumpMods();
                setScene("mod-selector");
            } else if (isButtonPressed("delete")) {
                setScene("mod-delete-confirm");
            } else if (isButtonPressed("add-block")) {
                setScene("block-creator");
            } else if (isButtonPressed("export")) {
                exportCurrentMod();
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
                setScene("mod-selector");
            } else if (isButtonPressed("nuh-uh")) {
                setScene("mod-edit");
            }
        }
        // Block edit screen
        else if (isScene("block-edit")) {
            if (isButtonPressed("back")) {
                currentBlock->id = currentScene->getUiElement<TextEdit>("id-edit").getText();
                currentBlock->name = currentScene->getUiElement<TextEdit>("name-edit").getText();
                currentBlock->slabs = currentScene->getUiElement<TextCheckbox>("slabs-checkbox").isChecked();
                dumpMods();
                setScene("mod-edit");
            } else if (isButtonPressed("delete")) {
                setScene("block-delete-confirm");
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
                setScene("mod-edit");
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
        
        BeginDrawing();
            ClearBackground(RAYWHITE);
            currentScene->draw();
        EndDrawing();
    }

    CloseWindow();

    dumpMods();
    for (Mod *m : mods) {
        for (Block *b : m->getBlocks())
            delete b;
        delete m;
    }

    return 0;
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
    std::string blockId, blockName, slabStr;
    std::string fullFace;
    std::string faceName, facePath;

    while (!file.eof()) {
        std::getline(file, id, ';');
        std::getline(file, name, ';');
        if (id.empty() || name.empty())
            break;
        while (true) {
            std::getline(file, blockId, ';');
            if (blockId == "==")
                break;
            std::getline(file, blockName, ';');
            std::getline(file, slabStr, ';');

            blocks.push_back(new Block{ blockId, blockName, { }, (slabStr == "true") });

            for (size_t i = 0; i < 4; i++) {
                std::getline(file, fullFace, ';');
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
            mod->getId(),
            { 395, 35 }, { 390, 50 }, true,
            addEllipsis(mod->getName(), 388, 32), 32, true,
            colorScheme_textEdit, 2
        );
    }
}

bool checkTexture(strRef filename) {
    int width, height, channels;
    stbi_info(filename.c_str(), &width, &height, &channels);
    return (width == 16 && height == 16);
}

void dumpMods() {
    std::ofstream file("../mods");

    std::vector<Block *> blocks;
    for (Mod *mod : mods) {
        blocks = mod->getBlocks();

        file << mod->getId() << ';' << mod->getName() << ';';
        for (Block *b : blocks) {
            file << b->id << ';' << b->name << ';' << (b->slabs ? "true" : "false") << ';';
            for (auto [face, path] : b->faces)
                file << getTextureFaceStr(face) << ':' << path << ';';
            file << "=;";
        }
        file << "==;";
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
            currentMod->getId(),
            { 395, 35 }, { 390, 50 }, true,
            addEllipsis(currentMod->getName(), 388, 32), 32, true,
            colorScheme_textEdit, 2
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
            b->id, { 300, 290 }, { 590, 50 },
            true, addEllipsis(b->name, 590, 32),
            32, true, colorScheme_textEdit, 2
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
            b->id, { 300, 290 }, { 590, 50 },
            true, addEllipsis(b->name, 590, 32),
            32, true, colorScheme_textEdit, 2
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

// std::string getFromFileDialog() {
//     HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
//     if (SUCCEEDED(hr)) {
//         IFileOpenDialog *pFileOpen;

//         // Create the FileOpenDialog object.
//         hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, 
//                 IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

//         if (SUCCEEDED(hr))
//         {
//             // Show the Open dialog box.
//             hr = pFileOpen->Show(NULL);

//             // Get the file name from the dialog box.
//             if (SUCCEEDED(hr))
//             {
//                 IShellItem *pItem;
//                 hr = pFileOpen->GetResult(&pItem);
//                 if (SUCCEEDED(hr))
//                 {
//                     PWSTR pszFilePath;
//                     hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

//                     // Display the file name to the user.
//                     if (SUCCEEDED(hr))
//                     {
//                         MessageBoxW(NULL, pszFilePath, L"File Path", MB_OK);
//                         CoTaskMemFree(pszFilePath);
//                     }
//                     pItem->Release();
//                 }
//             }
//             pFileOpen->Release();
//         }
//         CoUninitialize();
//     }
//     return "";
// }

void exportCurrentMod() {

}
