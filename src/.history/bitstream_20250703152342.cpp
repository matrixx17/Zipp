#include "bitstream.hpp"

BitWriter::BitWriter(std::ostream& os) : out(os), buffer(0), bits_filled(0) {}

BitWriter::~BitWriter() {
    flush();
}

void BitWriter::write_bit(bool bit) {
    buffer = (buffer << 1) | bit;
    bits_filled++;
    if (bits_filled == 8) {
        out.put(buffer);
        bits_filled = 0;
        buffer = 0;
    }
}

void BitWriter::write_bits(const std::string& bits) {
    for (char c : bits) {
        write_bit(c == '1');
    }
}

void BitWriter::flush() {
    if (bits_filled > 0) {
        buffer <<= (8 - bits_filled);
        out.put(buffer);
    }
}

BitReader::BitReader(std::istream& is) : in(is), buffer(0), bits_left(0) {}

bool BitReader::read_bit(bool& bit) {
    if (bits_left == 0) {
        int byte = in.get();
        if (byte == EOF) return false;
        buffer = static_cast<uint8_t>(byte);
        bits_left = 8;
    }
    bit = (buffer >> (bits_left - 1)) & 1;
    bits_left--;
    return true;
}