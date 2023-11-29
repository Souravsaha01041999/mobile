#include<SoftwareSerial.h>
#include<potentiometer_numberpad.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define RX 3
#define TX 4
#define NUMBER_PIN A0
#define NUMBER_WAIT 200
#define CALL_EVENT 8
#define LED 12
#define RCV_CALL 9

//0 no event, 1 calling, 2 wait for cut call,3 for cut call
int eventType=0;
int numberValue=0;
SoftwareSerial dev(RX,TX);
String callNumber="";
boolean ledState=false;
Adafruit_SSD1306 disp(128,64,&Wire,4);

void updateSerial()
{
  delay(500);
  while (Serial.available())
  {
    dev.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(dev.available())
  {
    Serial.write(dev.read());//Forward what Software Serial received to Serial Port
  }
}

void toggleLed()
{
   ledState=!ledState;
   digitalWrite(LED,ledState);
}

void setDisplay(String data)
{
  disp.clearDisplay();
  disp.setCursor(1,25);
  disp.setTextColor(WHITE);
  disp.setTextSize(2);
  disp.print(data);
  disp.display();
}

void setDisplay(int data)
{
  disp.clearDisplay();
  disp.setCursor(1,25);
  disp.setTextColor(WHITE);
  disp.setTextSize(2);
  disp.print(data);
  disp.display();
}

void setup()
{
   dev.begin(9600);
   pinMode(CALL_EVENT,INPUT);
   pinMode(LED,OUTPUT);
   pinMode(RCV_CALL,INPUT);

   disp.begin(SSD1306_SWITCHCAPVCC, 0x3C);
   delay(100);
   disp.clearDisplay();
   disp.setCursor(1,25);
   disp.setTextColor(WHITE);
   disp.setTextSize(1);
   disp.print("Mobile");
   disp.display();
   Serial.begin(9600);
}
void loop()
{
   //Key Event Section
   if(digitalRead(CALL_EVENT))
   {
      eventType++;
      delay(1000);
      if(eventType==1)
      {
        for(int i=1;i<=10;i++)
        {
           int j=0;
           int value=0;
           while(j<=20)
           {
              value=getValue(NUMBER_PIN);
              //display number here
              setDisplay(value);
              j++;
              delay(200);
           }
           toggleLed();
           callNumber=callNumber+(char)(48+value);
           delay(NUMBER_WAIT);
           toggleLed();
        }
      }
   }
   if(digitalRead(RCV_CALL))
   {
      eventType=4;
   }

   //Action Event Section
   switch(eventType)
   {
      case 1:
                  toggleLed();
                  dev.println("AT");
                  updateSerial();
                  dev.println("ATD+ +91 "+callNumber+";");
                  updateSerial();
//                  delay(20000); // wait for 20 seconds...
//                  dev.println("ATH");
                  updateSerial();
                  
                  eventType++;
                 //display "calling"
                 setDisplay("Calling");
                 delay(1000);
                 setDisplay(callNumber);
                 callNumber="";
                  break;
      case 3:
                 toggleLed();
                 dev.println("ATH");
                 //display call cut
                 setDisplay("Call Cut");
                 delay(2000);
                 eventType=0;
                 //display blank
                 setDisplay("");
                 break;
      case 4:
                toggleLed();
                setDisplay("Calling");
                eventType=2;
                break;
   }
   delay(10);
}
