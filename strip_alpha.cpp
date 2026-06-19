
#include "dpx.h"
#include "image.h"

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

	int result = 0;

	std::cout << "Processing frames " << start_frame << " to " << end_frame << '\n';
	for ( int frame = start_frame; frame <= end_frame; ++frame )
	{
		try
		{
			// Open input DPX file
			const std::string inputFilename = compose_filename( input, frame );
			std::ifstream inputFile( inputFilename, std::ios_base::binary | std::ios_base::in );
			if ( !inputFile )
			{
				throw std::runtime_error( "failed to open file " + inputFilename );
			}
			inputFile.exceptions( std::ios_base::failbit | std::ios_base::badbit );

			// Read DPX header
			DpxHeader header = read_header( inputFile, inputFilename );
			std::cout << header << '\n';

			// Check that the image is something we can handle...
			if ( header.image.element_count != 1 )
			{
				throw std::runtime_error( "Expected one element in image" );
			}
			if ( header.image.elements[0].data_sign != 0 )
			{
				throw std::runtime_error( "Expected image element to be unsigned" );
			}
			if ( header.image.elements[0].bit_size != 10 )
			{
				throw std::runtime_error( "Expected image element to be 10 bits" );
			}
			if ( header.image.elements[0].packing != 1 )
			{
				throw std::runtime_error( "Expected image element to be filled" );
			}
			if ( header.image.elements[0].encoding != 0 )
			{
				throw std::runtime_error( "Expected image element to not be encoded" );
			}
			if ( header.image.elements[0].eol_padding != 0 )
			{
				throw std::runtime_error( "Expected image element to not have padding at end of line" );
			}
			if ( header.image.elements[0].descriptor != Descriptor::RGBA )
			{
				throw std::runtime_error( "Expected image element to be RGBA" );
			}

			// Read image data
			size_t pixels = header.image.pixels_per_line * header.image.lines_per_element;
			size_t imagePels = pixels * 4;
			size_t dataBytes = ( header.file_info.file_size - header.file_info.image_offset );
			if ( dataBytes % 4 != 0 )
			{
				throw std::runtime_error( "Data size is not a multiple of 4 bytes, got " + std::to_string( dataBytes ) );
			}

			std::vector<uint32_t> image;
			size_t elements = dataBytes / 4;
			image.resize( elements );
			inputFile.read( reinterpret_cast<char *>( image.data() ), sizeof( uint32_t ) * elements );

			// Strip alpha channel
			image = pack_10bits( demangle( unpack_10bits( image, imagePels ) ) );
			std::cerr << "PIXELS: " << image.size() << std::endl;

			// Fix up the header
			clean_header( header );

			header.file_info.version[1] = '1';

			header.image.elements[0].descriptor = Descriptor::RGB;
			header.image.elements[0].low_data = 0;
			header.image.elements[0].low_quantity = 0.0;
			header.image.elements[0].high_data = 1023;
			header.image.elements[0].high_quantity = 2.047;
			header.image.elements[0].transfer = Transfer::PRINTING_DENSITY;
			header.image.elements[0].colorimetric = Colorimetric::PRINTING_DENSITY;
			header.image.elements[0].colorimetric = Colorimetric::PRINTING_DENSITY;

			// Open output DPX file
			const std::string outputFilename = compose_filename( output, frame );
			std::fstream outputFile( outputFilename, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc );
			if ( !outputFile )
			{
				throw std::runtime_error( "failed to open output file " + outputFilename );
			}
			outputFile.exceptions( std::ios_base::failbit | std::ios_base::badbit );

			write_header( outputFile, header );
			std::cout << "After header: " << outputFile.tellp() << std::endl;
			outputFile.write( reinterpret_cast<char*>( image.data() ), sizeof( uint32_t ) * image.size() );
			outputFile.close();
		}
		catch ( std::exception &e )
		{
			std::cerr << "Error: " << e.what() << std::endl;
			result = -1;
		}
	}

	return result;
}

