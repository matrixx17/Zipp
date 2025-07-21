#pragma once
#include <fstream>
#include <string>

class BitWriter {
    std::ostream& out;
    uint8_t buffer;
    int bits_filled;

public:
    BitWriter(std::ostream& os);
    ~BitWriter();
    void write_bit(bool bit);
    void write_bits(const std::string& bits);
    void flush();
};

class BitReader {
    std::istream& in;
    uint8_t buffer;
    int bits_left;
    
public:
    BitReader(std::istream& is);
    bool read_bit(bool& bit);
};