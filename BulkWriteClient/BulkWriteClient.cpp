// BulkWriteClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <iterator>
#include <algorithm>
#include "..\BulkWrite\interfaces.h"
#include <iostream>
#include <chrono>
#include <thread>

void WriteHandler(IOResult ioResult, size_t len)
{
	std::cout << " \n Completed writing with result: " << ioResult << "Length:" << len;
}
/*======================================================================*/
/*	calc_bulk_size							*/
/*======================================================================*/
long
GetBulkSize(
	int resolution,
	int bits_allocated)
{
	return (resolution * resolution * bits_allocated) / 8;
}
using namespace std;
int main()
{
	
    
    #define MAX_BULK_SIZE	  (2 * 1024 * 1024)
	static BYTE image_arr[MAX_BULK_SIZE];				/* Image bulk data				*/
	memset(image_arr, 1, sizeof(image_arr));

	int noImages = 0;
	int resolution = 0;
	int bits = 0;
	cout << "Enter Number of images :";
	cin >> noImages;

	cout << "\nEnter Resolution :";
	cin >> resolution;

	cout << "\nEnter BitsAllocated :";
	cin >> bits;

	std::shared_ptr<IFileProcessor> file(CreateFileProcessor());
	file->Enable();
	int i = 0;
	unsigned char* Data = new unsigned char[MAX_BULK_SIZE];
	size_t readsize = GetBulkSize(resolution, bits);
   
	
	std::string filename("c:\\Image.data");
	IFileStream::Ptr filestream = file->Open(filename, "w+");

	auto start = chrono::high_resolution_clock::now();
	auto bytes = noImages*readsize;

	//filestream->SetFileSize(bytes);

	while (i<noImages) // 5000 images
	{
		//std::cout << " \n DEBUG:Length writing: " << readsize;
		IRequest::Ptr ioptr = filestream->Write(image_arr, readsize,-1,nullptr);
		ioptr->Wait();
		++i;
	}
	auto end = chrono::high_resolution_clock::now();

	auto milli = chrono::duration_cast<chrono::milliseconds>(end - start);

	std::cout << "waiting enter key :" << milli.count();
	int a;
	std::cin >> a;
	filestream.reset();
	delete[] Data;
	file.reset();
	return 0;

}
