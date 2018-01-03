/////////////////////////////////////////////////////////////////////////////////////////
// version / startup screen
/////////////////////////////////////////////////////////////////////////////////////////
void version()
{
  //clear the screen
  myGLCD.clrScr();

  myGLCD.setFont(SmallFont);
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(0, 0, 319, 13);
  myGLCD.setColor(64, 64, 64);
  myGLCD.fillRect(0, 226, 319, 239);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(255, 0, 0);
  myGLCD.print((char *)"* SPNA *", CENTER, 1);
  myGLCD.setBackColor(64, 64, 64);
  myGLCD.setColor(255,255,0);
  myGLCD.print((char *)"<http://www.cs.wisc.edu/~timc/e/>", CENTER, 227);

  myGLCD.setColor(0, 0, 255);
  myGLCD.drawRect(0, 14, 319, 225);

  myGLCD.setFont(arial_bold);
  myGLCD.setColor(0,255,0); //rgb
  myGLCD.fillRoundRect(0, 0, 320,240);
  myGLCD.print((char *)"Single-port", CENTER, 35);
  myGLCD.print((char *)"Network Analyzer", CENTER, 55);

  myGLCD.print((char *)"Tim Czerwonka", CENTER, 100);
  myGLCD.print((char *)"WO9U", CENTER, 125);

  myGLCD.print((char *)"2018", CENTER, 180);
  myGLCD.print((char *)"version 0.31", CENTER, 200);
  
  //1
  analogWrite(9,50);
  delay(1000);
  //2
  analogWrite(9,110);
  delay(1000);
  //3
  analogWrite(9,160);
  delay(1000);
  //4
  analogWrite(9,210);
  delay(1000);
  //5
  analogWrite(9,255);
  delay(1000);
  //back to zero
  analogWrite(9,0);
}
