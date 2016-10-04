#ifndef COM_MASAERS_BASIC_TRIE_HPP
#define COM_MASAERS_BASIC_TRIE_HPP
#include "trie_internal.hpp"

namespace com_masaers {
  
  template<typename Node,
           template<typename...> class Traverser = internal::predfs_trie_traverser_t,
           template<typename...> class RTraverser = internal::rpredfs_trie_traverser_t>
  class basic_trie : public internal::trie_crtp_t<basic_trie<Node>, Node, Traverser, RTraverser> {
    typedef internal::trie_crtp_t<basic_trie<Node>, Node, Traverser, RTraverser> base_type;
  public:
    inline basic_trie() : base_type() {}
    inline basic_trie(const basic_trie&) = delete;
    inline basic_trie(basic_trie&&) = default;
    Node* root() { return &root_m; }
    const Node* root() const { return &root_m; }
    inline bool validate(const Node* node) const { return true; }
  private:
    Node root_m;
  }; // basic_trie
  
} // namespace com_masaers

/******************************************************************************/#endif
