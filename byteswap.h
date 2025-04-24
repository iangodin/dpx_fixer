
#include <cstdint>
#include <cstring>

inline uint32_t byteswap( uint32_t value )
{
	return
		((value & 0xFF000000) >> 24) |
		((value & 0x00FF0000) >> 8 ) |
		((value & 0x0000FF00) << 8 ) |
		((value & 0x000000FF) << 24);
}

inline float byteswap( float value )
{
	uint32_t integer = 0;
	std::memcpy( &integer, &value, sizeof(float) );
	integer = byteswap( integer );
	std::memcpy( &value, &integer, sizeof(float) );
	return value;
}

