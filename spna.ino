/////////////////////////////////////////////////////////////////////////////////////////
// SPNA - Single Port Network Analyzer
// v0.3
// A first pass at a single port network/antenna analyzer
/////////////////////////////////////////////////////////////////////////////////////////
// Tim Czerwonka WO9U <tczerwonka@gmail.com>
// 2jan2017
//  Picking up on the project again -- mostly having to fix shit that used to compile
//  with older versions of the environment
// 23jul2016
//  adding a joystick (gimbal) because the touchscreen suuuucks
// 10jul2016 v0.3
//  just picking it up again having robbed my mega2560 for other projects, need to switch
//  to URtouch -- UTouch is apparently gone now -- no touchscreen anything working.
// 29mar2015
//  just decided to fix a decimal point and let the user enter numbers around it in
//  drawInputScreen
// 12Mar2015
//  a general re-start and clean-up now that I have a better idea how this should look
// 18Feb2015
//  added a 0-5V meter on pin9
// 19dec2014
//  change to v0.2 -- use the UTFT_Buttons code instead of rolling my own
// 14nov2014
//  code cleanup, add comments, RCS
// November 2014
//  Added AD8302 phase/power detector, AD9850 1-40MHz DDS and MCL directional coupler
//  calculate the SWR on a graph
// Prior to November 2014:
//  some buttons, not much else
/////////////////////////////////////////////////////////////////////////////////////////
// https://github.com/tczerwonka/spna
/////////////////////////////////////////////////////////////////////////////////////////


#include <UTFT.h>
#include <URTouch.h>
#include <DDS.h>
#include <UTFT_Buttons.h>

// Initialize display
// ------------------
// Standard Arduino Mega/Due shield            : <display model>,38,39,40,41
UTFT    myGLCD(ITDB32S,38,39,40,41);

// Initialize touchscreen
// ----------------------
// Standard Arduino Mega/Due shield            :  6, 5, 4, 3, 2
URTouch  myTouch( 6, 5, 4, 3, 2);

//easy buttons
UTFT_Buttons  myButtons(&myGLCD, &myTouch);

//PWM for meter -- pin 9
const int METER_PIN = 9;

// AD9850 Module....
// set pin numbers:
const int W_CLK = 10;
const int FQ_UD = 11; 
const int DATA = 12;
const int RESET = 13;
// Instantiate the DDS...
DDS dds(W_CLK, FQ_UD, DATA, RESET);

//AD8302 pins
#define PDIF A0
#define PPHASE A1

// Declare which fonts we will be using
//extern uint8_t BigFont[];
extern uint8_t arial_bold[];
extern uint8_t SmallFont[];
extern uint8_t SevenSegNumFontPlus[];
extern uint8_t various_symbols[];
//extern uint8_t SevenSegmentFull[];


//how big is the screen
//int xmax = myGLCD.getDisplayXSize();
//int ymax = myGLCD.getDisplayYSize();
int xmax = 319;
int ymax = 239;

//what is the maximum swr to graph
int SWR_MAX = 10;

int x, y;
char stCurrent[20]="";
int stCurrentLen=0;
char stLast[20]="";
int current_page = 0;

#define GIMBALX A4
#define GIMBALY A3
#define GIMBALSW A2
//these don't center out at 512 like you'd hope
#define GIMBALXCENTER 850
#define GIMBALYCENTER 820

int cursor_xpos = 0;
int cursor_ypos = 0;

typedef struct {
  int x;
  int y;
} cursor_t;

#define CHAR 10   //Number of characters per LCD line
#define PAGEPOS 4  //which position on the menu page
#define PAGES  5    //Number of discrete menu pages 
#define MAIN_MENU 0
#define GENERATE_MENU 1
#define ANALYZE_MENU 2
#define SETUP_MENU 3
#define CAL_MENU 4
#define CARRIER_MENU 5
#define MODULATE_MENU 6
#define BEACON_MENU 7
#define SINGLE_MENU 8
#define SWEEP_MENU 9
#define VERSION_MENU 10
#define FRQ_MENU 11
#define OSL_MENU 12



char vocab[PAGES][PAGEPOS][CHAR]={  
  {
    "generate", "analyze", "setup"                    }
  ,
  {
    "main menu", "carrier", "modulate", "beacon"                    }
  ,
  {
    "main menu", "single", "sweep"                    }
  ,
  {
    "main menu", "calibrate", "version"                    }
  ,
  {
    "setupmenu", "freq", "OSL"                    }
};



/////////////////////////////////////////////////////////////////////////////////////////
//draw the four-button left-hand menus
//input: vocab page to print
/////////////////////////////////////////////////////////////////////////////////////////
void drawMenuButtons(int page)
{
  //page is the page number of buttons to display
  //if a button is blank - don't display anything
  int int_btnx = 160;
  int int_btny = 50;

  //clear the screen
  myGLCD.clrScr();

  //screen is 320x240 -- 4 down the left side -- 100x60?
  for (y=0; y<4; y++) {
    //x1, y1, x2, y2
    //if the button has no text - do not make a button
    if (strlen(vocab[page][y]) >= 1) {
      myGLCD.setColor(0,0,255); //rgb
      myGLCD.fillRoundRect(10, 10+(y*int_btny), int_btnx, int_btny+(y*int_btny));
      myGLCD.setColor(255, 255, 255);
      myGLCD.drawRoundRect(10, 10+(y*int_btny), int_btnx, int_btny+(y*int_btny));
      myGLCD.print(vocab[page][y], 15, 22+(y*int_btny));
    }
  }
  delay(500);
}





/////////////////////////////////////////////////////////////////////////////////////////
//double drawInputScreen(char unitlabel)
//draw a number input screen
//return a double for frequency
//input: unit label (step, MHz, kHz, kittens, frobulators
//output: a double -- frequency in hertz
//  -1 for cancel 
// top 60 is the display for what the user inputs
// next 60 is 1-5
// next 60 is 6-0
// last 60 is decimal, ok, cancel
/////////////////////////////////////////////////////////////////////////////////////////
double drawInputScreen(int page)
{
  //testing for now
  int b1, b2, b3, b4, b5, b6, b7, b8, b9, b0;
  int bOK, bCANCEL, bBACK, bDOT, pressed_button;

  char frequency[7]="";
  int frq_index=0;
  int index;
  double d_fvalue;
  double mhz = 0;
  double khz = 0;

  boolean default_colors = true;
  //display is 320x240
  //x width is 64
  //y height is 60
  //x -- 0, 63, 127, 191, 255, 319 
  //y -- 60, 120, 180

  myGLCD.clrScr();

  myGLCD.setColor(VGA_WHITE);
  b1 = myButtons.addButton(0, 60, 63, 60, (char *)"1");
  b2 = myButtons.addButton(64, 60, 63, 60, (char *)"2");
  b3 = myButtons.addButton(128, 60, 63, 60, (char *)"3");
  b4 = myButtons.addButton(192, 60, 63, 60, (char *)"4");
  b5 = myButtons.addButton(256, 60, 63, 60, (char *)"5");
  b6 = myButtons.addButton(0, 121, 63, 60, (char *)"6");
  b7 = myButtons.addButton(64, 121, 63, 60, (char *)"7");
  b8 = myButtons.addButton(128, 121, 63, 60, (char *)"8");
  b9 = myButtons.addButton(192, 121, 63, 60, (char *)"9");
  b0 = myButtons.addButton(256, 121, 63, 60, (char *)"0");
  
  bBACK = myButtons.addButton(256, 0, 63, 59, (char *)"<-");

  bDOT = myButtons.addButton(0, 182, 63, 57, (char *)".");
  myGLCD.setColor(VGA_LIME);
  bOK = myButtons.addButton(64, 182, 127, 57, (char *)"OK");
  myGLCD.setColor(VGA_RED);
  bCANCEL = myButtons.addButton(192, 182, 127, 57, (char *)"CANCEL");

  //but1 = myButtons.addButton( 10,  20, 300,  30, "Button 1");
  //but2 = myButtons.addButton( 10,  60, 300,  30, "Button 2");
  //but3 = myButtons.addButton( 10, 100, 300,  30, "Button 3");
  //but4 = myButtons.addButton( 10, 140, 300,  30, "Button 4", BUTTON_DISABLED);
  //butX = myButtons.addButton(279, 199,  40,  40, "a", BUTTON_SYMBOL);
  //butY = myButtons.addButton(  0, 199, 100,  40, "I", BUTTON_SYMBOL | BUTTON_SYMBOL_REP_3X);
  myButtons.drawButtons();
  //  myGLCD.print("You pressed:", 0, 30);
  //myGLCD.setColor(VGA_BLACK);
  //myGLCD.setBackColor(VGA_WHITE);
  //myGLCD.print("None    ", 0, 30);

  //green numbers
  myGLCD.setColor(VGA_LIME);

  while(1) 
  {
    if (myTouch.dataAvailable() == true)
    {
      pressed_button = myButtons.checkButtons();
      //can not use a switch/case here unfortunately
      if (pressed_button==b1)
        frequency[frq_index]=1;
      if (pressed_button==b2)
        frequency[frq_index]=2;
      if (pressed_button==b3)
        frequency[frq_index]=3;
      if (pressed_button==b4)
        frequency[frq_index]=4;
      if (pressed_button==b5)
        frequency[frq_index]=5;
      if (pressed_button==b6)
        frequency[frq_index]=6;
      if (pressed_button==b7)
        frequency[frq_index]=7;
      if (pressed_button==b8)
        frequency[frq_index]=8;
      if (pressed_button==b9)
        frequency[frq_index]=9;
      if (pressed_button==b0)
        frequency[frq_index]=0;
      if (pressed_button==bDOT)
        frequency[frq_index]=11;
      if (pressed_button==bBACK)
        frequency[frq_index]=12;
      if (pressed_button==bCANCEL)
        frequency[frq_index]=13;
      if (pressed_button==bOK)
        frequency[frq_index]=14;  

      //have button value
      //if no more buttons valid (>8) -- turn OK button green?
      //if button not valid -- ok/exit/back -- handle

      //Serial.print("frequency frq_index: ");
      //Serial.println(frequency[frq_index],DEC);
      //Serial.print("  frq_index: ");
      //Serial.println(frq_index);

      //if a number was entered and have space for a number -- print it, increment counter
      if ((frequency[frq_index] < 10) && (frq_index < 8))
      {
        //print button value
        //Serial.println("assign num - frq_index++");
        myGLCD.setFont(SevenSegNumFontPlus);
        myGLCD.printNumI(frequency[frq_index], frq_index*30, 5);
        myGLCD.setFont(arial_bold);
        //increment counter
        frq_index++;
      } // if < 10

      // special keys
      // decimal
      if (frequency[frq_index] == 11) 
      {
        myGLCD.setFont(SevenSegNumFontPlus);
        myGLCD.print((char *)":",frq_index*30,5);
        myGLCD.setFont(arial_bold);
        frq_index++;
      } //if

      // back
      if ((frequency[frq_index] == 12) && (frq_index > 0))
      {
        //print nothing -- erase current I guess -- no increment
        //filled rectangle
        frequency[frq_index]=-1;
        frq_index--;
        myGLCD.setColor(VGA_BLACK);
        myGLCD.fillRect(frq_index*30, 5, (frq_index*30)+31, 55);
        //Serial.print("backspace - frq_index: ");
        //Serial.println(frq_index);
        myGLCD.setColor(VGA_LIME);
        //frq_index--;
      }

      // cancel -- 13
      if (frequency[frq_index] == 13)
      {
        myButtons.deleteAllButtons();
        return -1;
      } 
      // OK -- 14
      if (frequency[frq_index] == 14)
      {
        myButtons.deleteAllButtons();
        break;
      }
      //frq_index++;
    } // if data available and button pressed


  }//while

  //supposedly we're at OK here -- return a valid number
  //step through array
  //bitching about a -Waggressive-loop-optimization error here -- changed from 10 to 7...wtf
  for (int index = 0; index < 7; index++) {
    if (frequency[index] < 7 ) {
      Serial.print("index: ");Serial.println(index);
      Serial.print("  freq[index]: ");Serial.println(frequency[index],DEC);
      Serial.print("  d_fvalue before: ");Serial.println(d_fvalue);
      d_fvalue += frequency[index];
      d_fvalue = d_fvalue * 10;
      Serial.print("  d_fvalue after : ");Serial.println(d_fvalue);
    } // if less than 10
  } //for

}




/////////////////////////////////////////////////////////////////////////////////////////
//decode which of the menu buttons was touched
//input: dx and dy coordinates of button touched
//return: which position was touched
/////////////////////////////////////////////////////////////////////////////////////////
int decodeMenuButton()
{

  int dx, dy;

  //myTouch.read();
  dx=myTouch.getX();
  dy=myTouch.getY();
  
  Serial.println(dx);
  Serial.println("\t");
  Serial.println(dy);
  Serial.println("\n");

  while (true)
  {
    if (!myTouch.dataAvailable())
    {
      break;
    }
  }

  if ((dx>=10) && (dx<=160))  // left-hand side
  {
    if ((dy>=10) && (dy<=60))  // Button: 1
    {
      //waitForIt(10, 10, 60, 60);
      Serial.println("ret1");
      return(1);
    }
    if ((dy>=60) && (dy<=110))  // Button: 2
    {
      //waitForIt(70, 10, 120, 60);
      Serial.println("ret2");
      return(2);
    }
    if ((dy>=120) && (dy<=160))  // Button: 3
    {
      //waitForIt(130, 10, 180, 60);
      Serial.println("ret3");
      return(3);
    }
    if ((dy>=160) && (dy<=210))  // Button: 4
    {
      //waitForIt(190, 10, 240, 60);
      Serial.println("ret4");
      return(4);
    }
  }
  //screen was pressed but the press was not a valid button
  Serial.println("ret0-nokeyfound");
  return(0);
}



void updateStr(int val)
{
  if (stCurrentLen<20)
  {
    stCurrent[stCurrentLen]=val;
    stCurrent[stCurrentLen+1]='\0';
    stCurrentLen++;
    myGLCD.setColor(0, 0, 255);
    myGLCD.print(stCurrent, LEFT, 224);
  }
  else
  {
    myGLCD.setColor(255, 0, 0);
    myGLCD.print((char *)"BUFFER FULL!", CENTER, 192);
    delay(500);
    myGLCD.print((char *)"            ", CENTER, 192);
    delay(500);
    myGLCD.print((char *)"BUFFER FULL!", CENTER, 192);
    delay(500);
    myGLCD.print((char *)"            ", CENTER, 192);
    myGLCD.setColor(0, 255, 0);
  }
}



/////////////////////////////////////////////////////////////////////////////////////////
// required function
/////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  // Initial setup
  myGLCD.InitLCD();
  myGLCD.clrScr();

  Serial.begin(9600);
  
  pinMode(GIMBALSW, INPUT);
  digitalWrite(GIMBALSW, HIGH);

  myTouch.InitTouch();
  //myTouch.setPrecision(PREC_LOW);
  myTouch.setPrecision(PREC_MEDIUM);

  //myButtons.setTextFont(BigFont);
  myButtons.setTextFont(arial_bold);

  myGLCD.setFont(arial_bold);
  myGLCD.setBackColor(0, 0, 255);
  version();
  delay(1000);
  // start up the DDS...   
  dds.init();  
  dds.trim(125000000); // enter actual osc freq  

  current_page=MAIN_MENU;
  drawMenuButtons(current_page);  
}



/////////////////////////////////////////////////////////////////////////////////////////
// main loop
/////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  int retval, touchpad_x, touchpad_y;

  draw_cursor();
  
    if (myTouch.dataAvailable())
    {
      retval = 0;
      myTouch.read();
      //touchpad_x=myTouch.getX();
      //touchpad_y=myTouch.getY();

      //retval = decodeMenuButton(touchpad_x, touchpad_y);  
      retval = decodeMenuButton();
      decodeGimbal();

      //main menu
      // {"generate", "analyze", "setup"},
      if ((retval == 1) && (current_page == MAIN_MENU)) {
        current_page = GENERATE_MENU;
        drawMenuButtons(GENERATE_MENU);
        retval = 0;
      }
      if ((retval == 2) && (current_page == MAIN_MENU)) {
        current_page = ANALYZE_MENU;
        drawMenuButtons(ANALYZE_MENU);
        retval = 0;
      }
      if ((retval == 3) && (current_page == MAIN_MENU)) {
        current_page = SETUP_MENU;
        drawMenuButtons(SETUP_MENU);
        retval = 0;
      }



      //generate menu
      // {"main menu", "carrier", "modulate", "beacon"},
      if ((retval == 1) && (current_page == GENERATE_MENU)) {
        current_page = MAIN_MENU;
        drawMenuButtons(MAIN_MENU);
        retval = 0;
      }
      if ((retval == 2) && (current_page == GENERATE_MENU)) {
        current_page = CARRIER_MENU;
        drawMenuButtons(CARRIER_MENU);
        retval = 0;
      }
      if ((retval == 3) && (current_page == GENERATE_MENU)) {
        current_page = MODULATE_MENU;
        drawMenuButtons(MODULATE_MENU);
        retval = 0;
      }
      if ((retval == 4) && (current_page == GENERATE_MENU)) {
        current_page = BEACON_MENU;
        drawMenuButtons(BEACON_MENU);
        retval = 0;
      }


      //analyze menu
      //{"main menu", "single", "sweep"},
      if ((retval == 1) && (current_page == ANALYZE_MENU)) {
        current_page = MAIN_MENU;
        drawMenuButtons(MAIN_MENU);
        retval = 0;
      }
      if ((retval == 2) && (current_page == ANALYZE_MENU)) {
        current_page = MAIN_MENU;
        //analyze a single frequency
        //can only select first button if this is enabled, wtf
        singleFrequencyData();
        drawMenuButtons(MAIN_MENU);
        retval = 0;
      }
      if ((retval == 3) && (current_page == ANALYZE_MENU)) {
        current_page = MAIN_MENU;
        //simpleSweep
        Serial.println("sweep");
        SimpleSweep(1);
        drawMenuButtons(MAIN_MENU);
        retval = 0;
      }


      //setup menu
      //{"main menu", "calibrate", "version"},
      if ((retval == 1) && (current_page == SETUP_MENU)) {
        current_page = MAIN_MENU;
        drawMenuButtons(MAIN_MENU);
        retval = 0;
      }
      if ((retval == 2) && (current_page == SETUP_MENU)) {
        current_page = CAL_MENU;
        drawMenuButtons(CAL_MENU);
        retval = 0;
      }
      if ((retval == 3) && (current_page == SETUP_MENU)) {
        current_page = SETUP_MENU;
        version();
        while (true) {
          if (myTouch.dataAvailable()) {
            break;
          } //if
        } //while
        drawMenuButtons(SETUP_MENU);
      }

      //{"cal menu", "freq", "OSL"}
      if ((retval == 1) && (current_page == CAL_MENU)) {
        current_page = SETUP_MENU;
        drawMenuButtons(SETUP_MENU);
      }

      Serial.print(retval);

    } //if data available
} //void loop



/////////////////////////////////////////////////////////////////////////////////////////
//singleFrequencyData
//  Analyze a single frequency
/////////////////////////////////////////////////////////////////////////////////////////
void singleFrequencyData () {
  double Fdefault = 10000000;
  drawInputScreen(1);

} //end singleFrequencyData



