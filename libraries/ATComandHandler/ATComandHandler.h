#ifndef ATComandHandler_h
#define ATComandHandler_h

#import <Arduino.h>
#include <SoftwareSerial.h>

//nastavitveni parametri za funkicjo nastaviNastavitve()
typedef struct {
  char* vprasaj;				//vprasanje ki ga da, da dobi podatek
  char* vprasajReadOdg;	//kaksen odgovor pricakuje
  int vrednost;					//vrednost ki jo pricakuje v odgovoru
  char* set;						//ukaz s katerim nastavi vrednost
} s_nastParam;


//primer:
/*
s_nastParam nast1 = {
  "AT+CIPMUX?",
  "+CIPMUX:_",
  1,
  "AT+CIPMUX=1"
}
*/

int preveriOdzivnost();
void ATHandlerInit(SoftwareSerial *ss);
int posljiAT();
void posljiUkaz(char *ukaz);
int getOdgovor(char *odg, long timeout);
int nastaviNastavitve(s_nastParam *nast);
int getNastavitve(char *odg, long timeout);
int pretovoriMacvInt(char *macStr, char *macint);
int getOdgovorMulty(char **odgovori, int stOdgovorov, long timeout);
int preberiPodatke(char *poz, int datalen, char* data, int timeout);
int getStrLength(char *str);
char* getMessage(long timeout);
int posljiSporocilo(char *msg);
int posljiSporociloClientu(int id, char *msg);
int posljiSporociloClientuArg(int id, char *msg, ...);
int getDecLength(long n);

#else
	#error Oops! Trying to include ATComandHandler.h on another device?
#endif
