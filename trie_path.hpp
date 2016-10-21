#ifndef COM_MASAERS_TRIE_PATH_HPP
#define COM_MASAERS_TRIE_PATH_HPP
#include <vector>

namespace com_masaers {
  namespace internal {
    template<typename Node>
    class trie_path_t {
    public:
      typedef Node node_type;
      typedef typename node_type::element_type element_type;
    protected:
      typedef std::vector<const node_type*> nodes_type;
      class iterator_t;
    public:
      trie_path_t() : nodes_m() {}
      trie_path_t(const node_type* oldest, const node_type* youngest) : nodes_m() {
        nodes_m.push_back(NULL);
        for(; youngest != oldest; youngest = youngest->parent()) {
          nodes_m.push_back(youngest);
        }
      }
      typedef iterator_t iterator;
      typedef iterator_t const_iterator;
      const_iterator begin() const { return const_iterator(nodes_m.rbegin()); }
      const_iterator end() const { return const_iterator(--nodes_m.rend()); }
    private:
      nodes_type nodes_m;
    }; // trie_path_t

    template<typename Node>
    class trie_path_t<Node>::iterator_t : public std::iterator<std::forward_iterator_tag, const element_type> {
    protected:
      typedef std::iterator<std::forward_iterator_tag, const element_type> base_type;
      typedef typename nodes_type::const_reverse_iterator at_type;
      friend class trie_path_t;
      inline iterator_t(at_type x) : at_m(std::move(x)) {}
    public:
      inline iterator_t() : at_m() {}
      inline iterator_t(const iterator_t&) = default;
      inline iterator_t& operator=(const iterator_t& x) {
        at_m = x.at_m;
        return *this;
      }
      inline iterator_t& operator++() { ++at_m; return *this; }
      inline iterator_t operator++(int) { iterator_t x(*this); operator++(); return x; }
      inline bool operator==(const iterator_t& x) const { return *at_m == *x.at_m; }
      inline bool operator!=(const iterator_t& x) const { return ! operator==(x); }
      inline typename base_type::reference operator*() const { return (**at_m).element(); }
      inline typename base_type::pointer operator->() const { return &(**at_m).element(); }
    private:
      at_type at_m;
    }; // trie_path_t<Node>::iterator_t  	
  }
}

/******************************************************************************/
#endif
