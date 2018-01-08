/////////////////////////////////////////////////////////////////////////////////////////
//decode the output of the gimbal
//input: nothing I guess
//return: something useful someday
/////////////////////////////////////////////////////////////////////////////////////////
cursor_t decodeGimbal()
{


    cursor_t dir;
    int gimx, gimy, gimsw;
    dir.x = 0;
    dir.y = 0;
  
    gimx = analogRead(GIMBALX);
    gimy = analogRead(GIMBALY);
    gimsw = digitalRead(GIMBALSW);

    //some rudimentary acceleration -- there's very likely a more
    //efficient way to do this but it ain't comin' to me at 11:30 p.m.
    //this should also be a %-age of GIMBALXCENTER as well...
    if(gimx < (GIMBALXCENTER - 50) ) dir.x = 1;
    if(gimx > (GIMBALXCENTER + 50) ) dir.x = -1;
    if(gimy < (GIMBALYCENTER - 50) ) dir.y = 1;
    if(gimy > (GIMBALYCENTER + 50) ) dir.y = -1; 

    if(gimx < (GIMBALXCENTER - 150) ) dir.x = 2;
    if(gimx > (GIMBALXCENTER + 150) ) dir.x = -2;
    if(gimy < (GIMBALYCENTER - 150) ) dir.y = 2;
    if(gimy > (GIMBALYCENTER + 150) ) dir.y = -2; 

    if(gimx < (GIMBALXCENTER - 250) ) dir.x = 3;
    if(gimx > (GIMBALXCENTER + 250) ) dir.x = -3;
    if(gimy < (GIMBALYCENTER - 250) ) dir.y = 3;
    if(gimy > (GIMBALYCENTER + 250) ) dir.y = -3; 

    if(gimx < (GIMBALXCENTER - 350) ) dir.x = 4;
    if(gimx > (GIMBALXCENTER + 350) ) dir.x = -4;
    if(gimy < (GIMBALYCENTER - 350) ) dir.y = 4;
    if(gimy > (GIMBALYCENTER + 350) ) dir.y = -4;
    
    Serial.print("x: ");Serial.println(gimx);
    Serial.print("y: ");Serial.println(gimy);
    Serial.print("b: ");Serial.println(gimsw);

    return (dir);
}



/////////////////////////////////////////////////////////////////////////////////////////
//draw_cursor()
//  draw a dot or something on the screen to indicate the location of the mouse
/////////////////////////////////////////////////////////////////////////////////////////
int draw_cursor()
{
  cursor_t dir_struct;
  dir_struct = decodeGimbal();
  //Serial.print("mcx: ");Serial.println(dir_struct.x);


  if (((cursor_xpos + dir_struct.x) > 0) && ((cursor_xpos + dir_struct.x) < xmax)) cursor_xpos += dir_struct.x;
  if (((cursor_ypos + dir_struct.y) > 0) && ((cursor_ypos + dir_struct.y) < ymax)) cursor_ypos += dir_struct.y;

  myGLCD.drawPixel(cursor_xpos,cursor_ypos);
}

