#pragma once

#include <raylib.h>
#include <raymath.h>

#include <iostream>
#include <vector>
#include <numeric>
#include <filesystem>
namespace fs = std::filesystem;
#include <fstream>

// #include <stb_image.h>

#define getter(variable, type, capitalized) inline type get##capitalized() const { return variable; }
#define getter_ref(variable, type, capitalized) inline const type &get##capitalized() const { return variable; }
#define getter_norm(variable, type, capitalized) inline type get##capitalized() { return variable; }
#define setter(variable, type, capitalized) inline void set##capitalized(type val) { variable = val; }

typedef const std::string &strRef;

#define beginEnd(container) container.begin(), container.end()

template <class T>
inline bool strContains(std::basic_string<T> str, T value) {
    for (T elem : str) {
        if (elem == value)
            return true;
    }
    return false;
}

template <class T>
inline size_t strCount(std::basic_string<T> str, T value) {
    size_t amount = 0;
    for (T elem : str) {
        if (elem == value)
            amount++;
    }
    return amount;
}

// #define DEBUG

