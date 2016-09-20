#include "mbed.h"
#include "WNCInterface.h"

#define DEBUG
#define MBED_PLATFORM

#include "minimal-mqtt.h"
#include "minimal-json.h"
#include "M2XMQTTClient.h"

#define CRLF    "\n\r"


//Device ID:        8bcfff8a8514678f0fc6b56cb0f55c87
//Master Key:       3d1f3c0f42a8c541205f706f62c65330

char deviceId[] = "8bcfff8a8514678f0fc6b56cb0f55c87"; // Device Primary API Key
char m2xKey[]   = "3d1f3c0f42a8c541205f706f62c65330"; // Your M2X Master API Key
char name[] = "Wake Forest"; // Name of current location of datasource

const char *hstreamName = "humidity";
const char *tstreamName = "temperature";

double latitude = -37.97884;
double longitude = -57.54787; // You can also read those values from a GPS
double elevation = 15;

const char *streamNames[] = { tstreamName, hstreamName };
int streamNum  = sizeof(streamNames)/sizeof(const char *);
const int counts[] = { 2, 1 };
const char *ats[] = { "2016-09-12T12:12:12.234Z", 
                      "2016-09-12T12:12:12.567Z", 
                      "2016-09-12T12:12:12.000Z" };
double values[] = { 27.9, 81.2, 16.1 };

Client client;
M2XMQTTClient m2xClient(&client, m2xKey);
WNCInterface eth;

int main() {
  int response, cnt=1;
  double tval = 0.9;
  double hval = 101.0;

  eth.init();
  eth.connect();
  printf(CRLF CRLF "M2X MQTT Test..." CRLF);
  printf("IP Address: %s" CRLF, eth.getIPAddress());

  while (true) {
    tval += 0.1;
    printf("\r\n\r\nSending readings #%d\r\n",cnt++);
    response = m2xClient.updateStreamValue(deviceId, tstreamName, tval);
    printf("Sending temperature value: %lf, Response= %d" CRLF, tval, response);

    hval -= 1.0;
    printf("Sending humidity value: %lf", hval);
    response = m2xClient.updateStreamValue(deviceId, hstreamName, hval);
    printf(", Response= %d" CRLF, response);

    printf("Calling postDeviceUpdates...");
    response = m2xClient.postDeviceUpdates(deviceId, 
                streamNum,
                streamNames, 
                counts, 
                ats, 
                values);
    printf(" Response = %d" CRLF, response);
    printf("Calling updateLocation...");
    response = m2xClient.updateLocation(deviceId, name, latitude, longitude, elevation);
    printf(" Response =  %d" CRLF, response);
    elevation++;
    
    delay(6000);
//eth.doDebug(3);
  }
}


