#include <vector>
#include <utility>
#include <list>
#include <exception>
#include <functional>

using std::vector;
using std::list;
using std::pair;
using std::initializer_list;

const size_t MAX_LOAD = 2;
const size_t MIN_LOAD = 6;
const size_t DEFAULT_SIZE = 17;

template<class KeyType, class ValueType, class Hash = std::hash<KeyType> > class HashMap {
private:
    using Element = pair<const KeyType, ValueType>;
    using Bucket = typename list<Element>::iterator;

    vector<Bucket> table;
    vector<char> deleted;
    list<Element> elements;
    size_t elements_cnt, table_sz;
    Hash hasher;

public:
    using iterator = typename list<Element>::iterator;
    using const_iterator = typename list<Element>::const_iterator;

    size_t size() const {
        return elements_cnt;
    }

    bool empty() const {
        return elements_cnt == 0;
    }

    void resize(size_t new_sz) {
        vector<Element> to_move(elements.begin(), elements.end());
        table_sz = new_sz;
        elements_cnt = 0;
        elements.clear();
        table.assign(table_sz, elements.end());
        deleted.assign(table_sz, 0);
        for (const auto& elem : to_move)
            insert(elem);
    }

    iterator insert(const Element& elem) {
        if ((elements_cnt + 1) * MAX_LOAD > table_sz)
            resize(table_sz * 2);
        size_t bucket = hasher(elem.first) % table_sz;
        for (size_t bucket_idx = bucket;; ++bucket_idx) {
            if (bucket_idx == table_sz)
                bucket_idx = 0;
            if (table[bucket_idx] != elements.end() && table[bucket_idx]->first == elem.first)
                return elements.end();
            if (table[bucket_idx] == elements.end()) {
                elements.push_back(elem);
                auto it = elements.end();
                --it;
                table[bucket_idx] = it;
                ++elements_cnt;
                return table[bucket_idx];
            }
        }
    }

    void erase(const KeyType& key) {
        if (table_sz > DEFAULT_SIZE && (elements_cnt - 1) * MIN_LOAD < table_sz)
            resize(table_sz / 2);
        size_t bucket = hasher(key) % table_sz;
        for (size_t bucket_idx = bucket;; ++bucket_idx) {
            if (bucket_idx == table_sz)
                bucket_idx = 0;
            if (deleted[bucket_idx] == 0 && table[bucket_idx] == elements.end())
                break;
            if (table[bucket_idx] != elements.end() && table[bucket_idx]->first == key) {
                deleted[bucket_idx] = 1;
                elements.erase(table[bucket_idx]);
                table[bucket_idx] = elements.end();
                --elements_cnt;
                break;
            }
        }
    }

    Hash hash_function() const {
        return hasher;
    }

    HashMap(Hash t_hasher = Hash()) : hasher(t_hasher) {
        table_sz = DEFAULT_SIZE;
        elements_cnt = 0;
        table.assign(table_sz, elements.end());
        deleted.resize(table_sz);
    }

    template<typename Iter>
    HashMap(Iter begin, Iter end, Hash t_hasher = Hash()) : HashMap(t_hasher) {
        for (auto it = begin; it != end; ++it)
            insert(*it);
    }

    HashMap(initializer_list<Element> i_list, Hash t_hasher = Hash()) : HashMap(t_hasher) {
        for (const auto &elem : i_list)
            insert(elem);
    }

    HashMap(const HashMap& other) {
        if (this == &other)
            return;
        clear();
        for (const auto& elem : other)
            insert(elem);
    }

    HashMap& operator=(const HashMap& other) {
        if (this != &other) {
            clear();
            for (const auto& elem : other)
                insert(elem);
        }
        return *this;
    }

    void clear() {
        table_sz = DEFAULT_SIZE;
        elements_cnt = 0;
        elements.clear();
        table.assign(table_sz, elements.end());
        deleted.assign(table_sz, 0);
    }

    iterator begin() {
        return elements.begin();
    }

    iterator end() {
        return elements.end();
    }

    const_iterator begin() const {
        return elements.cbegin();
    }

    const_iterator end() const {
        return elements.cend();
    }

    iterator find(const KeyType& key) {
        size_t bucket = hasher(key) % table_sz;
        for (size_t bucket_idx = bucket;; ++bucket_idx) {
            if (bucket_idx == table_sz)
                bucket_idx = 0;
            if (deleted[bucket_idx] == 0 && table[bucket_idx] == elements.end())
                return elements.end();
            if (table[bucket_idx] != elements.end() && table[bucket_idx]->first == key)
                return table[bucket_idx];
        }
    }

    const_iterator find(const KeyType& key) const {
        size_t bucket = hasher(key) % table_sz;
        for (size_t bucket_idx = bucket;; ++bucket_idx) {
            if (bucket_idx == table_sz)
                bucket_idx = 0;
            if (deleted[bucket_idx] == 0 && table[bucket_idx] == elements.end())
                return elements.end();
            if (table[bucket_idx] != elements.end() && table[bucket_idx]->first == key)
                return table[bucket_idx];
        }
    }

    const ValueType& at(const KeyType& key) const {
        size_t bucket = hasher(key) % table_sz;
        for (size_t bucket_idx = bucket;; ++bucket_idx) {
            if (bucket_idx == table_sz)
                bucket_idx = 0;
            if (deleted[bucket_idx] == 0 && table[bucket_idx] == elements.end())
                throw std::out_of_range("Key does not exist");
            if (table[bucket_idx] != elements.end() && table[bucket_idx]->first == key)
                return table[bucket_idx]->second;
        }
    }

    ValueType& operator[](const KeyType& key) {
        size_t bucket = hasher(key) % table_sz;
        for (size_t bucket_idx = bucket;; ++bucket_idx) {
            if (bucket_idx == table_sz)
                bucket_idx = 0;
            if (deleted[bucket_idx] == 0 && table[bucket_idx] == elements.end())
                return insert({key, ValueType()})->second;
            if (table[bucket_idx] != elements.end() && table[bucket_idx]->first == key)
                return table[bucket_idx]->second;
        }
    }
};
