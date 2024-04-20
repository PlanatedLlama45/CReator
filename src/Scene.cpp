#include "Scene.h"

Scene::Scene(strRef id) {
    this->id = id;
    elements = { };
}

Scene::~Scene() {
    for (auto [_, elem] : elements)
        delete elem;
}

void Scene::draw() {
    for (auto [_, elem] : elements)
        elem->draw();
}

Scene &Scene::addButton(strRef id, Vector2 pos, Vector2 size, bool centered, ButtonColorScheme colorScheme, int outline) {
    elements.insert(
        id,
        new Button(pos, size, centered, colorScheme, outline)
    );
    return *this;
}

Scene &Scene::addTextEdit(strRef id, Vector2 pos, Vector2 size, bool centered, int fontSize, bool centerText, TextEditColorScheme colorScheme, int outline, strRef initText, bool limitText) {
    elements.insert(
        id,
        new TextEdit(pos, size, centered, fontSize, centerText, colorScheme, outline, initText, limitText)
    );
    return *this;
}

Scene &Scene::addTextButton(const std::string& id, Vector2 pos, Vector2 size, bool centered, strRef text, int fontSize, bool centerText, TextEditColorScheme colorScheme, int outline) {
    elements.insert(
        id,
        new TextButton(pos, size, centered, text, fontSize, centerText, colorScheme, outline)
    );
    return *this;
}

Scene &Scene::addScrollBox(strRef id, Vector2 pos, Vector2 size, bool centered, float scrollSpeed, int spacing, strRef placeholder, int placeholderSize, ColorScheme colorScheme, int outline) {
    elements.insert(
        id,
        new ScrollBox(pos, size, centered, scrollSpeed, spacing, placeholder, placeholderSize, colorScheme, outline)
    );
    return *this;
}

Scene &Scene::addStaticText(strRef id, Vector2 pos, bool centered, strRef text, int fontSize, Color fontColor) {
    elements.insert(
        id,
        new StaticText(pos, centered, text, fontSize, fontColor)
    );
    return *this;
}

Scene &Scene::addTextCheckbox(strRef id, Vector2 pos, Vector2 size, bool centered, strRef text, int fontSize, float textOffset, TextCheckboxColorScheme colorScheme, int outline) {
    elements.insert(
        id,
        new TextCheckbox(pos, size, centered, text, fontSize, textOffset, colorScheme, outline)
    );
    return *this;
}

Scene &Scene::addImageBox(strRef id, Vector2 pos, Vector2 size, bool centered, strRef imagePath, Color outlineColor, int outline) {
    elements.insert(
        id,
        new ImageBox(pos, size, centered, imagePath, outlineColor, outline)
    );
    return *this;
}
