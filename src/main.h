#include "config.h"
#include "GUI.h"
#include "Scene.h"
#include "Map.h"
#include "Mod.h"

#define isScene(id) ( currentScene->getId() == id )
#define isButtonPressed(id) ( currentScene->getUiElement<Button>(id).isPressed() )

extern Scene *currentScene;
extern Map<std::string, Scene *> scenes;
extern Mod *currentMod;
extern Block *currentBlock;
extern std::vector<Mod *> mods;
extern std::string cosmicReachDir;
// extern std::string openFolderCommand;

extern ButtonColorScheme colorScheme_button;
extern TextEditColorScheme colorScheme_textEdit;
extern ColorScheme colorScheme_scrollBox;
extern TextCheckboxColorScheme colorScheme_textCheckbox;

extern int main(int argc, const char **argv);

extern Scene &addScene(strRef name);
extern Scene &getScene(strRef name);
extern Scene *setScene(strRef name);

extern void loadMods();
extern bool checkTexture(strRef filename);
extern void dumpMods();

extern bool modExists(strRef id);
extern void createMod(strRef id, strRef name);

extern bool blockExists(strRef id);
extern void createBlock(Block *b);

extern void setMod(strRef id);
extern void setBlock(strRef id);

extern std::string addEllipsis(std::string text, float width, int fontSize);

extern std::string getFromFileDialog();

extern void exportCurrentMod();
