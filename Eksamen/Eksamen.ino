
// This #include statement was automatically added by the Particle IDE.
#include <HC-SR04.h>

// This #include statement was automatically added by the Particle IDE.
#include <Adafruit_DHT.h>

#define DHTTYPE DHT11
#define DHTPIN D0
#define TRIGGER A0

#define ECHO A1
#define buzzerOne D2
#define pauseSystemButton  D3
#define turnOnOffSystemButton D4



DHT dht(DHTPIN, DHTTYPE);

HC_SR04 moveSensor = HC_SR04(TRIGGER, ECHO);


unsigned long thisTime;
unsigned long lastTime;
bool isSystem = true; 


int lastPausebuttonPressed = 0;
int pauseButtonState = 0;
int turnOnOfSystemState = 0; 
int lastTurnOfOnSystemPressed = 0; 


void setup() {
    dht.begin();
    moveSensor.init();
    lastTime  = millis();
    pinMode(pauseSystemButton, INPUT); 
    pinMode(turnOnOffSystemButton, INPUT); 
    
    
}

void loop() {
    
    turnOnOfSystemState = digitalRead(turnOnOffSystemButton);
    
    if(isSystem){
        HomeWatcherSystem(); 
    }

    if(turnOnOfSystemState == HIGH && lastTurnOfOnSystemPressed == LOW){
        Particle.publish("BTN", "PRESSED"); 
        //Turning on and off the System based on it's current state. 
        if(isSystem != true){
            isSystem = true;
        }else{
            isSystem = false; 
        }
        
    }
    
    lastTurnOfOnSystemPressed = turnOnOfSystemState;
    
  
}


void HomeWatcherSystem(){
    
     thisTime = millis();
    
    pauseButtonState = digitalRead(pauseSystemButton); 
    
    if(thisTime - lastTime > 100000){
        String temp = String(dht.getTempCelcius());
        Particle.publish("Notifier", temp);
    }
    
    if(thisTime - lastTime > 2000){
        float cm[10];
        float avgCm; 
        
        for(int i = 0; i < 10; i++)
        {
            cm[i] =  moveSensor.distCM();
            avgCm += cm[i];
        }
        
        avgCm /= 10.0;
        
        if(avgCm < 20)
        {
          Particle.publish("Notifier", "Home-Watcher picked up on movement in your apartment!!");
          //soundTheAlarm();  
        }
        
        lastTime = millis();
    }
    
    if (pauseButtonState == HIGH && lastPausebuttonPressed == LOW ){
        Particle.publish("AlarmNotifier", "The alarm was now turned off for 60 seconds! HURRY UP leaving the Student complex");
       // PauseTheHomeWatcher();   
    }
    
    lastPausebuttonPressed = pauseButtonState; 
}


void soundTheAlarm(){
    tone(buzzerOne, 2000, 5000);
}


void PauseTheHomeWatcher(){
    
    int index = 0;
    
    while(index <= 30){
        
        tone(buzzerOne, 2000, 500); 
        delay(1000);
        index++; 
    }
}
