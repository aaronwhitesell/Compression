#include "compression.h"
#define MINIZ_HEADER_FILE_ONLY
#include "miniz.c"

#include <algorithm>
#include <limits.h>
#include <sstream>

namespace Compression
{

	using uint8 = unsigned char;
	using uint = unsigned int;

	const uint bufSize = 256000; // 128k to 256k is recommended, if memory is available
	const int level = Z_BEST_COMPRESSION;

	int compression(std::istream &in, std::ostream &out)
	{
		std::cout << "Compression\n";

		uint8 inbuf[bufSize];
		uint8 outbuf[bufSize];

		// Determine input's size
		in.seekg(0, std::ios::end);
		const std::streampos inputSize = in.tellg();
		in.seekg(0, std::ios::beg);

		if ((inputSize < 0) || (inputSize > std::numeric_limits<int>::max()))
		{
			// To remove limitation implement chunking
			std::cout << "Input size must be between 0 and 2 GB\n";
			return EXIT_FAILURE;
		}

		// Init the z_stream
		z_stream stream;
		memset(&stream, 0, sizeof(stream));
		stream.next_in = inbuf;
		stream.avail_in = 0;
		stream.next_out = outbuf;
		stream.avail_out = bufSize;

		// Compression
		uint inputRemaining = static_cast<uint>(inputSize);

		if (deflateInit(&stream, level) != Z_OK)
		{
			std::cout << "deflateInit() failed\n";
			return EXIT_FAILURE;
		}

		for (;;)
		{
			int status;
			if (!stream.avail_in)
			{
				// Input buffer is empty, so read more bytes from input file
				uint n = std::min(bufSize, inputRemaining);

				if (!in.read(reinterpret_cast<char*>(inbuf), n))
				{
					std::cout << "Failed reading input\n";
					return EXIT_FAILURE;
				}

				stream.next_in = inbuf;
				stream.avail_in = n;

				inputRemaining -= n;
				std::cout << "Input bytes remaining: " << inputRemaining << "\n";
			}

			status = deflate(&stream, inputRemaining ? Z_NO_FLUSH : Z_FINISH);

			if ((status == Z_STREAM_END) || (!stream.avail_out))
			{
				// Output buffer is full, or compression is done, so write buffer to output file.
				uint n = bufSize - stream.avail_out;

				if (!out.write(reinterpret_cast<char*>(outbuf), n))
				{
					std::cout << "Failed writing output\n";
					return EXIT_FAILURE;
				}

				stream.next_out = outbuf;
				stream.avail_out = bufSize;
			}

			if (status == Z_STREAM_END)
			{
				break;
			}
			else if (status != Z_OK)
			{
				std::cout << "deflate() failed with status " << status << "\n";
				return EXIT_FAILURE;
			}
		}

		if (deflateEnd(&stream) != Z_OK)
		{
			std::cout << "deflateEnd() failed\n";
			return EXIT_FAILURE;
		}

		std::cout << "Total input bytes: " << (mz_uint32)stream.total_in << "\n";
		std::cout << "Total output bytes: " << (mz_uint32)stream.total_out << "\n";
		std::cout << "Success\n\n";
		return EXIT_SUCCESS;
	}

	int decompression(std::istream &in, std::ostream &out)
	{
		std::cout << "Decompression\n";

		uint8 inbuf[bufSize];
		uint8 outbuf[bufSize];

		in.seekg(0, std::ios::end);
		const std::streampos inputSize = in.tellg();
		in.seekg(0, std::ios::beg);

		if ((inputSize < 0) || (inputSize > std::numeric_limits<int>::max()))
		{
			// To remove limitation implement chunking
			std::cout << "Input size must be between 0 and 2 GB\n";
			return EXIT_FAILURE;
		}

		// Init the z_stream
		z_stream stream;
		memset(&stream, 0, sizeof(stream));
		stream.next_in = inbuf;
		stream.avail_in = 0;
		stream.next_out = outbuf;
		stream.avail_out = bufSize;

		// Decompression.
		uint inputRemaining = static_cast<uint>(inputSize);

		if (inflateInit(&stream))
		{
			std::cout << "inflateInit() failed\n";
			return EXIT_FAILURE;
		}

		for (;;)
		{
			int status;
			if (!stream.avail_in)
			{
				// Input buffer is empty, so read more bytes from input file.
				uint n = std::min(bufSize, inputRemaining);

				if (!in.read(reinterpret_cast<char*>(inbuf), n))
				{
					std::cout << "Failed reading input\n";
					return EXIT_FAILURE;
				}

				stream.next_in = inbuf;
				stream.avail_in = n;

				inputRemaining -= n;
			}

			status = inflate(&stream, Z_SYNC_FLUSH);

			if ((status == Z_STREAM_END) || (!stream.avail_out))
			{
				// Output buffer is full, or decompression is done, so write buffer to output file.
				uint n = bufSize - stream.avail_out;

				if (!out.write(reinterpret_cast<char*>(outbuf), n))
				{
					std::cout << "Failed writing output\n";
					return EXIT_FAILURE;
				}
				stream.next_out = outbuf;
				stream.avail_out = bufSize;
			}

			if (status == Z_STREAM_END)
			{
				break;
			}
			else if (status != Z_OK)
			{
				std::cout << "inflate() failed with status " << status << "\n";
				return EXIT_FAILURE;
			}
		}

		if (inflateEnd(&stream) != Z_OK)
		{
			std::cout << "inflateEnd() failed\n";
			return EXIT_FAILURE;
		}

		std::cout << "Total input bytes: " << (mz_uint32)stream.total_in << "\n";
		std::cout << "Total output bytes: " << (mz_uint32)stream.total_out << "\n";
		std::cout << "Success\n\n";
		return EXIT_SUCCESS;
	}

	int compression(std::string &data)
	{
		std::istringstream in(data);
		std::ostringstream out;

		const int result = compression(in, out);
		if (result == EXIT_SUCCESS)
		{
			data = out.str();
		}

		return result;
	}

	int decompression(std::string &data)
	{
		std::istringstream in(data);
		std::ostringstream out;

		const int result = decompression(in, out);
		if (result == EXIT_SUCCESS)
		{
			data = out.str();
		}

		return result;
	}

} // namespace Compression
