INTELLITYPE X - PROFESSIONAL C / DSA EDITION

REQUIREMENTS
- Windows 10/11
- GCC installed and available in PATH (MSYS2 UCRT64 GCC recommended)

RUN
1. Extract the project folder.
2. Double-click run_intellitype_x.bat.
3. Or compile manually:
   gcc -std=c11 -O2 -Wall -Wextra intellitype_x.c -o IntelliTypeX.exe
   IntelliTypeX.exe

FEATURES
- Trie insertion and prefix lookup
- DFS candidate collection
- Bounded Top-5 ranking
- Weighted frequency / recency / category scoring
- Levenshtein typo recovery
- Local persistence for selection counts and recency
- Search history, category analytics and Trie-vs-linear benchmark

The dictionary is a curated demonstration dataset. This is a rule-based offline search engine, not a trained AI model.
