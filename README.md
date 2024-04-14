<br/>
<p align="center">
  <a href="https://github.com/sosiskus/DNS-Resolver">
    <img src="https://networkappers.com/files/dns_lookup.svg" alt="Logo" width="80" height="80">
  </a>

  <h3 align="center">Universal DNS resolver</h3>

  <p align="center">
    Resolve Any Record, Adapt To Any Systems - The Ultimate DNS Lookup Solution!
    <br/>
    <br/>
  </p>
</p>

![Downloads](https://img.shields.io/github/downloads/sosiskus/DNS-Resolver/total) ![Issues](https://img.shields.io/github/issues/sosiskus/DNS-Resolver) 

## Table Of Contents

* [About the Project](#about-the-project)
* [Getting Started](#getting-started)
  * [Prerequisites](#prerequisites)
  * [Installation](#installation)
    * [For Windows](#for-windows)
    * [For Linux](#for-linux)
    * [VSCode integration](#vscode-integration)
* [Usage](#usage)
  * [Custom records](#custom-records)
  * [Custom UDP receiver](#custom-UDP-receiver)
* [Contributing](#contributing)
* [Authors](#authors)

## About The Project <a name="about-the-project"></a>

![ScreenShot](https://i.ibb.co/W0Hq0M7/Untitled.png)

The DNS Lookup Library is a powerful and versatile C++ library designed to simplify the process of performing DNS queries and retrieving domain information. Whether you need to resolve domain names, obtain IP addresses, or fetch other DNS record types, this library has got you covered.

Developed with flexibility in mind, the DNS Lookup Library supports multiple DNS server configurations, allowing you to specify multiple DNS servers. It also provides a user-friendly interface for constructing DNS queries, sending them over UDP, and parsing the responses.

One of the key features of this library is its extensibility. It allows you to define custom record types and provide your own data parsing functions, giving you complete control over how the DNS response data is interpreted and processed.

With its modular design, the DNS Lookup Library is easy to integrate into your existing projects. Whether you're building networking applications, system utilities, or any other software that requires DNS resolution, this library is an invaluable tool that will save you time and effort.

## Getting Started <a name="getting-started"></a>

Here's a short Getting Started Description for the DNS Lookup Library

### Prerequisites <a name="prerequisites"></a>

Here are the prerequisites that must be installed before using the DNS Lookup Library
*C++ and any of the compiler supporting C++ 20
*CMake to be able to use library, but it also works without CMake on embedded systems like ESP32 and ESP8266

### Installation <a name="installation"></a>

* Clone the repository into your project tree by 
```git clone https://github.com/sosiskus/DNS-Resolver``` or download release files and unzip 
![Tree](https://i.ibb.co/zxX4cfb/1.png)
* Add to CMakeLists.txt file following lines
```cmake
add_subdirectory(DNS-Resolver)
target_link_libraries(yourProjectName PRIVATE DnsResolver)
```
#### For Windows: <a name="for-windows"></a>
* Run ```cmake -G "MinGW Makefiles" .``` in the source directory
* To build run ```cmake --build .```

### For Linux: <a name="for-linux"></a> 
* Run ```cmake .```
* To build run ```make```

### VSCode integration: <a name="vscode-integration"></a>
* A fter cloning library in VSCode press SHIFT + CTRL + P and find
![Resetcmake](https://i.ibb.co/DG0j4kq/1.png)
* CMake should reload and this popup appears
![Popup](https://i.ibb.co/9Y2Q479/1.png)
You should choose your desired compiler e.g. GCC, if you do not see your installed compiler here make sure all environment variables is set correctly
* To build all press build button 
![Buld](https://i.ibb.co/MBD8JvM/g.png)
If you want to try my examples press [all] button and choose example, also when pressing run button make sure that chose example also is selected as target

For other systems like ESP32 only bare hpp and cpp files should be used


## Usage <a name="usage"></a>

The DNS Lookup Library is easy to set up and use in your C++ projects. Follow these simple steps to get started with default parser and winsockets:
* Include the Library: 
```c++
#include "DnsLookup/dnsLookup.hpp"
#include "DnsLookup/defaultParsers.hpp"
```
* Create a DnsLookup Instance:
```c++
DnsLookup dnsLookup;
```
* Specify one or more DNS servers to use for the lookup:
```c++
dnsLookup.addDnsServer("1.1.1.1", "8.8.8.8", "1.0.0.1");
```
* Create a DNS query for the desired domain and record type:
```c++
dnsLookup.formDnsQuery("example.com", DnsType::A);
```
* Send the DNS query and receive the response using a default winsock UDP interface:
```c++
dnsLookup.sendAndReceiveDnsQuery<WinSocketUDPInterface>();
```
* Parse the DNS response using the default record parser
```c++
std::vector<A_Record> records = dnsLookup.parseDnsResponse<A_Record>(A_RecordParser);
```

### Custom records <a name="custom-records"></a>
The DNS Lookup Library provides a flexible and extensible way to handle custom record types and data structures. Here's a getting started guide on how to define and parse your own record types and structs.

* Define a struct or class that represents your desired record type. This struct should contain the necessary fields to store the data from the DNS response. All the fields and information how to parse record can be found in the [official DNS documentation](https://www.ietf.org/rfc/rfc1035.txt 'docs')
```c++
struct TXTRecord
{
    const DnsType type = DnsType::TXT; // Record type
    std::string txt; // String to store the TXT record data
};
```
* Create a lambda function that will parse the raw data (rdata) from the DNS response and populate your record struct. This function should take two arguments: the DnsAnswer struct and the HelperFunctions object. How to parse specific DNS record is written in DNS documentation.
```c++
std::function<TXTRecord(const DnsAnswer &, const HelperFunctions &)> TXT_RecordParser =
    [](const DnsAnswer &ans, const HelperFunctions &functions)
{
    // Parse the raw rdata and populate the TXTRecord struct
    TXTRecord txt;
    int txtLength = ans.rdata[0];

    txt.txt = std::string(ans.rdata.begin() + 1, ans.rdata.begin() + txtLength + 1);

    return txt;
};
```
In this example, the lambda function reads the length of the TXT record from the first byte of the rdata, then copies the remaining bytes into the txt.txt string field.
* After defining your record struct and data parser, you can call the parseDnsResponse method of the DnsLookup class, passing your record type and the data parser lambda function as arguments.
```c++
std::vector<TXTRecord> answers = dnsLookup.parseDnsResponse<TXTRecord>(TXT_RecordParser);
```
The parseDnsResponse method will apply your data parser to each relevant answer in the DNS response and return a vector of your record structs.
* Finally, you can iterate over the vector of parsed records and access or manipulate the data as needed.
```c++
for (const auto &ans : answers)
{
    std::cout << "TXT: " << ans.txt << std::endl;
}
```

Remember to consult the library's documentation for more advanced usage scenarios, such as handling complex record types or implementing additional parsing logic. 

### Custom UDP receiver <a name="custom-UDP-receiver"></a>
The DNS Lookup Library provides an interface (IUDPInterface) for implementing custom UDP communication mechanisms. This allows you to seamlessly integrate the library with different socket libraries or network protocols. Here's a getting started guide on how to define your own implementation of the UDP interface
* Include the necessary headers in your implementation file:
```c++
#include "DnsLookup/udpInterface.hpp"
```
* Create a new class that inherits from IUDPInterface and implements its pure virtual functions:
```c++
class MyCustomUDPInterface : public IUDPInterface
{
public:
    MyCustomUDPInterface(std::string localPort) : IUDPInterface(localPort) {}

    bool begin() override;
    void close() override;
    void send(const std::vector<uint8_t>& data, std::string ip) override;
    std::vector<uint8_t> receive() override;

private:
    // Add any necessary private members or helper functions
};
```
* Implement the four pure virtual functions from IUDPInterface:
  * begin(): Initialize the UDP communication. This function should perform any necessary setup or configuration required by your UDP implementation.
  * close(): Clean up and close the UDP communication.
  * send(const std::vector<uint8_t>& data, std::string ip): Send the given data to the specified IP address.
  * std::vector<uint8_t> receive(): Receive data from the UDP socket and return it as a vector of bytes.
Here's an example implementation using a hypothetical CustomUDPSocket class:
```c++
bool MyCustomUDPInterface::begin()
{
    // Initialize CustomUDPSocket
    return mySocket.open(localPort);
}

void MyCustomUDPInterface::close()
{
    mySocket.close();
}

void MyCustomUDPInterface::send(const std::vector<uint8_t>& data, std::string ip)
{
    mySocket.send(data, ip);
}

std::vector<uint8_t> MyCustomUDPInterface::receive()
{
    return mySocket.receive();
}
```
* After implementing your custom UDP interface, you can use it with the DnsLookup class by passing it as a template argument to the sendAndReceiveDnsQuery method:
```c++
DnsLookup dnsLookup;
// ...
dnsLookup.sendAndReceiveDnsQuery<MyCustomUDPInterface>();
```
Remember to thoroughly test your implementation and ensure that it correctly handles all edge cases and error conditions.


## Contributing <a name="contributing"></a>

Contributions are what make the open source community such an amazing place to be learn, inspire, and create. Any contributions you make are **greatly appreciated**.
* If you have suggestions for adding or removing projects, feel free to [open an issue](https://github.com/sosiskus/DNS-Resolver/issues/new) to discuss it, or directly create a pull request after you edit the *README.md* file with necessary changes.

### Creating A Pull Request

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## Authors <a name="authors"></a>

* **Nikita Klepikov** - *Tech Startup Founder | LED Controller Developer* - [Nikita Klepikov](https://github.com/sosiskus/)
