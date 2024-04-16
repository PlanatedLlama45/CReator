#include "GUI.h"

bool UiElement::isHovered() const {
    Vector2 mousePos = GetMousePosition();
    Vector2 offPos = Vector2Add(pos, offset);
    return (
        mousePos.x >= offPos.x &&
        mousePos.x <= offPos.x + size.x &&
        mousePos.y >= offPos.y &&
        mousePos.y <= offPos.y + size.y
    );
}

/*
 * Button
*/

Button::Button(Vector2 pos, Vector2 size, bool centered, ButtonColorScheme colorScheme, int outline) {
    this->size = size;
    setPos(pos, centered);
    
    bgPos = Vector2AddValue(this->pos, outline);
    bgSize = Vector2SubtractValue(this->size, outline * 2.f);

    this->colorScheme = colorScheme;
}

Button::~Button() { }

void Button::draw() {
    ColorScheme currentColor;
    if (isHeld())
        currentColor = colorScheme.pressed;
    else if (isHovered())
        currentColor = colorScheme.hovered;
    else
        currentColor = colorScheme.normal;
    DrawRectangleV(Vector2Add(pos, offset), size, currentColor.outline);
    DrawRectangleV(Vector2Add(bgPos, offset), bgSize, currentColor.backround);
}

bool Button::isHeld() const {
    return isHovered() && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
}

bool Button::isPressed() const {
    return isHovered() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

/*
 * Text Edit
*/

TextEdit::TextEdit(Vector2 pos, Vector2 size, bool centered, int fontSize, bool centerText, TextEditColorScheme colorScheme, int outline, strRef initText) :
        Button(pos, size, centered, { colorScheme.normal, colorScheme.hovered, colorScheme.focused }, outline) {
    this->fontSize = fontSize;
    this->fontColor = colorScheme.fontColor;
    this->centerText = centerText;
    this->text = initText;
    textPos.x = bgPos.x + outline;
    textPos.y = bgPos.y + (bgSize.y - fontSize) * 0.5f;
    recalculateTextPos();
}

TextEdit::~TextEdit() { }

void TextEdit::draw() {
    ColorScheme currentColor;
    if (isFocused())
        currentColor = colorScheme.pressed;
    else if (isHovered())
        currentColor = colorScheme.hovered;
    else
        currentColor = colorScheme.normal;
    DrawRectangleV(Vector2Add(pos, offset), size, currentColor.outline);
    DrawRectangleV(Vector2Add(bgPos, offset), bgSize, currentColor.backround);

    std::string drawText = text;

    if (isFocused()) {
        char key = GetCharPressed();

        if (key) {
            text += key;
            recalculateTextPos();
        } else if (text.size() > 0 && (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE))) {
            text.erase(--text.end());
            recalculateTextPos();
        }

        drawText = text + '|';
    }

    DrawText(drawText.c_str(), textPos.x + offset.x, textPos.y + offset.y, fontSize, fontColor);
}

bool TextEdit::isFocused() {
    if (isPressed()) {
        focused = true;
        return true;
    }
    if (focused && (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)))
        focused = false;
    return focused;
}

void TextEdit::setText(const std::string& str) {
    text = str;
    while (recalculateTextPos());
}

bool TextEdit::recalculateTextPos() {
    int width = MeasureText(text.c_str(), fontSize);

    if (width >= bgSize.x)
        text.erase(--text.end());
    
    if (!centerText)
        return false;

    if (width < bgSize.x) {
        textPos.x = bgPos.x + (bgSize.x - width) * 0.5f;
        return false;
    }
    
    return true;
}

/*
 * TextButton
*/

TextButton::TextButton(Vector2 pos, Vector2 size, bool centered, strRef text, int fontSize, bool centerText, TextEditColorScheme colorScheme, int outline)
     : TextEdit(pos, size, centered, fontSize, centerText, colorScheme, outline, text) { }

TextButton::~TextButton() { }

void TextButton::draw() {
    Button::draw();

    DrawText(text.c_str(), textPos.x + offset.x, textPos.y + offset.y, fontSize, fontColor);
}

/*
 * ScrollBox
*/

ScrollBox::ScrollBox(Vector2 pos, Vector2 size, bool centered, float scrollSpeed, int spacing, strRef placeholder, int placeholderSize, ColorScheme colorScheme, int outline) {
    this->size = size;
    setPos(pos, centered);

    bgPos = Vector2AddValue(this->pos, outline);
    bgSize = Vector2SubtractValue(this->size, outline * 2.f);

    this->colorScheme = colorScheme;
    this->scrollSpeed = scrollSpeed;
    this->spacing = spacing;
    this->placeholder = placeholder;
    this->placeholderSize = placeholderSize;
    this->outline = outline;
    elements = { };
}

ScrollBox::~ScrollBox() {
    for (auto [_, elem] : elements)
        delete elem;
}

void ScrollBox::draw() {
    if (isHovered()) {
        scrollOffset += GetMouseWheelMove() * scrollSpeed;

        if (scrollOffset > 0)
            scrollOffset = 0;
    }
    
    DrawRectangleV(Vector2Add(pos, offset), size, colorScheme.outline);
    DrawRectangleV(Vector2Add(bgPos, offset), bgSize, colorScheme.backround);

    if (elements.empty()) {
        Font defaultFont = GetFontDefault();
        int width = MeasureText(placeholder.c_str(), placeholderSize);
        Vector2 textPos {
            bgPos.x + offset.x + (bgSize.x - width) * 0.5f,
            bgPos.y + offset.y + placeholderSize * 1.5f
        };
        DrawText(placeholder.c_str(), textPos.x, textPos.y, placeholderSize, BLACK);
        return;
    }
    float sizeOffset = 0;
    for (auto [_, elem] : elements) {
        elem->setOffset({ 0, scrollOffset + sizeOffset });
        
        if (
            elem->getOffPos().y + elem->getSize().y > pos.y &&
            elem->getOffPos().y < pos.y + size.y
        )
            elem->draw();
        sizeOffset += elem->getSize().y + spacing;
    }
    
    DrawRectangleV(bootlegPos, bootlegSize, RAYWHITE);
    DrawRectangleV(pos, { size.x, outline }, colorScheme.outline);
}

void ScrollBox::removeElement(strRef id) {
    delete elements[id];
    elements.erase(id);
    doBootleg();
}

void ScrollBox::clearElements() {
    while (!elements.empty()) {
        delete elements.back();
        elements.erase(--elements.end());
    }
}

Map<std::string, UiElement *> &ScrollBox::getElements() { return elements; }

void ScrollBox::doBootleg() {
    float maxHeight = -INFINITY;
    for (auto [id, elem] : elements) {
        if (elem->getSize().y > maxHeight)
            maxHeight = elem->getSize().y;
    }
    bootlegPos.x = pos.x;
    bootlegPos.y = pos.y - maxHeight;

    bootlegSize.x = size.x;
    bootlegSize.y = maxHeight;
}

ScrollBox &ScrollBox::addButton(strRef id, Vector2 pos, Vector2 size, bool centered, ButtonColorScheme colorScheme, int outline) {
    elements.insert(
        id,
        new Button(pos, size, centered, colorScheme, outline)
    );
    doBootleg();
    return *this;
}

ScrollBox &ScrollBox::addTextEdit(strRef id, Vector2 pos, Vector2 size, bool centered, int fontSize, bool centerText, TextEditColorScheme colorScheme, int outline, strRef initText) {
    elements.insert(
        id,
        new TextEdit(pos, size, centered, fontSize, centerText, colorScheme, outline, initText)
    );
    doBootleg();
    return *this;
}

ScrollBox &ScrollBox::addTextButton(strRef id, Vector2 pos, Vector2 size, bool centered, strRef text, int fontSize, bool centerText, TextEditColorScheme colorScheme, int outline) {
    elements.insert(
        id,
        new TextButton(pos, size, centered, text, fontSize, centerText, colorScheme, outline)
    );
    doBootleg();
    return *this;
}

ScrollBox &ScrollBox::addStaticText(strRef id, Vector2 pos, bool centered, strRef text, int fontSize, Color fontColor) {
    elements.insert(
        id,
        new StaticText(pos, centered, text, fontSize, fontColor)
    );
    doBootleg();
    return *this;
}

ScrollBox &ScrollBox::addTextCheckbox(strRef id, Vector2 pos, Vector2 size, bool centered, strRef text, int fontSize, float textOffset, TextCheckboxColorScheme colorScheme, int outline) {
    elements.insert(
        id,
        new TextCheckbox(pos, size, centered, text, fontSize, textOffset, colorScheme, outline)
    );
    doBootleg();
    return *this;
}

/*
 * StaticText
*/

StaticText::StaticText(Vector2 pos, bool centered, strRef text, int fontSize, Color fontColor) {
    this->pos = pos;
    textPos.y = pos.y;
    if (centered)
        textPos.y -= fontSize * 0.5f;
    this->centered = centered;
    this->fontSize = fontSize;
    this->fontColor = fontColor;
    setText(text);
}

StaticText::~StaticText() {}

void StaticText::draw() {
    DrawText(text.c_str(), textPos.x + offset.x, textPos.y + offset.y, fontSize, fontColor);
}

void StaticText::setText(strRef val) {
    text = val;
    recalculateTextPos();
}

void StaticText::recalculateTextPos() {
    if (!centered)
        return;
    
    int width = MeasureText(text.c_str(), fontSize);

    textPos.x = pos.x - width * 0.5f;
}

/*
 * TextCheckbox
*/

TextCheckbox::TextCheckbox(Vector2 pos, Vector2 size, bool centered, strRef text, int fontSize, float textOffset, TextCheckboxColorScheme colorScheme, int outline)
    : TextButton(pos, size, centered, text, fontSize, centerText, { }, outline) {
    this->size = size;
    setPos(pos, centered);
    this->text = text;
    this->fontSize = fontSize;
    textPos = { textOffset, (size.y - fontSize) * 0.5f };
    this->fontColor = colorScheme.textColor;

    bgPos = Vector2AddValue(this->pos, outline);
    bgSize = Vector2SubtractValue(this->size, outline * 2.f);
    
    dotPos = Vector2AddValue(this->bgPos, outline * 2.f);
    dotSize = Vector2SubtractValue(this->bgSize, outline * 4.f);

    this->colorScheme = ButtonColorScheme{ colorScheme.normal, colorScheme.hover, colorScheme.normal };
    dotColor = colorScheme.dotColor;
}

TextCheckbox::~TextCheckbox() { }

void TextCheckbox::draw() {
    ColorScheme currentColor;
    if (isHovered())
        currentColor = colorScheme.hovered;
    else
        currentColor = colorScheme.normal;
    
    DrawRectangleV(Vector2Add(pos, offset), size, currentColor.outline);
    DrawRectangleV(Vector2Add(bgPos, offset), bgSize, currentColor.backround);

    DrawText(text.c_str(), pos.x + offset.x + textPos.x + size.x * 0.5f, pos.y + offset.y + textPos.y, fontSize, fontColor);

    if (isChecked())
        DrawRectangleV(Vector2Add(dotPos, offset), dotSize, dotColor);
}

bool TextCheckbox::isChecked() {
    if (isPressed())
        checked = !checked;
    return checked;
}
