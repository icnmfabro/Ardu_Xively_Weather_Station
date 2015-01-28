
// Integration and Development Copyright (c) 2015 Massimiliano Neri
// ArduXWeatherStation V. 1.1
// E-mail: icnmfabro@gmail.com

// Arduino Wire library 
#include "Wire.h"
// Libreries BMP085 by Filipe Vieira
// Sensor pressure Library:
#include "BMP085.h"
// RTC Library
#include "RTClib.h"
// the sensor communicates using SPI, so include the library:
#include "SPI.h"
// Ethernet Library:
#include "Ethernet.h"
#include <EthernetUdp.h>
// DNS Library
#include "Dns.h"
// Time Library:
#include <Time.h>
// HttpClient library
#include "HttpClient.h"
// Xively library
#include <Xively.h>

// Your Xively key to let you upload data
// IMPORTANT!! Insert your XIVELY ID KEY in the variable char below
// example char xivelyKey[] = "WEYasBhp2ukzeeVYYfEYvPUkyX87fGfTmDfkqPHyumLNT4aa";
char xivelyKey[] = "--    YOUR XIVELY ID KEY    --";

// Define the strings for our datastream IDs
String stringId("A__DATE_AND_DAY_READING");
String stringIt("B__TIME_READING");
char readtc[] = "C__TEMPERATURE_CELSIUS";
char readtk[] = "D__TEMPERATURE_KELVIN";
char readtf[] = "E__TEMPERATURE_FAHRENHEIT";
char readhu[] = "F__RELATIVE_HUMIDITY";
char readpr[] = "G__ATMOSPHERIC_PRESSURE";
char readdp[] = "H__DEW_POINT";
char readdpf[] = "I__DEW_POINT_FAST"; 
char readdtwb[] = "J__APPROXIMATE_TEMPERATURE_WET_BULB";
char readdclh[] = "K__APPROXIMATE_HIGHT_BASE_CUMULUS_CLOUD";
char readdclt[] = "L__APPROXIMATE_CLOUD_TEMPERATURE";
char readhi[] = "M__VALUE_HEAT_INDEX";
String stringIhi("N__MESSAGE_HEAT_INDEX");
char readssi[] = "O__VALUE_SUMMER_SIMMER_INDEX";
String stringIssi("P__MESSAGE_SUMMER_SIMMER_INDEX");
char readsiw[] = "Q__VALUE_SCHARLAU_INDEX_WINTRY";
String stringIsiw("R__MESSAGE_SCHARLAU_INDEX_WINTRY");
char readsis[] = "S__VALUE_SCHARLAU_INDEX_SUMMER";
String stringIsis("T__MESSAGE_SCHARLAU_INDEX_SUMMER");
char readlx[] = "U__INTENSITY_DIGITAL_LIGHT_SENSOR";
char readuvi[] = "W__VALUE_UV_INDEX";
String stringUvc("X__UV_INDEX_CODE_COLOR");
String stringUvs("Y__UV_STRENGTH");
String stringUvw("Z__UV_WARNINGS");
char bufferId[] = "V__WEATHER_INFORMATION_AND_ALARMS";
const int bufferSize = 140;
char bufferValue[bufferSize]; // enough space to store the string we're going to send 

XivelyDatastream datastreams[] = {
  XivelyDatastream(bufferId, strlen(bufferId), DATASTREAM_BUFFER, bufferValue, bufferSize),
  XivelyDatastream(stringId, DATASTREAM_STRING),
  XivelyDatastream(stringIt, DATASTREAM_STRING),
  XivelyDatastream(readtc, strlen(readtc), DATASTREAM_FLOAT),
  XivelyDatastream(readtf, strlen(readtf), DATASTREAM_FLOAT),
  XivelyDatastream(readtk, strlen(readtk), DATASTREAM_FLOAT),
  XivelyDatastream(readpr, strlen(readpr), DATASTREAM_FLOAT),
  XivelyDatastream(readhu, strlen(readhu), DATASTREAM_FLOAT),
  XivelyDatastream(readdp, strlen(readdp), DATASTREAM_FLOAT),
  XivelyDatastream(readdpf, strlen(readdpf), DATASTREAM_FLOAT),
  XivelyDatastream(readdtwb, strlen(readdtwb), DATASTREAM_FLOAT), 
  XivelyDatastream(readdclh, strlen(readdclh), DATASTREAM_FLOAT),
  XivelyDatastream(readdclt, strlen(readdclt), DATASTREAM_FLOAT),
  XivelyDatastream(readhi, strlen(readhi), DATASTREAM_FLOAT),
  XivelyDatastream(stringIhi, DATASTREAM_STRING),
  XivelyDatastream(readssi, strlen(readssi), DATASTREAM_FLOAT),
  XivelyDatastream(stringIssi, DATASTREAM_STRING),
  XivelyDatastream(readsiw, strlen(readsiw), DATASTREAM_FLOAT),
  XivelyDatastream(stringIsiw, DATASTREAM_STRING),
  XivelyDatastream(readsis, strlen(readsis), DATASTREAM_FLOAT),
  XivelyDatastream(stringIsis, DATASTREAM_STRING),
  XivelyDatastream(readlx, strlen(readlx), DATASTREAM_FLOAT),
  XivelyDatastream(readuvi, strlen(readuvi), DATASTREAM_FLOAT),
  XivelyDatastream(stringUvc, DATASTREAM_STRING),
  XivelyDatastream(stringUvs, DATASTREAM_STRING),
  XivelyDatastream(stringUvw, DATASTREAM_STRING)
};
// Finally, wrap the datastreams into a feed
// IMPORTANT!! Insert your XIVELY FEED ID in the fuction  below 
// example XivelyFeed feed(1198905638, datastreams, 26 /* number of datastreams */); 
XivelyFeed feed(--    XIVELY FEED ID    --, datastreams, 26 /* number of datastreams */);
 
// fill in your address here:
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
//the IP address for the shield:
//byte ip[] = { 192, 168, 0, 20 };    
//byte dns[] = { 192, 168, 0, 254 };
//byte gateway[] = { 192, 168, 0, 254 };
//byte subnet[] = { 255, 255, 255, 0 };
/* ******** NTP Server Settings ******** 
   it.pool.ntp.org NTP server 
   (Set to your time server of choice) 
   * Last Updated - March 21, 2012
   * Open Ocean Reef Controller by Brandon Bearden 
*/
IPAddress timeServer(192, 43, 244, 18); 		// time.nist.gov NTP server (fallback)
/* Set this to the offset (in seconds) to your local time
   This example is GMT - 6 */
//const long timeZoneOffset = -21600L;   

/* Syncs to NTP server every 15 seconds for testing, 
   set to 1 hour or more to be reasonable */
unsigned int ntpSyncTime = 15;         
/* ALTER THESE VARIABLES AT YOUR OWN RISK */
// local port to listen for UDP packets
unsigned int localPort = 8888;
// NTP time stamp is in the first 48 bytes of the message
const int NTP_PACKET_SIZE= 48;      
// Buffer to hold incoming and outgoing packets
byte packetBuffer[NTP_PACKET_SIZE];
// Use random servers through DNS
const char* host = "pool.ntp.org"; 
// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;                    
// Keeps track of how long ago we updated the NTP server
unsigned long ntpLastUpdate = 0;    
// Check last time clock displayed (Not in Production)
time_t prevDisplay = 0;
// Statements DNS for NTP server
DNSClient Dns;
IPAddress rem_add;

EthernetClient client;
XivelyClient xivelyclient(client);

// class default I2C address is 0x77
// specific I2C addresses may be passed as a parameter here
// (though the BMA085 supports only one address)
BMP085 dps = BMP085();      // Digital Pressure Sensor 
// Variable for sensor and functions...
float  TempC1 = 0;
float  PressMbar = 0;
float  Humidity_dht22 = 0;
float  CloudHigh = 0;
float  CloudTemp = 0;
float  HI;
String HIinfo = " ";
float  SSI;
String SSIinfo = " ";
float  SIW;
String SIWinfo = " ";
float  SIS;
String SISinfo = " ";
uint16_t IntLux;
int32_t lastMicros;
// Variables for activations NTP synchronization -> RTC
uint8_t SyncA = 0;
uint8_t SyncB = 0;

/*
UVI for UVM-30a - Author: Shawon M. Shahryiar
https://www.facebook.com/MicroArena
*/
unsigned int UV_value = 0;
String       UVCcode = " ";
String       UVStrenght = " ";
String       UVWarnings = " ";
/*
Set the string variable "Tzone" with the time zone of your country. 
for example, Italy is in the first time zone then the string variable will be:

TZone String = "UTC +1.00";

for other countries just replace the time zone in the syntax as written above.
If you live in New York (USA), the time zone is -5, then the line syntax is:

TZone String = "UTC -5.00";

SET WITH UTC TIME (SEE ALSO GMT) THE DS1307. 
DO NOT TAKE INTO ACCOUNT THE TIME SAVING. 
TO ADJUST THE DS1307, SEE THE FOLLOWING LINES.
*/
String Datews = " ";
String Timews = " ";
String TZone = "UTC +1.00";
// DHT11/22 Library - Written by ladyada, public domain:
#include "DHT.h"
#define DHTPIN 2     // what pin we're connected to
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11 
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
DHT dht(DHTPIN, DHTTYPE);

/*
BH1750 library usage.

This example initalises the BH1750 object using the default
high resolution mode and then makes a light level reading every second.

Connection:
 VCC-5v
 GND-GND
 SCL-SCL(PIN 21 for Mega)
 SDA-SDA(PIN 20 for Mega)
 ADD-NC or GND
*/
// Sensor intensity light Library:
#include <BH1750.h>
BH1750   lightMeter;

// VirtualWire and DataCoder library
#include <DataCoder.h>
#include <VirtualWire.h>
// Pin and value baudrate and declare array for XY-FST FS1000A transmitter 
const int transmit_pin = 44;
const int led_pin = 13;
const int baudRate = 2000;
float Transmit_Value[RF_DATA_ARRAY_SIZE]; //the number fo elements that 
                                          //will be transmitted. I use four elements. 
                                          //RF_DATA_ARRAY_SIZE can be changed if need be inside DataCoder.h
RTC_DS1307 rtc;

void setup() {
  
    // Serial.begin(9600);
    // Ethernet.begin(mac, ip);
    // start the Ethernet connection:
    while (Ethernet.begin(mac) != 1)
    {
      // Serial.println("Error getting IP address via DHCP, trying again...");
      delay(5000);
    }   
    // join I2C bus (Wire library)
    Wire.begin();
    // disable pullup arduino for active pullup hardware 
    digitalWrite (SDA, 0);
    digitalWrite (SCL, 0);
  
    // inizialized sensor DHT11/DHT22
    dht.begin();

    // uncomment for different initialization settings
    //dps.init();     // QFE (Field Elevation above ground level) is set to 0 meters.
                      // same as init(MODE_STANDARD, 0, true);
  
    //dps.init(MODE_STANDARD, 101850, false);  // 101850Pa = 1018.50hPa, false = using Pa units
                      // this initialization is useful for normalizing pressure to specific datum.
                      // OR setting current local hPa information from a weather station/local airport (QNH).
  
    // IMPORTANT!! To get correct values you MUST CHANGE init() parameters, in 
    // this example I've set 364m (Locate in Fabro (TR) ITALY) based on GPS data for my location.
    // example dps.init(MODE_ULTRA_HIGHRES, 25000, true);  // for 250 meters, true = using meter units
    dps.init(MODE_ULTRA_HIGHRES, -- YOUR ALTITUDE --, true);  // insert you altitude, true = using meter units
                      // this initialization is useful if current altitude is known,
                      // pressure will be calculated based on TruePressure and known altitude.

    // note: use zeroCal only after initialization.
    // dps.zeroCal(101800, 0);    // set zero point
  
    lightMeter.begin();
  
    // Setup Pin and baudrate for XY-FST FS1000A transmitter
    pinMode(led_pin,OUTPUT);
    SetupRFDataTxnLink(transmit_pin, baudRate);
  
    //Try to get the date and time
    int  trys=0;
    while(!getTimeAndDate() && trys<10) {
         trys++;
    }
    
    // Timeout of 6s for Arduino Mega 2560
    delay(6000);
}

void loop() {
  
    Datews = " ";
    Timews = " ";
    HI  = 0;
    SSI = 0;
    long Temperature = 0;
    long Pressure = 0;
    // Reading Temperature and Pressure from BMP085
    // dps.getTemperature(&Temperature); // not used
    // dps.getAltitude(&Altitude);       // not used 
    dps.getPressure(&Pressure);
    // Reading temperature or humidity from DHT11/22 - takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    Humidity_dht22 = dht.readHumidity();
    Transmit_Value[6] = Humidity_dht22; // Assign value humidity for transmitter 433mhz
    TempC1 = dht.readTemperature();
    Transmit_Value[7] = TempC1; // Assign value temperature for transmitter 433mhz
    float TempF1 = dht.convertCtoF(TempC1);   
    // Conversion pressure in Millibar
    PressMbar = Pressure / 1.0e+2;    // Conversion Pressure scale Mbar
    Transmit_Value[8] = PressMbar; // Assign value pressure for transmitter 433mhz
    // Read DS1307 Clock - Date, time in UTC (set DS1307 in UTC) plus display time zone (for local time)
    Clock();
    // Cloud Altitude and Temperature     
    clouddata();
    // Temperature Wet Bulb
    wetbulb();
    // Function for calc Heat Index 
    if (TempF1 >= 80)  
    {
        if (Humidity_dht22 >= 40)
        {
            HI = heatIndex(TempF1, Humidity_dht22);
        }
    }
    else
    {
      HI = 0;  
      HIinfo = "Value Heat Index not available"; 
    }    
    // Function for calc Summer Simmer Index
    if (TempF1 >= 71.60)  
    {
            SSI = SummerSIndex(TempF1, Humidity_dht22);
    }
    else
    {
      SSI = 0;  
      SSIinfo = "Value Summer Simmer Index not available"; 
    }   
    // Function for calc Scharlau Index Wintry
    if ((TempC1 <= 15) && (Humidity_dht22 >= 40)) 
    {
            SIW = SIWintry(TempC1, Humidity_dht22);
    }
    else
    {
      SIW = 0;  
      SIWinfo = "Value Scharlau Index Wintry not available"; 
    }       
    // Function for calc Scharlau Index Summer
    if ((TempC1 >= 15) && (Humidity_dht22 >= 30)) 
    {
            SIS = SISummer(TempC1, Humidity_dht22);
    }
    else
    {
      SIS = 0;  
      SISinfo = "Value Scharlau Index Summer not available"; 
    }       
    // Sensor reading BH1750 converted in LUX. 
    IntLux = lightMeter.readLightLevel();
    Transmit_Value[9] =  IntLux / 10.0;  // Assign value in Milliphot for transmitter 433Mhz
    // Sensor reading UVM-30a, UV Index and info
    UviSelection(); 
    // forwards the data (sensors + algorithms weather) to the account of COSM.COM
    SensorXively();
    // Transmit data with XY-FST FS1000A for remote display 
    TransmitData();    
}

void Clock() {
    /*
    CAUTION: To standardize the time of sensor readings is used UTC time (see also GMT), not local time. 
    In addition, for the time being is not considered daylight saving time.

    What does the abbreviation UTC?
    Coordinated Universal Time or "Coordinated Universal Time", the official measure of time in the world, 
    independent of time zones. This measure is calculated as a statistical average of several of cesium atomic clocks 
    around the world (International Atomic Time) periodically updated by adding or subtracting the "leap second" 
    to keep the difference between UTC and Earth rotation time below 0.9 seconds. 
    From the practical point of view the UTC corresponds to Greenwich Mean Time (GMT), which is the mean solar 
    time of the prime meridian that passes precisely from the town of Greenwich in the UK. 
    The Italian local time is one hour ahead of UTC (two hours during which it is in daylight saving time). */ 
    char * Ndow[] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };
    String Dt;
    String Tm;
  
    DateTime RTCnow = rtc.now();
  
    if ((RTCnow.hour() == 0 && SyncB == 0) || SyncA == 0) {
       syncRtcfromNtp();
       SyncA = 1;
       SyncB = 1;
    }
    else if (RTCnow.hour() > 0)
    {
       SyncB = 0;
    }
  
    // Assign value date and time for transmitter 433mhz
    Transmit_Value[0] = RTCnow.hour();
    Transmit_Value[1] = RTCnow.minute();
    Transmit_Value[2] = RTCnow.second();
    Transmit_Value[3] = RTCnow.day();
    Transmit_Value[4] = RTCnow.month();
    Transmit_Value[5] = RTCnow.year();
    //Uncomment Serial.print following only for test.
    if (RTCnow.hour() < 10)                    // correct hour if necessary
    {
      Tm += "0" + String(RTCnow.hour(), DEC);
      //Serial.print("0");
      //Serial.print(RTCnow.hour(), DEC);
    } 
    else
    {
      Tm += String(RTCnow.hour(), DEC);
      // Serial.print(RTCnow.hour(), DEC);
    }
    Tm += ":";
    //Serial.print(":");
    if (RTCnow.minute() < 10)                  // correct minute if necessary
    {
      Tm += "0" + String(RTCnow.minute(), DEC);
      //Serial.print("0");
      //Serial.print(RTCnow.minute(), DEC);
    }
    else
    {
      Tm += String(RTCnow.minute(), DEC);
      //Serial.print(RTCnow.minute(), DEC);
    }
    Tm += ":";
    //Serial.print(":");
    if (RTCnow.second() < 10)                  // correct second if necessary
    {
      Tm += "0" + String(RTCnow.second(), DEC);
      //Serial.print("0");
      //Serial.print(RTCnow.second(), DEC);
    }
    else
    {
      Tm += String(RTCnow.second(), DEC);
      //Serial.print(RTCnow.second(), DEC);
    }
    Tm += TZone;
    //Serial.print(" ");
    if (RTCnow.day() < 10)                    // correct date if necessary
    {
      Dt += "0" + String(RTCnow.day(), DEC);
      //Serial.print("0");
      //Serial.print(RTCnow.day(), DEC);
    }
    else
    {
      Dt += String(RTCnow.day(), DEC);
      //Serial.print(RTCnow.day(), DEC);
    }
    Dt += "/";
    //Serial.print("-");
    if (RTCnow.month() < 10)                   // correct month if necessary
    {
      Dt += "0" + String(RTCnow.month(), DEC);
      //Serial.print("0");
      //Serial.print(RTCnow.month(), DEC);
    }
    else
    {
      Dt += String(RTCnow.month(), DEC);
      //Serial.print(RTCnow.month(), DEC);
    }
    Dt += "/";
    Dt += String(RTCnow.year(), DEC);
    //Serial.print("-");
    //Serial.print(RTCnow.year(), DEC);          // Year need not to be changed
    //Serial.print(" ");
  
    Dt += " " + String(Ndow[RTCnow.dayOfWeekRtc()]); 
    Datews  =  Dt;
    Timews  =  Tm; 
  
    return;
}

//Celsius to Kelvin conversion
double Kelvin()
{
  
    return TempC1 + 273.15;
}


float dewPoint() {
  
    // dewPoint function NOAA
    // reference: http://wahiduddin.net/calc/density_algorithms.htm   
    float XA0 = 373.15/(273.15 + TempC1);
    float SUM = -7.90298 * (XA0-1);
    SUM += 5.02808 * log10(XA0);
    SUM += -1.3816e-7 * (pow(10, (11.344*(1-1/XA0)))-1) ;
    SUM += 8.1328e-3 * (pow(10,(-3.49149*(XA0-1)))-1) ;
    SUM += log10(1013.246);
    float VP = pow(10, SUM-3) * Humidity_dht22;
    float T = log(VP/0.61078);   // temp var
    return (241.88 * T) / (17.558-T);
}


float dewPointFast() {
    
    // delta max = 0.6544 wrt dewPoint()
    // 5x faster than dewPoint()
    // reference: http://en.wikipedia.org/wiki/Dew_point   
    float a = 17.271;
    float b = 237.7;
    float temp = (a * TempC1) / (b + TempC1) + log(Humidity_dht22/100);
    float Td = (b * temp) / (a - temp); 
    return Td;
}

void clouddata() {
  
    CloudHigh = 0;
    CloudTemp = 0; 
    /* Heights of the bases of cumulus clouds.
       Calculation of the base height (CloudHigh in meters) of a cloud having parameters as a surface temperature and the dew point. */
    CloudHigh = 125 *(TempC1 - dewPoint());
    /* This calculator is based on the assumption that the air temperature drops 9.84 degrees C per 1000 m 
       of altitude and the dewpoint drops 1.82 degrees C per 1000 meters altitude. The following formulas must 
       be solved to estimate cloud height and temperature: */ 
    CloudTemp = (-0.00182 * CloudHigh) + dewPoint(); 
    return;
}

float wetbulb() {
  
    /* First Metod:
       To calculate the wet bulb temperature (tw), one can use a simple formula, derived from JEEVANANDA REDDY:
      
       Twb = t * (0.45 + 0006 * Ur * SQR (p/1060))

       where:
       tw = wet bulb temperature (° C)
       t = dry bulb temperature (° C)
       RH = relative humidity (in%)
       p = pressure (in hPa).
     
      
       float Twb = TempC1 * (0.45 + 0.006 * Humidity_dht22 * sqrt(PressMbar / 1060));     
       return Twb; 
    
       OR
    
       Second Metod:
    
       Wet-Bulb Temperature from Relative Humidity and Air Temperature
                             Roland Stull
       University of British Columbia, Vancouver, British Columbia, Canada
    
       An equation is presented for wet-bulb temperature as a function of air temperature
       and relative humidity at standard sea level pressure. It was found as an empirical 
       fit using gene-expression programming. This equation is valid for relative humidities 
       between 5% and 99% and for air temperatures between −20° and 50°C, except for 
       situations having both low humidity and cold temperature. Over the valid range, 
       errors in wet-bulb temperature range from −1° to +0.65°C, with mean absolute error 
       of less than 0.3°C.
    */
  
    float sqe1 = atan(0.151977 * sqrt(Humidity_dht22 + 8.313659)); 
    float sqe2 = atan(TempC1 + Humidity_dht22) - atan(Humidity_dht22 - 1.676331);
    float sqe3 = sqrt(pow(Humidity_dht22, 3)) * atan(0.023101 * Humidity_dht22);
   
    float Twb = TempC1 * sqe1 + sqe2 + 0.00391838 * sqe3 - 4.686035; 
   
    return Twb; 
}


float heatIndex(float Tf, float Rh)
{
  
    // Heat Index function.
    // reference: http://en.wikipedia.org/wiki/Heat_index
    float T = Tf;
    float R = Rh;
    float c1 = -42.38, c2 = 2.049, c3 = 10.14, c4 = -0.2248, c5= -6.838e-3, c6=-5.482e-2, c7=1.228e-3, c8=8.528e-4, c9=-1.99e-6;

    float A = (( c5 * T) + c2) * T + c1;
    float B = ((c7 * T) + c4) * T + c3;
    float C = ((c9 * T) + c8) * T + c6;

    float rv = (C * R + B) * R + A;
    // message composition for heat index

    if ((rv >= 79) && (rv < 90))
    {
       HIinfo = "Caution — fatigue is possible with prolonged exposure and activity. Continuing activity could result in heat cramps";
    }
    if ((rv >= 90 ) && (rv < 105))
    {
       HIinfo = "Extreme caution — heat cramps and heat exhaustion are possible. Continuing activity could result in heat stroke";
    }    
    if ((rv >= 105 ) && (rv < 130))
    {
       HIinfo = "Danger — heat cramps and heat exhaustion are likely; heat stroke is probable with continued activity";
    } 
    if (rv >= 130 ) 
    {
       HIinfo = "Extreme danger — heat stroke is imminent";
    }      

    return rv;
}

float SummerSIndex(float Tf1, float Rh1)
{
  
    // Summer Simmer Index function.
    // reference: http://web.fi.ibimet.cnr.it/teoria/SSI.htm
    //            http://www.summersimmer.com/home.htm
    float T1 = Tf1;
    float R1 = Rh1;
    float SSIndex = 1.98 * (T1 - (0.55 - 0.0055 * R1) * (T1 - 58)) - 56.83;
    // message composition for summer simmer index

    if ((SSIndex >= 70) && (SSIndex < 77))
    {
        SSIinfo = "Slightly cool - most of the people are at ease, even if it is slightly cool";
    }
    if ((SSIndex >= 77 ) && (SSIndex < 83))
    {
        SSIinfo = "Comfortable - almost all are in comfortable conditions";
    }    
    if ((SSIndex >= 83 ) && (SSIndex < 91))
    {
        SSIinfo = "Slightly warm - most people are at ease, even if it is slightly warm";
    } 
    if ((SSIndex >= 91 ) && (SSIndex < 100))
    {
        SSIinfo = "Warm - there is an increase in discomfort";
    }       
    if ((SSIndex >= 100 ) && (SSIndex < 112))
    {
        SSIinfo = "Average warm - significant discomfort. There is a danger of sunstroke and heat exhaustion from prolonged sun exposure and / or physical activity";
    }  
    if ((SSIndex >= 112 ) && (SSIndex < 125))
    {
        SSIinfo = "Very warm - uncomfortable high. All are uncomfortable. There is a danger of heat stroke";
    }  
    if ((SSIndex >= 125 ) && (SSIndex < 150))
    {
        SSIinfo = "Extremely warm - maximum discomfort. High risk of heat stroke, especially for the most vulnerable people, the elderly and even younger children";
    }  
  
    return SSIndex;
}

float SIWintry(float Tc, float Rh)
{ 
  
    // Scharlau Index Wintry function.
    // reference: http://www.meteo.unina.it/bioclimatologia/indici-bioclimatici/scharlau-invernale
    float T = Tc;
    float R = Rh;
    float Tcrw = (-0.0003 * pow(R, 2)) + (0.1497 * R) - 7.7133;
  
    float Fsiw = (T-Tcrw);

    // message composition for Scharlau Index Wintry
    SIWinfo = MsgSIWS(Fsiw);

    return Fsiw;
}

float SISummer(float Tc1, float Rh1)
{
  
    // Scharlau Index Summer function.
    // reference: http://www.meteo.unina.it/bioclimatologia/indici-bioclimatici/scharlau-estivo
    float T1 = Tc1;
    float R1 = Rh1;
    float Tcrs = (-17.089 * log(R1)) + 94.979;
  
    float Fsis = (Tcrs-T1);

    // message composition for Scharlau Index Wintry
    SISinfo = MsgSIWS(Fsis);

    return Fsis;
}

String MsgSIWS(float siws)
{
  
    // table lists the thresholds for both climate discomfort linked 
    // to the value of the Scharlau Index.
    String SIWSinfo = " ";

    if (siws <= -3 )
    {
       SIWSinfo = "Intense discomfort";
    } 
    if (siws >= 0 ) 
    {
       SIWSinfo = "Welfare";
    }        
    if ((siws < 0) && (siws > -1))
    {
       SIWSinfo = "Weak discomfort";
    }
    if ((siws <= -1 ) && (siws > -3))
    {
       SIWSinfo = "Moderate discomfort";
    }    

    return SIWSinfo;
}

void SensorXively() {
  
    float sensorValue = 0;
    
    String stringValue = Datews;
    datastreams[1].setString(stringValue);
  
    stringValue = Timews;
    datastreams[2].setString(stringValue);
  
    sensorValue = TempC1;
    datastreams[3].setFloat(sensorValue);
  
    sensorValue = dht.convertCtoF(TempC1);
    datastreams[4].setFloat(sensorValue);
  
    sensorValue = Kelvin();
    datastreams[5].setFloat(sensorValue);
  
    sensorValue = PressMbar;
    datastreams[6].setFloat(sensorValue);
  
    sensorValue = Humidity_dht22;
    datastreams[7].setFloat(sensorValue);
  
    sensorValue = dewPoint();
    datastreams[8].setFloat(sensorValue);
  
    sensorValue = dewPointFast();
    datastreams[9].setFloat(sensorValue);
   
    sensorValue = wetbulb();
    datastreams[10].setFloat(sensorValue);
   
    sensorValue = CloudHigh;
    datastreams[11].setFloat(sensorValue);

    sensorValue = CloudTemp;
    datastreams[12].setFloat(sensorValue);
    
    datastreams[0].setBuffer("");
  
    sensorValue = HI;
    datastreams[13].setFloat(sensorValue);
  
    stringValue = HIinfo;
    datastreams[14].setString(stringValue);
  
    sensorValue = SSI;
    datastreams[15].setFloat(sensorValue);
  
    stringValue = SSIinfo;
    datastreams[16].setString(stringValue);
  
    sensorValue = SIW;
    datastreams[17].setFloat(sensorValue);
  
    stringValue = SIWinfo;
    datastreams[18].setString(stringValue);
    
    sensorValue = SIS;
    datastreams[19].setFloat(sensorValue);
  
    stringValue = SISinfo;
    datastreams[20].setString(stringValue);
   
    sensorValue = IntLux;
    datastreams[21].setFloat(sensorValue);
  
    sensorValue = UV_value;
    datastreams[22].setFloat(sensorValue);
  
    stringValue = UVCcode;
    datastreams[23].setString(stringValue);
  
    stringValue = UVStrenght;
    datastreams[24].setString(stringValue);
  
    stringValue = UVWarnings;
    datastreams[25].setString(stringValue);  
  
    int ret = xivelyclient.put(feed, xivelyKey);
  
    delay(10000);
}

unsigned int adcAverage()
{
  
    unsigned char samples = 0;
    unsigned long avg = 0;
    for(samples = 0; samples < 50; samples += 1)
    {
      avg = (avg + analogRead(A8));
      delayMicroseconds(4);
    }  
    avg = (avg / 50.0);
    return avg;
}

void UviSelection() {
    
    UV_value   = 0;
    UVCcode    = "No Data";
    UVStrenght = "No Data";
    UVWarnings = "No Data";
    // function for read pin analog (A8) data uvm-30a
    UV_value   = adcAverage();
    // UV Index range and exposure categories and sun protection messages
    if((UV_value <= 10))
    {
      UV_value = 0;
    }
    else if((UV_value > 10) && (UV_value <= 46))
    {
      UV_value = 1;
    }
    else if((UV_value > 46) && (UV_value <= 65))
    {
      UV_value = 2;
    }
    else if((UV_value > 65) && (UV_value <= 83))
    {
      UV_value = 3;
    }
    else if((UV_value > 83) && (UV_value <= 103))
    {
      UV_value = 4;
    }
    else if((UV_value > 103) && (UV_value <= 124))
    {
      UV_value = 5;
    }
    else if((UV_value > 124) && (UV_value <= 142))
    {
      UV_value = 6;
    }
    else if((UV_value > 142) && (UV_value <= 163))
    {
      UV_value = 7;
    }
    else if((UV_value > 163) && (UV_value <= 180))
    {
      UV_value = 8;    
    }
    else if((UV_value > 180) && (UV_value <= 200))
    {
      UV_value = 9;
    }
    else if((UV_value > 200) && (UV_value <= 221))
    {
      UV_value = 9;
    }
    else if((UV_value > 221) && (UV_value <= 239))
    {
      UV_value = 10;
    }
    else
    {
      UV_value   = 11;
      UVCcode    = "VIOLET";
      UVStrenght = "Extreme risk - EXTRA PROTECTION REQUIRED";
      UVWarnings = "Avoid being outside during midday hours!. Make sure you seek shade!. Shirt, sunscreen and a hat are a must"; 
    }
    if ((UV_value == 1 ) || (UV_value == 2))
    {
      UVCcode    = "GREEN";
      UVStrenght = "Low danger - NO PROTECTION REQUIRED";
      UVWarnings = "You can safely stay outsides";
    }  
      if ((UV_value >= 3 ) && (UV_value <= 5))
    {
      UVCcode    = "YELLOW";
      UVStrenght = "Moderate risk - PROTECTION REQUIRED";
      UVWarnings = "Slip on a shirt, slop on sunscreen and slap on a hat";
    }
    if ((UV_value == 6 ) || (UV_value == 7))
    {
      UVCcode    = "ORANGE";
      UVStrenght = "High risk - PROTECTION REQUIRED";
      UVWarnings = "Slip on a shirt, slop on sunscreen and slap on a hat!. Seek shade during midday hours";
    }
      if ((UV_value >= 8 ) && (UV_value <= 10))
    {
      UVCcode    = "RED";
      UVStrenght = "Very High risk - EXTRA PROTECTION REQUIRED";
      UVWarnings = "Make sure you seek shade!. Shirt, sunscreen and a hat are a must!. Avoid being outside during midday hours";
    }  
    Transmit_Value[10] = UV_value; // Assign value UV Index for transmitter 433Mhz
}  

void TransmitData() {
  
    union RFData outDataSeq; 
    EncodeRFData(Transmit_Value, outDataSeq);  
    TransmitRFData(outDataSeq);  
    digitalWrite(led_pin, HIGH);
    delay(100);
    digitalWrite(led_pin,LOW);
    delay(1000); 
}  
    
int getTimeAndDate() {
   
    // Do not alter this function, it is used by the system
    int flag=0;
    Udp.begin(localPort);
    // Begin DNS 
    Dns.begin(Ethernet.dnsServerIP());
    if(Dns.getHostByName(host, rem_add) == 1 ){
	sendNTPpacket(rem_add);
    }
    else{
	sendNTPpacket(timeServer); 	// send an NTP packet to a time server
    }
    //sendNTPpacket(timeServer);
    delay(1000);
    if (Udp.parsePacket()){
        Udp.read(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer
        unsigned long highWord, lowWord, epoch;
        highWord = word(packetBuffer[40], packetBuffer[41]);
        lowWord = word(packetBuffer[42], packetBuffer[43]);  
        epoch = highWord << 16 | lowWord;
        epoch = epoch - 2208988800; // + timeZoneOffset if you add time zone 
        flag=1;
        setTime(epoch);
        ntpLastUpdate = now();
    }
    return flag;
}

unsigned long sendNTPpacket(IPAddress& address)
{
  
    // Do not alter this function, it is used by the system
    memset(packetBuffer, 0, NTP_PACKET_SIZE); 
    packetBuffer[0] = 0b11100011;
    packetBuffer[1] = 0;
    packetBuffer[2] = 6;
    packetBuffer[3] = 0xEC;
    packetBuffer[12]  = 49; 
    packetBuffer[13]  = 0x4E;
    packetBuffer[14]  = 49;
    packetBuffer[15]  = 52;		   
    Udp.beginPacket(address, 123);
    Udp.write(packetBuffer,NTP_PACKET_SIZE);
    Udp.endPacket(); 
}

void syncRtcfromNtp(){
  
    DateTime RTCnow = rtc.now();
    // Update the time via NTP server as often as the time you set at the top
    if(now()-ntpLastUpdate > ntpSyncTime) {
        int trys=0;
        while(!getTimeAndDate() && trys<10){
            trys++;
        }
        /* only for test 
        if(trys<10){
            Serial.println("ntp server update success");
        }
        else{
            Serial.println("ntp server update failed");
        } 
        */    
    }    
    // Display the time if it has changed by more than a second.
    if( now() != prevDisplay){
        prevDisplay = now();
        rtc.adjust(DateTime(year(), month(), day(), hour(), minute(), second())); 
    }
    // wait ten seconds before asking for the time again
    delay(10000); 
}
