// BulkWriteClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Windows.h"
#include <iostream>
#include <iterator>
#include <algorithm>
#include "..\BulkWrite\interfaces.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <future>         // std::async, std::future

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
using namespace FileAPI;

int main()
{

    #define MAX_BULK_SIZE	  (2 * 1024 * 1024)
	static BYTE image_arr[MAX_BULK_SIZE];				/* Image bulk data				*/
	memset(image_arr, 1, sizeof(image_arr));

	int noImages = 1;
	int resolution = 1024;
	int bits = 8;
	/*cout << "Enter Number of images :";
	cin >> noImages;

	cout << "\nEnter Resolution :";
	cin >> resolution;

	cout << "\nEnter BitsAllocated :";
	cin >> bits;*/

	std::unique_ptr<IFileProcessor> file(CreateFileProcessor());
	file->Enable();
	
	
	size_t imageSize = GetBulkSize(resolution, bits);

    std::unique_ptr<char> data(new char[imageSize]);
	
	std::string filename("c:\\temp\\Image.data");
	auto filestream = file->Open(filename, "w+");

	auto start = chrono::high_resolution_clock::now();
	auto bytes = noImages*imageSize;

	filestream->SetFileSize(bytes);

    int i = 0;
	while (i<noImages) // 5000 images
	{
        std::cout << " \n DEBUG:Length writing: " << imageSize << std::endl;
        auto request = filestream->Write(data.get(), imageSize, 0);
        request.wait();

		std::cout << " \n DEBUG:Length writing: " << request.get().transferedBytes << std::endl;
		++i;

	}
	auto end = chrono::high_resolution_clock::now();

	auto milli = chrono::duration_cast<chrono::milliseconds>(end - start);

	std::cout << "waiting enter key :" << milli.count() << std::endl;
	int a;
	std::cin >> a;
	filestream.reset();
	file.reset();
	return 0;

}
