// pacakge
#include "trie.hpp"
// c++
#include <vector>
#include <string>
#include <iostream>
// c
#include <cassert>

int main(const int argc, const char** argv) {
  using namespace std;
  using namespace com_masaers;
  
  vector<string> keys{ "aa", "a", "abb", "aaaa" };
  {
    typedef internal::ordered_trie_node_t<char> node_type;
    basic_trie<node_type> trie;
    for (const auto& key : keys) {
      trie.insert(key);
    }
    {
      typedef internal::predfs_trie_traverser_t<node_type> trav_type;
      vector<string> gold{ "", "a", "aa", "aaa", "aaaa", "ab", "abb" };
      auto git = begin(gold);
      for (trav_type t(trie.root(), trie.root()); t != trav_type(); ++t) {
        const auto key = t.path();
        assert(string(begin(key), end(key)) == *git++);
      }
      assert(git == end(gold));
    }
    {
      typedef internal::rpredfs_trie_traverser_t<node_type> trav_type;
      vector<string> gold{ "", "a", "aa", "aaa", "aaaa", "ab", "abb" };
      auto git = gold.rbegin();
      for (trav_type t(trie.root(), trie.root()); t != trav_type(); ++t) {
        const auto key = t.path();
        assert(string(begin(key), end(key)) == *git++);
      }
      assert(git == gold.rend());
    }
    {
      typedef internal::postdfs_trie_traverser_t<node_type> trav_type;
      vector<string> gold{ "aaaa", "aaa", "aa", "abb", "ab", "a", "" };
      auto git = begin(gold);
      for (trav_type t(trie.root(), trie.root()); t != trav_type(); t++) {
        const auto key = t.path();
        assert(string(begin(key), end(key)) == *git++);
      }
      assert(git == end(gold));
    }
    {
      typedef internal::rpostdfs_trie_traverser_t<node_type> trav_type;
      vector<string> gold{ "aaaa", "aaa", "aa", "abb", "ab", "a", "" };
      auto git = gold.rbegin();
      for (trav_type t(trie.root(), trie.root()); t != trav_type(); t++) {
        const auto key = t.path();
        assert(string(begin(key), end(key)) == *git++);
      }
      assert(git == gold.rend());
    }
  }
    
  return EXIT_SUCCESS;
}
