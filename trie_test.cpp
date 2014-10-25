#include "trie.hpp"
#include <vector>
#include <string>
#include <iostream>

int main(const int argc, const char** argv) {
  using namespace std;
  
  typedef unordered_trie_node_t<char> node_type;
  typedef trie_path_t<node_type> path_type;
  
  vector<string> keys{ "aa", "a", "abb", "aaaa" };
  
  trie_set<node_type> ts;
  for (const auto& key : keys) {
    ts.insert(key);
  }

  for (auto it = begin(ts); it != end(ts); ++it) {
    cout << '"';
    for (const auto& e : path_type(&ts.root(), &*it)) {
      cout << e;
    }
    cout << '"' << endl;
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

  
  return EXIT_SUCCESS;
}
