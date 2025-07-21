
# Zipp

`Zipp` is a lightweight, dependency‑free lossless file compression utility written in C++17. It implements a deterministic Huffman coding pipeline (frequency analysis → tree construction → bit‑level encoder/decoder) so that any file you compress can be decompressed back to the exact original bytes.

---
## Quick Start
### Build
```bash
g++ -std=c++17 *.cpp -o zipp -O2
```
This produces a `zipp` executable.

### Basic Usage
Compress a file:
```bash
./zipp c input.txt input.zp
```
Decompress:
```bash
./zipp d input.zp output.txt
```
If the command succeeds, `diff input.txt output.txt` prints no output (the files are identical).

---
## Command Line Usage
```
./zipp <mode> <source> <destination>

Modes:
  c   Compress <source> into <destination>
  d   Decompress <source> into <destination>
```
Examples:
```bash
./zipp c photo.raw photo.zp
./zipp d photo.zp restored.raw
```
---
## How It Works
1. **Frequency Analysis:** Count how many times each byte value (0–255) appears.
2. **Tree Construction:** Insert all symbols into a priority queue keyed by frequency. Extract the two least‑frequent nodes, merge them into a parent (frequency = sum), and reinsert. Repeat until one node (the root) remains.
3. **Determinism:** When two nodes share a frequency, a secondary `rank` value (assigned in byte order) breaks ties. Compressor and decompressor both apply the same ordering so their trees match.
4. **Code Generation:** Traverse the tree. Append `'0'` for a left edge and `'1'` for a right edge. The sequence from root to a leaf becomes that symbol’s code. (A single‑symbol file yields code `"0"`.) The set of codes is **prefix‑free**, so decoding is unambiguous.
5. **Encoding:** Replace each input byte with its code bits and write them through `BitWriter`. After all codes are written, flush the final partial byte.
6. **Decoding:** Rebuild the tree from the header. Read bits one by one via `BitReader`; walk left on `0`, right on `1`. When a leaf is reached, output that symbol and return to the root. Stop after emitting `original_size` bytes (ignoring padding bits).

---
## Implementation Details
### Deterministic Huffman Tree
`HuffmanNode` contains `{char ch, int freq, uint32_t rank}`. Nodes are ordered by `(freq, rank)` in the priority queue. Leaves are created in sorted byte order so both programs assign identical ranks.

### Bit Writer / Reader
- `BitWriter` packs bits MSB‑first into an 8‑bit buffer. When full, it writes the byte. On `flush()`, remaining bits are left‑shifted and zero‑padded.
- `BitReader` mirrors this: when out of bits it reads the next byte, then extracts the highest bit, shifts left, and repeats.

### Single‑Symbol Files
If the tree degenerates to one leaf, `generate_codes` assigns the code `"0"`. The decompressor detects a leaf root and directly emits the symbol `original_size` times without consuming bits.

### Safety
Decoding stops after writing `original_size` bytes, preventing garbage symbols from zero padding at the tail.

---
## Limitations & Future Work
- No streaming mode (reads entire file into memory).
- Single global Huffman tree (blockwise trees could improve ratio on mixed data and enable multithreading).
- No integrity checks.
- Huffman coding only (arithmetic or rANS coding would squeeze out extra compression).

---
## Project Structure
```
main.cpp          # CLI entry point
compressor.*      # compress_file() implementation
decompressor.*    # decompress_file() implementation
huffman.*         # Huffman tree + code generation
bitstream.*       # BitWriter / BitReader utilities
```

---
## Acknowledgements
Huffman coding algorithm originally published by David A. Huffman (1952). This implementation is educational and intentionally minimal.
