#define HumPin 11

uint32_t wetting_time = 5000;
long wetting_timer = 0;
long timer = 0;
uint32_t wetting_delay = 10000;

bool wetting_state = false;




void setup() {
  Serial.begin(9600);
  pinMode(HumPin, OUTPUT);
  pinMode(13, OUTPUT);
}

void bttn_click(){
    digitalWrite(HumPin, HIGH);
    delay(200);
    digitalWrite(HumPin, LOW);
}


void wetting_on(){

    wetting_state = true;
    bttn_click();
    wetting_timer = millis();
    Serial.println("Wetting On");
    digitalWrite(13, wetting_state);

}

void wetting_off(){
    wetting_state = false;
    bttn_click();
    Serial.println("Wetting Off");
    digitalWrite(13, wetting_state);
}

void blink(){
  static long timer = 0;
  static int delay_blink = 500;
  static bool led = false;
  if (millis() - timer >= delay_blink){
    timer = millis();
    led = !led;
    
  }
}



void loop() {

  if(millis() - timer >= wetting_delay)
  {
    wetting_on();
    timer = millis();
  }
  if(wetting_state && millis() - wetting_timer >= wetting_time){
    wetting_off();
    timer = millis();
  }
  //blink();
}
