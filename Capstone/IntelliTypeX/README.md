# IntelliType X — CSA0306 Capstone Source

This folder contains two related implementations:

- `Web/index.html`: browser interface; embeds the WebAssembly module.
- `Web/intellitype_engine.c`: C11 engine used by the WebAssembly build.
- `Desktop/intellitype_x.c`: portable C11 console implementation.
- `Desktop/run_intellitype_x.bat`: Windows build-and-run launcher.
- README files explain setup, demo flow and DSA concepts.

## Core DSA
Trie insertion and prefix traversal, DFS candidate collection, bounded Top-5 ranking, frequency/recency/context scoring, Levenshtein edit distance, and Trie-vs-linear benchmark.

## Build desktop version
Install GCC (MSYS2 UCRT64 recommended), open this folder in Command Prompt, and run:
```bat
gcc -std=c11 -O2 -Wall -Wextra intellitype_x.c -o IntelliTypeX.exe
IntelliTypeX.exe
```

The browser version uses C compiled to WebAssembly. Open `Web/index.html` in Chrome or Edge. It runs locally and does not call an external search API.

## Scope and limitations
This is an offline, rule-based educational search engine with a curated demonstration dictionary. It is not a trained machine-learning model or a production-scale search service. The web Trie Explorer is a conceptual visualisation; the benchmark is illustrative.
