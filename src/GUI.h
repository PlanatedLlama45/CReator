#pragma once

#include "config.h"
#include "Map.h"

class UiElement {
public:
    inline virtual void draw() = 0;

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
    }
    setter(size, Vector2, Size)
    setter(offset, Vector2, Offset)

protected:
    Vector2 pos;
    Vector2 size;
    Vector2 offset;
    Vector2 offPos;
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
    TextEdit(Vector2 pos, Vector2 size, bool centered, int fontSize, bool centerText, TextEditColorScheme colorScheme, int outline = 2, strRef initText = "");
    ~TextEdit();

    void draw() override;

    bool isFocused();

    getter(text, strRef, Text);
    void setText(strRef str);

protected:
    Color fontColor;
    int fontSize;
    std::string text;
    size_t cursorPos;
    Vector2 textPos;
    bool centerText;

    bool focused;

    bool recalculateTextPos();
};


class TextButton : public TextEdit {
public:
    TextButton(Vector2 pos, Vector2 size, bool centered, strRef text, int fontSize, bool centerText, TextEditColorScheme colorScheme, int outline = 2);
    ~TextButton();

    void draw() override;
};


class StaticText : public UiElement {
public:
    StaticText(Vector2 pos, bool centered, strRef text, int fontSize, Color fontColor);
    ~StaticText();

    void draw() override;

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
    bool isChecked();

    setter(checked, bool, Checked)

protected:
    Vector2 dotPos;
    Vector2 dotSize;
    Color dotColor;

    bool checked;
};


class ScrollBox : public UiElement {
public:
    ScrollBox(Vector2 pos, Vector2 size, bool centered, float scrollSpeed, int spacing, strRef placeholder, int placeholderSize, ColorScheme colorScheme, int outline = 2);
    ~ScrollBox();

    void draw() override;

    ScrollBox &addButton(strRef id, Vector2 pos, Vector2 size, bool centered, ButtonColorScheme colorScheme, int outline = 2);
    ScrollBox &addTextEdit(strRef id, Vector2 pos, Vector2 size, bool centered, int fontSize, bool centerText, TextEditColorScheme colorScheme, int outline = 2, strRef initText = "");
    ScrollBox &addTextButton(strRef id, Vector2 pos, Vector2 size, bool centered, strRef text, int fontSize, bool centerText, TextEditColorScheme colorScheme, int outline = 2);
    ScrollBox &addStaticText(strRef id, Vector2 pos, bool centered, strRef text, int fontSize, Color fontColor);
    ScrollBox &addTextCheckbox(strRef id, Vector2 pos, Vector2 size, bool centered, strRef text, int fontSize, float textOffset, TextCheckboxColorScheme colorScheme, int outline = 2);

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
