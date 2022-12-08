// Arduino Code example for Newhaven COG LCD display NHD-C0216CiZ.

/*****************************************************************************
 * 
/ Program for writing to NHD-C0216CiZ display series with the ST7032i Controller.
/ This code is written for the Arduino Uno R3 using I2C Interface
/
/ Newhaven Display invests time and resources providing this open source code,
/ Please support Newhaven Display by purchasing products from Newhaven Display!

* Copyright (c) 2019, Newhaven Display International
*
* This code is provided as an example only and without any warranty by Newhaven Display. 
* Newhaven Display accepts no responsibility for any issues resulting from its use. 
* The developer of the final application incorporating any parts of this 
* sample code is responsible for ensuring its safe and correct operation
* and for any consequences resulting from its use.
* See the GNU General Public License for more details. 
* 
*****************************************************************************/

#include <Wire.h>

int RES = 7;

/********Arduino Mega********/
int ASDA = 18;
int ASCL = 19;
int msg_enable = 8;
int msg_type = 12;
/********Arduino Mega********/

int test = 55;

unsigned char text1[]={"Newhaven Display"};
unsigned char text2[]={"  NHD-C0216CiZ  "};
unsigned char text3[]={" I2C Interface  "};
unsigned char text4[]={"Please enjoy! :)"};

const char slave2w = 0x3C; 
const char comsend = 0x00;
const char datasend = 0x40;
const char line2 = 0xC0;

void show(unsigned char *text)
{
  int n, d;
  d=0x00;
  Wire.beginTransmission(slave2w);
  Wire.write(datasend);
  for(n=0;n<16;n++)
  {
    Wire.write(*text);
    ++text;
  }
  Wire.endTransmission();
}

void nextline(void)
{
  Wire.beginTransmission(slave2w);
  Wire.write(comsend);
  Wire.write(line2);
  Wire.endTransmission();
}

void CGRAM(void)
{
  Wire.beginTransmission(slave2w);
  Wire.write(comsend);
  Wire.write(0x38);
  Wire.write(0x40);
  Wire.endTransmission();
  delay(10);
  Wire.beginTransmission(slave2w);
  Wire.write(datasend);
  Wire.write(0x00);
  Wire.write(0x1E);
  Wire.write(0x18);
  Wire.write(0x14);
  Wire.write(0x12);
  Wire.write(0x01);
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.endTransmission();
}

void CiZ_init()
{
  Wire.beginTransmission(slave2w);
  Wire.write(comsend);
  Wire.write(0x38);
  delay(10);
  Wire.write(0x39);
  delay(10);
  Wire.write(0x14);
  Wire.write(0x70); 
  Wire.write(0x5E);
  Wire.write(0x6D);
  Wire.write(0x0C);
  Wire.write(0x01);
  Wire.write(0x06);
  Wire.endTransmission();
  
  CGRAM();
  
  Wire.beginTransmission(slave2w);
  Wire.write(comsend);
  Wire.write(0x39);
  Wire.write(0x01);
  test = Wire.endTransmission();
  Wire.endTransmission();
  if(test!=0)
  {
    while(1)
    {
      delay(100);
    }
  }
  delay(10);
}

void setup() 
{
  pinMode(RES, OUTPUT);
  pinMode(ASCL, OUTPUT);
  pinMode(ASDA, OUTPUT);
  pinMode(msg_enable, INPUT);
  pinMode(msg_type, INPUT);
  digitalWrite(RES, HIGH);
  delay(10);
  digitalWrite(ASCL, LOW);
  digitalWrite(ASDA, LOW);
  delay(10);
  
  delay(10);
  //CiZ_init();
  //delay(5);
}

void loop() 
{
  
  Wire.begin();
  while(1)
  {
  CiZ_init();
  if (digitalRead(msg_enable) == 1) {
    if (digitalRead(msg_type) == 0) {
      show("bid order sent");
    } else {
      show("ask order sent");
    }
    delay(1000);
  }
  }
}

// /****************************************************
// * Initialization For ST7036i *
// *****************************************************/
// #define Slave 0x78
// #define Comsend 0x00
// #define Datasend 0x40

// void init_LCD()
// {
// I2C_Start();
// I2C_out(Slave);//Slave=0x78
// I2C_out(Comsend);//Comsend = 0x00
// I2C_out(0x38);
// delay(10);
// I2C_out(0x39);
// delay(10);
// I2C_out(0x14);
// I2C_out(0x78);
// I2C_out(0x5E);
// I2C_out(0x6D);
// I2C_out(0x0C);
// I2C_out(0x01);
// I2C_out(0x06);
// delay(10);
// I2C_Stop();
// }
// /*****************************************************/
// /****************************************************
// * Output command or data via I2C *
// *****************************************************/
// void I2C_out(unsigned char j) //I2C Output
// {
// int n;
// unsigned char d;
// d=j;
// for(n=0;n<8;n++){
// if((d&0x80)==0x80)
// digitalWrite(2, HIGH);
// else
// digitalWrite(2, LOW);
// d=(d<<1);
// digitalWrite(4, LOW);
// digitalWrite(4, HIGH);
// digitalWrite(4, LOW);
// }
// digitalWrite(4, HIGH);
// pinMode(2, INPUT_PULLUP);
// while(digitalRead(2)==HIGH){
// digitalWrite(4, LOW);
// digitalWrite(4, HIGH);
// }
// Serial.print("ack ");
// Serial.println(j);
// pinMode(2, OUTPUT);
// digitalWrite(4, LOW);
// }
// /*****************************************************/
// /****************************************************
// * I2C Start *
// *****************************************************/
// void I2C_Start(void)
// {

// digitalWrite(4, HIGH);
// digitalWrite(2, HIGH);
// digitalWrite(2, LOW);
// digitalWrite(4, LOW);
// }
// /*****************************************************/
// /****************************************************
// * I2C Stop *
// *****************************************************/
// void I2C_Stop(void)
// {
// digitalWrite(2, LOW);
// digitalWrite(4, LOW);
// digitalWrite(4, HIGH);
// digitalWrite(2, HIGH);
// }
// /*****************************************************/
// /****************************************************
// * Send string of ASCII data to LCD *
// *****************************************************/
// void Show(unsigned char *text)
// {
// int n,d;
// d=0x00;
// I2C_Start();
// I2C_out(Slave); //Slave=0x78
// I2C_out(Datasend);//Datasend=0x40
// for(n=0;n<20;n++){
// I2C_out(*text);
// ++text;
// }
// I2C_Stop();
// }
// /*****************************************************/
// /*****************************************************/
// /*****************************************************/


// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(9600);
//   pinMode(2, OUTPUT);
//   pinMode(4, OUTPUT);
//   init_LCD();
// }

// void loop() {
//   // put your main code here, to run repeatedly:
//   Show("aaaaaaaaaaaaaaaaaabb");
// }
