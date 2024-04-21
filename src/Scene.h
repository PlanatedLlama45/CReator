#pragma once

#include "config.h"
#include "GUI.h"
#include "Map.h"

class Scene {
public:
    Scene(strRef id);
    ~Scene();

    void draw();

    getter(id, strRef, Id)

    Scene &addButton(strRef id, Vector2 pos, Vector2 size, bool centered, ButtonColorScheme colorScheme, int outline = 2);
    Scene &addTextEdit(strRef id, Vector2 pos, Vector2 size, bool centered, int fontSize, bool centerText, TextEditColorScheme colorScheme, int outline = 2, strRef initText = "", bool limitText = true);
    Scene &addTextButton(strRef id, Vector2 pos, Vector2 size, bool centered, strRef text, int fontSize, bool centerText, TextEditColorScheme colorScheme, int outline = 2);
    Scene &addScrollBox(strRef id, Vector2 pos, Vector2 size, bool centered, float scrollSpeed, int spacing, strRef placeholder, int placeholderSize, ColorScheme colorScheme, int outline = 2);
    Scene &addStaticText(strRef id, Vector2 pos, bool centered, strRef text, int fontSize, Color fontColor);
    Scene &addTextCheckbox(strRef id, Vector2 pos, Vector2 size, bool centered, strRef text, int fontSize, float textOffset, TextCheckboxColorScheme colorScheme, int outline = 2);
    Scene &addImageBox(strRef id, Vector2 pos, Vector2 size, bool centered, strRef imagePath, Color outlineColor, int outline = 2);

    template <class T = UiElement>
    inline T &getUiElement(strRef id) {
        return *(T *)elements[id];
    }

    inline auto getElements() { return elements; }

private:
    Map<std::string, UiElement *> elements;
    std::string id;
};
