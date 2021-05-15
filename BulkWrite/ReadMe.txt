========================================================================
    DYNAMIC LINK LIBRARY : BulkWrite Project Overview
========================================================================

Usage:-

//create an sample buffer which need to be written
std::vector<unsigned char> image(2048*2048);
fillWithRandomValues(image);

//initialize file overlapped processor & create an file
auto fileProcessor(CreateFileProcessor(*logger));
fileProcessor->enable();

//open file with write
std::string filename("c:\\Image.data");
auto filestream = fileProcessor->Open(filename, "w+");

//initite async write request
auto writeRequest = fileStream->write(&image[0], image.size(), OFFSET_NONE);
//OFFSET_NONE indicates to read or write operations to consider offset as current file position.

//wait for request to get complete and check status
writeRequest.wait();
auto writeIOStatus = writeRequest.get();
assert(writeIOStatus.status == Status::OK && writeIOStatus.transferedBytes == image.size())

fileStream->seek(0); // reset to start of the file.
Image image2(SIZE*NOPIXELS);
auto readRequest = fileStream->read(&image2[0], image2.size(), OFFSET_NONE);
readRequest.wait();
auto readIOStatus = readRequest.get();
if (readIOStatus.status != Status::OK || readIOStatus.transferedBytes == image2.size())


BulkWrite.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

BulkWrite.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

BulkWrite.cpp
    This is the main DLL source file.

	When created, this DLL does not export any symbols. As a result, it
	will not produce a .lib file when it is built. If you wish this project
	to be a project dependency of some other project, you will either need to
	add code to export some symbols from the DLL so that an export library
	will be produced, or you can set the Ignore Input Library property to Yes
	on the General propert page of the Linker folder in the project's Property
	Pages dialog box.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named BulkWrite.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
