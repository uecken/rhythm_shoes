#include <M5StickCPlus.h>
#include <BleKeyboard.h>
BleKeyboard bleKeyboard;

#define SAMPLE_SIZE 4000
#define SAMPLE_PERIOD 30
float ax[SAMPLE_SIZE], ay[SAMPLE_SIZE], az[SAMPLE_SIZE];
float gx[SAMPLE_SIZE], gy[SAMPLE_SIZE], gz[SAMPLE_SIZE];
long i = 0;

boolean ACC_DEBUG = true;
boolean JUDGE_DEBUG = true;

void setup() {
  M5.begin();
  Serial.begin(115200);
  M5.IMU.Init();
  M5.IMU.SetAccelFsr(M5.IMU.AFS_16G);
  Serial.println("M5StickC started.");
  M5.Lcd.print("M5StickC started.");
  bleKeyboard.begin();
}

boolean eventDetection(String event ="acc_abs_event"){
  M5.IMU.getAccelData(&ax[i],&ay[i],&az[i]);
  if(ACC_DEBUG)Serial.printf("%4.2f %4.2f %4.2f \n",ax[i],ay[i],az[i]);
  if(event = "acc_abs_event"){
    if(i != 0){
      if(abs(az[i] - az[i-1]) > 3){
        if(ACC_DEBUG)Serial.printf("ACC Event Detected. ACCz-diff is %4.2f \n",az[i] - az[i-1]);
        return true;
      }
    }else if(i == SAMPLE_SIZE){
      if(abs(az[0] - az[SAMPLE_SIZE])>5000){
        if(JUDGE_DEBUG)Serial.printf("%4,2f",az[i] - az[i-1]);
        return true;
      }
    }
  }else if(event == "grounding_event"){
  }else if(event == "pressure_event" ){   
  }
  return false;
}

void eventAction(String actionType="print",String key="e"){
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
    previousTime = millis();
    if(bleKeyboard.isConnected()){  
      delay(20);
      if(eventDetection("acc_abs_event")){
        eventAction("write");
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
        eventAction("write","e");
        BtnBisPressed = true;
      }
      else if(M5.BtnB.wasReleased()){
        Serial.println("BtnB was released");
        eventClear();
      }



      M5.update();
    } 
    i++;
    if(i == (SAMPLE_SIZE-1)) i=0;
  }
}
