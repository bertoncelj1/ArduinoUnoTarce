#include <ATComandHandler.h>

#include <SoftwareSerial.h>


#define serialControl softwareSerial //serial ki se uporabla za debagirat
#define serialMaster  softwareSerial2 //serial s katerim se server pogovarja s masterjm -> duotom
#define serialWlan    Serial //serial ki se uporablja za komunikacijo z wlan modulom
#define WLAN_BAUD 115200 //hitrost prenosa med modulom in arduinotom


#define AP_SSID   "modulv2"     //ap ssid 
#define AP_PASS   "geslo1234"   //ap geslo
#define SRV_IP    "192.168.4.1" //IP lasten ip
#define SRV_PORT  "80"          //port TCP serverja

#define MAX_NAPRAV    15
#define NO_MSG_RST_TIME  60000  //naprava se restira ce ne pride sporocila v 60s




// software serial: TX = 10, RX = 11
SoftwareSerial softwareSerial(10,11);//za debugirat

// software serial: TX = 8, RX = 9
SoftwareSerial softwareSerial2(8,9);//uporablja za pogovarjat z masterjem


int resetPin = 4;
int statusLedPin = 13;
int aktivneNaprave = 0;
int statusPin = 6; //s tem pinom pove ali je na voljo za pogovor(HIGH), ali pa ne(LOW)
                    //ponavadi poklice trakrt ko gre v funkcije, kjer ne more odgovarjat

int state;

#define UNDEF -1 //neznana vrednost parametra
#define ON     1 
#define OFF    0


typedef struct {
  int score;       //tocke
  uint8_t state;   //ali je przgana ali ugasnjena
  uint8_t echo;    //ali takoj pozadetku sporoci svoje stanje
  uint8_t samoOff; //ali se sama ugasne po zadetku ?
} s_parametri;

typedef struct {
  int id;
  char mac[6];
  long lastSend;
  s_parametri param;
} s_naprava;


s_naprava *naprave[MAX_NAPRAV];

//osnovni parametri ob kreaciji naprave
const s_parametri DEF_PARAM = {
  0, UNDEF, UNDEF, UNDEF
};


//nastav naprave na osnovne vrednosti
void initNaprave(){
  int i = MAX_NAPRAV;
  while(i--){
    naprave[i] = (s_naprava *) malloc(sizeof(s_naprava));
    naprave[i]->id = -1;
    naprave[i]->lastSend = -1;
    newNapravaParam(i, DEF_PARAM);
  }
}

s_parametri zacasParam = {
  0,0,0,0
};

s_parametri fillZacasParam(int score, uint8_t state, uint8_t echo, uint8_t samoOff){
   zacasParam.score = score;
   zacasParam.state = state;
   zacasParam.echo = samoOff;
   zacasParam.score = score;
   return zacasParam;
}

void dodajNapravo(int id, char mac[6], long lastSend, int score, uint8_t state, uint8_t echo, uint8_t samoOff){
   dodajNapravo(id, mac, lastSend, fillZacasParam(score, state, echo, samoOff)); 
}
  
void dodajNapravo(int id, char mac[6], long lastSend, s_parametri param){
  //najprej pogleda ce napraa s temu MAC obstaja
  int i, j;
  int zeObstaja = 0;
  
 //pobrise vse obstojece naprave z istim id in MAC
  for(i=0; i<aktivneNaprave; i++){
    if(naprave[i]->id == id || primerjajMac(mac, naprave[i]->mac)){
      odstraniNapravo(i);
    }
  }
  
  if(aktivneNaprave < MAX_NAPRAV){
    naprave[aktivneNaprave]->id = id;
    prepisiMac(naprave[aktivneNaprave]->mac, mac);
    naprave[aktivneNaprave]->lastSend = lastSend;
    newNapravaParam(aktivneNaprave, param);
    aktivneNaprave++;
  }
  
}

void newNapravaParam(int id, s_parametri param){
    naprave[id]->param.score = param.score;
    naprave[id]->param.state = param.state;
    naprave[id]->param.echo = param.echo;
    naprave[id]->param.samoOff = param.samoOff;
}

//updatejta last send time
//vnre 0 ce ne najde naprave
int updateNapravoTime(int id){  
  int i;
  
  for(i=0; i<aktivneNaprave; i++){
    if(naprave[i]->id == id){
      naprave[i]->lastSend = millis();
      return 1;
    }
  }
  
  return 0;
}

void izpisiNaprave(){
  serialControl.print("Naprave,");
  serialControl.println(aktivneNaprave);
  
  int i;
  for(i=0; i<aktivneNaprave
; i++){
    
    serialControl.print(naprave[i]->id);
    serialControl.print(',');
    izpisiMac(naprave[i]->mac);
    serialControl.print(',');
    serialControl.println(naprave[i]->lastSend);
  }
  
  serialControl.println();
  
}

void izpisiMac(char mac[6]){
  int i;
  for(i=0; i<5; i++){
    serialControl.print((int)mac[i]);
    serialControl.print(':');
  }
  serialControl.print((int)mac[5]);
}

int primerjajMac(char mac1[6], char mac2[6]){
  int i = 6;
  while(i--){
    if(*mac1++ != *mac2++)return 0;
  }
  return 1; 
}

void prepisiMac(char macDest[6], char macSrc[6]){
  int i = 6;
  while(i--){
    *macDest++ = *macSrc++;
  }
}

void odstraniNapravo(int index){
  aktivneNaprave--;
  int i;
  if(index >= 0){
    serialControl.print("del npr, ");
    serialControl.println(index);
    for(i = index; i < aktivneNaprave; i++){
      naprave[i]->id = naprave[i+1]->id;
      naprave[i]->lastSend = naprave[i+1]->lastSend;
      prepisiMac(naprave[i]->mac, naprave[i+1]->mac);
    }
  }
  
}

int napravaObstaja(int id){
  int i;
  for(i=0; i<aktivneNaprave; i++){
    if(naprave[i]->id == id)return 1;  
  }
  return 0;
}

int getNapravoById(int id){
  int i;
  for(i=0; i<aktivneNaprave; i++){
    if(naprave[i]->id == id)return i;  
  }
  return -1;
}


enum states{
  ST_SLEEP,
  ST_GO_SLEEP,
  ST_RST,
  ST_CHK,
  ST_NAST,
  ST_AP,
  ST_SRV,
  ST_LISTEN,
};

void setup()
{
  
    pinMode(resetPin, OUTPUT);
    digitalWrite(resetPin, HIGH);
    
    pinMode(statusLedPin, OUTPUT);
    digitalWrite(statusLedPin, HIGH);
    
    pinMode(statusPin, OUTPUT);    //pove aktivnost serverja LOW->active, HIGH->inactive
    digitalWrite(statusPin, HIGH);
    
    
    timer2Init(); //zato da prikazuje signalizacijsko luck
    

    // initialize debug serial:
    serialControl.begin(9600);
    serialControl.println("\r\n\r\nServer is rolling!");
    
    //init serial
    serialMaster.begin(9600);
    serialMaster.println("\r\n\r\nServer at your command!");
    
    serialControl.listen();
    


    serialWlan.begin(WLAN_BAUD);
    
    ATHandlerInit(&softwareSerial); //inicilizira napravo za kumunikacijo z ESP8266
   
    
    initNaprave();//kreira vse naprave
    state = ST_RST;
}

//timer 2 se uporablaj za signalizacijsko lucko, ki utripa glede na stanje clienta
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

//timer2 overflow
int statusLed = HIGH;
volatile uint8_t tot_overflow = 0;
ISR(TIMER2_OVF_vect)
{
    // keep a track of number of overflows
    tot_overflow++;
    if(tot_overflow>=10){
      tot_overflow = 0;
      toggleLed();
    }
    
}

int time = 0;//trenutni preostali cas 
int statusLedOn = 0;
int statusLedOff = 0;
volatile int spreminjam = 0; 
//preklaplja med prizgano in ugasnjeno statusLed glede na podana parametra 
//statusLedOn in statusLedOff, ki povesta koliko ciklov bo statusLed prizgana in ugasnjena
//ce je parameter statusLedOn ali statusLedOff postavljen na vrednost ki je < 0 bo vedno v nasprotnem 
//stanju. Npr ce je statusLedOn < 0 bo statusLed vedno off. Ce sta oba paremetra na < 0 bo vrednost random
void toggleLed(){
  if(!spreminjam){//pogleda  ce se nastavitve ravno spreminjajo
    time--;
    //serialControl.println(time);
    if(time == 0){
      time = (statusLed == HIGH)? statusLedOff : statusLedOn;
      if(time > 0){
        //SerialControl.println('#');
        statusLed = (statusLed == HIGH)? LOW: HIGH;
        digitalWrite(statusLedPin,statusLed);
      }else{
        //if(statusLed == HIGH)serialControl.println("vedno bo HIGH");
        //else if(statusLed == LOW)serialControl.println("vedno bo LOW");
        //else serialControl.println("vedno bo ???");
      }
      // ce je time manjsi od nic pomeni da se statusLed ne sme zamenjati
      // if(time == 0) ne bo vec klical in se stanje led ne bo spreminjalo
    }
  }
}

//nastima koliko casa bo status led on in koliko cas off
// ce je parameter < 0 led nikoli ne bo v tem stanju
//ce sta oba < 0 se stanje led ne bo spreminjalo iz prejsnega nastavljenega stanja  
void setStatusLed(int onTime, int offTime){
  spreminjam = 1;
  statusLedOn = onTime;
  statusLedOff = offTime;
  if(onTime > 0){
    statusLed = HIGH;
    time=statusLedOn;
  }else{
    statusLed = LOW;
    time=statusLedOff;
  }
  time += 1;
  digitalWrite(statusLedPin,statusLed);
  spreminjam = 0;
}


void hardReset(){
    serialControl.println("#######hard rst!#######");
    digitalWrite(resetPin, LOW);
    delay(100);
    digitalWrite(resetPin, HIGH);

}

int setAPStation(int tries){
  
  while(tries--){
    posljiUkaz("AT+CWSAP=\""AP_SSID"\",\""AP_PASS"\",2,3");
    if(getOdgovor("OK", 3000)){
      serialControl.println("OK");
      return 1;
    }
    
  }
  
  return 0;

}

char *setSrvdg[2] = {"OK", "no change"};
int setServer(int tries){
  
  while(tries--){
    posljiUkaz("AT+CIPSERVER=1,"SRV_PORT);
    int odg = getOdgovorMulty(setSrvdg, 2, 3000);
    serialControl.println(setSrvdg[odg]);
    switch(odg){
      case 0:
      case 1:
        return 1;
    
    }
  }
  
  return 0;

}

int preveriNastavitve(int tries, s_nastParam **setArray, int setLen){
  int odg, sett, i;
  
  while(tries--){
    sett = 1;
    //gre cez vse podane nastavitve
    for(i=0; i<setLen; i++){
      if(nastaviNastavitve(setArray[i]) == 0){
        sett = 0;
      }
    }
    if(sett == 1)return 1;
    
  }
  
  return 0;
}

//=predeli Odgovor Send Clientu
void oosc(int odg, int id){
  if(odg == 2){
    int idNaprave = getNapravoById(id);
    if(idNaprave > 0){
      serialControl.print("ods npr.");
      serialControl.println(idNaprave);
      odstraniNapravo(idNaprave);
    }
  }
}

char masterMsg[100];
void poslusajMasterja(){
  int msgLen = 0;
  char *pmasterMsg = masterMsg;
  int idConn = 0; //id povezave na katero bomo poslali sporocilo
  int i;
  int value = 0; //prebrana vrednost posameznih parametrov
  
  //poslusa ce ma kej za povedat
  //serialControl.listen();
  while(serialControl.available() > 0){
    *pmasterMsg++ = serialControl.read();
    msgLen++;
  }
  *pmasterMsg++ = 0;
  
  //ponastavi pointer
  pmasterMsg = masterMsg;
  
  //izpise prebrano
  if(msgLen){
    serialControl.println();
    serialControl.print("++:");
    serialControl.print(msgLen);
    serialControl.print(":");
    serialControl.println(masterMsg);
  
    //ce se sporocilo zacne z X je namenjeno vsem drugace pa samo tistemu
    //katerega id je napisan pred sporocilom npr: 10on3000, naprava z id 10 on za 3000 casa  
    if(*pmasterMsg == 'x'){
      pmasterMsg ++;
      idConn = -1;
    }else if(*pmasterMsg >= '0' && *pmasterMsg <= '9'){
      do{
        idConn *= 10;
        idConn += *pmasterMsg - '0';
        pmasterMsg ++;
      }while(*pmasterMsg >= '0' && *pmasterMsg <= '9');
    }else{
      serialControl.println("napacni ukaz");
      return;
    }
    
    //ukaz wake up
    if(primerjajStr("start", pmasterMsg)){
      state = ST_CHK;
    }
    
    else if(primerjajStr("stop", pmasterMsg)){
      state = ST_GO_SLEEP;
    }
    
    else if(primerjajStr("state", pmasterMsg)){
      serialMaster.print("state:");
      int stateSend = 0;
      if(state == ST_SLEEP || state == ST_GO_SLEEP){
        stateSend = 2;
      }else if(state == ST_LISTEN){
        stateSend = 1;
      }
      serialMaster.println(stateSend);
    }
    
    //ukaz ON
    else if(primerjajStr("on", pmasterMsg)){
      value = getSt(pmasterMsg, "on%");
      serialControl.println("on");
      if(idConn == -1){
        for(i=0; i<aktivneNaprave; i++){
          oosc(posljiSporociloClientuArg(naprave[i]->id, "on%d", value), naprave[i]->id);
        }
      }else{
        if(napravaObstaja(idConn))oosc(posljiSporociloClientuArg(idConn, "on%d", value), idConn);
      }
    }

    //ukaz OFF
    else if(primerjajStr("off", pmasterMsg)){
      serialControl.println("off");
      if(idConn == -1){
        for(i=0; i<aktivneNaprave; i++){
          oosc(posljiSporociloClientu(naprave[i]->id, "off"), naprave[i]->id);
        }
      }else{
        if(napravaObstaja(idConn))oosc(posljiSporociloClientu(idConn, "off"), idConn);
      }
    }
    
    //vsem poslje ukaz samooff -> da se samodejno izklapljajo ob zadetku
    else if(primerjajStr("samoff", pmasterMsg)){
      value = getSt(pmasterMsg, "samoff%");
      if(value != 0 || value != 1){
        serialControl.println("napacni value samoff!");
        serialMaster.println("no");
        return;
      }
      if(idConn == -1){
        for(i=0; i<aktivneNaprave; i++){
          oosc(posljiSporociloClientuArg(naprave[i]->id, "samoff%d", value), naprave[i]->id);
        }
      }else{
        if(napravaObstaja(idConn))oosc(posljiSporociloClientuArg(idConn, "samoff%d", value), idConn);
      }
    }
    
    //ecno
    else if(primerjajStr("echo", pmasterMsg)){
      value = getSt(pmasterMsg, "echo%");
      if(value != 0 || value != 1){
        serialControl.println("napacni value echo!");
        serialMaster.println("no");
        return;
      }
      if(idConn == -1){
        for(i=0; i<aktivneNaprave; i++){
          oosc(posljiSporociloClientuArg(naprave[i]->id, "echo%d", value), naprave[i]->id);
        }
      }else{
        if(napravaObstaja(idConn))oosc(posljiSporociloClientuArg(idConn, "echo%d", value), idConn);
      }
    }
    
    
    //izpise vse naprave ki jih ima v imeniku
    else if(primerjajStr("dev", pmasterMsg)){
      long currMillis = millis();
      serialMaster.println(aktivneNaprave);
      if(idConn == -1){
        for(i=0; i<aktivneNaprave; i++){
          serialMaster.print(naprave[i]->id);
          serialMaster.print(" ");
          serialMaster.println(currMillis - naprave[i]->lastSend);
        }
      }else{
          serialMaster.print(naprave[idConn]->id);
          serialMaster.print(" ");
          serialMaster.println(currMillis - naprave[idConn]->lastSend);
      }
    }
    
    //debug output devices
    else if(primerjajStr("ddev", pmasterMsg)){
      izpisiNaprave();
    }
    
    //od naprave zahteva da ponovno poslje svoj mac adress
    else if(primerjajStr("mac", pmasterMsg)){
      if(idConn == -1){
        for(i=0; i<aktivneNaprave; i++){
          oosc(posljiSporociloClientu(naprave[i]->id, "mac"), naprave[i]->id);
        }
      }else{
        if(napravaObstaja(idConn))oosc(posljiSporociloClientu(idConn, "mac"), idConn);
      }
    }

  }

}

//dobi message ki so mu ga poslali clienti
char getMsgArr[100];
char prebrano2[100];
char *oblikaOdgovora = "+IPD,%i,%l:%m"; //%i->connId, %l->msglen, %m->message
int getMessage(int *connId, int *msgLen, char **pmsg, int timeout){

    //ponastavi parametre
    *connId = *msgLen = 0;
    *pmsg = getMsgArr;

    long tstart = millis(); //time start zacetek poslusanja
    long tcurrRead = millis(); //time start zacetek poslusanja

    char *pprebranoLast = prebrano2; //zadnji prebrani character
    char *pprebranoComp = prebrano2; //trenutni character ki ga preverjamo
    char *podgComp = oblikaOdgovora; //trenutno mesto v obliki odgovora ki ga preverjamo
    
    
    char *pgetMsgArr = getMsgArr; //prebrano sporocilo
    int dolzinaMsg = 0; //da ve kolk casa more se brt sporocilo

    int *treSt = 0; //trenutno prebrana stevilka

    int readState = 0;
    //poslusa vsaj za cas timeouta
    while (millis() - tstart < timeout) {
      
        while(serialWlan.available() > 0){
          *pprebranoLast++ = serialWlan.read();
        }


        //pogleda ce je ze kej prebral in kolk od tistega prebranega je ze primerjal
        if (pprebranoComp != pprebranoLast) {
            
            switch (readState) {
                case 0: //primerja prejete podatke pprebranoComp z pricakovanim odgovorom podgComp
                    if (*podgComp == *pprebranoComp) {
                        pprebranoComp++;
                        podgComp++;
                        if (*podgComp == 0) return 1;
                    }else if (*podgComp == '%') {
                        readState = 1;
                    }else{
                      //ce se nic ne ujema ponastavi pcompare, da spet od zacetka isce ujemanje
                      podgComp = oblikaOdgovora;
                      pprebranoComp++;
                    }
                    break;

                case 1: //prebere ker znak more vpisvt
                    switch (*podgComp++) {
                        case 'i': //beremo connection ID
                            readState = 2;
                            treSt = connId;
                            //serialControl.println("_i");
                            break;

                        case 'l': //beremo dolzino sporocila
                            readState = 2;
                            treSt = msgLen;
                            //serialControl.println("_l");
                            break;

                        case 'm': //beremo sporocilo
                            readState = 3;
                            dolzinaMsg = *msgLen;
                            //serialControl.print("_s,len:");
                            //serialControl.println(*msgLen);
                            break;
                    }
                    break;

                case 2: //prebere stevilko
                    if ('0' <= *pprebranoComp && *pprebranoComp <= '9') {
                        //zapovni si stevilko
                        *treSt *= 10; 
                        *treSt += *pprebranoComp - '0';
                        pprebranoComp++;
                    } else {
                        readState = 0;
                    }
                    break;

                case 3:
                    //prebere string tok casa kokr je msgLen
                    //ca mu dolzina sporocila to se dovoljuje prebere sporocilo
                    //bere do predtazadnjega, ko pride do zadnjega neha brat
                    if(dolzinaMsg > 1){ 
                        *pgetMsgArr++ = *pprebranoComp++;
                        dolzinaMsg--;
                    } else {
                        //pogleda ce je blo v dolzinaMsg 
                        if(dolzinaMsg > 0){
                          *pgetMsgArr++ = *pprebranoComp++;
                          dolzinaMsg--;
                        }
                        
                        *pgetMsgArr = 0;
                        
                        return 1;
                    }   
                break;
            }
        }
    }
    
    return 0;
}


//nastavitve potrebne za AP streznik in server
s_nastParam cipmux = {
  "AT+CIPMUX?",
  "+CIPMUX:_",
  1,
  "AT+CIPMUX=1"
};

s_nastParam cwmode = {
  "AT+CWMODE?",
  "+CWMODE:_",
  3,
  "AT+CWMODE=3"
};

s_nastParam *serverSettings[2] = {&cipmux, &cwmode};






char *odgovoriConnected[2] = {"+IPD,", "CLOSED"}; //mozni odgovoi potem ko je ze povezan s serverjem
long zadnjiConnChk = 0;//cas ko smo zadnjickrat preverili povezavo
long zadnjiZ = 0;
void loop(){
  
  switch(state){
    case ST_GO_SLEEP:
      //TODO izklopi wlan napravo
      serialControl.println("going to sleep ...");
      state = ST_SLEEP;
      break;
    
    case ST_SLEEP:
      //TODO: poslji ga spat
      if(millis() - zadnjiZ > 3000){
        serialControl.print('z');
        zadnjiZ = millis();
      }
      break;
    
    case ST_RST:
      hardReset();
      
    case ST_CHK://prever ali je naprava sploh priklopljena 
      setStatusLed(5,10);
      digitalWrite(statusPin, LOW);
      preveriOdzivnost();
      state = ST_NAST;
    
    case ST_NAST://preveri nastavitve naprave
      setStatusLed(1,5);
      serialControl.println("chk set.");
      digitalWrite(statusPin, LOW);
      if(!preveriNastavitve(5, serverSettings, 2)){
        serialControl.println("set. ERROR");
        state = ST_RST;
        break;
      }
      serialControl.println("set. OK");
      state = ST_AP;
    
    case ST_AP://preveri stanje AP postaje in jo poiskusa vspostaviti
      setStatusLed(1,10);
      serialControl.println("set AP station");
      digitalWrite(statusPin, LOW);
      if(!setAPStation(3)){
        serialControl.println("set AP ERROR");
        state = ST_RST;
        break;
      }
      serialControl.println("set AP OK");
      state = ST_SRV;
      
    case ST_SRV://vspostavi server
      setStatusLed(1,1);
      serialControl.println("set server");
      digitalWrite(statusPin, LOW);
      if(!setServer(3)){
        serialControl.println("set server");
        state = ST_RST;
        break;
      }
      serialControl.println("set server OK");
      setStatusLed(0,1);
      state = ST_LISTEN;
      zadnjiConnChk = millis();
      
    case ST_LISTEN://bere sporocila clientov
      int connId, msgLen;
      char *pmsg;
      
      //sam se resetira ce nekaj casa ne pride sporocila
      if(millis() - zadnjiConnChk > NO_MSG_RST_TIME){
        state = ST_RST;
        break;
      }
      
      //TODO tale delay onemogoca da bi sprejomov sporocila od masterja!
      if(getMessage(&connId, &msgLen, &pmsg, 100)){
        serialControl.print(connId);
        serialControl.print(',');
        serialControl.print(msgLen);
        serialControl.print(':');
        serialControl.println(pmsg);
        
        zadnjiConnChk = millis();
        
        if(primerjajStr(pmsg, "chk")){
          //update last time conntact
          if(!updateNapravoTime(connId)){
            //ce ni najdu naprave zahteva od naprave da poslje svoj MAC naslov
            delay(200); //TODO  nared da bo client zakoj po poslanem sporocilu znov poslusat
            posljiSporociloClientu(connId, "mac");
          }
          //delay(500);
          //posljiSporociloClientu(connId, "hi!");
          
        }else if(primerjajStr(pmsg, "mac:")){
          char mac[6];
          //prebere mac iz sporocila
          getStr(pmsg, "mac:%", mac,6);
          
          
          //doda napravo z prebranim mac naslovomf
          //dodajNapravo(connId, mac, millis());
          
          //izpise vse naprave
          serialControl.println("new dev.");
          izpisiNaprave();
        }
      }else{
        //serialControl.println("no msg.");
      }
      
      break;
      

  }//end switch
  
  digitalWrite(statusPin, HIGH);
  poslusajMasterja();
  
}

//vrne 1 ce najde string v stringu s1
//*poz opise kje se najaha iskano besedilo
//poz je naprimer "mac:%", funkcija bo poiskala stevilo ki je na mestu znaka %
//npr: s1="mac:4d:3a:f4:55:a1:11" bo funkcija vrnila "4d:3a:f4:55:a1:11"
int getStr(char *s1, char *poz, char *out, int outLen){
  int state = 0;
  *out = 0;//na prvo mesto zapise 0
  
  while(*s1 && *poz){
    switch(state){
      case 0:
        if(*poz == '%'){
          state = 1;
          break;
        }
        if(*s1 != *poz)return -2;
        s1++;
        poz++;
        break;
        
      case 1:
        if(outLen--){
          *out++ = *s1++;
        }
        break;

    }
  }
  
  return state;
  
}


//vrne stevilo ki ga najde v stringu s1
//*poz je pozicija stevila v besedilu s1
//poz je naprimer "visina%", funkcija bo poiskala stevilo ki je na mestu znaka %
//npr: s1="visina1234" bo funkcija vrnila 1234
int getSt(char *s1, char *poz){
  int st = 0;
  int state = 0;
  while(*s1 && *poz){
    switch(state){
      case 0:
        if(*poz == '%'){
          state = 1;
          break;
        }
        if(*s1 != *poz)return -2;
        s1++;
        poz++;
        break;
        
      case 1:
        if(*s1 >= '0' && *s1 <= '9'){
          st *= 10;
          st += *s1 - '0';
        }else{
          return st;
        }
        s1 ++;
        break;

    }
  }
  
  return st;
  
}
int primerjajStr(char *s1, char *s2){
  while(*s1 && *s2){
    if(*s1++ != *s2++)return 0;
  }
  return 1;
}


