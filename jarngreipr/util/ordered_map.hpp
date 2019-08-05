#ifndef JARNGRIEPR_ORDERED_MAP_HPP
#define JARNGRIEPR_ORDERED_MAP_HPP
#include <utility>
#include <initializer_list>
#include <iterator>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <tuple>

namespace jarngreipr
{

template<typename Key, typename Value, typename Comp = std::less<Key>,
         typename Alloc = std::allocator<std::pair<Key, Value>>>
class ordered_map
{
  public:
    using key_type        = Key;
    using mapped_type     = Value;
    using value_type      = std::pair<Key, Value>;
    using key_compare     = Comp;
    using allocator_type  = Alloc;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;

    using container_type          = std::vector<value_type, allocator_type>;
    using size_type               = typename container_type::size_type;
    using difference_type         = typename container_type::difference_type;
    using iterator                = typename container_type::iterator;
    using const_iterator          = typename container_type::const_iterator;
    using reverse_iterator        = typename container_type::reverse_iterator;
    using const_reverse_iterator  = typename container_type::const_reverse_iterator;

    struct value_compare
    {
        friend class ordered_map<Key, Value, Comp, Alloc>;

        value_compare() = default;
        ~value_compare() = default;
        value_compare(const value_compare&) = default;
        value_compare(value_compare&&)      = default;
        value_compare& operator=(const value_compare&) = default;
        value_compare& operator=(value_compare&&)      = default;

        value_compare(key_compare&&      kc): comp(std::move(kc)) {}
        value_compare(const key_compare& kc): comp(kc)            {}

        bool operator()(const value_type& lhs, const value_type& rhs) const
            noexcept(noexcept(comp(
                std::declval<const key_type&>(), std::declval<const key_type&>()
                )))
        {
            return comp(lhs.first, rhs.first);
        }

      private:

        key_compare comp;
    };

  public:

    ordered_map()  = default;
    ~ordered_map() = default;
    ordered_map(const ordered_map&) = default;
    ordered_map(ordered_map&&)      = default;
    ordered_map& operator=(const ordered_map&) = default;
    ordered_map& operator=(ordered_map&&)      = default;

    explicit ordered_map(const key_compare& kcmp,
                         const allocator_type& alloc = allocator_type())
        : container_(alloc), key_comp_(kcmp)
    {}
    explicit ordered_map(const allocator_type& alloc)
        : ordered_map(key_compare(), alloc)
    {}
    template<typename InputIterator>
    ordered_map(InputIterator first, InputIterator last, const key_compare& kcmp,
                const allocator_type& alloc = allocator_type())
        : container_(first, last, alloc), key_comp_(kcmp)
    {}
    template<typename InputIterator>
    ordered_map(InputIterator first, InputIterator last,
                const allocator_type& alloc = allocator_type())
        : ordered_map(first, last, key_compare(), alloc)
    {}
    ordered_map(const ordered_map& other,
                const allocator_type& alloc)
        : container_(other.container_, alloc), key_comp_(other.key_comp_)
    {}
    ordered_map(ordered_map&& other,
                const allocator_type& alloc)
        : container_(std::move(other.container_), alloc),
          key_comp_(std::move(other.key_comp_))
    {}

    explicit ordered_map(std::initializer_list<value_type> init,
                         const key_compare& cmp = key_compare(),
                         const allocator_type& alloc = allocator_type())
        : container_(std::move(init), alloc), key_comp_(cmp)
    {}
    explicit ordered_map(std::initializer_list<value_type> init,
                         const allocator_type& alloc)
        : ordered_map(std::move(init), key_compare(), alloc)
    {}
    ordered_map& operator=(std::initializer_list<value_type> init)
    {
        this->container_ = std::move(init);
        return *this;
    }

#ifdef __cpp_deduction_guides
#  if __cpp_deduction_guides >= 201606

    template<typename InputIterator,
             typename Compare = std::less<typename std::remove_const<
                 typename std::iterator_traits<InputIterator>::value_type::first_type
                 >::type>>,
             typename Allocator = std::allocator<std::pair<
                 typename std::iterator_traits<InputIterator>::value_type::first_type,
                 typename std::iterator_traits<InputIterator>::value_type::second_type
                 >>
             >
    ordered_map(InputIterator, InputIterator,
                Compare = Compare(), Allocator = Allocator())
    -> ordered_map<
        typename std::iterator_traits<InputIterator>::value_type::first_type,
        typename std::iterator_traits<InputIterator>::value_type::second_type,
        Compare, Allocator>;

    template<typename K, typename V,
             typename Compare   = std::less<K>,
             typename Allocator = std::allocator<std::pair<K, V>>
             >
    ordered_map(std::initializer_list<std::pair<K, V>>,
                Compare = Compare(), Allocator = Allocator())
    -> ordered_map<K, V, Compare, Allocator>;

    template<typename InputIterator,
             typename Allocator = std::allocator<std::pair<
                 typename std::iterator_traits<InputIterator>::value_type::first_type,
                 typename std::iterator_traits<InputIterator>::value_type::second_type
                 >>
             >
    ordered_map(InputIterator, InputIterator, Allocator = Allocator())
    -> ordered_map<
        typename std::iterator_traits<InputIterator>::value_type::first_type,
        typename std::iterator_traits<InputIterator>::value_type::second_type,
        std::less<typename std::iterator_traits<InputIterator>::value_type::first_type>,
        Allocator>;

    template<typename K, typename V,
             typename Allocator = std::allocator<std::pair<K, V>>
             >
    ordered_map(std::initializer_list<std::pair<K, V>>, Allocator)
    -> ordered_map<K, V, std::less<K>, Allocator>;

#  endif // deduction guide enabled
#endif // deduction guide defined

    bool empty()         const noexcept {return container_.empty();}
    size_type size()     const noexcept {return container_.size();}
    size_type max_size() const noexcept {return container_.max_size();}

    void clear() {container_.clear();}

    mapped_type& operator[](const key_type& k)
    {
        const auto found = this->find(k);
        if(found == container_.end())
        {
            container_.emplace_back(k, mapped_type());
            return container_.back().second;
        }
        return found->second;
    }
    mapped_type& operator[](key_type&& k)
    {
        const auto found = this->find(k);
        if(found == container_.end())
        {
            container_.emplace_back(std::move(k), mapped_type());
            return container_.back().second;
        }
        return found->second;
    }

    mapped_type& at(const key_type& k)
    {
        const auto found = this->find(k);
        if(found == container_.end())
        {
            std::ostringstream oss;
            oss << "ordered_map::at key\"" << k << "\" does not exist";
            throw std::out_of_range(oss.str());
        }
        return found->second;
    }
    mapped_type const& at(const key_type& k) const
    {
        const auto found = this->find(k);
        if(found == container_.end())
        {
            std::ostringstream oss;
            oss << "ordered_map::at key\"" << k << "\" does not exist";
            throw std::out_of_range(oss.str());
        }
        return found->second;
    }

    template<typename K>
    size_type count(const K& k) const
    {
        return this->count(key_type(k));
    }
    size_type count(const key_type& k) const
    {
        if(this->find(k) == this->end()) {return 0;}
        return 1;
    }
    template<typename K>
    bool containes(const K& k) const
    {
        return this->count(k) == 1;
    }
    bool contains(const key_type& k) const
    {
        return this->count(k) == 1;
    }

    template<typename K>
    iterator find(const K& k)
    {
        return this->find(key_type(k));
    }
    iterator find(const key_type& k)
    {
        return std::find_if(container_.begin(), container_.end(),
            [&](const value_type& v) {
                return !key_comp_(v.first, k) && !key_comp_(k, v.first);
            });
    }
    template<typename K>
    const_iterator find(const K& k) const
    {
        return this->find(key_type(k));
    }
    const_iterator find(const key_type& k) const
    {
        return std::find_if(container_.begin(), container_.end(),
            [&](const value_type& v) {
                return !key_comp_(v.first, k) && !key_comp_(k, v.first);
            });
    }

    std::pair<iterator, bool> insert(const value_type& x)
    {
        const auto found = this->find(x.first);
        if(found != this->container_.end())
        {
            return std::make_pair(found, false);
        }
        container_.push_back(x);
        return std::make_pair(std::prev(container_.end()), true);
    }
    std::pair<iterator, bool> insert(value_type&& x)
    {
        const auto found = this->find(x.first);
        if(found != this->container_.end())
        {
            return std::make_pair(found, false);
        }
        container_.push_back(std::move(x));
        return std::make_pair(std::prev(container_.end()), true);
    }

    template<typename InputIterator>
    void insert(InputIterator first, InputIterator last)
    {
        for(; first != last; ++first)
        {
            container_.push_back(*first);
        }
        return ;
    }
    void insert(std::initializer_list<value_type> init)
    {
        this->insert(init.begin(), init.end());
        return ;
    }

    template<typename ... Ts>
    std::pair<iterator, bool> emplace(Ts&& ... args)
    {
        return this->insert(value_type(std::forward<Ts>(args)...));
    }
    template<typename ... Ts>
    std::pair<iterator, bool> try_emplace(const key_type& k, Ts&& ... args)
    {
        const auto found = this->find(k);
        if(found != this->container_.end())
        {
            return std::make_pair(found, false);
        }
        container_.emplace_back(std::piecewise_construct,
                std::forward_as_tuple(k),
                std::forward_as_tuple(std::forward<Ts>(args)...));
        return std::make_pair(std::prev(container_.end()), true);
    }
    template<typename ... Ts>
    std::pair<iterator, bool> try_emplace(key_type&& k, Ts&& ... args)
    {
        const auto found = this->find(k);
        if(found != this->container_.end())
        {
            return std::make_pair(found, false);
        }
        container_.emplace_back(std::piecewise_construct,
                std::forward_as_tuple(std::move(k)),
                std::forward_as_tuple(std::forward<Ts>(args)...));
        return std::make_pair(std::prev(container_.end()), true);
    }

    void     erase(iterator       position)
    {
        container_.erase(position);
        return;
    }
    iterator erase(const_iterator position)
    {
        return container_.erase(position);
    }
    void     erase(iterator       first, iterator       last)
    {
        container_.erase(first, last);
        return;
    }
    iterator erase(const_iterator first, const_iterator last)
    {
        return container_.erase(first, last);
    }

    size_type erase(const key_type& k)
    {
        const auto found = this->find(k);
        if(found != this->container_.end())
        {
            container_.erase(found);
            return 1;
        }
        return 0;
    }

    void swap(ordered_map& other)
    {
        using std::swap;
        swap(this->container_, other.container_);
        swap(this->key_comp_,  other.key_comp_);
    }

    iterator       begin()        noexcept {return container_.begin();}
    iterator       end()          noexcept {return container_.end();}
    const_iterator begin()  const noexcept {return container_.begin();}
    const_iterator end()    const noexcept {return container_.end();}
    const_iterator cbegin() const noexcept {return container_.cbegin();}
    const_iterator cend()   const noexcept {return container_.cend();}

    reverse_iterator       rbegin()        noexcept {return container_.rbegin();}
    reverse_iterator       rend()          noexcept {return container_.rend();}
    const_reverse_iterator rbegin()  const noexcept {return container_.rbegin();}
    const_reverse_iterator rend()    const noexcept {return container_.rend();}
    const_reverse_iterator crbegin() const noexcept {return container_.crbegin();}
    const_reverse_iterator crend()   const noexcept {return container_.crend();}

    allocator_type get_allocator() const noexcept {return container_.get_allocator();}
    key_compare    key_comp()      const {return key_comp_;}
    value_compare  value_comp()    const {return value_compare(key_comp_);}

    template<typename K, typename V, typename C, typename A>
    friend bool operator==(const ordered_map<K, V, C, A>&, const ordered_map<K, V, C, A>&);
    template<typename K, typename V, typename C, typename A>
    friend bool operator!=(const ordered_map<K, V, C, A>&, const ordered_map<K, V, C, A>&);
    template<typename K, typename V, typename C, typename A>
    friend bool operator< (const ordered_map<K, V, C, A>&, const ordered_map<K, V, C, A>&);
    template<typename K, typename V, typename C, typename A>
    friend bool operator<=(const ordered_map<K, V, C, A>&, const ordered_map<K, V, C, A>&);
    template<typename K, typename V, typename C, typename A>
    friend bool operator> (const ordered_map<K, V, C, A>&, const ordered_map<K, V, C, A>&);
    template<typename K, typename V, typename C, typename A>
    friend bool operator>=(const ordered_map<K, V, C, A>&, const ordered_map<K, V, C, A>&);

  private:

    container_type container_;
    key_compare    key_comp_;
};

template<typename K, typename V, typename C, typename A>
void swap(ordered_map<K, V, C, A>& lhs, ordered_map<K, V, C, A>& rhs)
{
    lhs.swap(rhs);
    return ;
}

template<typename K, typename V, typename C, typename A>
bool operator==(const ordered_map<K, V, C, A>& lhs, const ordered_map<K, V, C, A>& rhs)
{
    return lhs.container_ == rhs.container_;
}
template<typename K, typename V, typename C, typename A>
bool operator!=(const ordered_map<K, V, C, A>& lhs, const ordered_map<K, V, C, A>& rhs)
{
    return lhs.container_ != rhs.container_;
}
template<typename K, typename V, typename C, typename A>
bool operator< (const ordered_map<K, V, C, A>& lhs, const ordered_map<K, V, C, A>& rhs)
{
    return lhs.container_ < rhs.container_;
}
template<typename K, typename V, typename C, typename A>
bool operator<=(const ordered_map<K, V, C, A>& lhs, const ordered_map<K, V, C, A>& rhs)
{
    return lhs.container_ <= rhs.container_;
}
template<typename K, typename V, typename C, typename A>
bool operator> (const ordered_map<K, V, C, A>& lhs, const ordered_map<K, V, C, A>& rhs)
{
    return lhs.container_ > rhs.container_;
}
template<typename K, typename V, typename C, typename A>
bool operator>=(const ordered_map<K, V, C, A>& lhs, const ordered_map<K, V, C, A>& rhs)
{
    return lhs.container_ >= rhs.container_;
}

} // jarngreipr
#endif//JARNGRIEPR_ORDERED_MAP_HPP
