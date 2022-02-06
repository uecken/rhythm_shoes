#include <M5StickCPlus.h>
#include <BleKeyboard.h>
BleKeyboard bleKeyboard;

#define SAMPLE_SIZE 4000
#define SAMPLE_PERIOD 10
float ax[SAMPLE_SIZE], ay[SAMPLE_SIZE], az[SAMPLE_SIZE];
float ax_prev =0,ay_prev=0,az_prev=0;
//float gx[SAMPLE_SIZE], gy[SAMPLE_SIZE], gz[SAMPLE_SIZE];

#define PIN_PURESSURE 26
#define PRESSED 0
uint16_t Vpressure;

long i = 0;

boolean ACC_DEBUG = false;
boolean PRESSURE_DEBUG = true;
boolean JUDGE_DEBUG = true;


void setup() {
  M5.begin();
  Serial.begin(115200);
  M5.IMU.Init();
  M5.IMU.SetAccelFsr(M5.IMU.AFS_16G);
  pinMode(PIN_PURESSURE,INPUT_PULLUP); // for puressure-sensor
  Serial.println("M5StickC started.");
  M5.Lcd.print("M5StickC started.");
  bleKeyboard.begin();
}

boolean eventDetector(char* sensor){
  boolean eventDetected = false;
  M5.IMU.getAccelData(&ax[i],&ay[i],&az[i]);
  if(ACC_DEBUG)Serial.printf("%4.2f %4.2f %4.2f \n",ax[i],ay[i],az[i]);
  if(strcmp(sensor,"accz_sensor")==0){
    float az_th = 3.5;
    if(i != 0){
      if((az[i] - az_prev) > az_th){
        if(ACC_DEBUG)Serial.printf("ACC-Z Event Detected. ACCz-diff is %4.2f \n",az[i] - az[i-1]);
        eventDetected =  true;
      }
    }
    /*
    else if(i == SAMPLE_SIZE){
      if((az[0] - az[SAMPLE_SIZE])>az_th){
        if(ACC_DEBUG)Serial.printf("ACC Event Detected. ACCz-diff is %4.2f \n",az[i] - az[i-1]);
        return true;
      }
    }
    */
  }else if(strcmp(sensor,"grounding")==0){
  }else if(strcmp(sensor,"pressure_sensor")==0){
    //Vpressure = analogRead(PIN_PURESSURE);
    Vpressure = digitalRead(PIN_PURESSURE);
    if(PRESSURE_DEBUG)Serial.printf("Pressure Event Detected. Vpressure is %d \n",Vpressure);
    if(Vpressure == PRESSED){  
      eventDetected =  true;
    }
  }

  ax_prev = ax[i]; ay_prev = ay[i]; az_prev = az[i];
  return eventDetected;
}

void eventAction(String actionType="print",String key="e"){
  Serial.printf("actionType:%s key:%s \n",actionType,key);
  if(actionType = "print"){
    bleKeyboard.print(key);  
  }else if(actionType = "write"){
  
  }else if(actionType = "press"){
    //bleKeyboard.write(KEY_RETURN);
    bleKeyboard.press(KEY_LEFT_CTRL);
    bleKeyboard.press(0x76);
  }
}

void eventClear(){
  bleKeyboard.releaseAll();
  
}

boolean BtnAisPressed;
boolean BtnBisPressed;
long previousTime = 0;
void loop() {
  delay(1);
  if((millis() - previousTime) > SAMPLE_PERIOD){
    M5.update();
    previousTime = millis();
    if(bleKeyboard.isConnected()){  
      /*
      if(eventDetector("accz_sensor")){
        eventAction("print");
        //複数回eventが実行されてしまう。
        //１．接地検出前後ではなく、接地時にevent実行される工夫を入れ、
        //２．接地前後では実行されないように、また連続実行されない処理とすること
      }
      */
      if(eventDetector("pressure_sensor")){
        //Serial.println("pressure_detected");
        //感圧センサイベント処理を入れること
        //ACCZセンサー同様に複数回検知させないこと。
        //eventAction("print");
        eventAction("press");
        delay(200);
        eventClear();
      }

    
      if(M5.BtnA.isPressed()){
        Serial.println("BtnA is pressed");
        eventAction("press");
        BtnAisPressed = true;
      }
      else if(M5.BtnA.wasReleased()){
        Serial.println("BtnA was released");
        eventClear();
      }



      //====bleKeyboard.print is best for Songle=====
      if(M5.BtnB.wasPressed()){
        Serial.println("BtnB is pressed");
        eventAction("print","e");
        BtnBisPressed = true;
      }
      else if(M5.BtnB.wasReleased()){
        Serial.println("BtnB was released");
        eventClear();
      }
    } 
    i++;
    if(i == (SAMPLE_SIZE-1)) i=0;
  }
}
