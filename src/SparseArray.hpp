#pragma once
#include <vector>
#include <optional>
#include <stdexcept>
#include <algorithm>
#include <memory>


template <typename Component>
class sparse_array {
public:
    using value_type = std::optional<Component>;
    using reference_type = value_type&;
    using const_reference_type = const value_type&;
    using container_t = std::vector<value_type>;
    using size_type = typename container_t::size_type;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;
    using allocator_type = typename container_t::allocator_type;

    // Constructeurs et opérateurs par défaut
    sparse_array() = default;
    sparse_array(const sparse_array&) = default;
    sparse_array(sparse_array&&) noexcept = default;
    ~sparse_array() = default;
    sparse_array& operator=(const sparse_array&) = default;
    sparse_array& operator=(sparse_array&&) noexcept = default;

    // Opérateurs d'accès
    reference_type operator[](size_t idx) {
        if (idx >= _data.size()) {
            _data.resize(idx + 1);
        }
        return _data[idx];
    }

    const_reference_type operator[](size_t idx) const {
        if (idx >= _data.size()) {
            throw std::out_of_range("Index out of bounds");
        }
        return _data[idx];
    }

    // Iterateurs
    iterator begin() { return _data.begin(); }
    const_iterator begin() const { return _data.begin(); }
    const_iterator cbegin() const { return _data.cbegin(); }
    iterator end() { return _data.end(); }
    const_iterator end() const { return _data.end(); }
    const_iterator cend() const { return _data.cend(); }

    size_type size() const { return _data.size(); }

    // Fonction insert_at corrigée
    reference_type insert_at(size_type pos, const Component& value) {
        if (pos >= _data.size()) {
            _data.resize(pos + 1);
        }
        _data[pos] = value;
        return _data[pos];
    }

    reference_type insert_at(size_type pos, Component&& value) {
        if (pos >= _data.size()) {
            _data.resize(pos + 1);
        }
        _data[pos] = std::move(value);
        return _data[pos];
    }

    template<typename... Args>
    reference_type emplace_at(size_type pos, Args&&... args) {
        if (pos >= _data.size()) {
            _data.resize(pos + 1);
        }

        // Récupération de l'allocateur
        auto alloc = _data.get_allocator();

        // Destruction de l'objet existant, si présent
        if (_data[pos].has_value()) {
            std::allocator_traits<allocator_type>::destroy(alloc, std::addressof(_data[pos].value()));
        }

        // Construction du nouvel objet
        std::allocator_traits<allocator_type>::construct(
            alloc,
            std::addressof(_data[pos].emplace()),
            std::forward<Args>(args)...
        );

        return _data[pos];
    }


    void erase(size_type pos) {
        if (pos < _data.size()) {
            _data.erase(_data.begin() + pos);
        }
    }

    // Fonction get_index corrigée pour utiliser std::addressof
    size_type get_index(const value_type& value) const {
        auto it = std::find_if(_data.begin(), _data.end(),
            [&value](const value_type& item) {
                return std::addressof(item) == std::addressof(value);
            });
        if (it != _data.end()) {
            return static_cast<size_type>(std::distance(_data.begin(), it));
        }
        throw std::invalid_argument("Value not found in sparse array");
    }

private:
    container_t _data;
};