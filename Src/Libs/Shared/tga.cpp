#include "stdafx.h"
#include "tga.h"
#include <stdio.h>


void StoreTGAImageToFile(const std::string& filename, int width, int height, unsigned char* pixels)
{
	FILE* file = 0;
	fopen_s(&file, filename.c_str(), "wb");
	ASSERT(file != 0);

	//write out identification number
	char idSize = (char)0;
	fwrite(&idSize, sizeof(unsigned char), 1, file);

	char colorMap = (char)0;
	fwrite(&colorMap, sizeof(unsigned char), 1, file);

	char colorType = (char)2;
	fwrite(&colorType, sizeof(char), 1, file);

	//colour map specification
	short int colourMapEntryOffset = 0;
	short int colourMapLength = 0;
	unsigned char colourMapEntrySize = 0;
	fwrite(&colourMapEntryOffset, sizeof(short int), 1, file);
	fwrite(&colourMapLength, sizeof(short int), 1, file);
	fwrite(&colourMapEntrySize, sizeof(unsigned char), 1, file);

	short int xOrig = 0;
	short int yOrig = 0;
	short int width16 = width;
	short int height16 = height;
	unsigned char bitdepth = (char)32;
	unsigned char imageDesc = (char)0;

	fwrite(&xOrig,sizeof(short int),1,file);
	fwrite(&yOrig,sizeof(short int),1,file);
	fwrite(&width16,sizeof(short int),1,file);
	fwrite(&height16,sizeof(short int),1,file);
	fwrite(&bitdepth,sizeof(unsigned char),1,file);

	imageDesc = 0;
	imageDesc = 0x00000000;
	fwrite(&imageDesc,sizeof(char),1,file);

	fwrite(pixels, 1, width*height*4, file);

	fclose(file);
}

#pragma pack (push, 1)

typedef struct
{
	unsigned char 	id_length, colormap_type, image_type;
	unsigned short	colormap_index, colormap_length;
	unsigned char	colormap_size;
	unsigned short	x_origin, y_origin, width, height;
	unsigned char	pixel_size, attributes;
} TGAHeader;

#pragma pack (pop)

void ReadTGAImage(unsigned char* data, int* outWidth, int* outHeight, unsigned char** outPixels)
{
	const TGAHeader& hdr = *(TGAHeader*)data;

	ASSERT(hdr.id_length == 0);
	ASSERT(hdr.colormap_type == 0);
	ASSERT(hdr.image_type == 2);
	ASSERT(hdr.colormap_index == 0);
	ASSERT(hdr.colormap_length == 0);
	ASSERT(hdr.colormap_size == 0);
	ASSERT(hdr.x_origin == 0);
	ASSERT(hdr.y_origin == 0);
	ASSERT(hdr.pixel_size == 32);
	ASSERT((hdr.attributes & 16) == 0); // X-flip bit

	bool flippedY = (hdr.attributes & 32) == 0; // Y-flip bit
		

	ASSERT(hdr.width >= 0);

	*outWidth = abs(hdr.width);
	*outHeight = abs(hdr.height);

	int numPixels = (*outWidth) * (*outHeight);

	*outPixels = new unsigned char[numPixels*4];

	if (flippedY)
	{
		int numBytesPerRow = (*outWidth) * 4;
		
		unsigned char* dest = *outPixels + ((*outHeight)-1) * numBytesPerRow;
		unsigned char* src = data + sizeof(TGAHeader);

		for (int y=0; y<*outHeight; ++y)
		{
			memcpy(dest, src, numBytesPerRow);

			dest -= numBytesPerRow;
			src += numBytesPerRow;
		}
	}
	else
	{
		memcpy(*outPixels, data + sizeof(TGAHeader), numPixels*4);
	}	
}
