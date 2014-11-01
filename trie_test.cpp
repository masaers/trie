// pacakge
#include "trie.hpp"
// c++
#include <vector>
#include <string>
#include <iostream>

int main(const int argc, const char** argv) {
  using namespace std;
  using namespace com_masaers;
  
  vector<string> keys{ "aa", "a", "abb", "aaaa" };

  // TODO test erase_suffixes, erase, clear, empty, size, find, cfind
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
