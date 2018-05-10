// This #include statement was automatically added by the Particle IDE.
#include <OneWire.h>

// This #include statement was automatically added by the Particle IDE.


// This #include statement was automatically added by the Particle IDE.
#include "spark-dallas-temperature.h"
OneWire oneWire(D0);
DallasTemperature dallas(&oneWire);



int buzzer = D1;
int sound[] = {2908,2550,2550,1273,1551,1005,2054,1708};
int soundDuration[] = {4,4,8,4,8,4,4,4};

int lastTemp = 0;
int temperature[5] = {0,0,0,0,0};
int humidity[5] = {0,0,0,0,0};
int tempThreshold = 5;
int humidThreshold = 5;

void setup(){
    pinMode(buzzer, OUTPUT);
    //Only work up to 4 times.
    Particle.subscribe("hook-response/AttentionBreaker_Temp", handleTemp, MY_DEVICES);
   // Particle.subscribe("hook-response/AttentionBreaker_Humid", handleHumid, MY_DEVICES);
    Particle.subscribe("hook-response/AttentionBreaker_Buzzer", handleBuzzer, MY_DEVICES);
    Serial.begin(9600);
}
      
void handleTemp(const char *event, const char *data) {
    String val = String(data);
    int value = atoi(data);
    Serial.print(event);
    Serial.print(" data: ");
    if (value){
        Serial.println(value);
        cycle(temperature,value);
    }
    else{
        Serial.println("NULL");
    }
}


void handleHumid(const char *event, const char *data){
    String val = String(data);
    int value = atoi(data);
    cycle(humidity, value);
}

void handleBuzzer(const char *event , const char *data){
    // alarm the buzzer!;
    Serial.print(event);
    Serial.print(" data: ");
    if (data){
        Serial.println(String(data));
        soundBuzzer();
    }
    else{
        Serial.println("NULL");
    }
   
}

void soundBuzzer(){
     for(int thisNote = 0; thisNote<8; thisNote++){
        int noteDur = 1000/soundDuration[thisNote];
        tone(buzzer, sound[thisNote], noteDur);
        int pause = noteDur * 1.20;
        delay(pause);
        noTone(buzzer);
    }
}
          
void shouldAlarm(){
    bool trigger = false;
    int tempAvg4 = findAverage4(temperature);
    int humidAvg4 = findAverage4(humidity);
    if(lastTemp >= tempThreshold + tempAvg4 ) {
        trigger = true;
    }
    if(lastTemp <= tempAvg4 - tempThreshold ) {
        trigger = true;
    }
    if(humidity[4] >= humidThreshold + humidAvg4){
        trigger = true;
    }
    if(trigger){
        
        String data = "ALARM: tempAvg4= " +  String(tempAvg4) + "/last=" + String(lastTemp);
        Particle.publish("AttentionBreaker_Buzzer", data, PRIVATE);
    
    }
}

int findAverage4(int array[]){
    float sum = 0;
    for(int i = 0; i < 4; i++){
        sum = sum + array[i];
    }
    int avg = (int) (sum/4);
    return avg;
}

void cycle(int array[], int data){
    int j = data;
    for(int i = 4; i >= 0; i--){
        int k = array[i];
        array[i] = j;
        j = k;
    }
}


int readTemperature(){
    dallas.requestTemperatures();
    float tempf = dallas.getTempFByIndex(0);
//    float tempf = DallasTemperature::toFahrenheit(tempc);
    return (int) tempf;
}

void loop() {
  // Get some data
  lastTemp = (int) readTemperature();
  // Trigger the integration
  Particle.publish("AttentionBreaker_Temp", String(lastTemp), PRIVATE);
  //Particle.publish("AttentionBreaker_Humid", "0", PRIVATE);
  shouldAlarm();
  
  // Wait 7 seconds
  delay(7000);
}
      
