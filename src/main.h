#include "config.h"
#include "GUI.h"
#include "Scene.h"
#include "Map.h"
#include "Mod.h"

#define isScene(id) ( currentScene->getId() == id )
#define isButtonPressed(id) ( currentScene->getUiElement<Button>(id).isPressed() )

#define modTextButtonArgs(mod) mod->getId(), { 395, 35 }, { 390, 50 }, true, addEllipsis(mod->getName(), 388, 32), 32, true, colorScheme_textEdit, 2
#define modTextButtonArgs_noId(mod) { 395, 35 }, { 390, 50 }, true, addEllipsis(mod->getName(), 388, 32), 32, true, colorScheme_textEdit, 2
#define blockTextButtonArgs(block) block->id, { 300, 290 }, { 590, 50 }, true, addEllipsis(block->name, 590, 32), 32, true, colorScheme_textEdit, 2
#define blockTextButtonArgs_noId(block) { 300, 290 }, { 590, 50 }, true, addEllipsis(block->name, 590, 32), 32, true, colorScheme_textEdit, 2

#define blockEditFaceChooseLogic(faceStr, faceEnum, faceDisplay) \
    else if (isButtonPressed("face-" faceStr)) { \
        currentFace = faceEnum; \
        setScene("face-edit"); \
        currentScene->getUiElement<StaticText>("face-label").setText(faceDisplay); \
        if (currentBlock->faces.contains(faceEnum)) { \
            currentScene->getUiElement<ImageBox>("face-image").setImage(currentBlock->faces[faceEnum]); \
            currentScene->getUiElement("face-paste").setHidden(true); \
            currentScene->getUiElement("face-delete").setHidden(false); \
        } \
    }

extern Scene *currentScene;
extern Map<std::string, Scene *> scenes;
extern Mod *currentMod;
extern Block *currentBlock;
extern std::vector<Mod *> mods;
extern TextureFace currentFace;
// extern fs::path cosmicReachDir;
extern fs::path exportModsDir;
extern std::string openFolderCommand;

extern ButtonColorScheme colorScheme_button;
extern TextEditColorScheme colorScheme_textEdit;
extern ColorScheme colorScheme_scrollBox;
extern TextCheckboxColorScheme colorScheme_textCheckbox;

extern int main(int argc, const char **argv);

extern void createUI();

extern Scene &addScene(strRef name);
extern Scene &getScene(strRef name);
extern Scene *setScene(strRef name);

extern void loadMods();
extern void dumpMods();

extern bool modExists(strRef id);
extern void createMod(strRef id, strRef name);

extern bool blockExists(strRef id);
extern void createBlock(Block *b);

extern void setMod(strRef id);
extern void setBlock(strRef id);

extern std::string addEllipsis(std::string text, float width, int fontSize);
extern bool isValidTexture(std::string filename);

extern void exportCurrentMod();
