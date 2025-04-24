
#include "dpx.h"
#include "byteswap.h"

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>

std::string compose_filename( const std::string &format, int frame )
{
	char filename[2048];
	const int name_chars = snprintf( filename, 2048, format.c_str(), frame );
	if ( name_chars < 0 || name_chars >= 2048 )
	{
		throw std::runtime_error( "unable to create filename for frame " + std::to_string( frame ) );
	}
	return filename;
}

void read_metadata( const std::string &filename, FilmInfoHeader &film_info, TelevisionInfoHeader &tv_info )
{
	FileInformation file_info;
	std::ifstream file( filename, std::ios_base::binary | std::ios_base::in );
	if ( !file )
	{
		throw std::runtime_error( "failed to open file " + filename );
	}
	file.exceptions( std::ios_base::failbit | std::ios_base::badbit );
	file.read( reinterpret_cast<char *>( &file_info ), sizeof( file_info ) );

	const bool big_endian = is_big_endian( file_info );
	if ( big_endian )
	{
		swap_bytes( file_info );
	}

	if ( file_info.generic_header_size != sizeof( FileInformation ) + sizeof( ImageHeader ) + sizeof( OrientationHeader ) )
	{
		std::cerr << "DPX header size (" << file_info.generic_header_size << " bytes) is not as expected (" << sizeof( FileInformation ) + sizeof( ImageHeader ) + sizeof( OrientationHeader ) << " bytes)\n";
		throw std::runtime_error( "invalid input file " + filename );
	}

	if ( file_info.industry_header_size != sizeof( FilmInfoHeader ) + sizeof( TelevisionInfoHeader ) )
	{
		std::cerr << "Metadata size (" << file_info.industry_header_size << " bytes) is not as expected (" << sizeof( FilmInfoHeader ) + sizeof( TelevisionInfoHeader ) << " bytes)\n";
		throw std::runtime_error( "invalid input file " + filename );
	}

	file.seekg( file_info.generic_header_size );
	file.read( reinterpret_cast<char *>( &film_info ), sizeof( film_info ) );
	file.read( reinterpret_cast<char *>( &tv_info ), sizeof( tv_info ) );

	if ( big_endian )
	{
		swap_bytes( film_info );
		swap_bytes( tv_info );
	}

	file.close();
}

void write_metadata( const std::string &filename, FilmInfoHeader &film_info, TelevisionInfoHeader &tv_info )
{
	FileInformation file_info;
	std::fstream file( filename, std::ios_base::binary | std::ios_base::in | std::ios_base::out );
	if ( !file )
	{
		throw std::runtime_error( "failed to open file " + filename );
	}
	file.exceptions( std::ios_base::failbit | std::ios_base::badbit );
	file.read( reinterpret_cast<char *>( &file_info ), sizeof( file_info ) );

	const bool big_endian = is_big_endian( file_info );
	if ( big_endian )
	{
		swap_bytes( file_info );
	}

	if ( file_info.generic_header_size != sizeof( FileInformation ) + sizeof( ImageHeader ) + sizeof( OrientationHeader ) )
	{
		std::cerr << "DPX header size (" << file_info.generic_header_size << " bytes) is not as expected (" << sizeof( FileInformation ) + sizeof( ImageHeader ) + sizeof( OrientationHeader ) << " bytes)\n";
		throw std::runtime_error( "invalid output file " + filename );
	}

	if ( file_info.industry_header_size != sizeof( FilmInfoHeader ) + sizeof( TelevisionInfoHeader ) )
	{
		std::cerr << "Metadata size (" << file_info.industry_header_size << " bytes) is not as expected (" << sizeof( FilmInfoHeader ) + sizeof( TelevisionInfoHeader ) << " bytes)\n";
		throw std::runtime_error( "invalid output file " + filename );
	}

	if ( big_endian )
	{
		swap_bytes( film_info );
		swap_bytes( tv_info );
	}

	file.seekp( file_info.generic_header_size );
	file.write( reinterpret_cast<const char *>( &film_info ), sizeof( film_info ) );
	file.write( reinterpret_cast<const char *>( &tv_info ), sizeof( tv_info ) );
	if ( !file )
	{
		throw std::runtime_error( "failed to write to file " + filename );
	}
	else
	{
		std::cout << "Replaced metadata in " << filename << '\n';
	}
	file.close();
}

int main( int argc, char *argv[] )
{
	if ( argc != 5 )
	{
		std::cerr << "Usage:\n";
		std::cerr << "\t" << argv[0] << " input_%08d.dpx output_%08d.dpx first_frame last_frame\n";
		return -1;
	}

	const std::string input = argv[1];
	const std::string output = argv[2];
	const int start_frame = std::stoi( argv[3] );
	const int end_frame = std::stoi( argv[4] );

	FilmInfoHeader film_info;
	TelevisionInfoHeader tv_info;

	int result = 0;

	std::cout << "Processing frames " << start_frame << " to " << end_frame << '\n';
	for ( int frame = start_frame; frame <= end_frame; ++frame )
	{
		try
		{
			const std::string inputFilename = compose_filename( input, frame );
			const std::string outputFilename = compose_filename( output, frame );
			read_metadata( inputFilename, film_info, tv_info );
			write_metadata( outputFilename, film_info, tv_info );
		}
		catch ( std::exception &e )
		{
			std::cerr << "Error: " << e.what() << std::endl;
			result = -1;
		}

	}

	return result;
}

