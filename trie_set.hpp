#ifndef TRIE_SET_HPP
#define TRIE_SET_HPP
#include "trie.hpp"

namespace com_masaers {

  namespace internal {
    template<typename Node, template<typename...> class Traverser = predfs_trie_traverser_t>
    class basic_trie_set : public trie_crtp_t<basic_trie_set<Node>, Node, Traverser> {
      typedef trie_crtp_t<basic_trie_set<Node>, Node, Traverser> base_type;
    public:
      typedef typename base_type::iterator iterator;
      typedef typename base_type::const_iterator const_iterator;
      inline basic_trie_set() : root_m(), members_m() {}
      inline basic_trie_set(const basic_trie_set&) = delete;
      inline basic_trie_set(basic_trie_set&&) = default;
      Node* root() { return &root_m; }
      const Node* root() const { return &root_m; }
      inline bool validate(const Node* node) const {
	return members_m.find(node) != members_m.end();
      }
      template<typename Key>
      inline std::pair<iterator, bool> insert(Key&& key) {
	const auto p(base_type::force_node(std::forward<Key>(key)));
	members_m.insert(p.first);
	return std::pair<iterator, bool>(iterator(*this, p.first), p.second);
      }
      void clear() {
	base_type::clear();
	members_m.clear();
      }
      iterator erase(iterator it) {
	return erase(&*it);
      }
      template<typename Key> iterator erase(Key&& key) {
	return erase(find_node(std::forward<Key>(key)));
      }
      std::size_t size() const { return members_m.size(); }
      bool empty() const { return members_m.empty(); }
      // TODO Implement erase_suffixes so that members are maintained.
    protected:
      iterator erase(Node* node) {
	members_m.erase(node);
	return iterator(this, prune_invalid_path(node));
      }
      Node root_m;
      std::unordered_set<const Node*> members_m;
    }; // basic_trie_set
  } // namespace internal
  
  template<typename Element,
	   typename Comp = std::less<Element> >
  using ordered_trie_set = internal::basic_trie_set<internal::ordered_trie_node_t<Element, Comp> >;

  template<typename Element,
	   typename Hash = std::hash<Element>,
	   typename Equal = std::equal_to<Element> >
  using unordered_trie_set = internal::basic_trie_set<internal::unordered_trie_node_t<Element, Hash, Equal> >;

} // namespace com_masaers

/******************************************************************************/
#endif
