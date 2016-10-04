
/*Program podpira dva programa PR_1 in PR_2 ki se spreminjata s stikalom

1 nano:
stikalo v položaju 1
ko fotodioda zazna laserski žarek, tarča pade. Dodati je potrebno še dve led diodi in piskač, kateri 4x utripneta in 4x zapiska ob zadetku. Po 10sek se tarča dvigne.

stikalo v položaju 2
led diodi gorita ko je tarča pokonci. Ko fotodiode zaznajo laser, tarča pade, diodi ugasneta in 4x zapiska. tarča se postavi pokonci po 10sek.

*/


#include <Servo.h> 
 
//pini
#define PIN_MOTOR  9
#define PIN_SENSOR A0
#define PIN_STIKALO 10
#define PIN_LED     11
#define PIN_PISKAC  12

//servo
#define SERVO_ON    120
#define SERVO_OFF   30
Servo myservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 

//mozna stanja 
#define PR_DAY    0
#define PR_NIGHT  1

#define ST_POKONCI      0 //samo poklicno stanje za init spremenljivke potem se takoj poklice _RUN stanje
#define ST_POKONCI_RUN  1
#define ST_PISKAJ       3
//#define ST_PISKAJ_RUN   3
#define ST_DOL          4

#define PR_1            1
#define PR_2            2

//nastavitve
#define TARCA_RST_TIME (5 * 1000) // 10s
#define ST_UTRIPOV     4

//ostale konstante
#define ON     1
#define OFF    0

int state = 0;          //pove kateri program je aktiven ali ST_NIGHT ali ST_DAY, 
int piskacState = OFF;  //stanje piskaca
int piskacCnt = 0;      //steje koliko casa je bil pisakc ze prizgan
int tarcaState = OFF;   //stanje tarce -> ali je gor ali dol
long millisHit = 0;     //cas ko je bila tarca zadeta
long millisPiski = 0;   //koliko casa je poteklo od zadnjega piska
int stPiskov = 0;       //uporablja da pove koliko piskov je preostalo
int program = PR_1;     //tip programa 1 ali 2, dolocen je z zunanjim stikalom priklopljen na pin "PIN_STIKALO"

void setup() 
{ 

  Serial.begin(9600);
  Serial.println("rst");

  myservo.attach(PIN_MOTOR);  // attaches the servo
  tarcaON();
  
  pinsInit();
  timer2Init();
  
  
  state = ST_POKONCI;
  dolociProgram(digitalRead(PIN_STIKALO));//doloci program glede na stikalo
  //attachInterrupt(1, blink2, RISING);
  //InitialiseInterrupt();
  //attachInterrupt(1, stikaloChange, CHANGE);
}

void loop() 
{ 
  switch(state){ 
    case ST_POKONCI:
      state = ST_POKONCI_RUN;
      tarcaON();
      //prizge ali pa ugasne ledico gleda na program
      if(program == PR_1)digitalWrite(PIN_LED, LOW);
      else if(program == PR_2)digitalWrite(PIN_LED, HIGH);
    
    case ST_POKONCI_RUN:
      if(analogRead(PIN_SENSOR) > 300){ 
        tarcaOFF();
        millisHit = millis();
        
        state = ST_PISKAJ;
        stPiskov = ST_UTRIPOV;
        millisPiski = 0;
        
        //neglede na program ugasne ledico
        digitalWrite(PIN_LED, LOW);
      }
    break;
    
    case ST_PISKAJ:
       if (millis() - millisPiski > 550){
         millisPiski = millis();
         if(stPiskov-- == 0){
           state = ST_DOL;
           break;
         }
         //prizge piskac z luck ali pa brez odvisno od programa
         if(program == PR_1) piskacOn(2, ON);
         else if(program == PR_2) piskacOn(2, OFF);
       }
       
    //tukej ni break stavka zato da preverja oba pogoja, v primeru da predolg piska
    
    case ST_DOL: // stanje trca dol
       if(millis() - millisHit > TARCA_RST_TIME){
         state = ST_POKONCI;
         digitalWrite(PIN_LED, LOW);
       }
    break;
  }
}

void tarcaON(){
  myservo.write(SERVO_ON);
  tarcaState = ON;
}

void tarcaOFF(){
  myservo.write(SERVO_OFF);
  tarcaState = OFF;
}

void piskacOn(int cas, int lucke){
  if(piskacState == OFF){
    digitalWrite(PIN_PISKAC, HIGH);
    if(lucke == ON)digitalWrite(PIN_LED, HIGH);
    //koliko casa bo piskac prizgan
    //podano v "deka sekundah", oz. st intervalov 
    //interupta
    piskacCnt = cas;
    piskacState = ON;
  }
}

void piskacOdstevaj(){
  if(piskacState == ON){
    //Serial.println(piskacCnt);
    if(piskacCnt <= 0){
      digitalWrite(PIN_PISKAC, LOW);
      digitalWrite(PIN_LED, LOW);
       piskacState = OFF;
    }
   piskacCnt --;
  }
}

void pinsInit(){
  pinMode(PIN_STIKALO, INPUT);
  pinMode(PIN_SENSOR, INPUT);
  
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_PISKAC, OUTPUT);
  
  digitalWrite(PIN_LED, LOW);
  digitalWrite(PIN_PISKAC, LOW); 
}


int staraVrednost = LOW;

void preberiStikalo(){
  int vrednost = digitalRead(PIN_STIKALO);
  
  // v primeru spremembe spremeni program in postavi stanje na zacetno mesto
  if(vrednost != staraVrednost){
    staraVrednost = vrednost;
    dolociProgram(vrednost);
    state = ST_POKONCI;
  }  
  
}

//doloci program glede na vrednost HIGH ali LOW
void dolociProgram(int vrednost){
    if(vrednost == HIGH){
       program = PR_1;
      //digitalWrite(PIN_LED, HIGH);
    }else{
       program = PR_2;
      //digitalWrite(PIN_LED, LOW);
    }
}

void timer2Init(){
  cli();//stop interrupts

  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register for 8khz increments
  //OCR2A = 249;// = (16*10^6) / (8000*8) - 1 (must be <256)

  //  TCCR2A |= (1 << WGM21);   // turn on CTC mode

  TIMSK2 |= (1 << TOIE2);// enable overflow interrupt
  TCCR2B |= (1 << CS22)|(1 << CS21)|(1 << CS20);    // Set CS21 bit for 1024 prescaler
  //TIMSK2 |= (1 << OCIE2A);  // enable timer compare interrupt
  sei();//allow interrupts

}


volatile uint8_t tot_overflow;
ISR(TIMER2_OVF_vect)
{
  // fajen je ce to mau hitrej dela 
  preberiStikalo();
  // keep a track of number of overflows
  tot_overflow++;
  if(tot_overflow >= 10){
    tot_overflow = 0;
    piskacOdstevaj();
    
    //Serial.print('t');
  }
}


