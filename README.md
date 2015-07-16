# tr064: a library and tool for querying, executing and exporting TR-064 devices and services

###tr064lib
*tr064lib* is a library which can be used for finding CPE capable devices in your local network. It implements its own very simple devices discovery mechanism (UPnP). 

Once found, the devices will be queried for available services which can then be executed accordingly (SOAP request). 

The specification of the devices can also be exported in a single file, e.g. *json* and later reused (import) in order to avoid re-discovery and re-parsing. 

### tr064tool
Is the front-end for *tr064lib*. It is a command-line tool which provides most of `tr064lib`s functionality via a single command line application, see manpage for details and usage.

**tr064tool** requires a service and an action, which are separated via colon. The same applies for action arguments:
```
tr064tool ServiceName:ActionName  Arg1:val1 [ ArgN:valN ]
```

#### Examples

List all available Services and Actions (-v prints action arguments) for a discovered root device.

```
tr064tool -v --list
```

----------

Execute an action for an imported root device from `fritzdesc_out.json`, using default username `admin` and password `foo`.  The service is WANIPConnection and  action  `X_GetDNSServers`.   Note, that `X_GetDNSServers` does not require any arguments.
```
tr064tool -i fritzdesc_out.json -pfoo WANIPConnection:X_GetDNSServers
```

              
### tr064export
Executes a CPE device discovery and export the found device to the given format, e.g. *json*:
```bash
tr064export myrouter.json
```

## Build 

Out-of-sources build with cmake:

```
git clone https://github.com/awidegreen/tr064.git tr064
cd tr064
git submodule update --init --recursive
mkdir build
cd build
cmake ..
make -j4 
```

## Dependencies
For certain functionality (sockets and json handling) the tr064 lib uses *boost* and built-in *pugixml*. Unit tests are implemented with the *google-test* framework.

Make sure that you have the following libraries installed:

* cmake
* boost
* google-test (optional)
* openssl (for MD5 algorithm)
* libcurl (for http requests)

## Debugging

You can run ``tr064tool`` with the ``-v`` parameter in order to get a more verbose output. 

If that is not enough, consider compiling it with debug mode:
```
mkdir debug && cd debug
cmake -DCMAKE_BUILD_TYPE=Debug .. 
```

## Alternatives
I'm aware that there is `upnpc` which provides similar functionality. However, in order to understand whats going on, I decided to implement most of the stuff myself. 

## TODO
Things that should be fixed or added

### Configuration transactions
Setting of CPE configurations is done via a sequence of SOAP requests: `ConfigurationStarted` - `SetUserName` - `ConfigurationFinished`
This is currently not support.
See [TR064 spec](https://www.broadband-forum.org/technical/download/TR-064.pdf)

### pointer usage
double check if certain implementations (Devices, Services) really need a shared_ptr, or if it can be solved via references instead.

### error handling
catch exceptions etc.

## License 
Copyright (c) Armin Widegreen

Distributed under the BSD-Clause-2 License, see LICENSE.

## Resources

* [TR064 spec](https://www.broadband-forum.org/technical/download/TR-064.pdf)
* AVW's TR-064 First steps

