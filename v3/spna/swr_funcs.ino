/////////////////////////////////////////////////////////////////////////////////////////
//calc_swr
//  Calculate the system SWR
/////////////////////////////////////////////////////////////////////////////////////////
float calc_swr(int ADCphase, int ADCmagnitude) {
  //take the phase and magnitude adc 
  //values as arguments
  float return_loss;
  float phase;
  float swr;
  float tmpa, tmpb;

  //calculate phase
  //Serial.print("ADCphase: ");Serial.println(ADCphase);
  phase = ((ADCphase / 1024.0) * 180.0 ) - 90.0;
  //Serial.print("phase: ");Serial.println(phase);

  //calculate magnitude
  return_loss = (ADCmagnitude / 1024.0) * 60.0;
  return_loss = return_loss - 30.0;
  //Serial.print("ADCmagnitude: ");Serial.println(ADCmagnitude);
  //Serial.print("return loss: ");Serial.println(return_loss);

  swr = return_loss / 20.0;
  swr = pow(10, swr);
  swr = 1.0 / swr;
  tmpa = 1.0 + swr;
  tmpb = 1.0 - swr;
  swr = abs(tmpa / tmpb);

  Serial.print("swr: ");
  Serial.println(swr);
  return swr;
} //end calc_swr



/////////////////////////////////////////////////////////////////////////////////////////
//drawSWRGraph 
// -- just draw the swr graph background
/////////////////////////////////////////////////////////////////////////////////////////
void drawSWRGraph () {

  int swrscale;
  int tmp_div;
  int tmp_count;
  int swr_printed_value = 0;
  int swr_increment;

  //set number of divisions
  int divisions = 4;


  swrscale = ymax / divisions;
  swr_increment = SWR_MAX / divisions;

  myGLCD.clrScr();
  myGLCD.setColor(VGA_BLUE);
  //  myGLCD.drawRect(x1,y1,x2,y2)
  //full screen rectangle
  myGLCD.drawRect(0,0,xmax,ymax);

  //iterate
  for (tmp_count = ymax; tmp_count > 0; tmp_count = tmp_count - swrscale) {
    myGLCD.drawLine(0,tmp_count,xmax,tmp_count);
    myGLCD.printNumI(swr_printed_value, 0, tmp_count-20);
    swr_printed_value = swr_printed_value + swr_increment;
  } // for

} //drawSWRGraph
