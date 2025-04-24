
#include <cstdint>
#include <iostream>

#pragma pack( push, 1)
struct FileInformation
{
	char magic[4];
	uint32_t offset;
	char version[8];
	uint32_t file_size;
	uint32_t ditto_key;
	uint32_t generic_header_size;
	uint32_t industry_header_size;
	uint32_t user_data_size;
	char file_name[100];
	char creation_time[24];
	char creator[100];
	char project[200];
	char copyright[200];
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
		uint8_t descriptor;
		uint8_t transfer;
		uint8_t colorimetric;
		uint8_t bit_size;
		uint16_t packing;
		uint16_t encoding;
		uint32_t dataOffset;
		uint32_t endOfLinePadding;
		uint32_t endOfImagePadding;
		char description[32];
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
	char file_name[100];
	char time_date[24];
	char input_name[32];
	char input_serial[32];
	uint16_t border[4];
	uint32_t aspect_Ratio[2];
	char reserved[28];
};

struct FilmInfoHeader
{
	char film_mfg_id[2];
	char film_type[2];
	char offset[2];
	char prefix[6];
	char count[4];
	char format[32];
	uint32_t frame_position;
	uint32_t sequence_length;
	uint32_t held_count;
	float frame_rate;
	float shutter_angle;
	char frame_id[32];
	char slate_info[100];
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

#pragma pack( pop )

bool is_big_endian( const FileInformation &file_info );
void swap_bytes( FileInformation &file_info );
void swap_bytes( FilmInfoHeader &film_info );
void swap_bytes( TelevisionInfoHeader &tv_info );

std::ostream &operator<<( std::ostream &output, const FileInformation &file_info );
std::ostream &operator<<( std::ostream &output, const FilmInfoHeader &film_info );
std::ostream &operator<<( std::ostream &output, const TelevisionInfoHeader &tv_info );

