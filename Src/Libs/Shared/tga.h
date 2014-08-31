#pragma once


void StoreTGAImageToFile(const std::string& filename, int width, int height, unsigned char* pixels);

// Read a 32bit TGA file. Only supports 32bit TGA, and doesn't support compression
void ReadTGAImage(unsigned char* data, int* outWidth, int* outHeight, unsigned char** outPixels); 