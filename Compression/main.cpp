#include "compression.h"
#include <fstream>
#include <sstream>
#include <string>

namespace {
	const std::string alphabet("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
		"abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
		"abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
		"abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
		"abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
}

void writeFile()
{
	std::ofstream outFile("alphabet.txt", std::ios::binary);
	outFile << alphabet;
}

void compressFile()
{
	std::cout << "File ";
	std::ifstream inFile("alphabet.txt", std::ios::binary);
	std::ofstream outFile("compressed.txt", std::ios::binary);

	Compression::compression(inFile, outFile);
}

void decompressFile()
{
	std::cout << "File ";
	std::ifstream inFile("compressed.txt", std::ios::binary);
	std::ofstream outFile("decompressed.txt", std::ios::binary);

	Compression::decompression(inFile, outFile);
}

std::string compressStringStream()
{
	std::cout << "StringStream ";
	std::istringstream iss(alphabet);
	std::ostringstream oss;

	Compression::compression(iss, oss);

	return oss.str();
}

void decompressStringStream(const std::string &compressed)
{
	std::cout << "StringStream ";
	std::istringstream iss(compressed);
	std::ostringstream oss;

	Compression::decompression(iss, oss);
}

std::string compressString()
{
	std::cout << "String ";
	std::string str(alphabet);

	Compression::compression(str);

	return str;
}

void decompressString(std::string compressed)
{
	std::cout << "String ";

	Compression::decompression(compressed);
}


int main()
{
	// Test file compression
	writeFile();
	compressFile();
	decompressFile();

	// Test stringstream compression
	std::string compressed = compressStringStream();
	decompressStringStream(compressed);

	// Test string compression
	compressed = compressString();
	decompressString(compressed);

	return 0;
}