#ifndef COM_MASAERS_TRIE_INTERNAL_HPP
#define COM_MASAERS_TRIE_INTERNAL_HPP
#include <set>
#include <unordered_set>
#include <vector>

namespace com_masaers {

  namespace internal {
    
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


    template<typename Node, typename Element>
    class trie_node_crtp_t : public trie_node_query_t<Element> {
    public:
      typedef Node node_type;
      typedef Element element_type;
    private:
      typedef trie_node_query_t<Element> base_type;
      inline node_type& me() { return static_cast<node_type&>(*this); }
      inline const node_type& me() const { return static_cast<const node_type&>(*this); }
    public:
      inline trie_node_crtp_t() : base_type(), parent_m(NULL) {}
      explicit inline trie_node_crtp_t(element_type element, node_type* parent)
        : base_type(std::move(element)), parent_m(parent)
      {}
      inline trie_node_crtp_t(trie_node_crtp_t&&) = default;
      trie_node_crtp_t& operator=(trie_node_crtp_t x) {
        swap(*this, x);
        return *this;
      }
      friend inline void swap(trie_node_crtp_t& a, trie_node_crtp_t& b) {
        using namespace std;
        swap(static_cast<base_type&>(a), static_cast<base_type&>(b));
        swap(a.parent_m, b.parent_m);
      }
      inline node_type* parent() const { return parent_m; }
      inline node_type* first_child() const {
        node_type* result = NULL;
        auto it = me().children().begin();
        if (it != me().children().end()) {
          result = *it;
        }
        return result;
      }
      inline node_type* last_child() const {
        node_type* result = NULL;
        auto it = me().children().rbegin();
        if (it != me().children().rend()) {
          result = *it;
        }
        return result;
      }
      inline node_type* child(const element_type& x) const {
        node_type* result = NULL;
        base_type query(x);
        auto it = me().children().find(static_cast<node_type*>(&query));
        if (it != me().children().end()) {
          result = *it;
        }
        return result;
      }
      inline std::pair<node_type*, bool> make_child(const element_type& x) {
        base_type query(x);
        auto p = me().children().insert(static_cast<node_type*>(&query));
        if (p.second) {
          const_cast<node_type*&>(*(p.first)) = new node_type(x, &me());
        }
        return std::pair<node_type*, bool>(*p.first, p.second);
      }
      inline node_type* disown_child(node_type* x) {
        return (me().children().erase(x) > 0) ? x : NULL;
      }
      inline node_type* disown_child(const element_type& x) {
        base_type query(x);
        return disown_child(*me().children().find(static_cast<node_type*>(&query)));
      }
      bool leaf_b() const { return me().children().empty(); }
      void force_to_leaf() {
        for (auto it = me().children().begin(); it != me().children().end(); ++it) {
          delete *it;
        }
        me().children().clear();
      }
      node_type* previous_sibling() const {
        node_type* result = NULL;
        if (me().parent() != NULL) {
          auto it = me().parent()->children().find(const_cast<node_type*>(&me()));
          if (it != me().parent()->children().end() &&
              it != me().parent()->children().begin()) {
            --it;
            result = *it;
          }
        }
        return result;
      }
      node_type* next_sibling() const {
        node_type* result = NULL;
        if (me().parent() != NULL) {
          auto it = me().parent()->children().find(const_cast<node_type*>(&me()));
          if (it != me().parent()->children().end()) {
            ++it;
            if (it != me().parent()->children().end()) {
              result = *it;
            }
          }
        }
        return result;
      }
    protected:
      node_type* parent_m;
    }; // trie_node_crtp_t
  
  
    template<typename Element,
             typename Hash = std::hash<Element>,
             typename Equal = std::equal_to<Element> >
    class unordered_trie_node_t : public trie_node_crtp_t<unordered_trie_node_t<Element, Hash, Equal>, Element> {
    private:
      typedef trie_node_crtp_t<unordered_trie_node_t<Element, Hash, Equal>, Element> base_type;
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
      inline unordered_trie_node_t() : base_type(), children_m() {}
      explicit inline unordered_trie_node_t(element_type element,
                                            unordered_trie_node_t* parent = NULL)
        : base_type(std::move(element), parent), children_m()
      {}
      inline unordered_trie_node_t(const unordered_trie_node_t&) = delete;
      inline unordered_trie_node_t(unordered_trie_node_t&&) = default;
      ~unordered_trie_node_t() { this->force_to_leaf(); }
      unordered_trie_node_t& operator=(unordered_trie_node_t x) {
        swap(*this, x);
        return *this;
      }
      friend inline void swap(unordered_trie_node_t& a, unordered_trie_node_t& b) {
        using namespace std;
        swap(static_cast<base_type&>(a), static_cast<base_type&>(b));
        swap(a.children_m, b.children_m);
      }
      inline children_type& children() { return children_m; }
      inline const children_type& children() const { return children_m; }
    private:
      children_type children_m;
    }; // unordered_trie_node_t

    template<typename Element,
             typename Comp = std::less<Element> >
    class ordered_trie_node_t : public trie_node_crtp_t<ordered_trie_node_t<Element, Comp>, Element> {
    public:
      typedef Element element_type;
      typedef Comp element_comp;
    protected:
      typedef trie_node_crtp_t<ordered_trie_node_t<Element, Comp>, Element> base_type;
      struct node_comp {
        inline bool operator()(const base_type* a, const base_type* b) const {
          return element_comp()(a->element(), b->element());
        }
      }; // node_comp
      typedef std::set<ordered_trie_node_t*, node_comp> children_type;
    public:
      inline ordered_trie_node_t() : base_type(), children_m() {}
      explicit inline ordered_trie_node_t(const element_type& element,
                                          ordered_trie_node_t* parent = NULL)
        : base_type(std::move(element), parent), children_m()
      {}
      inline ordered_trie_node_t(const ordered_trie_node_t&) = delete;
      inline ordered_trie_node_t(ordered_trie_node_t&&) = default;
      ~ordered_trie_node_t() { this->force_to_leaf(); }
      ordered_trie_node_t& operator=(ordered_trie_node_t x) {
        swap(*this, x);
        return *this;
      }
      friend inline void swap(ordered_trie_node_t& a, ordered_trie_node_t& b) {
        using namespace std;
        swap(static_cast<base_type&>(a), static_cast<base_type>(b));
        swap(a.parent_m, b.parent_m);
        swap(a.children_m, b.children_m);
      }
      children_type& children() { return children_m; }
      const children_type& children() const { return children_m; }
    private:
      children_type children_m;
    }; // ordered_trie_node_t

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

    
    template<typename Deriv, typename Node>
    class dfs_trie_traverser_crtp_t {
      inline Deriv& me() { return static_cast<Deriv&>(*this); }
      inline const Deriv& me() const { return static_cast<const Deriv&>(*this); }
      inline Deriv clone() const { return Deriv(me()); }
    public:
      typedef trie_path_t<const Node> path_type;
      inline dfs_trie_traverser_crtp_t() : root_m(NULL), node_m(NULL) {}
      inline dfs_trie_traverser_crtp_t(const Node* root, Node* node)
        : root_m(root), node_m(node)
      {}
      inline dfs_trie_traverser_crtp_t(const dfs_trie_traverser_crtp_t&) = default;
      inline dfs_trie_traverser_crtp_t(dfs_trie_traverser_crtp_t&&) = default;
      inline dfs_trie_traverser_crtp_t& operator=(dfs_trie_traverser_crtp_t x) {
        swap(*this, x);
        return *this;
      }
      friend inline void swap(dfs_trie_traverser_crtp_t& a, dfs_trie_traverser_crtp_t& b) {
        using namespace std;
        swap(a.root_m, b.root_m);
        swap(a.node_m, b.node_m);
      }
      inline bool operator==(const dfs_trie_traverser_crtp_t& x) const {
        return node_m == x.node_m;
      }
      inline bool operator!=(const dfs_trie_traverser_crtp_t& x) const {
        return ! operator==(x);
      }
      inline const Node* root() const { return root_m; }
      inline Node* node() const { return node_m; }
      inline path_type path() const { return path_type(root_m, node_m); }
    protected:
      void pre_next() {
        if (node_m != NULL) {
          if (node_m->leaf_b()) {
            // leaf
            Node* next = node_m->next_sibling();
            while (next == NULL && node_m != root_m) {
              node_m = node_m->parent();
              next = node_m->next_sibling();
            }
            node_m = next;
          } else {
            // not a leaf
            node_m = node_m->first_child();
          }
        }
      }
      void post_next() {
        if (node_m != NULL) {
          if (node_m == root_m) {
            // At root, cannot proceed beyond.
            node_m = NULL;
          } else {
            // At valid node dominated by the root.
            // Try to proceed sidways and plunge, go up if not successfull.
            Node* next = node_m->next_sibling();
            if (next != NULL) {
              next = pre_plunge(next);
            } else {
              next = node_m->parent();
            }
            node_m = next;
          }
        }
      }
      void pre_prev() {
        if (node_m != NULL) {
          if (node_m == root_m) {
            node_m = NULL;
          } else {
            Node* next = node_m->previous_sibling();
            if (next != NULL) {
              next = post_plunge(next);
            } else {
              next = node_m->parent();
            }
            node_m = next;
          }
        }
      }
      void post_prev() {
        if (node_m != NULL) {
          if (node_m->leaf_b()) {
            Node* next = node_m->previous_sibling();
            while (next == NULL && node_m != root_m) {
              node_m = node_m->parent();
              next = node_m->previous_sibling();
            }
            node_m = next;
          } else {
            node_m = node_m->last_child();
          }
        }
      }
      inline static Node* pre_plunge(Node* node) {
        while (! node->leaf_b()) {
          node = node->first_child();
        }
        return node;
      }
      inline static Node* post_plunge(Node* node) {
        while (! node->leaf_b()) {
          node = node->last_child();
        }
        return node;
      }
      const Node* root_m;
      Node* node_m;
    }; // dfs_trie_traverser_crtp_t
    template<typename Node>
    class predfs_trie_traverser_t : public dfs_trie_traverser_crtp_t<predfs_trie_traverser_t<Node>, Node> {
      typedef dfs_trie_traverser_crtp_t<predfs_trie_traverser_t<Node>, Node> base_type;
    public:
      inline predfs_trie_traverser_t() : base_type() {}
      inline predfs_trie_traverser_t(const Node* root, Node* node) : base_type(root, node) {}
      inline predfs_trie_traverser_t& operator++() {
        base_type::pre_next();
        return *this;
      }
      inline predfs_trie_traverser_t operator++(int) {
        predfs_trie_traverser_t result(*this);
        operator++();
        return result;
      }
    }; // predfs_trie_traverser_t
    template<typename Node>
    class rpredfs_trie_traverser_t : public dfs_trie_traverser_crtp_t<rpredfs_trie_traverser_t<Node>, Node> {
      typedef dfs_trie_traverser_crtp_t<rpredfs_trie_traverser_t<Node>, Node> base_type;
    public:
      inline rpredfs_trie_traverser_t() : base_type() {}
      inline rpredfs_trie_traverser_t(const Node* root, Node* node) : base_type(root, base_type::post_plunge(node)) {}
      inline rpredfs_trie_traverser_t& operator++() {
        base_type::pre_prev();
        return *this;
      }
      inline rpredfs_trie_traverser_t operator++(int) {
        rpredfs_trie_traverser_t result(*this);
        operator++();
        return result;
      }
    }; // rpredfs_trie_traverser_t
    template<typename Node>
    class postdfs_trie_traverser_t : public dfs_trie_traverser_crtp_t<postdfs_trie_traverser_t<Node>, Node> {
      typedef dfs_trie_traverser_crtp_t<postdfs_trie_traverser_t<Node>, Node> base_type;
    public:
      inline postdfs_trie_traverser_t() : base_type() {}
      inline postdfs_trie_traverser_t(const Node* root, Node* node) : base_type(root, base_type::pre_plunge(node)) {}
      inline postdfs_trie_traverser_t& operator++() {
        base_type::post_next();
        return *this;
      }
      inline postdfs_trie_traverser_t operator++(int) {
        postdfs_trie_traverser_t result(*this);
        operator++();
        return result;
      }
    }; // postdfs_trie_traverser_t
    template<typename Node>
    class rpostdfs_trie_traverser_t : public dfs_trie_traverser_crtp_t<rpostdfs_trie_traverser_t<Node>, Node> {
      typedef dfs_trie_traverser_crtp_t<rpostdfs_trie_traverser_t<Node>, Node> base_type;
    public:
      inline rpostdfs_trie_traverser_t() : base_type() {}
      inline rpostdfs_trie_traverser_t(const Node* root, Node* node) : base_type(root, node) {}
      inline rpostdfs_trie_traverser_t& operator++() {
        base_type::post_prev();
        return *this;
      }
      inline rpostdfs_trie_traverser_t operator++(int) {
        rpostdfs_trie_traverser_t result(*this);
        operator++();
        return result;
      }
    }; // rpostdfs_trie_traverser_t
    
    
    template<typename Derived, typename Node,
             template<typename...> class Traverser,
             template<typename...> class RTraverser>
    class trie_crtp_t {
      template<typename INode, typename IDerived, template<typename...> class ITraverser> class iterator_t;
      Derived& me() { return static_cast<Derived&>(*this); }
      const Derived& me() const { return static_cast<const Derived&>(*this); }
    public:
      typedef iterator_t<Node, Derived, Traverser> iterator;
      typedef iterator_t<const Node, const Derived, Traverser> const_iterator;
      typedef iterator_t<Node, Derived, RTraverser> reverse_iterator;
      typedef iterator_t<const Node, const Derived, RTraverser> reverse_const_iterator;
      inline const_iterator cbegin() const { return const_iterator(me()); }
      inline const_iterator cend() const { return const_iterator(); }
      inline iterator begin() { return iterator(me()); }
      inline iterator end() { return iterator(); }
      inline const_iterator begin() const { return cbegin(); }
      inline const_iterator end() const { return cend(); }
      inline reverse_const_iterator crbegin() const { return reverse_const_iterator(me()); }
      inline reverse_const_iterator crend() const { return reverse_const_iterator(); }
      inline reverse_iterator rbegin() { return reverse_iterator(me()); }
      inline reverse_iterator rend() { return reverse_iterator(); }
      inline reverse_const_iterator rbegin() const { return crbegin(); }
      inline reverse_const_iterator rend() const { return crend(); }
      inline trie_path_t<const Node> path_to(const const_iterator& it) const {
        return trie_path_t<const Node>(me().root(), it.node());
      }
      inline trie_path_t<Node> path_to(const iterator& it) const {
        return trie_path_t<Node>(me().root(), it.node());
      }
      inline trie_path_t<const Node> path_to(const reverse_const_iterator& it) const {
        return trie_path_t<const Node>(me().root(), it.node());
      }
      inline trie_path_t<Node> path_to(const reverse_iterator& it) const {
        return trie_path_t<Node>(me().root(), it.node());
      }
      inline trie_path_t<const Node> path_to(const Node& node) const {
        return trie_path_t<const Node>(me().root(), &node);
      }
      inline trie_path_t<Node> path_to(Node& node) const {
        return trie_path_t<Node>(me().root(), &node);
      }
      inline trie_path_t<const Node> path_to(const Node* node) const {
        return trie_path_t<const Node>(me().root(), node);
      }
      inline trie_path_t<Node> path_to(Node* node) const {
        return trie_path_t<Node>(me().root(), node);
      }
      template<typename Key> inline std::pair<iterator, bool> insert(Key&& key) {
        const auto p(insert_node(std::forward<Key>(key)));
        return std::pair<iterator, bool>(iterator(me(), p.first), p.second);
      }
      inline bool empty() const { return me().root() != NULL && ! me().root()->leaf_b(); }
      inline void clear() { me().root()->force_to_leaf(); }
      template<typename Key> inline const_iterator cfind(Key&& key) const {
        return const_iterator(me(), cfind_node(std::forward<Key>(key)));
      }
      template<typename Key> inline const_iterator find(Key&& key) const {
        return cfind(std::forward<Key>(key));
      }
      template<typename Key> inline iterator find(Key&& key) {
        return iterator(me(), find_node(std::forward<Key>(key)));
      }
      inline iterator erase(iterator it) {
        return erase_node(&*it);
      }
      template<typename Key> inline iterator erase(Key&& key) {
        return erase_node(find_node(std::forward<Key>(key)));
      }
    protected:
      template<typename Key> inline Node* find_node(Key&& key) {
        return const_cast<Node*>(cfind_node(std::forward<Key>(key)));
      }
      template<typename Key> const Node* cfind_node(Key&& key) const {
        const Node* result = me().root();
        auto it = std::begin(key);
        while (result != NULL && it != std::end(key)) {
          result = result->child(*it);
          ++it;
        }
        if (result != NULL && ! me().is_valid_node(result)) {
          result = NULL;
        }
        return result;
      }
      template<typename Key> std::pair<Node*, bool> insert_node(Key&& key) {
        std::pair<Node*, bool> result(me().root(), false);
        for (auto it = std::begin(key); it != std::end(key); ++it) {
          result = result.first->make_child(*it);
        }
        return result;
      }
      iterator erase_node(Node* node) {
        me().invalidate_node(node);
        return iterator(me(), prune_invalid_path(node));
      }
      Node* prune_invalid_path(Node* node) {
        while (node != me().root() && node->leaf_b() && ! me().is_valid_node(node)) {
          node->parent()->disown_child(node);
          Node* next = node->parent();
          delete node;
          node = next;
        }
        return node;
      }
    }; // trie_crtp_t


    template<typename Derived, typename Node,
             template<typename...> class Traverser,
             template<typename...> class RTraverser>
    template<typename INode,
             typename IDerived,
             template<typename...> class ITraverser>
    class trie_crtp_t<Derived, Node, Traverser, RTraverser>::iterator_t {
      friend class trie_crtp_t<Derived, Node, Traverser, RTraverser>;
      friend Derived;
    protected:
      void make_valid() {
        while (traverser_m.node() != NULL &&
               ! trie_m->is_valid_node(traverser_m.node())) {
          ++traverser_m;
        }
      }
      INode* node() const { return traverser_m.node(); }
      ITraverser<INode> traverser_m;
      IDerived* trie_m;
    public:
      inline iterator_t() : traverser_m(), trie_m(NULL) {}
      template<typename Trie> explicit inline iterator_t(Trie& trie)
        : traverser_m(trie.root(), trie.root()), trie_m(&trie)
      {
        make_valid();
      }
      template<typename Trie> inline iterator_t(Trie& trie, INode* node)
        : traverser_m(trie.root(), node), trie_m(&trie)
      {
        make_valid();
      }
      inline iterator_t(const iterator_t&) = default;
      inline iterator_t(iterator_t&&) = default;
      inline iterator_t& operator=(iterator_t x) {
        swap(*this, x);
        return *this;
      }
      friend inline void swap(iterator_t& a, iterator_t& b) {
        using namespace std;
        swap(a.traverser_m, b.traverser_m);
        swap(a.trie_m, b.trie_m);
      }
      iterator_t& operator++() {
        ++traverser_m;
        make_valid();
        return *this;
      }
      iterator_t operator++(int) { iterator_t x(*this); operator++(); return x; }
      inline bool operator==(const iterator_t& x) const {
        return traverser_m == x.traverser_m;
      }
      inline bool operator!=(const iterator_t& x) const { return ! operator==(x); }
      inline auto operator*() const -> decltype(this->trie_m->node_to_ref(this->node())) {
        return trie_m->node_to_ref(node());
      }
      inline auto operator->() const -> decltype(this->trie_m->node_to_ptr(this->node())) {
        return trie_m->node_to_ptr(node()); 
      }
    }; // trie_crtp_t::iterator_t

  } // namespace internal
} // namespace com_masaers

/******************************************************************************/
#endif
