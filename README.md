trie
===
Provides a trie structure for c++11.

Blame
===
The sole blame of the implementation currently lies with Markus Saers `masaers`, but the ideas being implemented were developed during several pair programming sessions with Karteek Addanki `k4rt33k`, who also deserves some of the blame.

Philosophy
===
The package is organized so that only the classes and functions intended for users to instantiate and call are visible in the `com_masaers` namespace; other classes and functions are tucked away in the `com_masaers::internal` namespace. This makes it relatively safe to `using namespace com_masaers`.

Usage
===
`trie_set`
---
Make sure you include the `trie_set.hpp` header, either directly or through the `trie.hpp` header. This makes the templates `ordered_trie_set` and `unordered_trie_set` available. To create a trie set, these templates need to be instantiated with the *element type* that of the key sequences. Example:

    #include "trie_set.hpp"
    int main(const int argc, const char** argv) {
      using namespace std;
      using namespace com_masaers;
      ordered_trie_set<char> trie;
      for (int i = 1; i < argc; ++i) {
        trie.insert(string(argv[i]));
      }
    }

Since the whole point of a trie is to *not* store the keys explicitly in the structure, the iterator over a trie dereferences to a node handle rather than a key. To get the key, you have to interpret the iterator using the trie, like this:

    for (const auto& node : trie) {
      const auto key = trie.path_to(it);
      for (const auto& e : key) {
        cout << e;
      }
      cout << endl;
    }

This prints all the keys in the trie to `cout`. Why `const auto& node`? Because nodes are not copyable, so we must take them by reference, and although we need to make it `const`, we do not need mutability for printing.