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
  
  // TODO test erase_suffixes
  unordered_trie_set<char> uts;
  for (const auto& key : keys) {
    uts.insert(key);
  }
  for (auto it = begin(uts); it != end(uts); ++it) {
    cout << '"';
    for (const auto& e : uts.path_to(it)) {
      cout << e;
    }
    cout << '"' << endl;
  }
  {
    assert(uts.size() == keys.size());
    assert(uts.find(string("aa")) != uts.end());
    assert(uts.cfind(string("aa")) != uts.cend());
    uts.erase(string("aa"));
    assert(uts.size() == keys.size() - 1);
    assert(uts.find(string("aa")) == uts.end());
    assert(uts.cfind(string("aa")) == uts.cend());
    uts.clear();
    assert(uts.empty());
    assert(uts.size() == 0);
    assert(uts.find(string("a")) == uts.end());
    assert(uts.cfind(string("a")) == uts.cend());
  }
  
  ordered_trie_set<char> ots;
  for (const auto& key : keys) {
    ots.insert(key);
  }
  for (auto it = begin(ots); it != end(ots); ++it) {
    cout << '"';
    for (const auto& e : ots.path_to(it)) {
      cout << e;
    }
    cout << '"' << endl;
  }
  for (auto& node : ots) {
    cout << '"';
    for (const auto& e : ots.path_to(node)) {
      cout << e;
    }
    cout << '"' << endl;
  }
  {
    vector<string> gold{ "a", "aa", "aaaa", "abb" };
    {
      // Forward
      auto git = gold.begin();
      for (auto it = ots.begin(); it != ots.end(); ++it) {
        const auto key = ots.path_to(*it);
        assert(string(begin(key), end(key)) == *git++);
      }
      assert(git == gold.end());
    }
    {
      // Reverse
      auto git = gold.rbegin();
      for (auto it = ots.rbegin(); it != ots.rend(); ++it) {
        const auto key = ots.path_to(*it);
        assert(string(begin(key), end(key)) == *git++);
      }
      assert(git == gold.rend());
    }
  }
  
  return EXIT_SUCCESS;
}
