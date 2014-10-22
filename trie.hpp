#ifndef TRIE_HPP
#define TRIE_HPP
#include <unordered_set>
#include <vector>
#include <cassert>

template<typename Element>
class trie_node_query_t {
public:
  typedef Element element_type;
  inline trie_node_query_t() : element_m() {}
  inline explicit trie_node_query_t(const element_type& element)
    : element_m(element)
  {}
  inline explicit trie_node_query_t(element_type&& element)
    : element_m(std::move(element))
  {}
  inline trie_node_query_t(const trie_node_query_t&) = default;
  inline trie_node_query_t(trie_node_query_t&&) = default;
  inline trie_node_query_t& operator=(trie_node_query_t x) {
    swap(*this, x);
    return *this;
  }
  friend inline void swap(trie_node_query_t& a, trie_node_query_t& b) {
    using namespace std;
    swap(a.element_m, b.element_m);
  }
  inline const element_type& element() const { return element_m; }
private:
  element_type element_m;
}; // trie_node_query_t

template<typename Element,
	 typename Hash = std::hash<Element>,
	 typename Equal = std::equal_to<Element> >
class unordered_trie_node_t : public trie_node_query_t<Element> {
private:
  typedef trie_node_query_t<Element> base_type;
public:
  typedef typename base_type::element_type element_type;
  typedef Hash element_hash;
  typedef Equal element_equal;
protected:
  struct node_hash {
    inline std::size_t operator()(const base_type* node) const {
      return element_hash()(node->element());
    }
  }; // node_hash
  struct node_equal {
    inline bool operator()(const base_type* a, const base_type* b) const {
      return element_equal()(a->element(), b->element());
    }
  }; // node_equal
  typedef std::unordered_set<unordered_trie_node_t*, node_hash, node_equal> children_type;
public:
  inline unordered_trie_node_t() : base_type(), parent_m(NULL), children_m() {}
  explicit inline unordered_trie_node_t(const element_type& element,
					unordered_trie_node_t* parent = NULL)
    : base_type(element), parent_m(parent), children_m()
  {}
  explicit inline unordered_trie_node_t(element_type&& element,
					unordered_trie_node_t* parent = NULL)
    : base_type(std::move(element)), parent_m(parent), children_m()
  {}
  inline unordered_trie_node_t(const unordered_trie_node_t&) = delete;
  inline unordered_trie_node_t(unordered_trie_node_t&&) = default;
  ~unordered_trie_node_t() { force_to_leaf(); }
  unordered_trie_node_t& operator=(unordered_trie_node_t x) {
    swap(*this, x);
    return *this;
  }
  friend inline void swap(unordered_trie_node_t& a, unordered_trie_node_t& b) {
    using namespace std;
    swap(static_cast<base_type&>(a), static_cast<base_type&>(b));
    swap(a.parent_m, b.parent_m);
    swap(a.children_m, b.children_m);
  }
  inline unordered_trie_node_t* parent() const { return parent_m; }
  unordered_trie_node_t* first_child() const {
    unordered_trie_node_t* result = NULL;
    auto it = children_m.begin();
    if (it != children_m.end()) {
      result = *it;
    }
    return result;
  }
  unordered_trie_node_t* last_child() const {
    unordered_trie_node_t* result = NULL;
    auto it = children_m.rbegin();
    if (it != children_m.rend()) {
      result = *it;
    }
    return result;
  }
  unordered_trie_node_t* child(const element_type& x) const {
    unordered_trie_node_t* result = NULL;
    base_type query(x);
    auto it = children_m.find(static_cast<unordered_trie_node_t*>(&query));
    if (it != children_m.end()) {
      result = *it;
    }
    return *it;
  }
  std::pair<unordered_trie_node_t*, bool> make_child(const element_type& x) {
    base_type query(x);
    auto p = children_m.insert(static_cast<unordered_trie_node_t*>(&query));
    if (p.second) {
      const_cast<unordered_trie_node_t*&>(*(p.first))
	= new unordered_trie_node_t(x, this);
    }
    return std::pair<unordered_trie_node_t*, bool>(*p.first, p.second);
  }
  unordered_trie_node_t* disown_child(unordered_trie_node_t* x) {
    return (children_m.erase(x) > 0) ? x : NULL;
  }
  unordered_trie_node_t* disown_child(const element_type& x) {
    base_type query(x);
    return disown_child(*children_m.find(static_cast<unordered_trie_node_t*>(&query)));
  }
  bool leaf_b() const { return children_m.empty(); }
  void force_to_leaf() {
    for (auto it = children_m.begin(); it != children_m.end(); ++it) {
      delete *it;
    }
    children_m.clear();
  }
  unordered_trie_node_t* previous_sibling() const {
    unordered_trie_node_t* result = NULL;
    if (parent_m != NULL) {
      auto it = parent_m->children_m.find(const_cast<unordered_trie_node_t*>(this));
      if (it != parent_m->children_m.end() &&
	  it != parent_m->children_m.begin()) {
	--it;
	result = *it;
      }
    }
    return result;
  }
  unordered_trie_node_t* next_sibling() const {
    unordered_trie_node_t* result = NULL;
    if (parent_m != NULL) {
      auto it = parent_m->children_m.find(const_cast<unordered_trie_node_t*>(this));
      if (it != parent_m->children_m.end()) {
	++it;
	if (it != parent_m->children_m.end()) {
	  result = *it;
	}
      }
    }
    return result;
  }
private:
  unordered_trie_node_t* parent_m;
  children_type children_m;
}; // unordered_trie_node_t


/******************************************************************************/#endif
