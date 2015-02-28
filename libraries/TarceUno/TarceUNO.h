//v 1.05 A.B.

#ifndef tarceUNO_h
#define tarceUNO_h

#define BESEDILO 0
#define NASLOV 1

#define ZNAK_DESNO  126
#define ZNAK_LEVO   127

#define ZVOK_OFF  0
#define ZVOK_LOW  1
#define ZVOK_MED  2
#define ZVOK_MAX  3

#define btnUP     0
#define btnDOWN   1
#define btnLEFT   2
#define btnRIGHT  3
#define btnSELECT 4


#define PISKAC_PIN_JAKOST1  1
#define PISKAC_PIN_JAKOST2  2
#define GUMBI_PIN    A0
#define SENZOR1_PIN  A1
#define SENZOR2_PIN  A2
#define SENZOR3_PIN  A3
#define MORTOR1_PIN  13
#define MORTOR2_PIN  12
#define MORTOR3_PIN  11

#define ON  1
#define OFF  0

#define MAX_EKRANOV	20
//ekrani in dejanska stanja
#define MENI_GLAVNI 	1
#define MENI_IGRAJ 	2
#define MENI_NASTAVITVE 3
#define MENI_PROG1 	4
#define MENI_PROG2 	5
#define MENI_PROG3 	6
#define NAST_MOTOR1     7
#define NAST_MOTOR2     8
#define NAST_MOTOR3 	9
#define NAST_SENZORJI 	10
#define PROGRAM_1	11
#define PROGRAM_2	16
#define NAST_PROG1 	12
#define PROGRAM_2	13
#define PROGRAM_3	14
#define ODSTEVAJ	15

//ukazi
#define INC_IZBRAN        20
#define DEC_IZBRAN        21
#define NAST_EDIT         16
#define NAST_EDIT_DEC     23
#define NAST_EDIT_INC     24
#define NAST_EDIT_FINISH  25 


//spomin
#define MAX_SPOMIN			20 //velikost arr v katerega se prepišejo podatki 
#define ST_SPOMINA 		 	7		//dejansko st spremenljivk

#define MEM_MOTOR_1_MIN        1
#define MEM_MOTOR_1_MAX        2
#define MEM_MOTOR_2_MIN        3
#define MEM_MOTOR_2_MAX        4
#define MEM_MOTOR_3_MIN        5
#define MEM_MOTOR_3_MAX        6
#define MEM_SENSOR    	   	   7



#define FUN_TIPKE 200

#define MAX_PARAMETROV 2 // kolk parametrov je lahko v posameznem oknu ekranNastavitve

#define d_retState   	 (((s_ekranNastavitveEdit*)vsiEkrani[NAST_EDIT])->returnState)		//int v katermu je shranjen ret state
#define d_tretIzbran	 (((s_ekranNastavitve*)vsiEkrani[d_retState])->trenutnoIzbran)		//ekran[ret_state] trenutno izbrani element
#define d_indexParametra (((s_ekranNastavitve*)vsiEkrani[d_retState])->indexi[d_tretIzbran])//od trenutno izbranega elementa dobi index parametra
#define d_parameter  	 (parametri[d_indexParametra])										//parameter




typedef struct
{
char *besedilo;
uint8_t funkcija;
}s_izbirnaVrstica;



typedef struct 
{
char* naslov;
uint8_t tipke[5];
uint8_t st_elementov;
uint8_t trenutnoIzbran;
s_izbirnaVrstica elementi[5];
}s_ekran;

typedef struct 
{
char* opis;
uint16_t min;
uint16_t max;
int16_t vrednost; // def vrednost-> uporablja samo takrat ce je v spominu shranjena napacna vrednost in takrat ko se ureja na ekranu eddit
uint8_t  korak;
}s_parameter;


typedef struct 
{
char* naslov;
uint8_t tipke[5];
uint8_t stParametrov;
uint8_t trenutnoIzbran;
uint8_t focus;
uint8_t indexi[MAX_PARAMETROV]; //indexi parametrov ki so shranjeni v areju "spomin"
}s_ekranNastavitve;

typedef struct 
{
char* naslov; //nima ga
uint8_t tipke[5];
uint16_t returnState;
}s_ekranNastavitveEdit;

typedef struct 
{
char* naslov;
uint8_t tipke[5];
}s_ekranProgram;


typedef struct 
{
uint8_t stanje;
uint8_t cnt;
uint8_t tocke;
Servo motor;
}s_tarca;

struct koncniNapisi 
{
  char *besedilo;
  uint16_t zamik;
};

void odstevaj();//uint8_t stOdstevanj, void (*initProgram)());
#endif



/*************************************************************+
s_ekran mainMenu = //MENI_GLAVNI
{
  "Glavni meni",
  //tipke: 
  //gor dol levo, desno, select
  0,FUN_TIPKE,DEC_IZBRAN,INC_IZBRAN, 0,
  4,0,
  "igraj !",      MENI_IGRAJ,
  "nastavitve",   MENI_NASTAVITVE,
  "info",         0,
  "random :)",    0,
};

s_ekran nastavitveMenu = //MENI_NASTAVITVE
{
  "Nastavitve",
  //gor dol levo, desno, select
  MENI_GLAVNI,FUN_TIPKE,DEC_IZBRAN,INC_IZBRAN, 0,
  4,0,
  "motor 1",        NAST_MOTOR1,
  "motor 2",        NAST_MOTOR2,
  "motor 3",        NAST_MOTOR3,
  "sensorji",       NAST_SENZORJI,
};



s_ekranNastavitve motor1Nast =  //NAST_MOTOR1
{
  "Nast. motor1",
  //gor dol levo, desno, select
  MENI_NASTAVITVE,NAST_EDIT,DEC_IZBRAN,INC_IZBRAN, 0,
  //st parametrov, trenutno izbran, focus
  2,0,OFF,
  //opis, min, max, def, korak
  "zaprt:", 1000, 2000, 1500, 100,
  "odprt:", 1000, 2000, 1500, 100,
};

s_ekranNastavitve motor2Nast =  //NAST_MOTOR2
{
  "Nast. motor2",
  MENI_NASTAVITVE,NAST_EDIT,DEC_IZBRAN,INC_IZBRAN, 0,
  //st parametrov, trenutno izbran, focus
  2,0,OFF,
  //opis, min, max, def, korak
  "zaprt:", 1000, 2000, 1500, 100,
  "odprt:", 1000, 2000, 1500, 100,
};

s_ekranNastavitve motor3Nast =  //NAST_MOTOR3
{
  "Nast. motor3",
  //gor dol levo, desno, select
  MENI_NASTAVITVE,NAST_EDIT,DEC_IZBRAN,INC_IZBRAN, 0,
  //st parametrov, trenutno izbran, focus
  2,0,OFF,
  //opis, min, max, def, korak
  "zaprt:", 1000, 2000, 1500, 100,
  "odprt:", 1000, 2000, 1500, 100,
};

s_ekranNastavitve senzorjiNast =  //NAST_SENZORJI
{
  "Nast. senzorji",
  //gor dol levo, desno, select
  MENI_NASTAVITVE,NAST_EDIT,DEC_IZBRAN,INC_IZBRAN, 0,
  //st parametrov, trenutno izbran, focus
  2,0,OFF,
  //opis,    min   max,  def,  korak
  "obcut.:", 1000, 2000, 1500, 100,
  "zamik:",  100,  1000, 400,  100,
};

s_ekranNastavitveEdit editNast =  //NAST_EDIT
{
  0,
  //gor dol levo, desno, select
  0,0,NAST_EDIT_DEC,NAST_EDIT_INC, 0,
  //return state
  0
};

s_ekran igrajMenu = //MENI_IGRAJ
{
  "Izberi program:",
  //gor dol levo, desno, select
  MENI_GLAVNI,FUN_TIPKE,DEC_IZBRAN,INC_IZBRAN, 0,
  3,0,
  "program 1",        MENI_PROG1,
  "program 2",        MENI_PROG2,
  "program 3",        MENI_PROG3
};

s_ekran prog1Menu = //MENI_PROG1
{
  "Program 1:",
  //gor dol levo, desno, select
  MENI_IGRAJ,FUN_TIPKE,DEC_IZBRAN,INC_IZBRAN, 0,
  2,0,
  "zacni z igro",        PROGRAM_1,
  "nastavitve",        	 0,
};

s_ekran prog2Menu = //MENI_PROG2
{
  "Program 2:",
  //gor dol levo, desno, select
  MENI_IGRAJ,FUN_TIPKE,DEC_IZBRAN,INC_IZBRAN, 0,
  2,0,
  "zacni z igro",        PROGRAM_2,
  "nastavitve",        	 0,
};

s_ekran prog3Menu = //MENI_PROG3
{
  "Program 3:",
  //gor dol levo, desno, select
  MENI_IGRAJ,FUN_TIPKE,DEC_IZBRAN,INC_IZBRAN, 0,
  2,0,
  "zacni z igro",        PROGRAM_3,
  "nastavitve",        	 0,
};

s_ekranProgram program1 =
{
  0,
  //gor dol levo, desno, select
  MENI_PROG1,0,0,0,0,
};

s_ekranProgram program2 =
{
  0,
  //gor dol levo, desno, select
  MENI_PROG2,0,0,0,0,
};

s_ekranProgram program3 =
{
  0,
  //gor dol levo, desno, select
  MENI_PROG3,0,0,0,0,
};

*///////////////////////////////////////////////////////////

/*
  motor1.writeMicroseconds(1900);
  motor2.writeMicroseconds(1900);
  motor3.writeMicroseconds(1900);
  delay(3000);
  motor1.writeMicroseconds(1000);
  motor2.writeMicroseconds(1000);
  motor3.writeMicroseconds(1000);
  delay(3000);
    char casAray[10]; 
   int cas = 10;
   
   lcd.cursorTo(1,0);
   
   while(!get_LCD_button(btnUP));
   
   
   //  lcd.printIn("Igra se zacne v",-1);
   //  
   //  izpis[7] = '3';
   //  lcd.cursorTo(2, 0);
   //  lcd.printIn(izpis,16);
   //  piskacOn(3,150);
   //  delay(760);
   //  
   //  izpis[7] = '2';
   //  lcd.cursorTo(2, 0);
   //  lcd.printIn(izpis,16);
   //  piskacOn(3,150);
   //  delay(760);
   //  
   //  izpis[7] = '1';
   //  lcd.cursorTo(2, 0);
   //  lcd.printIn(izpis,16);
   //  piskacOn(3,150);
   //  delay(760);
   //  
   
   //  piskacOn(15,255);
   lcd.clear();
   tocke[0] = tocke[1] = tocke[2] = 0;
   char score[2];
   char scoreCel[17] = "1:0   2:0   3:0 ";
   
   lcd.cursorTo(2, 0);
   lcd.printIn(scoreCel,16);
   zacetniCas = millis();
   while(cas > (millis()-zacetniCas)/100){
   lcd.cursorTo(1, 0);
   lcd.printIn("Cas:",-1);
   
   //izpisi cas
   lcd.cursorTo(1, 5);
   intToArr(&casAray[0],cas - (millis()-zacetniCas)/100,1,6,' ');
   lcd.printIn(casAray, 6);
   
   //izpisi trenutni score tarc:
   lcd.cursorTo(2, 0);
   for(int i=0;i<3;i++){
   //intToArr(score, tocke[i],0,2,'0');
   itoa(tocke[i],score,10);
   for(int j=0;j<getDecLength(tocke[i]);j++){
   scoreCel[2+i*6+j] = score[j];
   }
   }
   lcd.cursorTo(2, 0);
   lcd.printIn(scoreCel,16);
   
   //    
   //    intToArr(score, tocke[0],0,3);
   //    
   //    lcd.printIn(score,-1);
   //    
   //    lcd.cursorTo(2, 6);
   //    intToArr(score, tocke[1],0,3);
   //    lcd.printIn(score,-1);
   //    
   //    lcd.cursorTo(2, 12);
   //    intToArr(score, tocke[2],0,3);
   //    lcd.printIn(score,-1);
   
   //    itoa(cas - (millis()-zacetniCas)/100,&casAray[0],10);
   //    lcd.cursorTo(2, 0);
   //    lcd.printIn(casAray, -1);
   }
   while(!get_LCD_button(btnUP)){
   //lcd.cursorTo(1,0);
   lcd.cursorTo(1, 0);
   lcd.printIn("Koncni rezultat:",-1);
   delay(5000);
   
   //lcd.cursorTo(1, 8- (strlen("Pritisni tipko  ")/2));
   lcd.cursorTo(1, 0);
   lcd.printIn("Pritisni tipko  ",-1);
   delay(1500);
   
   lcd.cursorTo(1, 0);
   lcd.printIn("     gor za     ",-1);
   delay(1500);
   
   lcd.cursorTo(1, 0);
   lcd.printIn(" nadaljevanje! ",-1);
   delay(2000);
   }
   */

  //lcd.clear();
  //lcd.printIn("YOU LOSE HA HA!",-1);

  //t1:00 t2:00 t3:#
  //################
  //000    000   000
  //0123456789012345
  //  if(get_LCD_button(btnUP)){
  //    piskacOn(1,100);
  //  }
  //  if(get_LCD_button(btnDOWN)){
  //    piskacOn(2,150);
  //  }
  //  if(get_LCD_button(btnRIGHT)){
  //    piskacOn(5,200);
  //  }
  //  if(get_LCD_button(btnLEFT)){
  //    piskacOn(10,255);
  //  }

  //  delay(5000);
  //  piskacOn(3,150);
  //  delay(960);
  //  piskacOn(3,150);
  //  delay(960);
  //  piskacOn(3,150);
  //  delay(960);
  //  piskacOn(15,255);

  /*while(1){
   
   myservo.writeMicroseconds(cas);
   cas+=100;
   delay(1000);
   if(cas > 1900)cas = 1000;
   
   }
   
   delay(2000);
   motor1.writeMicroseconds(1900);
   motor2.writeMicroseconds(1900);
   motor3.writeMicroseconds(1900);
   digitalWrite(PISKAC_PIN, HIGH);
   
   delay(2000);
   motor1.writeMicroseconds(1000);
   motor2.writeMicroseconds(1000);
   motor3.writeMicroseconds(1000);
   digitalWrite(PISKAC_PIN, LOW);
   */
  /*
  if(digitalRead(SENZOR2_PIN) == LOW){
   digitalWrite(PISKAC_PIN, HIGH);
   delay(1000);
   }else{
   digitalWrite(PISKAC_PIN, LOW);
   
   }
   */




































