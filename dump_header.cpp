
#include "dpx.h"
#include "image.h"

#include <stdexcept>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

int main( int argc, char *argv[] )
{
	if ( argc < 2 )
	{
		throw std::runtime_error( "missing file" );
	}

	std::ifstream input( argv[1], std::ios_base::binary | std::ios_base::in );
	DpxHeader header = read_header( input, argv[1] );
	std::cout << header;
}
