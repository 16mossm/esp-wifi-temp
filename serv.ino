
#include <string>

#include <ESP8266WiFi.h>
#include <Servo.h>

#include "DHT.h"

#define servopin 4
#define Thermom 5
#define ChannelAmmount 3
#define buttonPin 2

DHT dht(Thermom, DHT11);
Servo Myservo;
WiFiServer server(80);

const int LEDs[] = {13,12,14};
bool ButtonPushed = false;

const char* ssid = "ForkliftEspWIF";
const char* password = "password";

int Channel = 0;
int Channel0 = -1;
int Channel1 = -1;
int DisplayingTemp = -1;

void setup() {
  Serial.begin(9600);
  
  pinMode(servopin,OUTPUT);
  pinMode(Thermom,INPUT);
  pinMode(buttonPin,INPUT_PULLUP);
  for(auto i : LEDs){
      pinMode(i,OUTPUT);
  }

  Myservo.attach(servopin, 500, 2500);
  
  dht.begin();

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid,password);
  server.begin();
  Serial.println(WiFi.softAPIP());
}



int TempToAngle(float temp){
  return min(max(180 - (int)(30 + (temp * 3) ) , 0), 180); 
}

void TempStuff(){
  float t = dht.readTemperature();  //fix this :) // cos it can be invalid
  float h = dht.readHumidity();       //same
  
  //Serial.print(F("%  angle: "));
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));   
  }
  else{Channel0 = dht.computeHeatIndex(t, h, false);}

  if(!digitalRead(buttonPin) && !ButtonPushed){
    Channel++;
    if(Channel>=ChannelAmmount){
        Channel = 0;
      }
    
  }
  ButtonPushed = !digitalRead(buttonPin);

  if(Channel == 0){
  DisplayingTemp = Channel0;
  }
  if(Channel == 1){
    DisplayingTemp = Channel1;
  }

  
  Myservo.write(TempToAngle(DisplayingTemp));
  //Serial.println(TempToAngle(dht.computeHeatIndex(t, h, false)));

  for(int i = 0;i<ChannelAmmount;i++){
      digitalWrite(LEDs[i],(i==Channel)?HIGH:LOW);
    }
  
  
  }




void loop(){
  std::string recived = "";
  WiFiClient c = server.available();
  while(c.connected()){
    if (c.available()) {    //add break when funky char appear;
        signed char a = c.read();
        if((char)a == '#'){recived = "";}
        recived += (char)a;
        
    }
    else{
      if(recived != ""){
          Serial.println(recived.c_str());
          if(recived[0]=='#' && recived[recived.length() -1 ] == ';'){
            recived = recived.substr(1,recived.length() - 2);
            size_t colonIndex = recived.find(':');

            if(colonIndex != std::string::npos){
                std::string MyChanel = (recived.substr(0,colonIndex));
                std::string Mytemp = (recived.substr(colonIndex + 1));


                Serial.println(MyChanel.c_str());

                if(MyChanel=="1" && isDigit(Mytemp[0]){
                    Channel1 = std::stoi(Mytemp);
                }
                
                Serial.println(Mytemp.c_str());
              }
            
            }
          
        }
      TempStuff();
      recived = "";
      }
  }

  

  TempStuff();
  
  delay(10);
}
