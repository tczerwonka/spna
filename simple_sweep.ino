/////////////////////////////////////////////////////////////////////////////////////////
//SimpleSweep
//  I just need a function to do a simple frequency sweep
/////////////////////////////////////////////////////////////////////////////////////////
void SimpleSweep (int whatever) {
  double Fstart = 8000000;
  double Fstop = 30000000;
  double Fstep = 20000;
  double freq;
  double power;
  double phase;
  double x = 0;
  double y;
  double xstep;
  double steptotal;
  float swr;
  float swr_first_min = 999;
  float freq_first_min;
  int meterval;

  //Serial.println("simple sweep");
  //myGLCD.clrScr();
  drawSWRGraph();
  Serial.print("xmax:");
  Serial.println(xmax);
  Serial.print("ymax:");
  Serial.println(ymax);

  myGLCD.setColor(VGA_LIME);


  //determine the amount to increment X on each frequency step
  steptotal = (Fstop - Fstart) / Fstep;
  xstep = xmax / steptotal;
  Serial.print("steptotal: ");
  Serial.println(steptotal);
  Serial.print("xstep: ");
  Serial.println(xstep);  


  // sweep form 14MHz to 14.350MHz
  for (freq = Fstart; freq < Fstop; freq += Fstep) {

    dds.setFrequency(freq);  
    power = analogRead(A0);
    phase = analogRead(A1);

    //scale y
    //y = (phase * ymax) / 1024;    
    //myGLCD.setColor(VGA_SILVER);
    //myGLCD.drawPixel(x,y);
    //Serial.print("power: "); Serial.print(power); Serial.print(" phase: "); Serial.print(phase); 
    //Serial.print(" y scale: ");Serial.println(y);

    //y = (power * ymax) / 1024;    
    //myGLCD.setColor(VGA_RED);
    //myGLCD.drawPixel(x,y);
    //Serial.print("power: "); Serial.print(power); Serial.print(" phase: "); Serial.print(phase); 
    //Serial.print(" y scale: ");Serial.println(y);

    Serial.print(" freq: ");
    Serial.println(freq);
    swr = calc_swr(phase,power);
    //origin is upper left so y is essentially inverted from what i want
    y = ymax - ((swr * ymax) / 10); 
    //check for minimum
    if (swr < swr_first_min) {
      swr_first_min = swr;
      freq_first_min = freq;
    }

    //how about some meter action
    //max swr of 5 -- so 50 scaling factor and 5 max
    meterval = (5 * swr);
    if (meterval > 255) meterval=255;
    analogWrite(9,meterval);


    myGLCD.setColor(VGA_LIME);
    myGLCD.drawPixel(x,y);

    x+=xstep;
  } //for
  //dds.setFrequency(14100000);  

  //print the frequency with the lowest swr
  //scale to mhz
  freq_first_min = freq_first_min / 1000000.0;
  myGLCD.printNumF(freq_first_min, 3, 100, 100);
  delay(10000);
  //set meter back to zero
  analogWrite(9,0);
} //end SimpleSweep
