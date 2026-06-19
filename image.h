
#include <cstdint>
#include <vector>

std::vector<uint16_t> unpack_10bits( const std::vector<uint32_t> &data, size_t pels, bool swapBytes = true );
std::vector<uint16_t> strip_alpha( const std::vector<uint16_t> &rgba );
std::vector<uint16_t> demangle( const std::vector<uint16_t> &rgba );
std::vector<uint32_t> pack_10bits( const std::vector<uint16_t> &data );

