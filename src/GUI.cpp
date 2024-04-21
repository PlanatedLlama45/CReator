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

Button::Button(Vector2 pos, Vector2 size, bool centered, ButtonColorScheme colorScheme, int outline) : UiElement() {
    this->size = size;
    setPos(pos, centered);
    
    bgPos = Vector2AddValue(this->pos, outline);
    bgSize = Vector2SubtractValue(this->size, outline * 2.f);

    this->colorScheme = colorScheme;
}

Button::~Button() { }

void Button::draw() {
    if (hidden) return;
#ifdef DEBUG
    std::cout << "Button ";
#endif // DEBUG

    DrawRectangleV(Vector2Add(pos, offset), size, currentColorScheme.outline);
    DrawRectangleV(Vector2Add(bgPos, offset), bgSize, currentColorScheme.backround);
}

void Button::update() {
    if (hidden) return;

    if (isHeld())
        currentColorScheme = colorScheme.pressed;
    else if (isHovered())
        currentColorScheme = colorScheme.hovered;
    else
        currentColorScheme = colorScheme.normal;
}

bool Button::isHeld() const {
    return !hidden && isHovered() && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
}

bool Button::isPressed() const {
    return !hidden && isHovered() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

/*
 * Text Edit
*/

TextEdit::TextEdit(Vector2 pos, Vector2 size, bool centered, int fontSize, bool centerText, TextEditColorScheme colorScheme, int outline, strRef initText, bool limitText) :
        Button(pos, size, centered, { colorScheme.normal, colorScheme.hovered, colorScheme.focused }, outline) {
    this->fontSize = fontSize;
    this->fontColor = colorScheme.fontColor;
    this->centerText = centerText;
    this->limitText = limitText;
    this->text = initText;
    textPos.x = bgPos.x + outline;
    textPos.y = bgPos.y + bgSize.y * 0.5f - fontSize * (strCount(text, '\n') + 1) * 0.5f;
    recalculateTextPos();
    cursorPos = initText.length();
}

TextEdit::~TextEdit() { }

void TextEdit::draw() {
    if (hidden) return;
#ifdef DEBUG
    std::cout << "TextEdit ";
#endif // DEBUG
    
    DrawRectangleV(Vector2Add(pos, offset), size, currentColorScheme.outline);
    DrawRectangleV(Vector2Add(bgPos, offset), bgSize, currentColorScheme.backround);

    DrawText(drawText.c_str(), textPos.x + offset.x, textPos.y + offset.y, fontSize, fontColor);
}

void TextEdit::update() {
    if (hidden) return;

    if (isFocused())
        currentColorScheme = colorScheme.pressed;
    else if (isHovered())
        currentColorScheme = colorScheme.hovered;
    else
        currentColorScheme = colorScheme.normal;

    drawText = text;

    if (isFocused()) {
        if (isKeyPressedRepeat(KEY_LEFT) && cursorPos != 0)
            cursorPos--;
        else if (isKeyPressedRepeat(KEY_RIGHT) && cursorPos != text.length())
            cursorPos++;
        else if (IsKeyPressed(KEY_HOME))
            cursorPos = 0;
        else if (IsKeyPressed(KEY_END))
            cursorPos = text.length();
        else if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V)) {
            size_t oldSize = text.length();
            text += GetClipboardText();
            cursorPos += text.length() - oldSize;
            recalculateTextPos();
        }

        char key = GetCharPressed();

        if (key != '|') {
            if (key) {
                text.insert(cursorPos, 1, key);
                recalculateTextPos();
                cursorPos++;
            } else if (!text.empty()) {
                if (isKeyPressedRepeat(KEY_BACKSPACE) && cursorPos != 0) {
                    text.erase(cursorPos - 1, 1);
                    recalculateTextPos();
                    cursorPos--;
                } else if (isKeyPressedRepeat(KEY_DELETE) && cursorPos != text.length()) {
                    text.erase(cursorPos, 1);
                    recalculateTextPos();
                }
            }
        }

        if (cursorPos == text.length())
            drawText += '|';
        else
            drawText.insert(cursorPos, 1, '|');
    }

    if (isHovered())
        SetMouseCursor(MOUSE_CURSOR_IBEAM);
}

bool TextEdit::isFocused() {
    if (hidden) return false;

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
    cursorPos = text.length();
    while (recalculateTextPos());
}

bool TextEdit::recalculateTextPos() {
    int width = MeasureText(text.c_str(), fontSize);

    if (limitText && width >= bgSize.x) {
        text.erase(text.length() - 1);
        cursorPos--;
    }
    
    if (!centerText)
        return false;

    if (!limitText || width < bgSize.x) {
        textPos.x = bgPos.x + (bgSize.x - width) * 0.5f;
        return false;
    }

    textPos.y = bgPos.y + bgSize.y * 0.5f - fontSize * (strCount(text, '\n') + 1) * 0.5f;
    
    return true;
}

/*
 * TextButton
*/

TextButton::TextButton(Vector2 pos, Vector2 size, bool centered, strRef text, int fontSize, bool centerText, TextEditColorScheme colorScheme, int outline)
     : TextEdit(pos, size, centered, fontSize, centerText, colorScheme, outline, text) { }

TextButton::~TextButton() { }

void TextButton::draw() {
    if (hidden) return;
#ifdef DEBUG
    std::cout << "Text ";
#endif // DEBUG
    
    Button::draw();
    DrawText(text.c_str(), textPos.x + offset.x, textPos.y + offset.y, fontSize, fontColor);
}

void TextButton::update() {
    Button::update();
}

/*
 * ScrollBox
*/

ScrollBox::ScrollBox(Vector2 pos, Vector2 size, bool centered, float scrollSpeed, int spacing, strRef placeholder, int placeholderSize, ColorScheme colorScheme, int outline) : UiElement() {
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
    if (hidden) return;
#ifdef DEBUG
    std::cout << "ScrollBox ";
#endif // DEBUG
    
    DrawRectangleV(Vector2Add(pos, offset), size, colorScheme.outline);
    DrawRectangleV(Vector2Add(bgPos, offset), bgSize, colorScheme.backround);

    // Draw placeholder text
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

    // Draw elements
    float sizeOffset = 0;
    for (auto [_, elem] : elements) {
        elem->setOffset({ 0, scrollOffset + sizeOffset });
        
        if (
            elem->getOffPos().y + elem->getSize().y > pos.y &&
            elem->getOffPos().y < pos.y + size.y
        ) elem->draw();
        sizeOffset += elem->getSize().y + spacing;
    }
    
    // Draw bootleg
    DrawRectangleV(bootlegPos, bootlegSize, RAYWHITE);
    DrawRectangleV(pos, { size.x, outline }, colorScheme.outline);
}

void ScrollBox::update() {
    if (hidden) return;

    for (auto [id, elem] : elements)
        elem->update();

    if (isHovered()) {
        scrollOffset += GetMouseWheelMove() * scrollSpeed;

        if (scrollOffset > 0)
            scrollOffset = 0;
    }
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

ScrollBox &ScrollBox::addTextEdit(strRef id, Vector2 pos, Vector2 size, bool centered, int fontSize, bool centerText, TextEditColorScheme colorScheme, int outline, strRef initText, bool limitText) {
    elements.insert(
        id,
        new TextEdit(pos, size, centered, fontSize, centerText, colorScheme, outline, initText, limitText)
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

ScrollBox &ScrollBox::addImageBox(strRef id, Vector2 pos, Vector2 size, bool centered, strRef imagePath, Color outlineColor, int outline) {
    elements.insert(
        id,
        new ImageBox(pos, size, centered, imagePath, outlineColor, outline)
    );
    doBootleg();
    return *this;
}

/*
 * StaticText
*/

StaticText::StaticText(Vector2 pos, bool centered, strRef text, int fontSize, Color fontColor) : UiElement() {
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
    if (hidden) return;
#ifdef DEBUG
    std::cout << "StaticText ";
#endif // DEBUG
    
    DrawText(text.c_str(), textPos.x + offset.x, textPos.y + offset.y, fontSize, fontColor);
}

void StaticText::update() { }

void StaticText::setText(strRef val) {
    text = val;
    recalculateTextPos();
}

void StaticText::recalculateTextPos() {
    if (!centered)
        return;
    
    int width = MeasureText(text.c_str(), fontSize);

    textPos.x = pos.x - width * 0.5f;
    textPos.y = pos.y - fontSize * (strCount(text, '\n') + 1) * 0.5f;
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
    if (hidden) return;
#ifdef DEBUG
    std::cout << "TextCheckbox ";
#endif // DEBUG
    
    DrawRectangleV(Vector2Add(pos, offset), size, currentColorScheme.outline);
    DrawRectangleV(Vector2Add(bgPos, offset), bgSize, currentColorScheme.backround);

    DrawText(text.c_str(), pos.x + offset.x + textPos.x + size.x * 0.5f, pos.y + offset.y + textPos.y, fontSize, fontColor);

    if (isChecked())
        DrawRectangleV(Vector2Add(dotPos, offset), dotSize, dotColor);
}

void TextCheckbox::update() {
    if (hidden) return;

    if (isHovered())
        currentColorScheme = colorScheme.hovered;
    else
        currentColorScheme = colorScheme.normal;
    
    if (isPressed())
        checked = !checked;
}

bool TextCheckbox::isChecked() {
    return checked;
}

/*
 * ImageBox
*/

ImageBox::ImageBox(Vector2 pos, Vector2 size, bool centered, strRef imagePath, Color outlineColor, int outline) : UiElement() {
    this->size = size;
    setPos(pos, centered);
    this->outlineColor = outlineColor;

    image = LoadTexture(imagePath.c_str());

    imgPos = Vector2AddValue(this->pos, outline);
    imgSize = Vector2SubtractValue(size, outline * 2.f);
}

ImageBox::~ImageBox() { }

void ImageBox::draw() {
    if (hidden) return;
#ifdef DEBUG
    std::cout << "ImageBox ";
#endif // DEBUG
    
    DrawRectangleV(Vector2Add(pos, offset), size, outlineColor);
    DrawTexturePro(
        image,
        Rectangle{ 0, 0, (float)image.width, (float)image.height },
        Rectangle{ imgPos.x + offset.x, imgPos.y + offset.y, imgSize.x, imgSize.y },
        Vector2Zero(), 0,
        WHITE
    );
}

void ImageBox::update() { }

void ImageBox::setImage(strRef imagePath) {
    UnloadTexture(image);
    image = LoadTexture(imagePath.c_str());
}
