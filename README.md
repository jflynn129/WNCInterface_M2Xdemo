HTTP implmented M2X access
==========================

This example program uses the ARM mbed HTTP client library to send/receive data to/from [AT&amp;T's M2X 
service](https://m2x.att.com/) using the AT&T Cellular IoT Starter Kit from Avnet 
(http://cloudconnectkits.org/product/att-cellular-iot-starter-kit).  This kit contains a 
[FRDM-K64F](https://developer.mbed.org/platforms/FRDM-K64F/) from NXP.

**NOTE**: This doc is specific to using the AT&T Cellular IoT Starter Kit so ensure that the 
[mbed online compiler](https://mbed.org/compiler/) has the platform set to FRDM-K64F. 

M2X-HTTP Example Program
========================

This M2X-HTTP Example Program shows how to:

1. Setting up a M2X device.

2. Setup the development environment using the [mbed online compiler](https://mbed.org/compiler/).

3. How to modify the source code so it interacts with your M2X device.

4. Expected execution outcome.

Setting up an M2X Device
==========================
1. Signup for an [M2X Account](https://m2x.att.com/signup).

2. You will need to access the following information from the M2X account to modify the example program:
2.1 Obtain the _Master Key_ from the Master Keys tab of your [Account Settings](https://m2x.att.com/account) screen.
2.2 Create a [Device](https://m2x.att.com/devices) and obtain its _Device ID_.

3. Review the [M2X API Documentation](https://m2x.att.com/developer/documentation/overview).

Please consult the [M2X glossary](https://m2x.att.com/developer/documentation/glossary) if you have questions about any M2X specific terms.


Setting the development environment
===================================
1. Launch [mbed online compiler](https://mbed.org/compiler/) in your browser

2. From within the mbed.org compiler IDE, create the example program by:
   2.1) Right click on 'My Programs' and select 'New Program'.  Select 'Import Library' from the drop-down, then 'From URL'.  
         use https://github.com/jflynn129/WNCInterface_M2Xdemo as the URL to import from.

   2.2) Create the WNCInterface.
         Righ click on the WNCInterface_M2Xdemo program entry.  Select 'Import Library' from the drop-down, then 'From URL'.  At
         this point, you will be asked for the Source URL, use https://github.com/jflynn129/WNCInterface.git. The dialog will 
         automatically populate as a Library, set the import name and use 'WNCInterface_HTTP' as the Target Path.  Note that
         the Library name will take the name of the github repository including the ".git", remove the ".git" suffix to 
         successfully import.

   2.2.1) Now import the WNC Controller class.  Right click on the WNCInterface label and select 'Import Library' from 
         the drop-down, then 'From URL'.  Use https://developer.mbed.org/users/fkellermavnet/code/WncControllerK64F/ as the 
         and the other fields will be populated correctly.

   2.3)  Create a M2XStreamClient-JMF library.
         Righ click on the WNCInterface_M2Xdemo program entry.  Select 'Import Library' from the drop-down, then 'From URL'.  At
         this point,  use https://developer.mbed.org/users/JMF/code/M2XStreamClient-JMF/

   2.4)  Create a jsonlite library.
         Righ click on the WNCInterface_M2Xdemo program entry.  Select 'Import Library' from the drop-down, then 'From URL'.  At
         this point,  use http://mbed.org/users/citrusbyte/code/jsonlite/.
         
   2.6) Create the mbed-rtos and mbed libraries.
         Righ click on the WNCInterface_M2Xdemo program entry.  Select 'Import Library' from the drop-down, then 
         'From Import Wizard'.  At this point, enter 'mbed' in the search box and search.  Multiple entries will be displayed.  
         highlight 'mbed' and 'mbed-rtos' and select 'import'.

3. You now have all the components for the example program. If you don't modify the _Master Key_ and _Device ID_ the program
   will utilize a device that may or may not work.  It is recomedned that you replace these values with your device's information
   (that you obtained when 'Setting up an M2X Device'. 


Modify source code for your M2X device
======================================


To modify the example code (main.cpp), please follow these steps so your device variables are properly configured.  The variables you need to modify are in main.cpp as described below:

M2X API Key
-----------
Once you [register](https://m2x.att.com/signup) for an AT&amp;T M2X account, an API key is automatically generated for you. This key is called a _Primary Master Key_ and can be found in the _Master Keys_ tab of your [Account Settings](https://m2x.att.com/account). This key cannot be edited nor deleted, but it can be regenerated. It will give you full access to all APIs.


```
char m2xKey[] = "<M2X access key>";
```

Device ID
-------

The  _Device API Key_ is associated with a given Device. This key is used to access the streams belonging to 
a Device (such as streams of locations, temperatures, etc.). The following line configures the device Id:

```
char deviceId[] = "<device id>";
```

Stream Name
------------

A stream in a device is a set of timed series data of a specific type (i,e. humidity, temperature), it is used to 
to send stream values to M2X server, or receive stream values from M2X server. Use the following line to configure 
the stream:

```
char streamName[] = "<stream name>";
```

The example program has the following values currently defined by these variables:

char deviceId[] = "e83cdd8645ab1a7c0c480156efbf78f6"; // Device you want to post to
char m2xKey[]   = "4d7e1da7f05c3fa4d5426419891a254d"; // Your M2X API Key or Master API Key

const char *hstreamName = "humidity";
const char *tstreamName = "temperature";
const char *streamNames[] = { tstreamName, hstreamName }; // array of stream names
char name[] = "Wake Forest";                              // Name of current location of datasource



Using the M2XMQTTClient library
=========================

The M2XStreamClient, uses the following API functions:

* `updateStreamValue`: Send stream value to M2X server
* `postDeviceUpdates`: Post values from multiple streams to M2X server
* `listStreamValues`: Receive stream value from M2X server
* `updateLocation`: Send location value of a device to M2X server
* `readLocation`: Receive location values of a device from M2X server
* `deleteValues`: Delete stream values from M2X server
* `deleteLocations`: Delete locations from the M2X server
* `getTimestamp`: Return the current time in  ISO 8601 format


Returned values
---------------

For all functions, the HTTP status code will be returned if we can fulfill a HTTP request. For example, `200` will be returned upon success, `401` will be returned if a valid M2X API Key wasn't provided.  The following error codes are returned for specific situations:


```
static const int E_NOCONNECTION = -1;
static const int E_DISCONNECTED = -2;
static const int E_NOTREACHABLE = -3;
static const int E_INVALID      = -4;
static const int E_JSON_INVALID = -5;
```

Update stream value
-------------------

The following functions will post a single value to a stream:

```
template <class T>
int updateStreamValue(const char* deviceId, const char* streamName, T value);
```

Here C++ templates are used to generate functions for different types of values, use values of `float`, `int`, `long` or even `const char*` types here.


Post device updates
-------------------

To post multiple values to multiple streams in one call, use the following function:

```
template <class T>
int postDeviceUpdates(const char* deviceId, int streamNum,
                      const char* names[], const int counts[],
                      const char* ats[], T values[]);
```

Refer to the source code on how to use this function, basically, you provide the list of streams you want to post to, and values for each stream.


List stream values
------------------

Because mbed targets limited memory controllers, we donot put the whole returned string in memory, parse it into JSON representations and read it, instead, we use a callback-based mechanism to parse the returned JSON string piece by piece, whenever we get a new stream value point, we will call the user defined callback function:

```
typedef void (*stream_value_read_callback)(const char* at,
                                           const char* value,
                                           int index,
                                           void* context,
                                           int type);
```

The implementation of the callback function is user defined.  In the example, the the returned values are simply printed out.
The value of the data point is in the `value` argument, and the timestamp of the point in the `at` argument. The index of this this data point in the whole stream is also retuned as well as a user-specified context variable.

`type` indicates the type of value stored in `value`: 1 for string, 2 for number. However, keep in mind that `value` will always be a pointer to an array of char, even though `type` indicates the current value is a number--in which case you will need to convert the text value to its appropriate value.

To read the stream values, call the listStreamValues function:

```
int listStreamValues(const char* deviceId, const char* streamName,
                     stream_value_read_callback callback, void* context,
                     const char* query = NULL);
```

Besides the device ID and stream name, only the callback function and a user context needs to be specified. Optional query parameters might also be available here, for example, a parameter to pick from a specific range.  In the example program this field is not used:

```
start=2014-10-01T00:00:00Z&end=2014-10-10T00:00:00Z
```

Update Device Location
--------------------------

You can use the following function to update the location for a device:

```
template <class T>
int updateLocation(const char* deviceId, const char* name,
                   T latitude, T longitude, T elevation);
```

Locations are attached to devices rather than streams. We use templates here, since the values may be in different format, for example, you can express latitudes in both `double` and `const char*`.

Read Device Location
------------------------

Similar to reading stream values, callback functions are also used. The difference is the parameters used in the function:

```
void (*location_read_callback)(const char* name,
                               double latitude,
                               double longitude,
                               double elevation,
                               const char* timestamp,
                               int index,
                               void* context);

```

For memory space consideration, only double-precision is used when reading locations. An index of the location points is also provided together with a user-specified context.

The API is also slightly different, in that the stream name is not needed here:

```
int readLocation(const char* deviceId, location_read_callback callback,
                 void* context);

```

Delete stream values
--------------------

The following function can be used to delete stream values within a date range:

```
int deleteValues(const char* deviceId, const char* streamName,
                 const char* from, const char* end);
```

`from` and `end` fields here follow ISO 8601 time format.


Delete location values
--------------------

The following function can be used to delete location values within a date range:

```
int deleteLocations(const char* deviceId, const char* from, const char* end);
```

`from` and `end` fields here follow ISO 8601 time format.


Expected execution outcome
=======================
Once the necessary source code updates are made, the program compiled and downloaded
to the IoT Kit, perform the following steps:

1. Using a terminal program such as Hyperterm or Putty, connect to the Kit (select comm parameters of 115200-N81

2. Press the `reset` button, then you should be able to run the program!

When the program runs, the output will look similar to:

    Start m2x-http-demo by initializng the network
    WNC Module IS initialized (07).
    IP Address: 10.61.137.203 
    
    initialize the M2X time service
    Current timestamp: 2016-09-13T00:43:21.000Z
    Now delete all existing values
    Delete humidity values...
    Delete response code: 204
    Delete temp values...
    Delete response code: 204
    Delete location values...
    Delete response code: 204
    Query for possible commands using this device...
    listCommands response code: 200
    Post updateStreamValue (humidity)...
    Post response code: 202
    Post updateStreamValue (temp)...
    Post response code: 202
    listStreamValues (temp)...
    >>Found a data point, index: 0 type: 1
    >>At: 2016-09-13T00:43:46.844Z
     Value: 0.01
    listStreamValues response code: 200
    listStreamValues (humid)...
    >>Found a data point, index: 0 type: 2
    >>At: 2016-09-13T00:43:45.859Z
     Value: 1.0
    listStreamValues response code: 200
    updateLocation...
    updateLocation response code: 202
    readLocation...
    ...    

License
=======
This library is released under the Licensed under the Apache License, Version 2.0 (the "License"); you may not use this 
file except in compliance with the License and may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an 
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific 
language governing permissions and limitations under the License.


