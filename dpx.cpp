
#include "dpx.h"
#include "byteswap.h"

#include <string>
#include <cstdint>
#include <cmath>
#include <iostream>
#include <sstream>

bool is_big_endian( const FileInformation &file_info )
{
	return to_string( file_info.magic ) == "SDPX";
}

void swap_bytes( FileInformation &file_info )
{
	file_info.image_offset = byteswap( file_info.image_offset );
	file_info.file_size = byteswap( file_info.file_size );
	file_info.ditto_key = byteswap( file_info.ditto_key );
	file_info.generic_header_size = byteswap( file_info.generic_header_size );
	file_info.industry_header_size = byteswap( file_info.industry_header_size );
	file_info.user_data_size = byteswap( file_info.user_data_size );
	file_info.key = byteswap( file_info.key );
}

void swap_bytes( ImageHeader &image )
{
	image.orientation = byteswap( image.orientation );
	image.element_count = byteswap( image.element_count );
	image.pixels_per_line = byteswap( image.pixels_per_line );
	image.lines_per_element = byteswap( image.lines_per_element );
	for ( uint16_t i = 0; i < 8; ++i )
	{
		image.elements[i].data_sign = byteswap( image.elements[i].data_sign );
		image.elements[i].low_data = byteswap( image.elements[i].low_data );
		image.elements[i].low_quantity = byteswap( image.elements[i].low_quantity );
		image.elements[i].high_data = byteswap( image.elements[i].high_data );
		image.elements[i].high_quantity = byteswap( image.elements[i].high_quantity );
		image.elements[i].packing = byteswap( image.elements[i].packing );
		image.elements[i].encoding = byteswap( image.elements[i].encoding );
		image.elements[i].data_offset = byteswap( image.elements[i].data_offset );
		image.elements[i].eol_padding = byteswap( image.elements[i].eol_padding );
		image.elements[i].eoi_padding = byteswap( image.elements[i].eoi_padding );
	}
}

void swap_bytes( OrientationHeader &orientation )
{
	orientation.x_offset = byteswap( orientation.x_offset );
	orientation.y_offset = byteswap( orientation.y_offset );
	orientation.x_center = byteswap( orientation.x_center );
	orientation.y_center = byteswap( orientation.y_center );
	orientation.x_original_size = byteswap( orientation.x_original_size );
	orientation.y_original_size = byteswap( orientation.y_original_size );
	orientation.border[0] = byteswap( orientation.border[0] );
	orientation.border[1] = byteswap( orientation.border[1] );
	orientation.border[2] = byteswap( orientation.border[2] );
	orientation.border[3] = byteswap( orientation.border[3] );
	orientation.aspect_ratio[0] = byteswap( orientation.aspect_ratio[0] );
	orientation.aspect_ratio[1] = byteswap( orientation.aspect_ratio[1] );
}

void swap_bytes( FilmInfoHeader &film_info )
{
	film_info.frame_position = byteswap( film_info.frame_position );
	film_info.sequence_length = byteswap( film_info.sequence_length );
	film_info.held_count = byteswap( film_info.held_count );
	film_info.frame_rate = byteswap( film_info.frame_rate );
	film_info.shutter_angle = byteswap( film_info.shutter_angle );
}

void swap_bytes( TelevisionInfoHeader &tv_info )
{
	tv_info.time_code = byteswap( tv_info.time_code );
	tv_info.user_bits = byteswap( tv_info.user_bits );
	tv_info.horz_sample_rate = byteswap( tv_info.horz_sample_rate );
	tv_info.vert_sample_rate = byteswap( tv_info.vert_sample_rate );
	tv_info.frame_rate = byteswap( tv_info.frame_rate );
	tv_info.time_offset = byteswap( tv_info.time_offset );
	tv_info.gamma = byteswap( tv_info.gamma );
	tv_info.black_level = byteswap( tv_info.black_level );
	tv_info.black_gain = byteswap( tv_info.black_gain );
	tv_info.breakpoint = byteswap( tv_info.breakpoint );
	tv_info.white_level = byteswap( tv_info.white_level );
	tv_info.integration_times = byteswap( tv_info.integration_times );
}

DpxHeader read_header( std::istream &file, const std::string &filename )
{
	DpxHeader header;

	file.read( reinterpret_cast<char *>( &header.file_info ), sizeof( header.file_info ) );

	const bool big_endian = is_big_endian( header.file_info );
	if ( big_endian )
	{
		swap_bytes( header.file_info );
	}

	if ( header.file_info.generic_header_size != sizeof( FileInformation ) + sizeof( ImageHeader ) + sizeof( OrientationHeader ) )
	{
		std::cerr << "DPX header size (" << header.file_info.generic_header_size << " bytes) is not as expected (" << sizeof( FileInformation ) + sizeof( ImageHeader ) + sizeof( OrientationHeader ) << " bytes)\n";
		throw std::runtime_error( "invalid input file" + filename );
	}

	if ( header.file_info.industry_header_size != sizeof( FilmInfoHeader ) + sizeof( TelevisionInfoHeader ) )
	{
		std::cerr << "Metadata size (" << header.file_info.industry_header_size << " bytes) is not as expected (" << sizeof( FilmInfoHeader ) + sizeof( TelevisionInfoHeader ) << " bytes)\n";
		throw std::runtime_error( "invalid input file " + filename );
	}

	file.read( reinterpret_cast<char *>( &header.image ), sizeof( header.image ) );
	file.read( reinterpret_cast<char *>( &header.orientation ), sizeof( header.orientation ) );
	file.read( reinterpret_cast<char *>( &header.film_info ), sizeof( header.film_info ) );
	file.read( reinterpret_cast<char *>( &header.tv_info ), sizeof( header.tv_info ) );

	file.seekg( header.file_info.image_offset, std::ios_base::beg );

	if ( big_endian )
	{
		swap_bytes( header.image );
		swap_bytes( header.orientation );
		swap_bytes( header.film_info );
		swap_bytes( header.tv_info );
	}

	return header;
}

namespace
{
void scrub_float( float &x )
{
	if ( std::isnan( x ) || std::isinf( x ) )
	{
		x = dpx_undefined_float();
	}
}

std::string print_float( float x )
{
	std::ostringstream oss;
	if ( std::isnan( x ) || std::isinf( x ) )
		oss << "undefined (" << x << ')';
	else
		oss << x;
	return oss.str();
}
}

void clean_header( DpxHeader &header )
{
	for ( uint32_t i = 0; i < header.image.element_count; ++i )
	{
		scrub_float( header.image.elements[i].low_quantity );
		scrub_float( header.image.elements[i].high_quantity );
	}

	scrub_float( header.orientation.x_center );
	scrub_float( header.orientation.y_center );

	scrub_float( header.film_info.frame_rate );
	scrub_float( header.film_info.shutter_angle );

	scrub_float( header.tv_info.horz_sample_rate );
	scrub_float( header.tv_info.vert_sample_rate );
	scrub_float( header.tv_info.frame_rate );
	scrub_float( header.tv_info.time_offset );
	scrub_float( header.tv_info.gamma );
	scrub_float( header.tv_info.black_level );
	scrub_float( header.tv_info.black_gain );
	scrub_float( header.tv_info.breakpoint );
	scrub_float( header.tv_info.white_level );
	scrub_float( header.tv_info.integration_times );
}

void write_header( std::ostream &file, DpxHeader header )
{
	uint32_t image_offset = header.file_info.image_offset;

	const bool big_endian = is_big_endian( header.file_info );
	if ( big_endian )
	{
		swap_bytes( header.file_info );
		swap_bytes( header.image );
		swap_bytes( header.orientation );
		swap_bytes( header.film_info );
		swap_bytes( header.tv_info );
	}

	file.write( reinterpret_cast<char *>( &header.file_info ), sizeof( header.file_info ) );
	file.write( reinterpret_cast<char *>( &header.image ), sizeof( header.image ) );
	file.write( reinterpret_cast<char *>( &header.orientation ), sizeof( header.orientation ) );
	file.write( reinterpret_cast<char *>( &header.film_info ), sizeof( header.film_info ) );
	file.write( reinterpret_cast<char *>( &header.tv_info ), sizeof( header.tv_info ) );

	file.seekp( image_offset, std::ios_base::beg );
}

const std::string orientation_desc( uint16_t orientation )
{
	switch ( orientation )
	{
		case 0: return "Left to right, top to bottom";
		case 1: return "Right to left, top to bottom";
		case 2: return "Left to right, bottom to top";
		case 3: return "Right to left, bottom to top";
		case 4: return "Top to bottom, left to right";
		case 5: return "Top to bottom, right to left";
		case 6: return "Bottom to top, left to right";
		case 7: return "Bottom to top, right to left";
	}
	return "Unknown orientation " + std::to_string( orientation );
}

std::string data_sign( uint8_t data_sign )
{
	switch ( data_sign )
	{
		case 0: return "Unsigned";
		case 1: return "Signed";
	}
	return "Unknown sign " + std::to_string( data_sign );
}

std::string descriptor_name( Descriptor descriptor )
{
	switch ( descriptor )
	{
		case Descriptor::USER_DEFINED: return "User defined";
		case Descriptor::RED: return "Red";
		case Descriptor::GREEN: return "Green";
		case Descriptor::BLUE: return "Blue";
		case Descriptor::ALPHA: return "Alpha";
		case Descriptor::LUMINANCE: return "Luminance";
		case Descriptor::CHROMINANCE: return "Chrominance";
		case Descriptor::DEPTH: return "Depth";
		case Descriptor::COMPOSITE_VIDEO: return "Composite Video";

		case Descriptor::RGB: return "RGB";
		case Descriptor::RGBA: return "RGBA";
		case Descriptor::ABGR: return "ABGR";

		case Descriptor::CBYCRY: return "CbYCrY";
		case Descriptor::CBYACRYA: return "CbYaCrYa";
		case Descriptor::CBYCR: return "CbYCr";
		case Descriptor::CBYCRA: return "CbYCra";

		case Descriptor::USER_2_COMPONENT: return "User-defined 2-component element";
		case Descriptor::USER_3_COMPONENT: return "User-defined 3-component element";
		case Descriptor::USER_4_COMPONENT: return "User-defined 4-component element";
		case Descriptor::USER_5_COMPONENT: return "User-defined 5-component element";
		case Descriptor::USER_6_COMPONENT: return "User-defined 6-component element";
		case Descriptor::USER_7_COMPONENT: return "User-defined 7-component element";
		case Descriptor::USER_8_COMPONENT: return "User-defined 8-component element";
	}
	return "Unknown descriptor " + std::to_string( static_cast<std::underlying_type<Descriptor>::type>( descriptor ) );
}

std::string transfer_name( Transfer transfer )
{
	switch ( transfer )
	{
		case Transfer::USER_DEFINED: return "User-defined";
		case Transfer::PRINTING_DENSITY: return "Printing density";
		case Transfer::LINEAR: return "Linear";
		case Transfer::LOGARITHMIC: return "Logarithmic";
		case Transfer::UNSPECIFIED_VIDEO: return "Unspecified video";
		case Transfer::SMPTE_240M: return "SMPTE 240M";
		case Transfer::CCIR_709_1: return "CCIR 709-1";
		case Transfer::CCIR_601_2_SYSTEM_B_OR_G: return "CCIR 601-2 system B or G";
		case Transfer::CCIR_601_2_SYSTEM_M: return "CCIR 601-2 system M";
		case Transfer::NTSC_COMPOSITE_VIDEO: return "NTSC composite video";
		case Transfer::PAL_COMPOSITE_VIDEO: return "PAL composite video";
		case Transfer::Z_LINEAR: return "Z linear";
		case Transfer::Z_HOMOGENEOUS: return "Z homogeneous";
	}
	return "Unknown transfer " + std::to_string( static_cast<std::underlying_type<Transfer>::type>( transfer ) );
}

std::string colorimetric_name( Colorimetric colorimetric )
{
	switch ( colorimetric )
	{
		case Colorimetric::USER_DEFINED: return "User-defined";
		case Colorimetric::PRINTING_DENSITY_LEGACY: return "Printing density (Legacy)";
		case Colorimetric::PRINTING_DENSITY: return "Printing density";
		case Colorimetric::UNSPECIFIED_VIDEO: return "Unspecified video";
		case Colorimetric::SMPTE_240M: return "SMPTE 240M";
		case Colorimetric::CCIR_709_1: return "CCIR 709-1";
		case Colorimetric::CCIR_601_2_SYSTEM_B_OR_G: return "CCIR 601-2 system B or G";
		case Colorimetric::CCIR_601_2_SYSTEM_M: return "CCIR 601-2 system M";
		case Colorimetric::NTSC_COMPOSITE_VIDEO: return "NTSC composite video";
		case Colorimetric::PAL_COMPOSITE_VIDEO: return "PAL composite video";
	}
	return "Unknown colorimetric " + std::to_string( static_cast<std::underlying_type<Colorimetric>::type>( colorimetric ) );
}

std::string encrypt_key( uint32_t key )
{
	if ( key == 0xFFFFFFFF )
	{
		return "Not encrypted";
	}
	else
	{
		char buffer[32];
		sprintf( buffer, "%08X", key );
		return buffer;
	}
}

std::string packing_desc( uint16_t packing )
{
	switch ( packing )
	{
		case 0: return "Packed 32-bit words";
		case 1: return "Filled 32-bit words";
	}
	return "Unknown packing " + std::to_string( packing );
}

std::string encoding_desc( uint16_t packing )
{
	switch ( packing )
	{
		case 0: return "Not encoded";
		case 1: return "Run-length encoded";
	}
	return "Unknown encoding " + std::to_string( packing );
}

std::ostream &operator<<( std::ostream &output, const FileInformation &file_info )
{
	output << "Magic: " << to_string( file_info.magic ) << '\n';
	output << "Image Offset: " << file_info.image_offset << '\n';
	output << "Version: " << to_string( file_info.version ) << '\n';
	output << "File size: " << file_info.file_size << '\n';
	output << "Ditto Key: " << file_info.ditto_key << '\n';
	output << "Generic header size: " << file_info.generic_header_size << '\n';
	output << "Industry header size: " << file_info.industry_header_size << '\n';
	output << "User data size: " << file_info.user_data_size << '\n';
	output << "File name: " << to_string( file_info.file_name ) << '\n';
	output << "Creation time: " << to_string( file_info.creation_time ) << '\n';
	output << "Creator: " << to_string( file_info.creator ) << '\n';
	output << "Project: " << to_string( file_info.project ) << '\n';
	output << "Copyright: " << to_string( file_info.copyright ) << '\n';
	output << "Encrypt Key: " << encrypt_key( file_info.key ) << '\n';
	return output;
}

std::ostream &operator<<( std::ostream &output, const ImageHeader &header )
{
	output << "Orientation: " << orientation_desc( header.orientation ) << '\n';
	output << "Element count: " << header.element_count << '\n';
	output << "Pixels per line: " << header.pixels_per_line << '\n';
	output << "Lines per element: " << header.lines_per_element << '\n';
	for ( int i = 0; i < std::min( header.element_count, uint16_t(8) ); ++i )
	{
		output << "Element " << i + 1 << ": \n";
		output << "\tData sign: " << data_sign( header.elements[i].data_sign ) << '\n';
		output << "\tLow data: " << header.elements[i].low_data << '\n';
		output << "\tLow quantity: " << print_float( header.elements[i].low_quantity ) << '\n';
		output << "\tHigh data: " << header.elements[i].high_data << '\n';
		output << "\tHigh quantity: " << print_float( header.elements[i].high_quantity ) << '\n';
		output << "\tDescriptor: " << descriptor_name( header.elements[i].descriptor ) << '\n';
		output << "\tTransfer: " << transfer_name( header.elements[i].transfer ) << '\n';
		output << "\tColorimetric: " << colorimetric_name( header.elements[i].colorimetric ) << '\n';
		output << "\tBit size: " << (int)header.elements[i].bit_size << '\n';
		output << "\tPacking: " << packing_desc( header.elements[i].packing ) << '\n';
		output << "\tEncoding: " << encoding_desc( header.elements[i].encoding ) << '\n';
		output << "\tData offset: " << header.elements[i].data_offset << '\n';
		output << "\tEnd of line padding: " << header.elements[i].eol_padding << '\n';
		output << "\tEnd of image padding: " << header.elements[i].eoi_padding << '\n';
		output << "\tDescription: " << to_string( header.elements[i].description ) << '\n';
	}
	return output;
}

std::ostream &operator<<( std::ostream &output, const OrientationHeader &orientation )
{
	output << "X offset: " << orientation.x_offset << '\n';
	output << "Y offset: " << orientation.y_offset << '\n';
	output << "X center: " << print_float( orientation.x_center ) << '\n';
	output << "Y center: " << print_float( orientation.y_center ) << '\n';
	output << "Original X size: " << orientation.x_original_size << '\n';
	output << "Original Y size: " << orientation.y_original_size << '\n';
	output << "File name: " << to_string( orientation.file_name ) << '\n';
	output << "Time date: " << to_string( orientation.time_date ) << '\n';
	output << "Input name: " << to_string( orientation.input_name ) << '\n';
	output << "Input serial: " << to_string( orientation.input_serial ) << '\n';
	output << "Border: " << orientation.border[0] << ", " << orientation.border[1] << ", " << orientation.border[2] << ", " << orientation.border[3] << '\n';
	output << "Aspect ratio: " << orientation.aspect_ratio[0] << " : " << orientation.aspect_ratio[1] << '\n';
	return output;
}

std::ostream &operator<<( std::ostream &output, const FilmInfoHeader &film_info )
{
	output << "Film Manufacturer ID: " << to_string( film_info.film_mfg_id ) << '\n';
	output << "Film type: " << to_string( film_info.film_type ) << '\n';
	output << "Offset perfs: " << to_string( film_info.offset ) << '\n';
	output << "Film prefix code: " << to_string( film_info.prefix ) << '\n';
	output << "Film count code: " << to_string( film_info.count ) << '\n';
	output << "Film format: " << to_string( film_info.format ) << '\n';
	output << "Frame position: " << film_info.frame_position << '\n';
	output << "Sequence length: " << film_info.sequence_length << '\n';
	output << "Held count: " << film_info.held_count << '\n';
	output << "Frame rate: " << print_float( film_info.frame_rate ) << '\n';
	output << "Shutter angle: " << print_float( film_info.shutter_angle ) << '\n';
	output << "Frame ID: " << to_string( film_info.frame_id ) << '\n';
	output << "Slate info: " << to_string( film_info.slate_info ) << '\n';
	return output;
}

std::ostream &operator<<( std::ostream &output, const TelevisionInfoHeader &tv_info )
{
	output << "Timecode: " << tv_info.time_code << '\n';
	output << "User bits: " << tv_info.user_bits << '\n';
	output << "Interlace: " << (int)tv_info.interlace << '\n';
	output << "Field number: " << (int)tv_info.field_number << '\n';
	output << "Video signal: " << (int)tv_info.video_signal << '\n';
	output << "Horizontal rate: " << print_float( tv_info.horz_sample_rate ) << '\n';
	output << "Vertical rate: " << print_float( tv_info.vert_sample_rate ) << '\n';
	output << "Frame rate: " << print_float( tv_info.frame_rate ) << '\n';
	output << "Time offset: " << print_float( tv_info.time_offset ) << '\n';
	output << "Gamma: " << print_float( tv_info.gamma ) << '\n';
	output << "Black level: " << print_float( tv_info.black_level ) << '\n';
	output << "Black gain: " << print_float( tv_info.black_gain ) << '\n';
	output << "Breakpoint: " << print_float( tv_info.breakpoint ) << '\n';
	output << "White level: " << print_float( tv_info.white_level ) << '\n';
	output << "Integration times: " << print_float( tv_info.integration_times ) << '\n';
	return output;
}

std::ostream &operator<<( std::ostream &output, const DpxHeader &header )
{
	output << "File\n";
	output << "====\n";
	output << header.file_info;

	output << "\nImage\n";
	output <<   "=====\n";
	output << header.image;

	output << "\nOrientation\n";
	output <<   "===========\n";
	output << header.orientation;

	output << "\nFilm Info\n";
	output <<   "=========\n";
	output << header.film_info;

	output << "\nTV Info\n";
	output <<   "=======\n";
	output << header.tv_info;
	return output;
}

