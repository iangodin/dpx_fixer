
#include "dpx.h"
#include "byteswap.h"

#include <stdexcept>
#include <string>
#include <iostream>
#include <fstream>

int main( int argc, char *argv[] )
{
	if ( argc < 2 )
	{
		throw std::runtime_error( "missing file" );
	}

	FileInformation file_info;
	std::ifstream input( argv[1], std::ios_base::binary | std::ios_base::in );
	input.read( reinterpret_cast<char *>( &file_info ), sizeof( file_info ) );

	const bool big_endian = is_big_endian( file_info );
	if ( big_endian )
	{
		swap_bytes( file_info );
	}

	if ( file_info.generic_header_size != sizeof( FileInformation ) + sizeof( ImageHeader ) + sizeof( OrientationHeader ) )
	{
		std::cerr << "DPX header size (" << file_info.generic_header_size << " bytes) is not as expected (" << sizeof( FileInformation ) + sizeof( ImageHeader ) + sizeof( OrientationHeader ) << " bytes)\n";
		return -1;
	}

	if ( file_info.industry_header_size != sizeof( FilmInfoHeader ) + sizeof( TelevisionInfoHeader ) )
	{
		std::cerr << "Metadata size (" << file_info.industry_header_size << " bytes) is not as expected (" << sizeof( FilmInfoHeader ) + sizeof( TelevisionInfoHeader ) << " bytes)\n";
		return -1;
	}

	FilmInfoHeader film_info;
	TelevisionInfoHeader tv_info;

	input.seekg( file_info.generic_header_size );
	input.read( reinterpret_cast<char *>( &film_info ), sizeof( film_info ) );
	input.read( reinterpret_cast<char *>( &tv_info ), sizeof( tv_info ) );

	if ( big_endian )
	{
		swap_bytes( film_info );
		swap_bytes( tv_info );
	}

	std::cout << "Generic header information:\n";
	std::cout << file_info << '\n';
	std::cout << "Film Metadata:\n";
	std::cout << film_info << '\n';
	std::cout << "Television Metadata:\n";
	std::cout << tv_info << '\n';
}
