
#include "image.h"
#include "byteswap.h"

#include <stdexcept>
#include <iostream>

std::vector<uint16_t> unpack_10bits( const std::vector<uint32_t> &data, size_t pels, bool swapBytes )
{
	if ( data.size() * 3 < pels )
	{
		throw std::runtime_error( "Not enough data to unpack all the pixels" );
	}
	std::vector<uint16_t> result;
	result.reserve( pels );

	for ( uint32_t value: data )
	{
		if ( swapBytes )
			value = byteswap( value );

		if ( result.size() < pels )
			result.push_back( static_cast<uint16_t>( ( value >>  2 ) & 0x3FF ) );

		if ( result.size() < pels )
			result.push_back( static_cast<uint16_t>( ( value >> 12 ) & 0x3FF ) );

		if ( result.size() < pels )
			result.push_back( static_cast<uint16_t>( ( value >> 22 ) & 0x3FF ) );
	}

	std::cerr << "unpacked: " << result.size() << std::endl;

	return result;
}

std::vector<uint16_t> strip_alpha( const std::vector<uint16_t> &rgba )
{
	if ( rgba.size() % 4 != 0 )
	{
		throw std::runtime_error( "RGBA data must be a multiple of 4, got " + std::to_string( rgba.size() ) );
	}

	std::vector<uint16_t> result;
	result.reserve( rgba.size() * 3 / 4 );
	for ( size_t i = 0; i < rgba.size(); i += 4 )
	{
		result.push_back( rgba.at( i + 0 ) );
		result.push_back( rgba.at( i + 1 ) );
		result.push_back( rgba.at( i + 2 ) );
	}

	std::cerr << "stripped: " << result.size() << std::endl;

	return result;
}

std::vector<uint16_t> demangle( const std::vector<uint16_t> &rgba )
{
	if ( rgba.size() % 4 != 0 )
	{
		throw std::runtime_error( "RGBA data must be a multiple of 4, got " + std::to_string( rgba.size() ) );
	}

	std::vector<uint16_t> result;
	result.reserve( rgba.size() * 3 / 4 );
	for ( size_t i = 0; i < rgba.size() - 12; i += 12 )
	{
		result.push_back( rgba.at( i +  0 ) ); // Red 0
		result.push_back( rgba.at( i +  1 ) ); // Green 0
		result.push_back( rgba.at( i +  2 ) ); // Blue 0
											   //
		result.push_back( rgba.at( i +  8 ) ); // Red 1
		result.push_back( rgba.at( i +  3 ) ); // Green 1
		result.push_back( rgba.at( i +  4 ) ); // Blue 1
											   //
		result.push_back( rgba.at( i + 10 ) ); // Red 2
		result.push_back( rgba.at( i + 11 ) ); // Green 2
		result.push_back( rgba.at( i +  6 ) ); // Blue 2
	}
	result.push_back( 0 );
	result.push_back( 0 );
	result.push_back( 0 );

	std::cerr << "demangled: " << result.size() << std::endl;

	return result;
}

std::vector<uint32_t> pack_10bits( const std::vector<uint16_t> &data )
{
	std::vector<uint32_t> result;
	result.reserve( ( data.size() + 2 ) / 3 );

	for ( size_t i = 0; i < data.size(); i += 3 )
	{
		uint32_t value = 0;
		value |= ( data.at( i + 0 ) & 0x3FF ) <<  2;
		value |= ( data.at( i + 1 ) & 0x3FF ) << 12;
		value |= ( data.at( i + 2 ) & 0x3FF ) << 22;

		value = byteswap( value );
		result.push_back( value );
	}

	return result;
}

