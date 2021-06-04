# AsyncIO

Provides **asynchronous** mechanism to **read** / **write** files by using [Windows IOCP](https://docs.microsoft.com/en-us/windows/win32/fileio/i-o-completion-ports).

1) Simultaniously can support multiple files read and write.  
2) Uses best of two worlds IOCP and multi threading.
3) C++ 11/14 functionality.

## Build

Use Visual stuido 2015 or above (tested with 2015 and 2019)
Install [vcpkg](https://vcpkg.io/en/getting-started.html)  
Install dependent packages (logging packages)

```bash
vcpkg install spdlog
```

## Usage

```cpp

//create an sample buffer which need to be written
IOBuffer imageBuffer(2048 * 2048);
fillWithRandomValues(imageBuffer);

//initialize file overlapped processor & create an file
auto fileProcessor(CreateFileProcessor(*logger));
fileProcessor->enable();

//open file with write
std::string filename("c:\\Image.data");
auto filestream = fileProcessor->Open(filename, "w+");

//write request
auto writeRequest = fileStream->write(&imageBuffer);
writeRequest.wait(); //wait for request to get complete and check status
auto writeIOStatus = writeRequest.get();
assert(writeIOStatus.status == Status::OK && writeIOStatus.transferedBytes == imageBuffer.size())

 //read request
IOBuffer imageReadBuffer(2048 * 2048);
auto readRequest = fileStream->read(&imageReadBuffer); 
readRequest.wait();

auto& readIOStatus = readRequest.get();
assert(readIOStatus.status == Status::OK && (readIOStatus.transferredBytes == imageReadBuffer))

//close the file
filestream.reset();
```


Example program can be look at 
[AsyncFileIOClient](https://github.com/mohandarsi/FastBulk/tree/master/AsyncFileIOClient)
