#pragma once

#include "config.h"

template <class Key, class Val>
class Map {
public:
    using Entry = std::pair<Key, Val>;

    struct Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_value = ptrdiff_t;
        using value_type = Entry;
        using pointer = Entry *;
        using reference = Entry &;

        inline Iterator(pointer ptr) : m_ptr(ptr) { }

        inline reference operator *() const { return *m_ptr; }
        inline pointer operator ->() { return m_ptr; }

        inline Iterator &operator ++() { m_ptr++; return *this; }
        inline Iterator operator ++(int) { Iterator tmp = *this; ++(*this); return tmp; }

        inline Iterator &operator --() { m_ptr--; return *this; }
        inline Iterator operator --(int) { Iterator tmp = *this; --(*this); return tmp; }

        inline Iterator &operator +(difference_value offset) { m_ptr += offset; return *this; }
        inline Iterator &operator -(difference_value offset) { m_ptr -= offset; return *this; }

        inline difference_value operator -(const Iterator &it) { return m_ptr - it.m_ptr; }

        inline friend bool operator ==(const Iterator &it1, const Iterator &it2) { return (it1.m_ptr == it2.m_ptr); }
        inline friend bool operator !=(const Iterator &it1, const Iterator &it2) { return (it1.m_ptr != it2.m_ptr); }

        inline pointer _m_get_ptr() { return m_ptr; }

    private:
        pointer m_ptr;
    };

    inline Map() { }
    inline Map(std::initializer_list<Entry> init) {
        entries = init;
    }
    inline ~Map() { }

    inline Map &insert(const Key &key, const Val &val) {
        Iterator it = find(key);
        if (it != end())
            it->second = val;
        else
            entries.push_back({ key, val });

        return *this;
    }
    inline Map &insert(const Entry &entry) {
        entries.push_back(entry);
        return *this;
    }

    inline Map &erase(Iterator iter) {
        entries.erase(toVecIterator(iter));
        return *this;
    }
    inline Map &erase(const Key &key) {
        vec_iterator iter = toVecIterator(find(key));
        if (iter != entries.end())
            entries.erase(iter);
        return *this;
    }
    inline Map &erase(const Entry &entry) {
        vec_iterator iter = toVecIterator(find(entry));
        if (iter != entries.end())
            entries.erase(iter);
        return *this;
    }

    inline size_t size() const {
        return entries.size();
    }
    inline bool empty() const {
        return (size() == 0);
    }

    inline Iterator begin() {
        return Iterator(entries.data());
    }
    inline Iterator end() {
        return begin() + size();
    }
    inline Val front() {
        return begin()->second;
    }
    inline Val back() {
        return (--end())->second;
    }
    inline Iterator find(const Key &key) {
        auto iter = begin();
        for (; iter != end(); iter++) {
            if (iter->first == key)
                break;
        }
        return iter;
    }
    inline Iterator find(const Entry &entry) const {
        auto iter = begin();
        for (; iter != end(); iter++) {
            if (*iter == entry)
                break;
        }
        return iter;
    }

    inline bool contains(const Key &key) {
        return (find(key) != end());
    }
    inline bool contains(const Entry &entry) {
        return (find(entry) != end());
    }

    inline const Val &at(const Key &key) {
        Iterator iter = find(key);
        if (iter == end())
            std::__throw_out_of_range("Map::at");
        return iter->second;
    }

    inline Val &operator [](const Key &key) {
        Iterator iter = find(key);
        if (iter == end())
            std::__throw_out_of_range("Map::at");
        return iter->second;
    }

private:
    std::vector<Entry> entries;
    using vec_iterator = typename std::vector<Entry>::iterator;

    inline vec_iterator toVecIterator(Iterator iter) {
        return vec_iterator(iter._m_get_ptr());
    }
};