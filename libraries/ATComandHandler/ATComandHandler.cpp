#include "ATComandHandler.h"
#include <SoftwareSerial.h>


#define serialControl  	(*softwareSer) 		//serial ki se uporabla za debagirat
#define serialWlan 			Serial 								//serial ki se uporablja za komunikacijo z wlan modulom


SoftwareSerial *softwareSer = 0;
char prebrano[100];//arr kamor se shranjuje odgovor wlanmodula
char odgovor[100]; //sem se shranjuje odgovor serverja

//privatne funkcije
int posljiSporociloClientuLen(int id, char *msg, int len);

//nastavi software serial communication
void ATHandlerInit(SoftwareSerial *ss){
	softwareSer = ss;
}

//preveri ali se wlan modul odziva ukazom
int preveriOdzivnost(){
  while(!posljiAT()){
    serialControl.println("naprava se ne odziva!");
    delay(2000);
  }
  serialControl.println("odziv ok");
}

//poslje AT ukaz
int posljiAT(){
  posljiUkaz("AT");
  if(getOdgovor("OK", 1000)){
    serialControl.println("OK");
    return 1;
  }else{
    serialControl.println("...");
    return 0;
  }
}

void posljiUkaz(char *ukaz){
  serialControl.print("--:");
  serialControl.print(ukaz);
  serialControl.print(' ');
  
  serialWlan.println(ukaz);

}


int getOdgovor(char *odg, long timeout){
  char *podg = odg; //pointer na oodgovor str
  int readIndex = 0;  //index do katerega smo prebrali
  int compIndex = 0;  //index do katerega mesta smo ze primerjali
  
  long tstart = millis(); //time start zacetek poslusanja
  
  char *pprebrano = prebrano; //nastavi pointer prebrano na zacetek
  
  //poslusa vsaj za cas timeouta
  while(millis() - tstart < timeout){
    //ce ima kaj za prebrat prebere
    while(serialWlan.available() > 0){
      *pprebrano++ = serialWlan.read();
      readIndex++;
    }
    
    //pogleda ce je ze kej prebral in kolk od tistega prebranega je ze primerjal
    if(compIndex < readIndex){
      //podg povecuje tolk cajta dokler ne pride do 0 -> to pomeni da je odgovor vsebovan
      //v odgovoru
      if(prebrano[compIndex] == *podg){
        podg ++;
        if(*podg == 0)return 1; 
      }else{
        podg = odg;
      }
      
      compIndex++;
    }
    
  }
  //primerja se tisti del ki ga ni mogel med zadanim timeoutom
  //naprimer da je biu predolgo v "while(serialWlan.available() > 0)" in ni mogel 
  //nic preverjati
  while(compIndex < readIndex){
    if(prebrano[compIndex] == *podg){
      podg ++;
      if(*podg == 0)return 1; 
    }else{
      podg = odg;
    }
    
    compIndex++;
  }

  return 0; 
}


//TODO: poprav, da bo ta funkcija ze sprot iskala besede
//dela cist isto kokr 'getOdgovor'  samo ta tuki priemrja vec odgovorov
//in poj vrne index tistega odgovora ka se je ujemov
//-2 ce nc ni prispelo in
//-1 ce se noben odgovor ni ujemal s prejetim
int getOdgovorMulty(char **odgovori, int stOdgovorov, long timeout){
  int i, j;
  
  //nastavi zacasne pointerje za vsak odgovor
  char *podg[stOdgovorov];
  for(i=0; i<stOdgovorov; i++){
    podg[i] = odgovori[i];
  }
  
  long tstart = millis(); //time start zacetek poslusanja
  
  char *pprebrano = prebrano; //nastavi pointer prebrano na zacetek
  //char *pcompare; //pointer ki kaze na mesto, ki ga preverjamo
  
  int readIndex = 0;
  int compIndex = 0;
  
  //poslusa vsaj za cas timeouta
  while(millis() - tstart < timeout){
    //ce ima kaj za prebrat prebere
    while(serialWlan.available() > 0){
      *pprebrano++ = serialWlan.read();
      readIndex++;
    }
    
    if(compIndex < readIndex){
      //podg povecuje tolk cajta dokler ne pride do 0 -> to pomeni da je odgovor vsebovan
      //v odgovoru
      for(i=0; i<stOdgovorov; i++){
        if(prebrano[compIndex] == *podg[i]){
          podg[i] ++;
          if(*podg[i] == 0)return i;
        }else{
          podg[i] = odgovori[i];
        }
      }
      compIndex++;
    }
     
  }

	
  //primerja se tisti del ki ga ni mogel med zadanim timeoutom
  //naprimer da je biu predolgo v "while(serialWlan.available() > 0)" in ni mogel 
  //nic preverjati
  while(compIndex < readIndex){
    if(compIndex < readIndex){
      //podg povecuje tolk cajta dokler ne pride do 0 -> to pomeni da je odgovor vsebovan
      //v odgovoru
      for(i=0; i<stOdgovorov; i++){
        if(prebrano[compIndex] == *podg[i]){
          podg[i] ++;
          if(*podg[i] == 0)return i;
        }else{
          podg[i] = odgovori[i];
        }
      }
      
      compIndex++;
    }
  }

  if(readIndex == 0)return -2;//nic ni prispelo v tem casu
  return -1; 
}

//drugi nacin getOdgovorMulty
/*
int getOdgovorMulty(char **odgovori, int stOdgovorov, long timeout){
  int i, j;
  
  //nastavi zacasne pointerje za vsak odgovor
  char *podg[stOdgovorov];
  for(i=0; i<stOdgovorov; i++){
    podg[i] = odgovori[i];
  }
  
  long tstart = millis(); //time start zacetek poslusanja
  
  char *pprebrano = prebrano; //nastavi pointer prebrano na zacetek
  char *pcompare; //pointer ki kaze na mesto, ki ga preverjamo
  
  //poslusa vsaj za cas timeouta
  while(millis() - tstart < timeout){
    //ce ima kaj za prebrat prebere
    while(serialWlan.available() > 0){
      *pprebrano++ = serialWlan.read();
    }
     
  }
  

  
  //sprehodi se cez vse podane besede
	for(i=0; i<stOdgovorov; i++){
		pcompare = prebrano;
		//vsako besedo primerja z celotnim doblejnim besedilom, ce je ta vsebovana vrne index te besede
		while(pcompare != pprebrano){
			
			if(*pcompare == *podg[i]){
					podg[i] ++;
					if(*podg[i] == 0)return i;
			}else{
					podg[i] = odgovori[i];//ponastavi pointer
			}
			pcompare++;
		}
		
	}  
  

  if(pprebrano == prebrano)return -2;//nic ni prispelo v tem casu
  return -1; 
}
*/

//poslusa kaj ima modul za povedati 
//vrne stevilko ki stoji na mesu '_'
//kar pomeni ce damo v odg 'STATUS:_'
//nam bo vrnil enomestno stevilko ki bo sledila 'STATUS:'
//npr ce je modul odgovori 'STATUS:3' funkcija vrne st. 3
//v primeru pa da ne najde zapisa 'STATUS:' vrne -1
int getNastavitve(char *odg, long timeout){
  char *podg = odg; //pointer na oodgovor str
  int readIndex = 0;  //index do katerega smo prebrali
  int compIndex = 0;  //index do katerega mesta smo ze primerjali
  
  long tstart = millis(); //time start zacetek poslusanja
  
  char *pprebrano = prebrano; //nastavi pointer prebrano na zacetek
  
  //poslusa vsaj za cas timeouta
  while(millis() - tstart < timeout){
    //ce ima kaj za prebrat prebere
    while(serialWlan.available() > 0){
      *pprebrano++ = serialWlan.read();
      readIndex++;
    }
    
    //pogleda ce je ze kej prebral in kolk od tistega prebranega je ze primerjal
    if(compIndex < readIndex){
      //podg povecuje tolk cajta dokler ne pride do 0 -> to pomeni da je odgovor vsebovan
      //v odgovoru
      if(*podg == '_')return prebrano[compIndex] - '0';
      if(prebrano[compIndex] == *podg){
        podg ++;
        if(*podg == 0)return -1; 
      }else{
        podg = odg;
      }
      
      compIndex++;
    }
    
  }
    
  //primerja se tisti del ki ga ni mogel med zadanim timeoutom
  //naprimer da je biu predolgo v "while(serialWlan.available() > 0)" in ni mogel 
  //nic preverjati
  while(compIndex < readIndex){
    if(*podg == '_')return prebrano[compIndex] - '0';
    if(prebrano[compIndex] == *podg){
      podg ++;
      if(*podg == 0)return -1; 
    }else{
      podg = odg;
    }
    
    compIndex++;
  }

  return -1; 

}


int getStrLength(char *str){
  int count = 0;
  while(*str++)count ++;
  return count;
}

//prebere sporocilo ki je bilo poslano od serverja, vrne pointer na prebranao sporocilo
//char *getMessageAT = "+IPD,"; //to se izpise pred vsakim prejetim sporocilom
char* getMessage(long timeout){
  //char *pmsgat = getMessageAT; //pointer na message AT string
  int stanje = 1;      //pove v katerem stanju branja sporocila .. branje cifre, ali branje dejanske vsebine
  int readIndex = 0;  //index do katerega smo prebrali
  int compIndex = 0;  //index do katerega mesta smo ze primerjali
  int odgovorIndex = 0; //index do kateerega mesta smo zapisali odgovor
  char* podgovor = &odgovor[0];//pointer na zapisano mesto v odgovoru
  int dolzina = 0; //dolzina prebranega sporocila
  char cifra; //zacasno shranjevalno mesto za prebrano stevilko
  
  long tstart = millis(); //time start zacetek poslusanja
  
  char *pprebrano = prebrano; //nastavi pointer prebrano na zacetek
  
  //poslusa vsaj za cas timeouta
  while(millis() - tstart < timeout){
    //ce ima kaj za prebrat prebere
    while(serialWlan.available() > 0){
      *pprebrano++ = serialWlan.read();
      readIndex++;
    }
    
    //pogleda ce je ze kej prebral in kolk od tistega prebranega je ze primerjal
 
  }
    
  //primerja se tisti del ki ga ni mogel med zadanim timeoutom
  //naprimer da je biu predolgo v "while(serialWlan.available() > 0)" in ni mogel 
  //nic preverjati

  while(compIndex < readIndex){
    switch(stanje){
      /*
      case 0:
        if(prebrano[compIndex] == *pmsgat){
          pmsgat ++;
          if(*pmsgat == 0){
            stanje = 1;
            break;             
          } 
        }else{
          pmsgat = getMessageAT;
        }
        break;
      */  
      case 1://v temu delu odgovora pricakuje dolzino sporocilazapisano v desetiski obliki. Po podani dolzini sledi znak ':' in nato podano sporocilo
        cifra = prebrano[compIndex];
        if(cifra >= '0' && cifra <= '9'){
          dolzina *= 10;
          dolzina += cifra - '0';
        }else if(cifra == ':'){
          stanje = 2;
          break;
        }else{
          return 0;
        }
        break;
      
      case 2://bere sporocilo
        //serialControl.print(prebrano[compIndex]);
        *podgovor++ = prebrano[compIndex];
        if(--dolzina == 0){
          *podgovor++ = 0;
          return &odgovor[0];
        }
        break;
    }
    
    compIndex++;
  }
  //sporocilo je bilo glede na podano dolzino prekratko
  return 0; 
}

//poslje sporocilo strezniku
int posljiSporocilo(char *msg){
  serialWlan.print("AT+CIPSEND=");
  serialWlan.println(getStrLength(msg));
  
  if(getOdgovor(">", 2000)){
    serialControl.print("posiljam=\"");
    serialControl.print(msg);
    serialControl.print("\"");
    serialWlan.println(msg);
  }else{
    return 0;
  }
  
  if(getOdgovor("SEND OK", 8000)){
    serialControl.println(" OK");
    return 1;
  }else{
    serialControl.println(" ERROR");
    return 0;
  }
  
  return 1;

}

int posljiSporociloClientu(int id, char *msg){
		return posljiSporociloClientuLen(id, msg, getStrLength(msg));
}

char *posljiSporOdg[2] = {">", "link is not"};
//server poslje sporocilo clientu
//vrne:
//2 -> povezava ne obstaja
//1 -> vse OK
//0 -> napaka
int posljiSporociloClientuLen(int id, char *msg, int len){
  serialWlan.print("AT+CIPSEND=");
  serialWlan.print(id);
  serialWlan.print(',');
  serialWlan.println(len);
  
  int odg = getOdgovorMulty(posljiSporOdg, 2, 2000);
  if(odg == 0){
    serialControl.print("send,");
    serialControl.print(id);
    serialControl.print("=\"");
    serialControl.print(msg);
    serialControl.print("\"");
    serialWlan.println(msg);
  }else if(odg == 1){
		serialControl.println(posljiSporOdg[odg]);
    return 2;
  }else{
		serialControl.println(" ERROR");
		return 0;
	}
  
  if(getOdgovor("SEND OK", 8000)){
    serialControl.println(" OK");
    return 1;
  }else{
    serialControl.println(" ERROR");
    return 0;
  }
  
  return 0;

}

char sporocilo[100];
//server poslje sporocilo clientu s podanimi argumenti
int posljiSporociloClientuArg(int id, char *msg, ...){
	va_list arguments;
	// Initializing arguments to store all values after msg
	va_start(arguments, msg);
	
	//sestavi sporocilo
	int msgLen = 0;
	int state = 0;
	char *psporocilo = sporocilo;
	long treSt; //trenutna stevilka ki jo zelimo zapisati
	int treStLen;
	

	
	while(*msg != 0 || state != 0){
		switch (state){
			case 0:
				if(*msg == '%'){
						state = 1;
						msg ++;
				}else{
						*psporocilo++ = *msg++;
						msgLen ++;
				}
				break;
			
			case 1:
				if(*msg == 'd'){
					treSt = va_arg(arguments, int);
					if(treSt == 0){
							*psporocilo++ = '0';
							msgLen ++;
							state = 0;
					}else{
							treStLen = getDecLength(treSt);
							
							state = 2;
					}
				}
				if(*msg == 'l'){
					treSt = va_arg(arguments, long);
					if(treSt == 0){
							*psporocilo++ = '0';
							msgLen ++;
							state = 0;
					}else{
							treStLen = getDecLength(treSt);
							
							state = 2;
					}
				}
				if(*msg == 'c')state = 3;
				
				msg ++;
				break;
				
			case 2://zapise stevilko
				psporocilo += treStLen - 1;
				msgLen += treStLen;
				
				while(treSt > 0){
						*psporocilo-- = (treSt % 10) + '0';
						//serialControl.println(treSt % 10);
						treSt /= 10;
				}
				
				psporocilo += treStLen + 1;
				state = 0;
				break;
			
			case 3://zapise char
				*psporocilo++ = va_arg(arguments, int);
				msgLen++;
				state = 0;
				break;
		}
	}
	*psporocilo++ = 0;
	
	return posljiSporociloClientuLen(id, sporocilo, msgLen);
	
	/*
	posljiSporociloClientu()
  serialWlan.print("AT+CIPSEND=");
  serialWlan.print(id);
  serialWlan.print(',');
  serialWlan.println(msgLen);
  

  if(getOdgovor(">", 2000)){
    serialControl.print("send,");
    serialControl.print(id);
    serialControl.print("=\"");
    serialControl.print(sporocilo);
    serialControl.print("\"");
    
    serialWlan.println(sporocilo);
  }else{
    return 0;
  }
  
  if(getOdgovor("SEND OK", 8000)){
    serialControl.println(" OK");
    return 1;
  }else{
    serialControl.println(" ERROR");
    return 0;
  }
  
  return 1;
  * */

}



//nastavi nastavitve podane v paremetru
int nastaviNastavitve(s_nastParam *nast){
  //vprasas kasne nastavitve so treenutno nastavljene
  posljiUkaz(nast->vprasaj);
  //prebere odgovor in izve nastavitve
  int odg = getNastavitve(nast->vprasajReadOdg, 2000);
  serialControl.print(odg);
  //ce se nastavitve ujemajo z zeleno vrednostjo vrne 1
  if(odg == nast->vrednost){
    serialControl.println(" OK");
    return 1;
  }
  else {
    //ali je bila pri branju napaka ?
    if(odg == -1)serialControl.println(" ERROR");
    else {
      //poiskusi nastaviti nastavitve
      serialControl.println(" WRONG SETT");
      posljiUkaz(nast->set);
      if(getOdgovor("OK", 1000)){
        serialControl.println("OK");
        return 1;
      }
      else {
        serialControl.println("ERROR");
        return 0;
      }
    }
  }
  return 0;
}

//vrne 1 ce je uspesno prebral podatke, 0 ce nic ni nasel in -1 ce je bilo premalo poslanih podatkov glede na zahtevano dolzino datalen
//v char *poz je opisana lokacija podatkov, ki sledijo znaku %
//npr poz="+CIFSR:APMAC,%", bo funkcija v data zapisala datalen podatkov ki sledijo % znaku
//datalen more bit za 1 daljsi ker na koncu doda se 0 !!!
int preberiPodatke(char *poz, int datalen, char* data, int timeout){
  char *podg = poz; 	//pointer na oodgovor str
  int readIndex = 0;  //index do katerega smo prebrali
  int compIndex = 0;  //index do katerega mesta smo ze primerjali
  
  long tstart = millis(); //time start zacetek poslusanja
  int readState = 0;
  
  char *pprebrano = prebrano; //nastavi pointer prebrano na zacetek
  
  //poslusa vsaj za cas timeouta
  while(millis() - tstart < timeout){
    //ce ima kaj za prebrat prebere
    while(serialWlan.available() > 0){
      *pprebrano++ = serialWlan.read();
      readIndex++;
    }
    
    //pogleda ce je ze kej prebral in kolk od tistega prebranega je ze primerjal
    if(compIndex < readIndex){
			
			switch(readState){
				
				case 0: //isce znak '%' in primerja dobljeno besedilo z podg
					if(prebrano[compIndex] == *podg){
						podg ++;
						compIndex++;
						if(*podg == 0)return 1; 
					}else if(*podg == '%'){
						readState = 1;//gre v stanje branja besedila
						//serialControl.println("b");
					}else{//postavi pointer podg na zacetek
						podg = poz;
						compIndex++;	
					}
					break;
					
				case 1://bere besedilo in ga zapisuje v data
					//pogleda ce ce more prebrati vsaj 2 podatka
					if(datalen > 1){
							*data++ = prebrano[compIndex];
							compIndex ++;
							datalen--;
					}else{
							//prebere se zadnji podatek ce ta obstaja
							if(datalen > 0){
									*data++ = prebrano[compIndex];
									compIndex ++;
									datalen --;
							}
							//na koncu doda se 0
							*data = 0;
							return 1;
					}
					break;
			}
    
		}
    
  }
  //primerja se tisti del ki ga ni mogel med zadanim timeoutom
  //naprimer da je biu predolgo v "while(serialWlan.available() > 0)" in ni mogel 
  //nic preverjati
	while(compIndex < readIndex){
		
		switch(readState){
			
			case 0: //isce znak '%' in primerja dobljeno besedilo z podg
				if(prebrano[compIndex] == *podg){
					podg ++;
					if(*podg == 0)return 1; 
				}else if(*podg == '%'){
					readState = 1;//gre v stanje branja besedila
					
				}else{//postavi pointer podg na zacetek
					podg = poz;
					compIndex++;	
				}
				break;
				
			case 1://bere besedilo in ga zapisuje v data
				//pogleda ce ce more prebrati vsaj 2 podatka
				if(datalen > 1){
						*data++ = prebrano[compIndex];
						compIndex ++;
						datalen--;
				}else{
						//prebere se zadnji podatek ce ta obstaja
						if(datalen > 0){
								*data++ = prebrano[compIndex];
								compIndex ++;
								datalen --;
						}
						//na koncu doda se 0
						*data = 0;
						return 1;
				}
				break;
		}

	}

  //ce je readState == 1, pomeni da ni bilo poslanih dovolj podatkov, ali pa da je datalen predolg
	//zato vrne -1
	//0 pomeni da ni nic nasel
	return (readState == 1)? -1 : 0;
}   


//pretvori sestnajstiski znak v desetisko obliko
int hexCharToDec(char c16){
	if(c16 >= '0' && c16 <= '9')return c16 - '0';
	else if(c16 >= 'a' && c16 <= 'f')return c16 - 'a' + 10;
	else if(c16 >= 'A' && c16 <= 'F')return c16 - 'A' + 10;
	return -1;
}

//pricakovana olika macStr je "1a:fe:34:a0:bb:66" z dolzino 18 znakov
int pretovoriMacvInt(char *macStr, char *macint){
		int macLen = 6;
		char c16_1, c16_2;
		int c10_1, c10_2;
		
		int i;
		/*
		//izpise podani mac adrs.
		for(i=0; i<18; i++){
				serialControl.print(macStr[i]);
		}
		*/
		for(i=0; i<macLen; i+=1){
					c16_1 = *macStr++;
					c16_2 = *macStr++;
					//napacni podatki
					//preveri za vse razen za zadnjega
					if(i != macLen-1 && *macStr++ != ':') return 0;
					
					c10_1 = hexCharToDec(c16_1);
					c10_2 = hexCharToDec(c16_2);
					
					//napaka pri pretovorbi
					if(c10_1 == -1 || c10_2 == -1)return 0;
					
					*macint++ = (c10_1 << 4) + c10_2;
		}
		
		return 1;
		
} 


int getDecLength(long n){
if(n<100000){if(n<100){if(n<10)return 1;else return 2;}else{
if(n<1000)return 3;else{if(n<10000)return 4;else return 5;}}}else{
if(n<10000000){if(n<1000000)return 6;else return 7;}else{
if(n<100000000)return 8;else{if(n<1000000000)return 9;else return 10;}}}
}

    
    /*
    //pogleda ce je ze kej prebral in kolk od tistega prebranega je ze primerjal
    if(compIndex < readIndex){
      //podg povecuje tolk cajta dokler ne pride do 0 -> to pomeni da je odgovor vsebovan
      //v odgovoru
      for(i=0; i<stOdgovorov; i++){
        if(prebrano[compIndex] == *podg[i]){
          podg[i] ++;
          if(*podg[i] == 0){
						int vsiSoNic = 1;
						for(j=0; j<stOdgovorov; j++){
								if()
						}
            return i;
          } 
        }else{
          podg[i] = odgovori[i];
        }
      }
      
      compIndex++;
    }
    */
    
      /*
  //primerja se tisti del ki ga ni mogel med zadanim timeoutom
  //naprimer da je biu predolgo v "while(serialWlan.available() > 0)" in ni mogel 
  //nic preverjati
  while(compIndex < readIndex){
    if(compIndex < readIndex){
      //podg povecuje tolk cajta dokler ne pride do 0 -> to pomeni da je odgovor vsebovan
      //v odgovoru
      for(i=0; i<stOdgovorov; i++){
        if(prebrano[compIndex] == *podg[i]){
          podg[i] ++;
          if(*podg[i] == 0){
            return i;
          } 
        }else{
          podg[i] = odgovori[i];
        }
      }
      
      compIndex++;
    }
  }
  */
