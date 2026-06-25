
#include <cstdint>
#include <iostream>
#include <array>
#include <cstring>

template<size_t N>
using String = std::array<char,N>;

enum class Transfer : uint8_t
{
	USER_DEFINED = 0,
	PRINTING_DENSITY = 1,
	LINEAR = 2,
	LOGARITHMIC = 3,
	UNSPECIFIED_VIDEO = 4,
	SMPTE_240M = 5,
	CCIR_709_1 = 6,
	CCIR_601_2_SYSTEM_B_OR_G = 7,
	CCIR_601_2_SYSTEM_M = 8,
	NTSC_COMPOSITE_VIDEO = 9,
	PAL_COMPOSITE_VIDEO = 10,
	Z_LINEAR = 11,
	Z_HOMOGENEOUS = 12
};

enum class Descriptor : uint8_t
{
	USER_DEFINED = 0,
	RED = 1,
	GREEN = 2,
	BLUE = 3,
	ALPHA = 4,
	LUMINANCE = 6,
	CHROMINANCE = 7,
	DEPTH = 8,
	COMPOSITE_VIDEO = 9,

	RGB = 50,
	RGBA = 51,
	ABGR = 52,

	CBYCRY = 100,
	CBYACRYA = 101,
	CBYCR = 102,
	CBYCRA = 103,

	USER_2_COMPONENT = 150,
	USER_3_COMPONENT = 151,
	USER_4_COMPONENT = 152,
	USER_5_COMPONENT = 153,
	USER_6_COMPONENT = 154,
	USER_7_COMPONENT = 155,
	USER_8_COMPONENT = 156
};

enum class Colorimetric : uint8_t
{
	USER_DEFINED = 0,
	PRINTING_DENSITY_LEGACY = 1,
	PRINTING_DENSITY = 2,
	UNSPECIFIED_VIDEO = 4,
	SMPTE_240M = 5,
	CCIR_709_1 = 6,
	CCIR_601_2_SYSTEM_B_OR_G = 7,
	CCIR_601_2_SYSTEM_M = 8,
	NTSC_COMPOSITE_VIDEO = 9,
	PAL_COMPOSITE_VIDEO = 10,
	LINEAR_ZDEPTH = 11,
	HOMOGENOUS_ZDEPTH = 12
};

#pragma pack( push, 1)
struct FileInformation
{
	String<4> magic;
	uint32_t image_offset;
	String<8> version;
	uint32_t file_size;
	uint32_t ditto_key;
	uint32_t generic_header_size;
	uint32_t industry_header_size;
	uint32_t user_data_size;
	String<100> file_name;
	String<24> creation_time;
	String<100> creator;
	String<200> project;
	String<200> copyright;
	uint32_t key;
	char reserved[104];
};

struct ImageHeader
{
	uint16_t orientation;
	uint16_t element_count;
	uint32_t pixels_per_line;
	uint32_t lines_per_element;
	struct
	{
		uint32_t data_sign;
		uint32_t low_data;
		float low_quantity;
		uint32_t high_data;
		float high_quantity;
		Descriptor descriptor;
		Transfer transfer;
		Colorimetric colorimetric;
		uint8_t bit_size;
		uint16_t packing;
		uint16_t encoding;
		uint32_t data_offset;
		uint32_t eol_padding;
		uint32_t eoi_padding;
		String<32> description;
	} elements[8];
	uint8_t reserved[52];
};

struct OrientationHeader
{
	uint32_t x_offset;
	uint32_t y_offset;
	float x_center;
	float y_center;
	uint32_t x_original_size;
	uint32_t y_original_size;
	String<100> file_name;
	String<24> time_date;
	String<32> input_name;
	String<32> input_serial;
	uint16_t border[4];
	uint32_t aspect_ratio[2];
	char reserved[28];
};

struct FilmInfoHeader
{
	String<2> film_mfg_id;
	String<2> film_type;
	String<2> offset;
	String<6> prefix;
	String<4> count;
	String<32> format;
	uint32_t frame_position;
	uint32_t sequence_length;
	uint32_t held_count;
	float frame_rate;
	float shutter_angle;
	String<32> frame_id;
	String<100> slate_info;
	char reserved[56];
};

struct TelevisionInfoHeader
{
	uint32_t time_code;
	uint32_t user_bits;
	uint8_t interlace;
	uint8_t field_number;
	uint8_t video_signal;
	uint8_t padding;
	float horz_sample_rate;
	float vert_sample_rate;
	float frame_rate;
	float time_offset;
	float gamma;
	float black_level;
	float black_gain;
	float breakpoint;
	float white_level;
	float integration_times;
	char reserved[76];
};

struct DpxHeader
{
	FileInformation file_info;
	ImageHeader image;
	OrientationHeader orientation;
	FilmInfoHeader film_info;
	TelevisionInfoHeader tv_info;
};

template <std::size_t N>
void copy_string( String<N> &dest, const std::string &src )
{
    if ( src.size() > N )
	{
        throw std::runtime_error( "string overflow" );
    }
    std::copy( src.begin(), src.end(), dest.begin() );
    if ( src.size() < N )
	{
        std::fill( dest.begin() + src.size(), dest.end(), 0 );
    }
}

template<std::size_t N>
std::string to_string( const String<N> &str )
{
	return std::string( str.data(), strnlen( str.data(), N ) );
}

inline float dpx_undefined_float( void )
{
	uint32_t undefined = 0xFFFFFFFF;
	float result;
	std::memcpy( &result, &undefined, sizeof(result) );
	return result;
}

#pragma pack( pop )

bool is_big_endian( const FileInformation &file_info );
void swap_bytes( FileInformation &file_info );
void swap_bytes( ImageHeader &image );
void swap_bytes( OrientationHeader &orientation );
void swap_bytes( FilmInfoHeader &film_info );
void swap_bytes( TelevisionInfoHeader &tv_info );

void clean_header( DpxHeader &header );

void write_header( std::ostream &file, DpxHeader header );
DpxHeader read_header( std::istream &file, const std::string &filename );

std::ostream &operator<<( std::ostream &output, const FileInformation &file_info );
std::ostream &operator<<( std::ostream &output, const OrientationHeader &orientation );
std::ostream &operator<<( std::ostream &output, const ImageHeader &image );
std::ostream &operator<<( std::ostream &output, const FilmInfoHeader &film_info );
std::ostream &operator<<( std::ostream &output, const TelevisionInfoHeader &tv_info );
std::ostream &operator<<( std::ostream &output, const DpxHeader &header );

