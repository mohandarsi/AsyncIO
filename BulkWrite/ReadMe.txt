========================================================================
    DYNAMIC LINK LIBRARY : BulkWrite Project Overview
========================================================================

Usage:-

//create data
 #define MAX_SIZE	  (2 * 1024 * 1024)
 static BYTE arr[MAX_SIZE];			
 memset(arr, 1, sizeof(image_arr));
  
//open file with write
std::string filename("c:\\Image.data");
IFileStream::Ptr filestream = file->Open(filename, "w+");

//write on the stream
auto ioRequestPtr = filestream->Write(image_arr,sizeof(arr),-1,nullptr);

//wait on the request to complete
ioRequestPtr->Wait();

TODO: Implelemention of Read yet to be supported.


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
