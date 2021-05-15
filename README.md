# FastBulk

Provides **asynchronous** mechanism to **read** / **write** files by using [Windows IOCP](https://docs.microsoft.com/en-us/windows/win32/fileio/i-o-completion-ports).

1) Simultaniously can support multiple files read and write.  
2) Uses best of two worlds IOCP and multi threading.
3) C++ 11/14 functionality.

## Build

Uses Visual stuido 2015 or above  
Install [vcpkg](https://vcpkg.io/en/getting-started.html)  
Install dependent packages (logging packages)

```bash
vcpkg install spdlog
```

## Usage

```cpp

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

//close the file
filestream.reset();
```


Example program can be look at 
[BulkWriteClient](https://github.com/mohandarsi/FastBulk/tree/master/BulkWriteClient)
