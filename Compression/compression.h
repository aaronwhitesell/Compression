#pragma once

#include <iostream>
#include <string>

namespace Compression
{
	/// <summary>Compress a stream</summary>
	int compression(std::istream &in, std::ostream &out);
	/// <summary>Decompress a stream</summary>
	int decompression(std::istream &in, std::ostream &out);
	/// <summary>Compress a string in place</summary>
	int compression(std::string &data);
	/// <summary>Decompress a string in place</summary>
	int decompression(std::string &data);
}
