
#include "dpx.h"
#include "byteswap.h"

#include <string>

bool is_big_endian( const FileInformation &file_info )
{
	return std::string( file_info.magic, 4 ) == "SDPX";
}

void swap_bytes( FileInformation &file_info )
{
	file_info.offset = byteswap( file_info.offset );
	file_info.file_size = byteswap( file_info.file_size );
	file_info.ditto_key = byteswap( file_info.ditto_key );
	file_info.generic_header_size = byteswap( file_info.generic_header_size );
	file_info.industry_header_size = byteswap( file_info.industry_header_size );
	file_info.user_data_size = byteswap( file_info.user_data_size );
	file_info.key = byteswap( file_info.key );
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

std::ostream &operator<<( std::ostream &output, const FileInformation &file_info )
{
	output << "Magic: " << std::string( file_info.magic, 4 ) << '\n';
	output << "Offset: " << file_info.offset << '\n';
	output << "Version: " << std::string( file_info.version, 8 ) << '\n';
	output << "File size: " << file_info.file_size << '\n';
	output << "Ditto Key: " << file_info.ditto_key << '\n';
	output << "Generic header size: " << file_info.generic_header_size << '\n';
	output << "Industry header size: " << file_info.industry_header_size << '\n';
	output << "User data size: " << file_info.user_data_size << '\n';
	output << "Creation time: " << std::string( file_info.creation_time, 24 ) << '\n';
	output << "Creator: " << std::string( file_info.creator, 100 ) << '\n';
	output << "Project: " << std::string( file_info.project, 200 ) << '\n';
	output << "Copyright: " << std::string( file_info.copyright, 200 ) << '\n';
	output << "Key: " << file_info.key << '\n';
	return output;
}

std::ostream &operator<<( std::ostream &output, const FilmInfoHeader &film_info )
{
	output << "Film Manufacturer ID: " << std::string( film_info.film_mfg_id, 2 ) << ' ' << (int)film_info.film_mfg_id[1] << '\n';
	output << "Film type: " << std::string( film_info.film_type, 2 ) << '\n';
	output << "Offset perfs: " << std::string( film_info.offset, 2 ) << '\n';
	output << "Film prefix code: " << std::string( film_info.prefix, 6 ) << '\n';
	output << "Film count code: " << std::string( film_info.count, 4 ) << '\n';
	output << "Film format: " << std::string( film_info.format, 32 ) << '\n';
	output << "Frame position: " << film_info.frame_position << '\n';
	output << "Sequence length: " << film_info.sequence_length << '\n';
	output << "Held count: " << film_info.held_count << '\n';
	output << "Frame rate: " << film_info.frame_rate << '\n';
	output << "Shutter angle: " << film_info.shutter_angle << '\n';
	output << "Frame ID: " << std::string( film_info.frame_id, 2 ) << '\n';
	output << "Slate info: " << std::string( film_info.slate_info, 100 ) << '\n';
	return output;
}

std::ostream &operator<<( std::ostream &output, const TelevisionInfoHeader &tv_info )
{
	output << "Timecode: " << tv_info.time_code << '\n';
	output << "User bits: " << tv_info.user_bits << '\n';
	output << "Interlace: " << (int)tv_info.interlace << '\n';
	output << "Field number: " << (int)tv_info.field_number << '\n';
	output << "Video signal: " << (int)tv_info.video_signal << '\n';
	output << "Horizontal rate: " << tv_info.horz_sample_rate << '\n';
	output << "Vertical rate: " << tv_info.vert_sample_rate << '\n';
	output << "Frame rate: " << tv_info.frame_rate << '\n';
	output << "Time offset: " << tv_info.time_offset << '\n';
	output << "Gamma: " << tv_info.gamma << '\n';
	output << "Black level: " << tv_info.black_level << '\n';
	output << "Black gain: " << tv_info.black_gain << '\n';
	output << "Breakpoint: " << tv_info.breakpoint << '\n';
	output << "White level: " << tv_info.white_level << '\n';
	output << "Integration times: " << tv_info.integration_times << '\n';
	return output;
}


