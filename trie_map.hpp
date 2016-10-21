#ifndef COM_MASAERS_TRIE_MAP_HPP
#define COM_MASAERS_TRIE_MAP_HPP
#include "trie_internal.hpp"
#include <unordered_map>
#include <memory>

namespace com_masaers {
  
  namespace internal {
    template<typename Node,
             typename Mapped,
             template<typename...> class Traverser = predfs_trie_traverser_t,
             template<typename...> class RTraverser = rpredfs_trie_traverser_t>
    class basic_trie_map : public trie_crtp_t<basic_trie_map<Node, Mapped>, Node, Traverser, RTraverser> {
      typedef trie_crtp_t<basic_trie_map<Node, Mapped>, Node, Traverser, RTraverser> base_type;
      friend base_type;
    public:
      typedef Mapped mapped_type;
      typedef typename base_type::iterator iterator;
      typedef typename base_type::const_iterator const_iterator;
      inline basic_trie_map() : root_m(), members_m() {}
      inline basic_trie_map(const basic_trie_map&) = delete;
      inline basic_trie_map(basic_trie_map&&) = default;
      Node* root() { return &root_m; }
      const Node* root() const { return &root_m; }
      template<typename Value>
      inline std::pair<iterator, bool> insert(Value&& value) {
        Node* node = base_type::insert_node(value.first).first;
        auto p = members_m.insert(make_pair(node, value.second));
        return std::pair<iterator, bool>(iterator(*this, node), p.second);
      }
      void clear() {
        base_type::clear();
        members_m.clear();
      }
      std::size_t size() const { return members_m.size(); }
      bool empty() const { return members_m.empty(); }
      typedef std::pair<Node*, mapped_type&> iterator_ref_type;
      typedef std::pair<const Node*, const mapped_type&> const_iterator_ref_type;
      iterator_ref_type node_to_ref(Node* node) {
        return iterator_ref_type(node, members_m.find(node)->second);
      }
      const_iterator_ref_type node_to_ref(const Node* node) const {
        return const_iterator_ref_type(node, members_m.cfind(node)->second);
      }
      std::unique_ptr<iterator_ref_type> node_to_ptr(Node* node) {
        return std::unique_ptr<iterator_ref_type>(new iterator_ref_type(node_to_ref(node)));
      }
      std::unique_ptr<const_iterator_ref_type> node_to_ptr(const Node* node) const {
        return std::unique_ptr<const_iterator_ref_type>(new const_iterator_ref_type(node_to_ref(node)));
      }

    protected:
      inline bool is_valid_node(const Node* node) const {
        return members_m.find(node) != members_m.end();
      }
      void invalidate_node(const Node* node) {
        members_m.erase(node);
      }
      Node root_m;
      std::unordered_map<const Node*, mapped_type> members_m;
    }; // basic_trie_map
  } // namespace internal
  
  template<typename Element,
           typename Mapped,
           typename Comp = std::less<Element> >
  using ordered_trie_map = internal::basic_trie_map<internal::ordered_trie_node_t<Element, Comp>, Mapped>;
  
  template<typename Element,
           typename Mapped,
           typename Hash = std::hash<Element>,
           typename Equal = std::equal_to<Element> >
  using unordered_trie_map = internal::basic_trie_map<internal::unordered_trie_node_t<Element, Hash, Equal>, Mapped>;
  
} // namespace com_masaers

/******************************************************************************/
#endif
