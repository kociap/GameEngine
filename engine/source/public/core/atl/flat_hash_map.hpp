#ifndef CORE_ATL_FLAT_HASH_MAP_HPP_INCLUDE
#define CORE_ATL_FLAT_HASH_MAP_HPP_INCLUDE

#include <core/assert.hpp>
#include <core/atl/allocator.hpp>
#include <core/atl/functors.hpp>
#include <core/atl/memory.hpp>
#include <core/atl/tags.hpp>
#include <core/math/math.hpp>
#include <core/types.hpp>

namespace anton_engine::atl {
    // Stores both keys and values in the main array, which minimizes memory indirections.
    // It doesn't provide pointer stability and moves data on rehashing.
    // TODO: Implement state sentinel for iterators.
    // TODO: Add launder.
    //
    template<typename Key, typename Value, typename Hash = Default_Hash<Key>, typename Key_Equal = Equal_Compare<Key>>
    class Flat_Hash_Map {
    private:
        enum class State : u8;
        class Slot;

        template<typename _Key, typename _Hash, typename _Key_Equal, typename = void>
        struct Transparent_Key {
            template<typename>
            using type = _Key const&;
        };

        template<typename _Key, typename _Hash, typename _Key_Equal>
        struct Transparent_Key<_Key, _Hash, _Key_Equal, enable_if<is_transparent<_Hash> && is_transparent<_Key_Equal>>> {
            template<typename Key_Type>
            using type = Key_Type const&;
        };

        template<typename T>
        using transparent_key = typename Transparent_Key<Key, Hash, Key_Equal>::template type<T>;

    public:
        class Entry {
        public:
            Key const key;
            Value value;
        };

        using value_type = Entry;
        using allocator_type = Polymorphic_Allocator;
        using hasher = Hash;
        using key_equal = Key_Equal;

        class const_iterator {
        public:
            using value_type = Entry const;
            using difference_type = isize;
            using iterator_category = atl::Bidirectional_Input_Tag;

            const_iterator() = delete;
            const_iterator(const_iterator const&) = default;
            const_iterator(const_iterator&&) = default;
            ~const_iterator() = default;
            const_iterator& operator=(const_iterator const&) = default;
            const_iterator& operator=(const_iterator&&) = default;

            const_iterator& operator++() {
                _slots += 1;
                _states += 1;
                while(*_states == State::empty || *_states == State::deleted) {
                    _slots += 1;
                    _states += 1;
                }
                return *this;
            }

            const_iterator operator++(int) {
                const_iterator iter = *this;
                ++(*this);
                return iter;
            }

            const_iterator& operator--() {
                _slots -= 1;
                _states -= 1;
                while(*_states == State::empty || *_states == State::deleted) {
                    _slots -= 1;
                    _states -= 1;
                }
                return *this;
            }

            const_iterator operator--(int) {
                const_iterator iter = *this;
                --(*this);
                return iter;
            }

            [[nodiscard]] value_type* operator->() const {
                if constexpr(ANTON_ITERATOR_DEBUG) {
                    ANTON_FAIL(*_states == State::active, "Dereferencing invalid Flat_Hash_Map iterator.");
                }
                return reinterpret_cast<value_type const*>(_slots);
            }

            [[nodiscard]] value_type& operator*() const {
                if constexpr(ANTON_ITERATOR_DEBUG) {
                    ANTON_FAIL(*_states == State::active, "Dereferencing invalid Flat_Hash_Map iterator.");
                }
                return *reinterpret_cast<value_type const*>(_slots);
            }

            [[nodiscard]] bool operator==(const_iterator const& b) const {
                return _slots == b._slots;
            }

            [[nodiscard]] bool operator!=(const_iterator const& b) const {
                return _slots != b._slots;
            }

        private:
            friend class Flat_Hash_Map;
            friend class iterator;

            Slot const* _slots;
            State const* _states;

            const_iterator(Slot const* slots, State const* states): _slots(slots), _states(states) {}
        };

        class iterator {
        public:
            using value_type = Entry;
            using difference_type = isize;
            using iterator_category = atl::Bidirectional_Input_Tag;

            iterator() = delete;
            iterator(iterator const&) = default;
            iterator(iterator&&) = default;
            ~iterator() = default;
            iterator& operator=(iterator const&) = default;
            iterator& operator=(iterator&&) = default;

            [[nodiscard]] operator const_iterator() const {
                return _iter;
            }

            iterator& operator++() {
                ++_iter;
                return *this;
            }

            iterator operator++(int) {
                iterator iter = *this;
                ++(*this);
                return iter;
            }

            iterator& operator--() {
                --_iter;
                return *this;
            }

            iterator operator--(int) {
                iterator iter = *this;
                --(*this);
                return iter;
            }

            [[nodiscard]] value_type* operator->() const {
                return const_cast<value_type*>(_iter.operator->());
            }

            [[nodiscard]] value_type& operator*() const {
                return const_cast<value_type&>(*_iter);
            }

            [[nodiscard]] bool operator==(iterator const& b) const {
                return _iter == b._iter;
            }

            [[nodiscard]] bool operator!=(iterator const& b) const {
                return _iter != b._iter;
            }

        private:
            friend Flat_Hash_Map;

            const_iterator _iter;

            iterator(Slot* slots, State* states): _iter(slots, states) {}
        };

        Flat_Hash_Map(allocator_type const& = allocator_type(), hasher const& = hasher(), key_equal const& = key_equal());
        Flat_Hash_Map(Reserve_Tag, i64 size, allocator_type const& = allocator_type(), hasher const& = hasher(), key_equal const& = key_equal());
        Flat_Hash_Map(Flat_Hash_Map const&, allocator_type const& = allocator_type());
        Flat_Hash_Map(Flat_Hash_Map&&) noexcept;
        Flat_Hash_Map& operator=(Flat_Hash_Map const&);
        Flat_Hash_Map& operator=(Flat_Hash_Map&&) noexcept;
        ~Flat_Hash_Map();

        [[nodiscard]] iterator begin() {
            i64 offset = 0;
            while(_states[offset] == State::empty || _states[offset] == State::deleted) {
                offset += 1;
            }
            return iterator(_slots + offset, _states + offset);
        }

        [[nodiscard]] const_iterator begin() const {
            i64 offset = 0;
            while(_states[offset] == State::empty || _states[offset] == State::deleted) {
                offset += 1;
            }
            return const_iterator(_slots + offset, _states + offset);
        }

        [[nodiscard]] const_iterator cbegin() {
            i64 offset = 0;
            while(_states[offset] == State::empty || _states[offset] == State::deleted) {
                offset += 1;
            }
            return const_iterator(_slots + offset, _states + offset);
        }

        [[nodiscard]] iterator end() {
            return iterator(_slots + _capacity, _states + _capacity);
        }

        [[nodiscard]] const_iterator end() const {
            return const_iterator(_slots + _capacity, _states + _capacity);
        }

        [[nodiscard]] const_iterator cend() {
            return const_iterator(_slots + _capacity, _states + _capacity);
        }

        template<typename K = void>
        [[nodiscard]] iterator find(transparent_key<K> key) {
            if(_capacity) {
                u64 const h = _hasher(key);
                i64 index = h % _capacity;
                while(true) {
                    State const state = _states[index];
                    if(state == State::active && _key_equal(key, _slots[index].key)) {
                        return iterator(_slots + index, _states + index);
                    } else if(state == State::empty) {
                        return end();
                    }
                    index = (index + 1) % _capacity;
                }
            }
            return end();
        }

        template<typename K = void>
        [[nodiscard]] const_iterator find(transparent_key<K> key) const {
            if(_capacity) {
                u64 const h = _hasher(key);
                i64 index = h % _capacity;
                while(true) {
                    State const state = _states[index];
                    if(state == State::active && _key_equal(key, _slots[index].key)) {
                        return iterator(_slots + index, _states + index);
                    } else if(state == State::empty) {
                        return end();
                    }
                    index = (index + 1) % _capacity;
                }
            }
            return end();
        }

        // find_or_emplace
        // Finds the entry with given key or default constructs one if it doesn't exist.
        //
        template<typename... Args>
        [[nodiscard]] iterator find_or_emplace(Key const& key, Args&&... args);

        // emplace
        // Overwrites the value if it already exists.
        //
        template<typename Key_Type, typename... Args>
        iterator emplace(Key_Type&& key, Args&&... args);

        void erase(const_iterator position);
        void clear();

        // ensure_capacity
        // Resizes and rehashes the hash map if c elements wouldn't fit into the hash map.
        //
        void ensure_capacity(i64 c);

        void rehash();

        [[nodiscard]] i64 capacity() const {
            return _capacity;
        }

        [[nodiscard]] i64 size() const {
            return _size;
        }

        [[nodiscard]] allocator_type& get_allocator() {
            return _allocator;
        }

        [[nodiscard]] allocator_type const& get_allocator() const {
            return _allocator;
        }

        [[nodiscard]] f32 load_factor() const {
            return (f32)(_capacity - _empty_slots_left) / (f32)_capacity;
        }

        [[nodiscard]] f32 max_load_factor() const {
            return 0.75f;
        }

    private:
        enum class State : u8 {
            empty = 0,
            active,
            deleted,
            sentinel,
        };

        class Slot {
        public:
            Key key;
            Value value;

            template<typename K, typename... Args, atl::enable_if<!is_same<remove_const_ref<K>, Slot>, int> = 0>
            Slot(K&& k, Args&&... args): key(atl::forward<K>(k)), value(atl::forward<Args>(args)...) {}
            Slot(Slot const&) = default;
            Slot(Slot&&) = default;
            Slot& operator=(Slot const&) = default;
            Slot& operator=(Slot&&) = default;
            ~Slot() = default;
        };

        // TODO: Compressed tuple.
        allocator_type _allocator;
        hasher _hasher;
        key_equal _key_equal;
        State* _states = nullptr;
        Slot* _slots = nullptr;
        i64 _capacity = 0;
        i64 _size = 0;
        i64 _empty_slots_left = 0;

        static State* empty_initial_states();
        i64 find_non_active(u64 hash, i64 capacity, State* states);
    };
} // namespace anton_engine::atl

namespace anton_engine::atl {
    template<typename Key, typename Value, typename Hash, typename Key_Compare>
    Flat_Hash_Map<Key, Value, Hash, Key_Compare>::Flat_Hash_Map(allocator_type const& alloc, hasher const& h, key_equal const& eq)
        : _allocator(alloc), _hasher(h), _key_equal(eq), _states(empty_initial_states()) {}

    template<typename Key, typename Value, typename Hash, typename Key_Compare>
    Flat_Hash_Map<Key, Value, Hash, Key_Compare>::Flat_Hash_Map(Reserve_Tag, i64 size, allocator_type const& alloc, hasher const& h, key_equal const& eq)
        : _allocator(alloc), _hasher(h), _key_equal(eq) {
        ensure_capacity(size);
    }

    template<typename Key, typename Value, typename Hash, typename Key_Compare>
    Flat_Hash_Map<Key, Value, Hash, Key_Compare>::Flat_Hash_Map(Flat_Hash_Map const& other, allocator_type const& alloc)
        : _allocator(alloc), _hasher(other._hasher), _key_equal(other._key_equal), _capacity(other._capacity), _size(other._size),
          _empty_slots_left(other._empty_slots_left) {
        if(_capacity) {
            _states = (State*)_allocator.allocate((_capacity + 16 + 1) * sizeof(State), 16);
            memcpy(_states, other._states - 16, (_capacity + 16 + 1) * sizeof(State));
            _states += 16;
            _slots = (Slot*)_allocator.allocate(_capacity * sizeof(Slot), alignof(Slot));
            for(i64 i = 0; i < _capacity; ++i) {
                if(_states[i] == State::active) {
                    construct(_slots + i, other._slots[i]);
                }
            }
        } else {
            _states = empty_initial_states();
        }
    }

    template<typename Key, typename Value, typename Hash, typename Key_Compare>
    Flat_Hash_Map<Key, Value, Hash, Key_Compare>::Flat_Hash_Map(Flat_Hash_Map&& other) noexcept
        : _allocator(), _hasher(), _key_equal(), _states(other._states), _slots(other._slots), _capacity(other._capacity), _size(other._size),
          _empty_slots_left(other._empty_slots_left) {
        other._slots = nullptr;
        other._states = nullptr;
        other._capacity = 0;
        other._size = 0;
        other._empty_slots_left = 0;
    }

    template<typename Key, typename Value, typename Hash, typename Key_Compare>
    auto Flat_Hash_Map<Key, Value, Hash, Key_Compare>::operator=(Flat_Hash_Map const& other) -> Flat_Hash_Map& {
        // TODO: Should it copy the allocator, hasher or key_equal?
        // TODO: Assumes no exceptions.
        // TODO: Turn off the fucking exceptions.
        if(_capacity) {
            _allocator.deallocate(_states - 16, (_capacity + 16 + 1) * sizeof(State), 16);
            _allocator.deallocate(_slots, _capacity * sizeof(Slot), alignof(Slot));
        }

        if(other._capacity) {
            _capacity = other._capacity;
            _size = other._size;
            _empty_slots_left = other._empty_slots_left;
            _states = (State*)_allocator.allocate((_capacity + 16 + 1) * sizeof(State), 16);
            memcpy(_states, other._states - 16, (_capacity + 16 + 1) * sizeof(State));
            _states += 16;
            _slots = (Slot*)_allocator.allocate(_capacity * sizeof(Slot), alignof(Slot));
            for(i64 i = 0; i < _capacity; ++i) {
                if(_states[i] == State::active) {
                    construct(_slots + i, other._slots[i]);
                }
            }
        } else {
            _states = empty_initial_states();
        }
        return *this;
    }

    template<typename Key, typename Value, typename Hash, typename Key_Compare>
    auto Flat_Hash_Map<Key, Value, Hash, Key_Compare>::operator=(Flat_Hash_Map&& other) noexcept -> Flat_Hash_Map& {
        atl::swap(_slots, other._slots);
        atl::swap(_states, other._states);
        atl::swap(_capacity, other._capacity);
        atl::swap(_size, other._size);
        atl::swap(_hasher, other._hasher);
        atl::swap(_allocator, other._allocator);
        atl::swap(_key_equal, other._key_equal);
        atl::swap(_empty_slots_left, other._empty_slots_left);
        return other;
    }

    template<typename Key, typename Value, typename Hash, typename Key_Compare>
    Flat_Hash_Map<Key, Value, Hash, Key_Compare>::~Flat_Hash_Map() {
        for(i64 i = 0; i < _capacity; ++i) {
            if(_states[i] == State::active) {
                destruct(_slots + i);
            }
        }

        if(_capacity) {
            _allocator.deallocate(_states - 16, (_capacity + 16 + 1) * sizeof(State), 16);
            _allocator.deallocate(_slots, _capacity * sizeof(Slot), alignof(Slot));
        }
    }

    template<typename Key, typename Value, typename Hash, typename Key_Compare>
    template<typename... Args>
    auto Flat_Hash_Map<Key, Value, Hash, Key_Compare>::find_or_emplace(Key const& key, Args&&... args) -> iterator {
        auto iter = find(key);
        if(iter != end()) {
            return iter;
        } else {
            return emplace(key, atl::forward<Args>(args)...);
        }
    }

    template<typename Key, typename Value, typename Hash, typename Key_Compare>
    template<typename Key_Type, typename... Args>
    auto Flat_Hash_Map<Key, Value, Hash, Key_Compare>::emplace(Key_Type&& key, Args&&... args) -> iterator {
        ensure_capacity(_size + 1);
        u64 const h = _hasher(key);
        i64 index = h % _capacity;
        while(true) {
            State const state = _states[index];
            if(state != State::active) {
                _states[index] = State::active;
                construct(_slots + index, key, atl::forward<Args>(args)...);
                _size += 1;
                _empty_slots_left -= 1;
                return iterator(_slots + index, _states + index);
            } else {
                if(_key_equal(key, _slots[index].key)) {
                    Value* ptr = &_slots[index].value;
                    destruct(ptr);
                    construct(ptr, atl::forward<Args>(args)...);
                    return iterator(_slots + index, _states + index);
                }
                index = (index + 1) % _capacity;
            }
        }
    }

    template<typename Key, typename Value, typename Hash, typename Key_Compare>
    void Flat_Hash_Map<Key, Value, Hash, Key_Compare>::erase(const_iterator pos) {
        if constexpr(ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(pos._states >= _states && pos._states < _states + _capacity,
                       "Flat_Hash_Map::erase(const_iterator): Attepmting to erase an iterator outside the container.");
            ANTON_FAIL(*pos._states == State::active,
                       "Flat_Hash_Map::erase(const_iterator): Attempting to erase an iterator that doesn't point to a valid object.");
        }

        *const_cast<State*>(pos._states) = State::deleted;
        destruct(pos._slots);
        _size -= 1;
    }

    template<typename Key, typename Value, typename Hash, typename Key_Compare>
    void Flat_Hash_Map<Key, Value, Hash, Key_Compare>::clear() {
        for(auto it = begin(); it != end(); ++it) {
            erase(it);
        }
    }

    template<typename Key, typename Value, typename Hash, typename Key_Compare>
    void Flat_Hash_Map<Key, Value, Hash, Key_Compare>::ensure_capacity(i64 const c) {
        i64 const new_elements_count = math::max(c - _size, (i64)0);
        i64 const removed_elements_count = _capacity - _empty_slots_left - _size;
        if(_capacity != 0 && removed_elements_count >= new_elements_count) {
            rehash();
        } else {
            i64 const required_slots = _size + new_elements_count;
            i64 const required_capacity = math::ceil((f32)required_slots / max_load_factor());
            i64 new_capacity = _capacity != 0 ? _capacity : 64;
            while(new_capacity < required_capacity) {
                new_capacity *= 2;
            }

            if(new_capacity == _capacity) {
                return;
            }

            State* new_states = (State*)_allocator.allocate((new_capacity + 16 + 1) * sizeof(State), 16);
            new_states += 16;
            new_states[-1] = State::sentinel;
            memset(new_states, (u8)State::empty, new_capacity);
            new_states[new_capacity] = State::sentinel;
            Slot* new_slots = (Slot*)_allocator.allocate(new_capacity * sizeof(Slot), alignof(Slot));

            for(i64 i = 0; i < _capacity; ++i) {
                if(_states[i] == State::active) {
                    u64 const h = _hasher(_slots[i].key);
                    i64 const index = find_non_active(h, new_capacity, new_states);
                    construct(new_slots + index, atl::move(_slots[i]));
                    destruct(_slots + i);
                    new_states[index] = State::active;
                }
            }

            if(_capacity) {
                _allocator.deallocate(_states - 16, (_capacity + 16 + 1) * sizeof(State), 16);
                _allocator.deallocate(_slots, _capacity * sizeof(Slot), alignof(Slot));
            }
            _states = new_states;
            _slots = new_slots;
            _capacity = new_capacity;
        }
    }

    template<typename Key, typename Value, typename Hash, typename Key_Compare>
    void Flat_Hash_Map<Key, Value, Hash, Key_Compare>::rehash() {
        // Convert deleted to empty and active to deleted.
        for(i64 i = 0; i < _capacity; ++i) {
            if(_states[i] == State::deleted) {
                _states[i] = State::empty;
            } else if(_states[i] == State::active) {
                _states[i] = State::deleted;
            }
        }

        // Rehash all deleted.
        for(i64 i = 0; i < _capacity; ++i) {
            if(_states[i] == State::deleted) {
                Slot& slot = _slots[i];
                u64 const h = _hasher(slot.key);
                i64 index = h % _capacity;
                while(true) {
                    State const state = _states[index];
                    if(state == State::empty) {
                        _states[index] = State::active;
                        construct(_slots + index, atl::move(_slots[i]));
                        break;
                    } else if(state == State::deleted) {
                        _states[index] = State::active;
                        atl::swap(slot, _slots[index]);
                        break;
                    }
                    index = (index + 1) % _capacity;
                }
            }
        }

        _empty_slots_left = _capacity - _size;
    }

    template<typename Key, typename Value, typename Hash, typename Key_Compare>
    auto Flat_Hash_Map<Key, Value, Hash, Key_Compare>::empty_initial_states() -> State* {
        // constexpr to force the values to be allocated at compiletime.
        // TODO: I have no idea what I'm doing.
        // TODO: Remind myself at one point why I'm aligning stuff.
        alignas(16) static constexpr State empty_state[] = {
            State::sentinel, State::sentinel, State::sentinel, State::sentinel, State::sentinel, State::sentinel, State::sentinel, State::sentinel,
            State::sentinel, State::sentinel, State::sentinel, State::sentinel, State::sentinel, State::sentinel, State::sentinel, State::sentinel,
        };
        return const_cast<State*>(empty_state);
    }

    template<typename Key, typename Value, typename Hash, typename Key_Compare>
    i64 Flat_Hash_Map<Key, Value, Hash, Key_Compare>::find_non_active(u64 h, i64 cap, State* states) {
        i64 index = h % cap;
        while(true) {
            State const state = states[index];
            if(state != State::active) {
                break;
            } else {
                index = (index + 1) % cap;
            }
        }
        return index;
    }
} // namespace anton_engine::atl

#endif // !CORE_ATL_FLAT_HASH_MAP_HPP_INCLUDE
