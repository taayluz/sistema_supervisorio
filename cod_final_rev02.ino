#ifdef ESP8266
 #include <ESP8266WiFi.h>
#else //ESP32
 #include <WiFi.h>
#endif
#include <ModbusIP_ESP8266.h>
#include "DHT.h"

#define DHTPIN 5 //GPIO5     // what digital pin the DHT22 is conected to
#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors
#define pinSensorD 16

//Modbus Registers Offsets
const int SENSOR_IREG = 100; //umidade do solo
const int SENSOR2_IREG = 101; //temperatura
const int SENSOR3_IREG = 102; //umidade relativa do ar
const int SENSOR4_IREG = 103; //LDR
const int LED_COIL1 = 100; //lampada incandescente
const int LED_COIL2 = 101; // valvula de retencao
const int LED_COIL3 = 107;// bomba
const int LED_COIL4 = 103; //exaustor 
const int LED_COIL5 = 104; //led red
const int LED_COIL6 = 105; // led blue
const int LED_COIL7 = 106; // medidor de nivel

const int ledPin1 = 13; //GPIO13
const int ledPin2 = 15; //GPIO15
const int ledPin3 = 2; //GPIO2
const int ledPin4 = 12; //GPIO12
const int ledPin5 = 14; //GPIO14
const int ledPin6 = 4; //GPIO4
const int ledPin7 = 0; //GPIO0
//const int pinSensorD = 16; //GPIO16

//int UMIDADE = 0;
int LDR = 0;

DHT dht(DHTPIN, DHTTYPE);

//ModbusIP object
ModbusIP mb;

long ts;

void setup() {
    Serial.begin(115200);
 
    WiFi.begin("Taynara_EXT", "willa2809");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    mb.server();		//Start Modbus IP
     pinMode(ledPin1, OUTPUT);
     pinMode(ledPin2, OUTPUT);
     pinMode(ledPin3, OUTPUT);
     pinMode(ledPin4, OUTPUT);
     pinMode(ledPin5, OUTPUT);
     pinMode(ledPin6, OUTPUT);
     pinMode(ledPin7, OUTPUT);
     pinMode(pinSensorD, INPUT);

    dht.begin();   

    // Add SENSOR_IREG register - Use addIreg() for analog Inputs
    mb.addIreg(SENSOR_IREG);
    mb.addIreg(SENSOR2_IREG);
    mb.addIreg(SENSOR3_IREG);
    mb.addIreg(SENSOR4_IREG);
  
    mb.addCoil(LED_COIL1);
    mb.addCoil(LED_COIL2);    
    mb.addCoil(LED_COIL3);
    mb.addCoil(LED_COIL4);
    mb.addCoil(LED_COIL5);
    mb.addCoil(LED_COIL6);
    mb.addCoil(LED_COIL7);

    ts = millis();
}

void loop() {
   //Call once inside loop() - all magic here
   mb.task(); 

   digitalWrite(ledPin7, mb.Coil(LED_COIL7));    

    float t = dht.readTemperature();
    float h = dht.readHumidity();
  
    LDR = analogRead(A0);    
    //UMIDADE = map(ledPin1, 0, 1024, 100, 0);
    //UMIDADE = digitalRead(13);

  //Setting raw value (0-1024)
       //mb.Ireg(SENSOR_IREG, UMIDADE);             
       mb.Ireg(SENSOR2_IREG, t); //temperatura
       mb.Ireg(SENSOR3_IREG, h); //umidade relativa do ar
       mb.Ireg(SENSOR4_IREG, LDR);

       //mb.Coil(LED_COIL1, digitalRead(ledPin1));
       mb.Coil(LED_COIL2, digitalRead(ledPin2));
       mb.Coil(LED_COIL3, digitalRead(ledPin3));
       mb.Coil(LED_COIL4, digitalRead(ledPin4));
       mb.Coil(LED_COIL5, digitalRead(ledPin5));
       mb.Coil(LED_COIL6, digitalRead(ledPin6)); 
       mb.Coil(LED_COIL7, digitalRead(ledPin7));  

       mb.Coil(SENSOR_IREG, digitalRead(pinSensorD));  //umidade do solo                            

    if (digitalRead(ledPin7) == HIGH) 
       {
         Serial.println("Contém água no reservatório");
         digitalWrite(ledPin3, LOW); // bomba nao acionada
         delay (2000); 
       }    
    else {
          Serial.println("Bomba acionada, NIVEL BAIXO");
          digitalWrite(ledPin3, HIGH); //led bomba acionada
          delay (2000);
          }                  
  
    if (digitalRead(pinSensorD) == true && h <= 60 && digitalRead(ledPin7) == HIGH){
          Serial.println("Valvula de retencao LIBERADA");
          digitalWrite(ledPin2, HIGH);  //aciona led de valv de ret
          delay (2000);    
          }      
     else  {
       digitalWrite(ledPin2, LOW);  //n aciona led de valv de ret
       delay (200);
     }  
    
    if(analogRead(A0) > 600){ //SE O VALOR LIDO FOR MAIOR QUE 600, FAZ
     Serial.println("NOITE"); 
     digitalWrite(ledPin5, HIGH);
     digitalWrite(ledPin6, HIGH);
  }  
    else{ //SENÃO, FAZ
    Serial.println("DIA"); 
    digitalWrite(ledPin5, LOW);
    digitalWrite(ledPin6, LOW); 
    delay (200);
  } 

  if (h >= 60 && analogRead(A0) < 600){
          Serial.println("Exaustores acionados");
          digitalWrite(ledPin4, HIGH);  //aciona led exaustor
          delay (200);
          }      
     else  {
       digitalWrite(ledPin4, LOW);  //n aciona led exaustor
       
     }   

   if(t <= 18 && analogRead(A0) >= 600){
     Serial.println("Lampada incandescente acionada"); 
     digitalWrite(ledPin1, HIGH);
  }  
  else{ //SENÃO, FAZ 
  digitalWrite(ledPin1, LOW);  
    delay (200);
      }       
  
   //Read each two seconds
   if (millis() > ts + 2000) {
       ts = millis();     

         if (digitalRead(pinSensorD) == true){
         Serial.println("SEM UMIDADE");  
         }
         else {
        Serial.println("COM UMIDADE");  
         } 
    
        Serial.println("Temperatura: ");
        Serial.print(t);
        Serial.println(" *C ");
        Serial.println("Umidade relativa do ar: ");
        Serial.print(h);
        Serial.println(" % ");
        Serial.print("Valor lido pelo LDR = ");//Imprime na serial a mensagem Valor lido pelo LDR
        Serial.println(LDR);
        
   }
   delay(100);
}
