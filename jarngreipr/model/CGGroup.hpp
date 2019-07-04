#ifndef JARNGREIPR_MODEL_CG_GROUP_HPP
#define JARNGREIPR_MODEL_CG_GROUP_HPP
#include <jarngreipr/model/CGChain.hpp>

namespace jarngreipr
{

//
// A group of chains, with name.
//
// Since PDB ID is just one character, it often collides and difficult to
// distinguish. Also, forcefield can be defined according to a group of chains
// that has a special meaning. E.g. if proteins form a complex, it is intuitive
// to make them a group.
//
template<typename realT>
class CGGroup
{
  public:
    using real_type      = realT;
    using chain_type     = CGChain<real_type>;
    using bead_type      = typename chain_type::bead_type;
    using bead_ptr       = typename chain_type::bead_ptr;
    using container_type = std::vector<chain_type>;
    using iterator       = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;

  public:

    CGGroup(const std::string& name): name_(name){}

    CGGroup(const CGGroup&) = default;
    CGGroup(CGGroup&&)      = default;
    CGGroup& operator=(const CGGroup&) = default;
    CGGroup& operator=(CGGroup&&)      = default;
    ~CGGroup() = default;

    bool       empty() const noexcept {return chains_.empty();}
    std::size_t size() const noexcept {return chains_.size();}

    void push_back(const chain_type& c) {chains_.push_back(c);}
    void push_back(chain_type&& c)      {chains_.push_back(std::move(c));}

    chain_type&       operator[](const std::size_t i)       noexcept {return chains_[i];}
    chain_type const& operator[](const std::size_t i) const noexcept {return chains_[i];}
    chain_type&       at(const std::size_t i)       {return chains_.at(i);}
    chain_type const& at(const std::size_t i) const {return chains_.at(i);}

    iterator       begin()        noexcept {return chains_.begin();}
    iterator       end()          noexcept {return chains_.end();}
    const_iterator begin()  const noexcept {return chains_.begin();}
    const_iterator end()    const noexcept {return chains_.end();}
    const_iterator cbegin() const noexcept {return chains_.cbegin();}
    const_iterator cend()   const noexcept {return chains_.cend();}

    chain_type&       front()       noexcept {return chains_.front();}
    chain_type const& front() const noexcept {return chains_.front();}
    chain_type&       back ()       noexcept {return chains_.back();}
    chain_type const& back () const noexcept {return chains_.back();}

    container_type const& chains() const noexcept {return chains_;}
    container_type&       chains()       noexcept {return chains_;}
    std::string const&    name()  const noexcept {return name_;}
    std::string&          name()        noexcept {return name_;}

  protected:

    std::string    name_;
    container_type chains_;
};

} // jarngreipr
#endif// JARNGREIPR_MODEL_CG_GROUP_H
