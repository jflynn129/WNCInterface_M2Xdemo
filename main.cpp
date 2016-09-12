/* =====================================================================
   Copyright © 2016, Avnet (R)

   Contributors:
     * James M Flynn, www.em.avnet.com 
 
   Licensed under the Apache License, Version 2.0 (the "License"); 
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, 
   software distributed under the License is distributed on an 
   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
   either express or implied. See the License for the specific 
   language governing permissions and limitations under the License.

    @file          WNCInterface.cpp
    @version       1.0
    @date          Sept 2016

======================================================================== */

//
// This file contains an example implementation of M2X using the HTTP interface as the underlying 
// transport.
//

#include "mbed.h"
#include "WNCInterface.h"

#define MBED_PLATFORM
#define M2X_ENABLE_READER

#include <jsonlite.h>
#include "M2XStreamClient.h"

#define CRLF "\n\r"

char deviceId[] = "e83cdd8645ab1a7c0c480156efbf78f6"; // Device you want to post to
char m2xKey[]   = "4d7e1da7f05c3fa4d5426419891a254d"; // Your M2X API Key or Master API Key

const char *hstreamName = "humidity";
const char *tstreamName = "temperature";
const char *streamNames[] = { tstreamName, hstreamName };
char name[] = "Wake Forest"; // Name of current location of datasource

int counts[] = { 2, 1 };
const char *ats[] = { "2016-09-09T02:05:14.692Z", 
                      "2016-09-09T02:05:14.700Z", 
                      "2016-09-09T02:05:14.692Z" };
double values[] = { 10.9, 11.2, 6.1 };

char fromTime[]= "1969-12-31T19:00:01.000Z"; // yyyy-mm-ddTHH:MM:SS.SSSZ
char endTime[25];

double latitude = 33.007872;   // You could read these values from a GPS but
double longitude = -96.751614; // for now, will just hardcode them
double elevation = 697.00;

WNCInterface wnc;
Client client;
M2XStreamClient m2xClient(&client, m2xKey);
TimeService timeService(&m2xClient);

void on_data_point_found(const char* at, const char* value, int index, void* context, int type) {
  printf(">>Found a data point, index: %d type: %d" CRLF, index, type);
  printf(">>At: %s" CRLF " Value: %s" CRLF, at, value);
}


void on_command_found(const char* id, const char* name, int index, void *context) {
  printf(">>Found a command, index: %d" CRLF, index);
  printf(">>ID: %s\n Name: %s" CRLF, id, name);
}

void on_location_found(const char* name,
                       double latitude,
                       double longitude,
                       double elevation,
                       const char* timestamp,
                       int index,
                       void* context) {
  printf(">>Found a location, index: %d" CRLF, index);
  printf(">>Name: %s" CRLF ">>Latitude: %lf" CRLF ">>Longitude: %lf" CRLF, name, latitude, longitude);
  printf(">>Elevation: %lf" CRLF ">>Timestamp: %s" CRLF, elevation, timestamp);
}

int main() {
  char timestamp[25];
  int length = 25;
  char amb_temp[6];
  char amb_humd[6];
  int response;
  double temp=0.00;  //we will just increment these 0.01 each time through the loop
  double humid=0.00; //we will just increment these 1 each time through the loop wrapping at 100

  printf("Start m2x-demo-all by initializng the network" CRLF);
  response = wnc.init();                     
  printf("WNC Module %s initialized (%02X)." CRLF, response?"IS":"IS NOT", response);
  if( !response ) {
      printf(" - - - - - - - ALL DONE - - - - - - - " CRLF);
      while(1);
  }
        
    response = wnc.connect();                 
    printf("IP Address: %s " CRLF CRLF, wnc.getIPAddress());

  printf("initialize the M2X time service" CRLF);
  if (!m2x_status_is_success(timeService.init())) 
    printf("Cannot initialize time service!" CRLF);
  else {
    timeService.getTimestamp(timestamp, &length);
    printf("Current timestamp: %s" CRLF, timestamp);
    strcpy(endTime,timestamp);
  }
  
  printf("Now delete all existing values" CRLF);
  // Delete values
  printf("Delete humidity values..." CRLF);
  response = m2xClient.deleteValues(deviceId,hstreamName, fromTime, endTime);
  printf("Delete response code: %d" CRLF, response); 

  printf("Delete temp values..." CRLF);
  response = m2xClient.deleteValues(deviceId,tstreamName, fromTime, endTime);
  printf("Delete response code: %d" CRLF, response);  

  printf("Delete location values..." CRLF);
  response = m2xClient.deleteLocations(deviceId, fromTime, endTime);
  printf("Delete response code: %d" CRLF, response);  
  
  printf("Query for possible commands using this device..." CRLF);
  response = m2xClient.listCommands(deviceId, on_command_found, NULL);
  printf("listCommands response code: %d" CRLF, response);  

  while (true) {
    // read temp -- for now, just use a fixed temp, but will need to read the HTS221
    // and put it into a 6 byte string formatted as "%0.2f"
    sprintf(amb_temp,"%0.2f",temp);
    sprintf(amb_humd,"%0.2f",humid);
    temp  += .01;
    humid += 1.0;
    humid = fmod(humid,100.0);
    
    // post the humidity value
    printf("Post updateStreamValue (humidity)..." CRLF);
    response = m2xClient.updateStreamValue(deviceId, "humidity", humid);
    printf("Post response code: %d" CRLF, response);

    // post the temp value
    printf("Post updateStreamValue (temp)..." CRLF);
    response = m2xClient.updateStreamValue(deviceId, "temperature", temp);
    printf("Post response code: %d" CRLF, response);

    // read temperature
    printf("listStreamValues (temp)..." CRLF);
    response = m2xClient.listStreamValues(deviceId, tstreamName, on_data_point_found, NULL);
    printf("listStreamValues response code: %d" CRLF, response);
    if (response == -1) while (true) ;

    // read temperature
    printf("listStreamValues (humid)..." CRLF);
    response = m2xClient.listStreamValues(deviceId, hstreamName, on_data_point_found, NULL);
    printf("listStreamValues response code: %d" CRLF, response);
    if (response == -1) while (true) ;

    // update location
    printf("updateLocation..." CRLF);
    response = m2xClient.updateLocation(deviceId, name, latitude, longitude, elevation);
    printf("updateLocation response code: %d" CRLF, response);
    if (response == -1) while (true) ;
    
    // read location
    printf("readLocation..." CRLF);
    int response = m2xClient.readLocation(deviceId, on_location_found, NULL);
    printf("readLocation response code: %d" CRLF, response);

    printf("PostDeviceUpdates..." CRLF);
    response = m2xClient.postDeviceUpdates(deviceId, 2, streamNames, counts, ats, values);
    printf("Post response code: %d" CRLF, response);
       
    timeService.getTimestamp(timestamp, &length);
    printf("Thats all folks, got to wait 60 seconds... (%s)" CRLF, timestamp);

    // wait 60 secs and then loop
    delay(60000);
    
  }
}


