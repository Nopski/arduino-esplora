#include <Esplora.h>
#include <TFT.h>            // Arduino LCD library
#include <SPI.h>

#define sclk 15
#define mosi 16
#define cs   7
#define dc   0
#define rst  1

int buttonState = 0;

/*//this is example lightcalibrator
int lightMin = 1023;        // minimum sensor value
int lightMax = 0;           // maximum sensor value
boolean calibrated = false;  // whether the sensor's been calibrated yet
//ended example lightcalibrator*/


const int MENU_STATE = 0;
const int GAME_STATE = 1;
const int HELP_STATE = 2;
const int CREDITS_STATE = 3;
const int SETTINGS_STATE = 4;
const int TRNBLTON_STATE = 5;
const int TRNBLTOFF_STATE = 6;
const int CHANGEBCK_STATE = 7;
const int RED_BACKGROUND = 255;
const int GREEN_BACKGROUND = 255;
const int BLUE_BACKGROUND = 255;
const int MENU_OFFSET = 5;
const int SETTINGS_OFFSET = 6;
char* MENU[] = {"GAME", "HELP", "INFORMATIONS", "SETTINGS"};
char* SETTINGS[] = {"TRNBLTON", "TRNBLTOFF", "CHANGEBCK"};

#define MENU_LENGTH (sizeof(MENU)/sizeof(char*))
#define SETTINGS_LENGTH (sizeof(SETTINGS)/sizeof(char*))

const int BLINK_DELAY = 4000;
int currentBlinkCount = 0;
boolean blinkOver = true;

TFT screen = TFT(cs, dc, rst);
int width, height;

int state = 0;
//int state1 = 0
int menuItemSelected = 1;
int menuItemSelected1 = 4;

boolean redraw = true;

void setup() {

  Serial.begin(9600);

  // initialize the display
  screen.begin();
  width = screen.width();
  height = screen.height();
  // set the background the black
  screen.background(255,255,255);
}

int lowLight = 400;   // the light sensor reading when it's covered
int highLight = 1023; // the maximum light sensor reading
int minGreen = 0;     // minimum brightness of the green LED
int maxGreen = 100;   // maximum brightness of the green LED

void drawMenu(){
  int textHeight;
  
  int stroke;
  int posY = 24;
  int textSize;
  boolean background;
  int fill;  
  
  for(int i = 1; i < MENU_LENGTH +1; i++){
    if(i == menuItemSelected){
      stroke = 255;
      textSize = 3;
      fill = 30;
      background = true;
      
      textHeight = 30;
    } else {
      textSize = 2;
      stroke = 0;
      fill = 255;
      textHeight = 20;
      background = false;
    }

    drawText(MENU[i -1], 10, posY, stroke, stroke, stroke, textSize, background, fill, fill, fill, width, 29, 0, posY - 4, false);
    posY += (textHeight + MENU_OFFSET);
  }
  for(int f = 1; f < SETTINGS_LENGTH +1; f++){
    if(f == menuItemSelected1){
      stroke = 255;
      textSize = 3;
      fill = 30;
      background = true;
      
      textHeight = 30;
    } else {
      textSize = 2;
      stroke = 0;
      fill = 255;
      textHeight = 20;
      background = false;
    }
    drawText(SETTINGS[f -1], 10, posY, stroke, stroke, stroke, textSize, background, fill, fill, fill, width, 29, 0, posY - 4, false);
    posY += (textHeight + SETTINGS_OFFSET);
    }
}

void drawBackBtn(boolean over){
  int stroke;
  int fill;
  if(over){
    stroke = 255;
    fill = 30;
  } else {
    stroke = 0;
    fill = 0;
  }
  drawText("BACK", 130, 115, stroke, stroke, stroke, 1, true, fill, fill, fill, 31, 15, 126, 111, !over);
}

void drawTrnbltBtn(boolean over){
  int stroke;
  int fill;
  if(over){
    stroke = 255;
    fill = 30;
  } else {
    stroke = 0;
    fill = 0;
  }
  drawText("NEXT", 130, 115, stroke, stroke, stroke, 1, true, fill, fill, fill, 31, 15, -126, -111, !over);
}

void drawGame(){
  initGame();
}

void drawHelp(){
  drawTitle("HELP", 50);
  drawBackBtn(blinkOver);
  
  drawText("Use the bottom slider", 5, 35, 45, 45, 45, 1, false, 0, 0, 0, 0, 0, 0, 0, false);
  drawText("to navigate in the menus.", 5, 47, 45, 45, 45, 1, false, 0, 0, 0, 0, 0, 0, 0, false);
  drawText("SWITCH1: Enter", 5, 59, 45, 45, 45, 1, false, 0, 0, 0, 0, 0, 0, 0, false);
  drawText("SWITCH3: Back", 5, 71, 45, 45, 45, 1, false, 0, 0, 0, 0, 0, 0, 0, false);
  drawText("Use the left joystick", 5, 83, 45, 45, 45, 1, false, 0, 0, 0, 0, 0, 0, 0, false);
  drawText("to controle the racket", 5, 95, 45, 45, 45, 1, false, 0, 0, 0, 0, 0, 0, 0, false);
  drawText("in game.", 5, 107, 45, 45, 45, 1, false, 0, 0, 0, 0, 0, 0, 0, false);
}

void drawCredits(){
  drawTitle("INFORMATIONS", 18);
  drawBackBtn(blinkOver);
  
  drawText("Created Nunky L@|>0,", 18, 50, 45, 45, 205, 1, false, 0, 0, 0, 0, 0, 0, 0, false);
  drawText("using Arduino Esplora", 16, 68, 45, 45, 45, 1, false, 0, 0, 0, 0, 0, 0, 0, false);
  drawText("Date 22/01/2016.", 35, 86, 45, 45, 45, 1, false, 0, 0, 0, 0, 0, 0, 0, false);
}

void drawSettings(){
  drawTitle("SETTINGS", 18);
  drawBackBtn(blinkOver);
  drawTrnbltBtn(blinkOver);

  if(state == SETTINGS_STATE){
    int tempSelected = map(Esplora.readSlider(), 1023, 0, SETTINGS_LENGTH, 1);
    
    if(tempSelected != menuItemSelected1){
      menuItemSelected1 = tempSelected;
      redraw = true;
    } else if(Esplora.readButton(SWITCH_DOWN) == LOW){
      state = menuItemSelected1;
      redraw = true;
    } else if(Esplora.readButton(SWITCH_RIGHT) == LOW){
      //state = menuItemSelected;
      redraw = true;
      state = SETTINGS_STATE; 
    }
  } else if(Esplora.readButton(SWITCH_UP) == LOW){
    redraw = true;
    state = SETTINGS_STATE; 
  } else if((state == TRNBLTON_STATE || state == TRNBLTOFF_STATE || state == CHANGEBCK_STATE)){
    currentBlinkCount ++;
    if(currentBlinkCount == BLINK_DELAY){
      blinkOver = !blinkOver;
      currentBlinkCount = 0;
      drawBackBtn(blinkOver);
  }

   if(state == SETTINGS_STATE){
      drawMenu();
    } /*else if(state == GAME_STATE){
      drawGame();
    } else if(state == HELP_STATE){
      currentBlinkCount = 0;
      drawHelp();
    } else if(state == CREDITS_STATE){
      currentBlinkCount = 0;
      drawCredits();
    } else if(state == SETTINGS_STATE){
      currentBlinkCount = 0;
      drawMenu();
    } if(state == TRNBLTON_STATE){
        drawTrnblton();
      }*/
      else if(state == TRNBLTOFF_STATE){
        drawTrnbltoff();
      }
      else if(state == CHANGEBCK_STATE){
        drawChangebck();
      }
  /*drawText("Turn bluetooth on,", 18, 50, 45, 45, 205, 1, false, 0, 0, 0, 0, 0, 0, 0, false);
  drawText("Turn bluetooth off", 16, 68, 45, 45, 45, 1, false, 0, 0, 0, 0, 0, 0, 0, false);
  drawText("Change background.", 35, 86, 45, 45, 45, 1, false, 0, 0, 0, 0, 0, 0, 0, false);*/

  /*for(int f = 1; f < SETTINGS_LENGTH +1; f++){
    if(f == menuItemSelected1){
      stroke = 255;
      textSize = 3;
      fill = 30;
      background = true;
      
      textHeight = 30;
    } else {
      textSize = 2;
      stroke = 0;
      fill = 255;
      textHeight = 20;
      background = false;
    }
    drawText(SETTINGS[f -1], 10, posY, stroke, stroke, stroke, textSize, background, fill, fill, fill, width, 29, 0, posY - 4, false);
    posY += (textHeight + SETTINGS_OFFSET);
  }*/
}
}

void drawTrnblton(){
  drawTitle("TRNBLTON", 18);
  drawBackBtn(blinkOver);

  drawText("Turn bluetooth on,", 18, 50, 45, 45, 205, 1, false, 0, 0, 0, 0, 0, 0, 0, false);
}

void drawTrnbltoff(){
  drawTitle("TRNBLTOFF", 18);
  drawBackBtn(blinkOver);

  drawText("Turn bluetooth off,", 18, 50, 45, 45, 205, 1, false, 0, 0, 0, 0, 0, 0, 0, false);
}

void drawChangebck(){
  drawTitle("CHANGEBCK", 18);
  drawBackBtn(blinkOver);

  drawText("Change background.", 35, 86, 45, 45, 45, 1, false, 0, 0, 0, 0, 0, 0, 0, false);
}

void loop() {
/*//example lightcalibrator
// if switch 1 is pressed, go to the calibration function again:
  if (Esplora.readButton(1) == LOW) {
    calibrate();
  }
  // read the sensor into a variable:
  int light = Esplora.readLightSensor();

  // map the light level to a brightness level for the LED
  // using the calibration min and max:
  int brightness = map(light, lightMin, lightMax, 0, 255);
  // limit the brightness to a range from 0 to 255:
  brightness = constrain(brightness, 0, 255);
  // write the brightness to the blue LED.
  Esplora.writeBlue(brightness);

  // if the calibration's been done, show the sensor and brightness
  // levels in the serial monitor:
  if (calibrated == true) {
    // print the light sensor levels and the LED levels (to see what's going on):
    Serial.print("light sensor level: ");
    Serial.print(light);
    Serial.print(" blue brightness: ");
    Serial.println(brightness);
  }
  // add a delay to keep the LED from flickering:
  delay(10);
//this example ended*/

  // read the state of the pushbutton value:
  // Esplora.readButton replaces digitalRead()
  buttonState = Esplora.readButton(SWITCH_2);
 
  // check if the pushbutton is pressed.
  // if it is, the buttonState is LOW:
  if (buttonState == LOW) {     // Button is pushed when LOW, not HIGH
    // turn LED on:    
    Esplora.writeRed(255);  // Esplora.writeRed() replaces digitalWrite()
  }
  else {
    // turn LED off:
    Esplora.writeRed(0);  // Esplora.writeRed() replaces digitalWrite()
  }


  
  if(state == MENU_STATE){
    int tempSelected = map(Esplora.readSlider(), 0, 1023, MENU_LENGTH, 1);
    
   if(tempSelected != menuItemSelected){
      menuItemSelected = tempSelected;
      redraw = true;
    } else if(Esplora.readButton(SWITCH_DOWN) == LOW){
      state = menuItemSelected;
      redraw = true;
    } else if(state == SETTINGS_STATE){
      /*state = menuItemSelected1;
      redraw = true;
      state = SETTINGS_STATE;*/
      int tempSelected = map(Esplora.readSlider(), 1023, 0, SETTINGS_LENGTH, 1);
    
   if(tempSelected != menuItemSelected1){
      menuItemSelected1 = tempSelected;
      redraw = true;
    }
    }
  } else if(Esplora.readButton(SWITCH_UP) == LOW){
    redraw = true;
    state = MENU_STATE; 
  } else if((state == HELP_STATE || state == CREDITS_STATE || state == SETTINGS_STATE)){
    currentBlinkCount ++;
    if(currentBlinkCount == BLINK_DELAY){
      blinkOver = !blinkOver;
      currentBlinkCount = 0;
      drawBackBtn(blinkOver);
    } 
    }
  
  if(redraw){
    eraseScreen();

    
    if(state == MENU_STATE){
      drawMenu();
    } else if(state == GAME_STATE){
      drawGame();
    } else if(state == HELP_STATE){
      currentBlinkCount = 0;
      drawHelp();
    } else if(state == CREDITS_STATE){
      currentBlinkCount = 0;
      drawCredits();
    } else if(state == SETTINGS_STATE){
      currentBlinkCount = 0;
      drawMenu();
    } if(state == TRNBLTON_STATE){
        drawTrnblton();
      }
      else if(state == TRNBLTOFF_STATE){
        drawTrnbltoff();
      }
      else if(state == CHANGEBCK_STATE){
        drawChangebck();
      }
   // }
  } else if(state == GAME_STATE){
    update();
  } 
}

/*//started voidcalibrator
void calibrate() {
  // tell the user what do to using the serial monitor:
  Serial.println("While holding switch 1, shine a light on the light sensor, then cover it.");

  // calibrate while switch 1 is pressed:
  while (Esplora.readButton(1) == LOW) {
    // read the sensor value:
    int light  = Esplora.readLightSensor();

    // record the maximum sensor value:
    if (light > lightMax) {
      lightMax = light;
    }

    // record the minimum sensor value:
    if (light < lightMin) {
      lightMin = light;
    }
    // note that you're calibrated, for future reference:
    calibrated = true;
  }
}
//voidcalibrator ended*/
