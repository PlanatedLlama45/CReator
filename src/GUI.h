#pragma once

#include "config.h"
#include "Map.h"

class UiElement {
public:
    inline UiElement() { hidden = false; }

    inline virtual void draw() = 0;
    inline virtual void update() = 0;

    bool isHovered() const;
    
    getter_ref(pos, Vector2, Pos)
    getter_ref(size, Vector2, Size)
    getter_ref(offset, Vector2, Offset)
    inline Vector2 getOffPos() const { return Vector2Add(pos, offset); }

    inline void setPos(Vector2 val, bool centered = false) {
        if (centered)
            pos = Vector2Subtract(val, Vector2Scale(size, 0.5f));
        else
            pos = val;
        
        offPos = Vector2Add(pos, offset);
    }
    setter(size, Vector2, Size)
    setter(offset, Vector2, Offset)

    getter(hidden, bool, Hidden)
    setter(hidden, bool, Hidden)

protected:
    Vector2 pos;
    Vector2 size;
    Vector2 offset;
    Vector2 offPos;
    bool hidden = false;
};


struct ColorScheme {
    Color backround;
    Color outline;
};


struct ButtonColorScheme {
    ColorScheme normal;
    ColorScheme hovered;
    ColorScheme pressed;
};

class Button : public UiElement {
public:
    Button(Vector2 pos, Vector2 size, bool centered, ButtonColorScheme colorScheme, int outline = 2);
    ~Button();

    void draw() override;
    void update() override;

    bool isHeld() const;
    bool isPressed() const;

    getter_ref(bgPos, Vector2, BgPos)
    getter_ref(bgSize, Vector2, BgSize)

    setter(bgPos, Vector2, BgPos)
    setter(bgSize, Vector2, BgSize)

protected:
    Vector2 bgPos;
    Vector2 bgSize;

    ButtonColorScheme colorScheme;
    ColorScheme currentColorScheme;
};


struct TextEditColorScheme {
    ColorScheme normal;
    ColorScheme hovered;
    ColorScheme focused;
    Color fontColor;
};


#define isKeyPressedRepeat(key) ( IsKeyPressed(key) || IsKeyPressedRepeat(key) )

class TextEdit : public Button {
public:
    TextEdit(Vector2 pos, Vector2 size, bool centered, int fontSize, bool centerText, TextEditColorScheme colorScheme, int outline = 2, strRef initText = "", bool limitText = true);
    ~TextEdit();

    void draw() override;
    void update() override;

    bool isFocused();

    getter(text, strRef, Text);
    void setText(strRef str);

protected:
    Color fontColor;
    int fontSize;
    std::string text, drawText;
    size_t cursorPos;
    Vector2 textPos;
    bool centerText;
    bool limitText;

    bool focused;

    bool recalculateTextPos();
};


class TextButton : public TextEdit {
public:
    TextButton(Vector2 pos, Vector2 size, bool centered, strRef text, int fontSize, bool centerText, TextEditColorScheme colorScheme, int outline = 2);
    ~TextButton();

    void draw() override;
    void update() override;
};


class StaticText : public UiElement {
public:
    StaticText(Vector2 pos, bool centered, strRef text, int fontSize, Color fontColor);
    ~StaticText();

    void draw() override;
    void update() override;

    void setText(strRef val);

protected:
    Vector2 textPos;
    std::string text;
    int fontSize;
    Color fontColor;
    bool centered;

    void recalculateTextPos();
};


struct TextCheckboxColorScheme {
    ColorScheme normal;
    ColorScheme hover;
    Color dotColor;
    Color textColor;
};

class TextCheckbox : public TextButton {
public:
    TextCheckbox(Vector2 pos, Vector2 size, bool centered, strRef text, int fontSize, float textOffset, TextCheckboxColorScheme colorScheme, int outline = 2);
    ~TextCheckbox();

    void draw() override;
    void update() override;

    bool isChecked();

    setter(checked, bool, Checked)

protected:
    Vector2 dotPos;
    Vector2 dotSize;
    Color dotColor;

    bool checked;
};


class ImageBox : public UiElement {
public:
    ImageBox(Vector2 pos, Vector2 size, bool centered, strRef imagePath, Color outlineColor, int outline = 2);
    ~ImageBox();

    void draw() override;
    void update() override;

    void setImage(strRef imagePath);

protected:
    Texture2D image;
    Vector2 imgPos;
    Vector2 imgSize;
    Color outlineColor;
};


class ScrollBox : public UiElement {
public:
    ScrollBox(Vector2 pos, Vector2 size, bool centered, float scrollSpeed, int spacing, strRef placeholder, int placeholderSize, ColorScheme colorScheme, int outline = 2);
    ~ScrollBox();

    void draw() override;
    void update() override;

    ScrollBox &addButton(strRef id, Vector2 pos, Vector2 size, bool centered, ButtonColorScheme colorScheme, int outline = 2);
    ScrollBox &addTextEdit(strRef id, Vector2 pos, Vector2 size, bool centered, int fontSize, bool centerText, TextEditColorScheme colorScheme, int outline = 2, strRef initText = "", bool limitText = true);
    ScrollBox &addTextButton(strRef id, Vector2 pos, Vector2 size, bool centered, strRef text, int fontSize, bool centerText, TextEditColorScheme colorScheme, int outline = 2);
    ScrollBox &addStaticText(strRef id, Vector2 pos, bool centered, strRef text, int fontSize, Color fontColor);
    ScrollBox &addTextCheckbox(strRef id, Vector2 pos, Vector2 size, bool centered, strRef text, int fontSize, float textOffset, TextCheckboxColorScheme colorScheme, int outline = 2);
    ScrollBox &addImageBox(strRef id, Vector2 pos, Vector2 size, bool centered, strRef imagePath, Color outlineColor, int outline = 2);

    template <class T = UiElement>
    inline T &getUiElement(strRef id) {
        return *(T *)elements[id];
    }

    void removeElement(strRef id);
    void clearElements();

    Map<std::string, UiElement *> &getElements();

protected:
    Vector2 bgPos;
    Vector2 bgSize;
    
    Vector2 bootlegPos;
    Vector2 bootlegSize;

    Map<std::string, UiElement *> elements;
    float scrollSpeed, scrollOffset;
    int spacing;
    std::string placeholder;
    int placeholderSize;
    ColorScheme colorScheme;
    float outline;

    void doBootleg();
};
