// Integration and Development Copyright (c) 2015 Massimiliano Neri
// E-mail: icnmfabro@gmail.com

// VirtualWire and DataCoder library
#include <DataCoder.h>
#include <VirtualWire.h>
// UTFT and UTouch  (C)2010-2012 Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
#include <UTFT.h>
#include <UTouch.h>
// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t hallfetica_normal[];
// To use your screen 3.2 " TFT Touch modify the following two lines
// Next two lines for the Arduino Mega
UTFT          myGLCD(ELEE32_REVA,A1,A2,A0,A4,A5);   // Remember to change the model parameter to suit your display module!
UTouch        myTouch(13,10,11,12,A4);

int      x, y;
// Counter variable for views on the TFT display
unsigned int NDisplay  = 0;
unsigned int NDisplayW = 0;
/*
Set the string variable "Tzone" with the time zone of your country. 
for example, Italy is in the first time zone then the string variable will be:

TZone String = "UTC +1.00";

for other countries just replace the time zone in the syntax as written above.
If you live in New York (USA), the time zone is -5, then the line syntax is:

TZone String = "UTC -5.00";
*/

String TZone = "UTC+1";
// Pin and value baudrate and declare array for XY-FST FS1000A transmitter 
const int transmit_pin = 44;
const int led_pin = 13;
const int baudRate = 2000;
float inArray[RF_DATA_ARRAY_SIZE];//To store decoded information
//RF_DATA_ARRAY_SIZE can be changed if need be inside DataCoder.h
                          /* 0 = hour
                             1 = minute
                             2 = seconds
                             3 = day
                             4 = month
                             5 = year 
                             6 = Umidity
                             7 = Temperature °C
                             8 = Pressure
                             9 = Intensity Light in milliphot
                             10= UV Index
                          */

void setup()
{
  delay(1000);
  pinMode(led_pin,OUTPUT);
  digitalWrite(led_pin,LOW);
  SetupRFDataRxnLink(transmit_pin, baudRate);
  // Initial setup
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);
  // Graphic main menu
  DispGrapMain(); 
}

void loop()
{      	
     uint8_t buf[VW_MAX_MESSAGE_LEN];
     uint8_t buflen = VW_MAX_MESSAGE_LEN;
  
     union RFData inDataSeq;//To store incoming data
  
     if(RFLinkDataAvailable(buf, &buflen))
     {
        digitalWrite(led_pin, HIGH);
        for(int i =0; i< buflen; i++)
        {
          inDataSeq.s[i] = buf[i];
        }
 
        digitalWrite(led_pin, LOW);
        DecodeRFData(inArray, inDataSeq);
        switch (NDisplay) {
               case 0:                    
               DispDateTime();    
               Data1();
               break;
               case 1:    
               DispDateTime();             
               Data2();
               break;
               case 2:    
               DispDateTime();    
               Data3();                  
               break;
               case 3:    
               DispDateTime();    
               Data4();                   
               break;
               default:
               DispDateTime();    
               Data5();                 
       } 
     }      

    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();

      if ((x>=0) && (x<=70))  
      {      
        if ((y>=50) && (y<=70))  // Button: 1
        {
          waitForIt(0, 50, 70, 70);
          CleanDataDisp();
          DispWait();
          NDisplay  = 0;  
        } 
        if ((y>=80) && (y<=100))  // Button: 2
        {
          waitForIt(0, 80, 70, 100);
          CleanDataDisp();
          DispWait();
          NDisplay = 1;  
        }
        if ((y>=110) && (y<=130))  // Button: 3
        {
          waitForIt(0, 110, 70, 130);
          CleanDataDisp();
          DispWait();
          NDisplay = 2;  
        }
        if ((y>=140) && (y<=160))  // Button: 4
        {
          waitForIt(0, 140, 70, 160);
          CleanDataDisp();
          DispWait();
          NDisplay = 3;  
        }
        if ((y>=170) && (y<=190))  // Button: 5
        {
          waitForIt(0, 170, 70, 190);
          CleanDataDisp();
          DispWait();
          NDisplay = 4;
        }  
      }      
    }      
} 

void DispDateTime()
{  
  /* CAUTION: To standardize the time of sensor readings is used UTC time (see also GMT), not local time. 
  In addition, for the time being is not considered daylight saving time.

  What does the abbreviation UTC?
  Coordinated Universal Time or "Coordinated Universal Time", the official measure of time in the world, 
  independent of time zones. This measure is calculated as a statistical average of several of cesium atomic clocks 
  around the world (International Atomic Time) periodically updated by adding or subtracting the "leap second" 
  to keep the difference between UTC and Earth rotation time below 0.9 seconds. 
  From the practical point of view the UTC corresponds to Greenwich Mean Time (GMT), which is the mean solar 
  time of the prime meridian that passes precisely from the town of Greenwich in the UK. 
  The Italian local time is one hour ahead of UTC (two hours during which it is in daylight saving time). */ 
  String Dt;
  String Tm;
  
  if (inArray[0] < 10)                    // correct hour if necessary
  {
    Tm += "0" + String(int(inArray[0]));
  } 
  else
  {
    Tm += String(int(inArray[0]));
  }
  Tm += ":";
  if (inArray[1] < 10)                  // correct minute if necessary
  {
    Tm += "0" + String(int(inArray[1]));
  }
  else
  {
    Tm += String(int(inArray[1]));
  }
  Tm += ":";
  if (inArray[2] < 10)                  // correct second if necessary
  {
    Tm += "0" + String(int(inArray[2]));
  }
  else
  {
    Tm += String(int(inArray[2]));
  }
  Tm += TZone;
  if (inArray[3] < 10)                    // correct date if necessary
  {
    Dt += "0" + String(int(inArray[3]));
  }
  else
  {
    Dt += String(int(inArray[3]));
  }
  Dt += "/";
  if (inArray[4] < 10)                   // correct month if necessary
  {
    Dt += "0" + String(int(inArray[4]));
  }
  else
  {
    Dt += String(int(inArray[4]));
  }
  Dt += "/";
  Dt += String(int(inArray[5] - 2000));

  myGLCD.setFont(SmallFont);
  myGLCD.setColor(VGA_BLACK);
  myGLCD.setBackColor(255, 127, 0);
  myGLCD.print(Dt , 5, 25);
  myGLCD.print(Tm , 208, 225);   
} 

void DispGrapMain()
{  
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.print("ARDUXWS FABRO1", 172, 5);
  myGLCD.setColor(255, 127, 0);
  myGLCD.fillRoundRect(0, 0, 170, 20);
  myGLCD.fillRoundRect(0, 10, 70, 40);
  myGLCD.fillRoundRect(285, 0, 319, 20);
  myGLCD.setColor(VGA_OLIVE);
  myGLCD.fillRoundRect(0, 50, 70, 70);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (0, 50, 70, 70);
  myGLCD.setColor(VGA_BLACK);
  myGLCD.setBackColor(VGA_OLIVE);
  myGLCD.print("DATA 1", 12, 54);
  myGLCD.setColor(VGA_GREEN);
  myGLCD.fillRoundRect(0, 80, 70, 100);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (0, 80, 70, 100);
  myGLCD.setColor(VGA_BLACK);
  myGLCD.setBackColor(VGA_GREEN);
  myGLCD.print("DATA 2", 12, 84);
  myGLCD.setColor(VGA_LIME);
  myGLCD.fillRoundRect(0, 110, 70, 130);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (0, 110, 70, 130);
  myGLCD.setColor(VGA_BLACK);
  myGLCD.setBackColor(VGA_LIME);
  myGLCD.print("DATA 3", 12, 114);
  myGLCD.setColor(VGA_SILVER);
  myGLCD.fillRoundRect(0, 140, 70, 160);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (0, 140, 70, 160);
  myGLCD.setColor(VGA_SILVER);
  myGLCD.fillRoundRect(0, 170, 70, 190);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (0, 170, 70, 190);
  myGLCD.setColor(VGA_BLACK);
  myGLCD.setBackColor(VGA_SILVER);
  myGLCD.print("HI/SSI", 13, 144);
  myGLCD.print("SIW/SIS", 9, 174);
  myGLCD.setColor(255, 127, 0);
  myGLCD.fillRoundRect(200, 220, 319, 239);
  DispWait();  
} 

void Data1()
{ 
  String Data1TC = String(inArray[7]) + " `C";
  String Data1TF = String(inArray[7] * 9 / 5 + 32) + " `F";
  String Data1TK = String(inArray[7] + 273.15) + " K";            
  String Data1UM = String(inArray[6]) + " %Rh";
  String Data1PR = String(inArray[8]) + " Mbar";
  String Data1IL = String(int(inArray[9] * 10.00)) + " Lux";
      
  if (NDisplayW == 1)
  { 
      CleanWait();
  }  
  
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK); 
  myGLCD.print("Temperature", 90, 30); 
  myGLCD.setFont(hallfetica_normal);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK); 
  myGLCD.print("              ", 100, 45);
  myGLCD.print("              ", 100, 65);
  myGLCD.print("              ", 100, 85);
  myGLCD.print(Data1TC, 100, 45);
  myGLCD.print(Data1TF, 100, 65);
  myGLCD.print(Data1TK, 100, 85);
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK); 
  myGLCD.print("Humidity", 90, 110);
  myGLCD.setFont(hallfetica_normal);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.print("              ", 100, 125); 
  myGLCD.print(Data1UM, 100, 125);
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.print("Atmospheric Pressure", 90, 150);
  myGLCD.setFont(hallfetica_normal);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK); 
  myGLCD.print("              ", 100, 165);
  myGLCD.print(Data1PR, 100, 165);
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK); 
  myGLCD.print("Intensity Light", 5, 205); 
  myGLCD.setFont(hallfetica_normal);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK); 
  myGLCD.print("           ", 15, 220);
  myGLCD.print(Data1IL, 15, 220);   
} 

void Data2()
{ 
  String UVCcode        = " ";
  unsigned int UV_value = int(inArray[10]);
  String Data1UVI       = String(int(inArray[10])) + " UVI";

  if (NDisplayW == 1)
  { 
      CleanWait();
  }  
  
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK); 
  myGLCD.print("UV Index", 90, 30); 
  myGLCD.setFont(hallfetica_normal);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.print("        ", 100, 45); 
  myGLCD.print(Data1UVI, 100, 45);

  myGLCD.setFont(SmallFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK); 
  myGLCD.print("UV Warning", 90, 70);
  myGLCD.print("UV Strenght", 90, 160);
  
  myGLCD.print("                            ", 100, 85);
  myGLCD.print("                            ", 100, 95);
  myGLCD.print("                            ", 100, 105);
  myGLCD.print("                            ", 100, 115);
  myGLCD.print("                            ", 100, 125);
  myGLCD.print("                            ", 100, 135); 
  myGLCD.print("                            ", 100, 175);
  myGLCD.print("                            ", 100, 185); 
      
  if ( UV_value == 0 )
  {
      myGLCD.print("NO DATA                     ", 100, 85);
      myGLCD.print("                            ", 100, 97);
      myGLCD.print("                            ", 100, 109);
      myGLCD.print("                            ", 100, 121);
      myGLCD.print("                            ", 100, 133); 
      myGLCD.print("NO DATA                     ", 100, 175);
      myGLCD.print("                            ", 100, 185); 
      UVCcode = "NODATA";
  }        
  if ((UV_value == 1 ) || (UV_value == 2))
  {
      myGLCD.print("You can safely stay         ", 100, 85);
      myGLCD.print("outsides.                   ", 100, 97);
      myGLCD.print("                            ", 100, 109);
      myGLCD.print("                            ", 100, 121);
      myGLCD.print("                            ", 100, 133); 
      myGLCD.print("LOW DANGER                  ", 100, 175);
      myGLCD.print("NO PROTECTION REQUIRED      ", 100, 185); 
      UVCcode = "GREEN";
  }  
    if ((UV_value >= 3 ) && (UV_value <= 5))
  {
      myGLCD.print("Slip on a shirt, slop on    ", 100, 85);
      myGLCD.print("sunscreen and slap on a     ", 100, 97);
      myGLCD.print("hat.                        ", 100, 109);
      myGLCD.print("                            ", 100, 121);
      myGLCD.print("                            ", 100, 133);
      myGLCD.print("MODERATE RISK               ", 100, 175);
      myGLCD.print("PROTECTION REQUIRED         ", 100, 185); 
      UVCcode = "YELLOW";
  }
  if ((UV_value == 6 ) || (UV_value == 7))
  {
      myGLCD.print("Slip on a shirt, slop on    ", 100, 85);
      myGLCD.print("sunscreen and slap on a     ", 100, 97);
      myGLCD.print("hat!.                       ", 100, 109);
      myGLCD.print("Seek shade during midday    ", 100, 121);
      myGLCD.print("hours.                      ", 100, 133);
      myGLCD.print("HIGH RISK                   ", 100, 175);
      myGLCD.print("PROTECTION REQUIRED         ", 100, 185); 
      UVCcode = "ORANGE";
  }
    if ((UV_value >= 8 ) && (UV_value <= 10))
  {
      myGLCD.print("Make sure you seek shade!.  ", 100, 85);
      myGLCD.print("Shirt, sunscreen and a hat  ", 100, 97);
      myGLCD.print("are a must!.                ", 100, 109);
      myGLCD.print("Avoid being outside during  ", 100, 121);
      myGLCD.print("midday hours.               ", 100, 133);
      myGLCD.print("VERY HIGH RISK              ", 100, 175);
      myGLCD.print("EXTRA PROTECTION REQUIRED   ", 100, 185); 
      UVCcode = "RED";
  }    
    if ( UV_value >= 11 )
  {
      myGLCD.print("Avoid being outside during  ", 100, 85);
      myGLCD.print("midday hours!.              ", 100, 97);
      myGLCD.print("Make sure you seek shade!.  ", 100, 109);
      myGLCD.print("Shirt, sunscreen and a hat  ", 100, 121);
      myGLCD.print("are a must.                 ", 100, 133);
      myGLCD.print("EXTREME RISK                ", 100, 175);
      myGLCD.print("EXTRA PROTECTION REQUIRED   ", 100, 185); 
      UVCcode = "VIOLET";
  }      

  myGLCD.print("UV Code", 5, 205); 
  myGLCD.setFont(hallfetica_normal);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.print("      ", 15, 220); 
  myGLCD.print(UVCcode, 15, 220);
  
  if (UVCcode == "GREEN")  {
     myGLCD.setColor(VGA_GREEN);
  }
  else if (UVCcode == "YELLOW")  {
     myGLCD.setColor(VGA_YELLOW);
  }
  else if (UVCcode == "ORANGE")  {
     myGLCD.setColor(255, 165, 0);
  }
  else if (UVCcode == "RED")  {
     myGLCD.setColor(VGA_RED);
  }   
  else if (UVCcode == "VIOLET")  {
     myGLCD.setColor(143, 0, 255);
  }   
  else {
     myGLCD.setColor(VGA_BLACK);   
  }

  myGLCD.fillRoundRect(118, 220, 128, 239);
  myGLCD.fillRoundRect(138, 220, 148, 239);
  myGLCD.fillRoundRect(158, 220, 168, 239);
  myGLCD.fillRoundRect(178, 220, 188, 239); 
}

void Data3()
{
  String Data3DP = String(dewPoint()) + " `C"; 
  String Data3DPF = String(dewPointFast()) + " `C";
  String Data3WB = String(wetbulb()) + " `C";
  String Data3HBCC = String(cloudH()) + " M";
  String Data3CT = String(cloudT()) + " `C"; 
 
  if (NDisplayW == 1)
  { 
      CleanWait();
  }  
  
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK); 
  myGLCD.print("Approximate", 90, 30);
  myGLCD.print("Temperature Wet Bulb", 90, 40); 
  myGLCD.setFont(hallfetica_normal);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.print("              ", 100, 55); 
  myGLCD.print(Data3WB, 100, 55);
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK); 
  myGLCD.print("Approximate", 90, 75);
  myGLCD.print("Hight Base Cumulus Cloud", 90, 85);
  myGLCD.setFont(hallfetica_normal);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.print("              ", 100, 100);  
  myGLCD.print(Data3HBCC, 100, 100);
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.print("Approximate Cloud", 90, 120);
  myGLCD.print("Temperature", 90, 130);
  myGLCD.setFont(hallfetica_normal);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.print("              ", 100, 145);  
  myGLCD.print(Data3CT, 100, 145);
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.print("Dev Point", 90, 165);
  myGLCD.setFont(hallfetica_normal);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.print("              ", 100, 180);  
  myGLCD.print(Data3DP, 100, 180);
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.print("Dev Point Fast", 5, 205);
  myGLCD.setFont(hallfetica_normal);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.print("           ", 15, 220);  
  myGLCD.print(Data3DPF, 15, 220);   
} 

void Data4()
{
  float  Data4HI = 0;
  float  Data4SSI = 0;
  float  Data4TF = inArray[7] * 9 / 5 + 32;
  
  if (NDisplayW == 1)
  { 
      CleanWait();
  }  
  
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK); 
  myGLCD.print("Heat Index", 90, 30);
  myGLCD.print("Summer Simmer Index", 90, 115);
  myGLCD.print("                           ", 100, 60);
  myGLCD.print("                           ", 100, 70);
  myGLCD.print("                           ", 100, 80);
  myGLCD.print("                           ", 100, 90);
  myGLCD.print("                           ", 100, 100);
  
  // Function for calc Heat Index 
  if (Data4TF >= 80)  
  {
      if (inArray[6] >= 40)
      {
          Data4HI = heatIndex(Data4TF, inArray[6]);
      }
  }
  else
  {
      myGLCD.print("Value Heat Index           ", 100, 60);
      myGLCD.print("not available              ", 100, 70);
      myGLCD.print("                           ", 100, 80);
      myGLCD.print("                           ", 100, 90);
      myGLCD.print("                           ", 100, 100); 
  } 
  myGLCD.setFont(hallfetica_normal);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.print("              ", 100, 45); 
  myGLCD.print(String(Data4HI), 100, 45);
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.print("                           ", 100, 145);
  myGLCD.print("                           ", 100, 155);
  myGLCD.print("                           ", 100, 165);    
  myGLCD.print("                           ", 100, 175);
  myGLCD.print("                           ", 100, 185);
  myGLCD.print("                           ", 100, 195);    
  
  // Function for calc Summer Simmer Index
  if (Data4TF >= 71.60)  
  {
      Data4SSI = SummerSIndex(Data4TF, inArray[6]);
  }
  else
  {
      myGLCD.print("Value Summer Simmer Index  ", 100, 145);
      myGLCD.print("not available              ", 100, 155);
      myGLCD.print("                           ", 100, 165);    
      myGLCD.print("                           ", 100, 175);
      myGLCD.print("                           ", 100, 185);
      myGLCD.print("                           ", 100, 195); 
  }    

  myGLCD.setFont(hallfetica_normal);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.print("              ", 100, 130); 
  myGLCD.print(String(Data4SSI), 100, 130);
} 

void Data5()
{
  float  Data4SIW = 0;
  float  Data4SIS = 0;
  
  if (NDisplayW == 1)
  { 
      CleanWait();
  }  
  
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK); 
  myGLCD.print("Scharlau Index Wintry", 90, 30);
  myGLCD.print("Scharlau Index Summer", 90, 115);  
  myGLCD.print("                           ", 100, 60);
  myGLCD.print("                           ", 100, 70);

  // Function for calc Scharlau Index Wintry
  if ((inArray[7] <= 15) && (inArray[6] >= 40)) 
  {
      Data4SIW = SIWintry(inArray[7], inArray[6]);
  }
  else
  {
      myGLCD.print("Value Scharlau Index Wintry", 100, 60);
      myGLCD.print("not available              ", 100, 70);   
  }         

  myGLCD.setFont(hallfetica_normal);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.print("              ", 100, 45); 
  myGLCD.print(String(Data4SIW), 100, 45);
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.print("                           ", 100, 145);
  myGLCD.print("                           ", 100, 155);    
  
  // Function for calc Scharlau Index Summer
  if ((inArray[7] >= 15) && (inArray[6] >= 30)) 
  {
      Data4SIS = SISummer(inArray[7], inArray[6]);
  }
  else
  {
      myGLCD.print("Value Scharlau Index Summer", 100, 145);
      myGLCD.print("not available              ", 100, 155); 
  }       

  myGLCD.setFont(hallfetica_normal);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.print("              ", 100, 130); 
  myGLCD.print(String(Data4SIS), 100, 130);   
} 

void CleanDataDisp()
{
  // Clean rect of display data
  myGLCD.setColor(VGA_BLACK);
  myGLCD.fillRoundRect(0, 193, 199, 239);
  myGLCD.fillRoundRect(71, 21, 319, 219);   
} 

void waitForIt(int x1, int y1, int x2, int y2)
{
  // Draw a red frame while a button is touched
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  while (myTouch.dataAvailable())
        myTouch.read();
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
}

float dewPoint() {
   // dewPoint function NOAA
   // reference: http://wahiduddin.net/calc/density_algorithms.htm  
   float XA0 = 373.15/(273.15 + inArray[7]);
   float SUM = -7.90298 * (XA0-1);
   SUM += 5.02808 * log10(XA0);
   SUM += -1.3816e-7 * (pow(10, (11.344*(1-1/XA0)))-1);
   SUM += 8.1328e-3 * (pow(10,(-3.49149*(XA0-1)))-1);
   SUM += log10(1013.246);
   float VP = pow(10, SUM-3) * inArray[6];
   float T = log(VP/0.61078);   // temp var
   return (241.88 * T) / (17.558-T);
}

float dewPointFast() {
   // delta max = 0.6544 wrt dewPoint()
   // 5x faster than dewPoint()
   // reference: http://en.wikipedia.org/wiki/Dew_point   
   float a = 17.271;
   float b = 237.7;
   float temp = (a * inArray[7]) / (b + inArray[7]) + log(inArray[6]/100);
   float Td = (b * temp) / (a - temp); 
   return Td;
}

float cloudH() {  
   float CloudHigh = 0; 
   /* Heights of the bases of cumulus clouds.
      Calculation of the base height (CloudHigh in meters) of a cloud having parameters as a surface temperature and the dew point. */
   CloudHigh = 125 *(inArray[7] - dewPoint());

   return CloudHigh;
}

float cloudT() {  
   float CloudTemp = 0; 
   /* This calculator is based on the assumption that the air temperature drops 9.84 degrees C per 1000 m 
      of altitude and the dewpoint drops 1.82 degrees C per 1000 meters altitude. The following formulas must 
      be solved to estimate cloud height and temperature: */ 
   CloudTemp = (-0.00182 * cloudH()) + dewPoint();
   
   return CloudTemp;
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
  
   float sqe1 = atan(0.151977 * sqrt(inArray[6] + 8.313659)); 
   float sqe2 = atan(inArray[7] + inArray[6]) - atan(inArray[6] - 1.676331);
   float sqe3 = sqrt(pow(inArray[6], 3)) * atan(0.023101 * inArray[6]);
   
   float Twb = inArray[7] * sqe1 + sqe2 + 0.00391838 * sqe3 - 4.686035; 
   
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
      myGLCD.print("CAUTION - Fatigue is       ", 100, 60);
      myGLCD.print("possible with prolonged    ", 100, 70);
      myGLCD.print("exposure and activity.     ", 100, 80);
      myGLCD.print("Continuing activity could  ", 100, 90);
      myGLCD.print("result in heat cramps.     ", 100, 100); 
  }
  if ((rv >= 90 ) && (rv < 105))
  {
      myGLCD.print("EXTREME CAUTION - Heat     ", 100, 60);
      myGLCD.print("cramps and heat exhaustion ", 100, 70);
      myGLCD.print("are possible. Continuing   ", 100, 80);
      myGLCD.print("activity could result in   ", 100, 90);
      myGLCD.print("heat stroke.               ", 100, 100);     
  }    
  if ((rv >= 105 ) && (rv < 130))
  {
      myGLCD.print("DANGER - Heat cramps and   ", 100, 60);
      myGLCD.print("heat exhaustion are        ", 100, 70);
      myGLCD.print("likely; heat stroke is     ", 100, 80);
      myGLCD.print("probable with continued    ", 100, 90);
      myGLCD.print("activity.                  ", 100, 100);
  } 
  if (rv >= 130 ) 
  {
      myGLCD.print("EXTREME DANGER - heat      ", 100, 60);
      myGLCD.print("stroke is imminent.        ", 100, 70);
      myGLCD.print("                           ", 100, 80);
      myGLCD.print("                           ", 100, 90);
      myGLCD.print("                           ", 100, 100);
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
      myGLCD.print("SLIGHTLY COOL - Most of the", 100, 145);
      myGLCD.print("people are at ease, even if", 100, 155);
      myGLCD.print("it is slightly cool.       ", 100, 165);    
      myGLCD.print("                           ", 100, 175);
      myGLCD.print("                           ", 100, 185);
      myGLCD.print("                           ", 100, 195); 
  }
  if ((SSIndex >= 77 ) && (SSIndex < 83))
  {
      myGLCD.print("CONFORTABLE - Almost all   ", 100, 145);
      myGLCD.print("are in comfortable         ", 100, 155);
      myGLCD.print("conditions.                ", 100, 165);    
      myGLCD.print("                           ", 100, 175);
      myGLCD.print("                           ", 100, 185);
      myGLCD.print("                           ", 100, 195);     
  }    
  if ((SSIndex >= 83 ) && (SSIndex < 91))
  {
      myGLCD.print("SLIGHTLY WARM - Most people", 100, 145);
      myGLCD.print("are at ease, even if it is ", 100, 155);
      myGLCD.print("slightly warm.             ", 100, 165);    
      myGLCD.print("                           ", 100, 175);
      myGLCD.print("                           ", 100, 185);
      myGLCD.print("                           ", 100, 195);    
  } 
  if ((SSIndex >= 91 ) && (SSIndex < 100))
  {
      myGLCD.print("WARM - There is an increase", 100, 145);
      myGLCD.print("in discomfort.             ", 100, 155);
      myGLCD.print("                           ", 100, 165);    
      myGLCD.print("                           ", 100, 175);
      myGLCD.print("                           ", 100, 185);
      myGLCD.print("                           ", 100, 195); 
  }       
  if ((SSIndex >= 100 ) && (SSIndex < 112))
  {
      myGLCD.print("AVERAGE WARM - Significant ", 100, 145);
      myGLCD.print("discomfort. There is a     ", 100, 155);
      myGLCD.print("danger of sunstroke and    ", 100, 165);    
      myGLCD.print("heat exhaustion from       ", 100, 175);
      myGLCD.print("prolonged sun exposure and ", 100, 185);
      myGLCD.print("/ or physical activity.    ", 100, 195); 
  }  
  if ((SSIndex >= 112 ) && (SSIndex < 125))
  {
      myGLCD.print("VERY WARM - Uncomfortable  ", 100, 145);
      myGLCD.print("high.                      ", 100, 155);
      myGLCD.print("All are uncomfortable.     ", 100, 165);    
      myGLCD.print("There is a danger of heat  ", 100, 175);
      myGLCD.print("stroke.                    ", 100, 185);
      myGLCD.print("                           ", 100, 195); 
  }  
  if ((SSIndex >= 125 ) && (SSIndex < 150))
  {
      myGLCD.print("EXTREMELY WARM - Maximum   ", 100, 145);
      myGLCD.print("discomfort. High risk of   ", 100, 155);
      myGLCD.print("heat stroke, especially for", 100, 165);    
      myGLCD.print("the most vulnerable people,", 100, 175);
      myGLCD.print("the elderly and even       ", 100, 185);
      myGLCD.print("younger children.          ", 100, 195); 
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
  myGLCD.print(MsgSIWS(Fsiw), 100, 60);

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
 
  // message composition for Scharlau Index Summer
  myGLCD.print(MsgSIWS(Fsis), 100, 145);

  return Fsis;
}

String MsgSIWS(float siws)
{
  // table lists the thresholds for both climate discomfort linked 
  // to the value of the Scharlau Index.
  String SIWSinfo = " ";

  if (siws <= -3 )
  {
     SIWSinfo = "INTENSE DISCOMFORT";
  } 
  if (siws >= 0 ) 
  {
     SIWSinfo = "WELFARE";
  }        
  if ((siws < 0) && (siws > -1))
  {
     SIWSinfo = "WEAK DISCOMFORT";
  }
  if ((siws <= -1 ) && (siws > -3))
  {
     SIWSinfo = "MODERATE DISCOMFORT";
  }    

  return SIWSinfo;
}

void DispWait()
{  
  myGLCD.setFont(hallfetica_normal);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK); 
  myGLCD.print("WAIT . . .", 110, 114);
  NDisplayW = 1;
}

void CleanWait()
{
  myGLCD.setFont(hallfetica_normal);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK); 
  myGLCD.print("           ", 110, 114);
  NDisplayW = 0;
}
