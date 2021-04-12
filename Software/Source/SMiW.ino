//Dołączenie bibliotek do obsługi wyświetlacza oraz WiFi
#include "DFRobot_LedDisplayModule.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

//WYŚWIETLACZ
DFRobot_LedDisplayModule LED(Wire, 0xE0);

//CZUJNIK WIĄZKI LASERA
const int Sensor = 19;

//DIODA
const int Led_Red = 18;
const int Led_Green = 5;
const int Led_Blue = 17;

//WiFi
const char *ssid = "ESP"; //nazwa
const char *password = "XXX"; //hasło
WiFiServer server(80);

//Zmienne pomocnicze
unsigned long start, finish;
int i, n;
String duration;


void setup() {

//Inicjalizacja WYŚWIETLACZA
  while(LED.begin8() != 0){
    delay(500);
  }
//Wyłączenie WYŚWIETLACZA
  LED.displayOff();

//Ustawienie CZUJNIKA WIĄZKI LASERA jako wejście
  pinMode(Sensor, INPUT);

//Ustawienie DIODY jako wyjście
  pinMode(Led_Red, OUTPUT); 
  pinMode(Led_Green, OUTPUT); 
  pinMode(Led_Blue, OUTPUT);
//Ustawienie koloru diody na zielony
  digitalWrite (Led_Red, LOW);
  digitalWrite (Led_Green, HIGH);
  digitalWrite (Led_Blue, LOW);
  
//Ustawienie zmiennych pomocniczych
  i = 0;
  duration = "...";

//Inicjalizacja WiFi
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  server.begin();

  delay(500);
}

void loop() {
//PROGRAM

  //Sprawdzanie czy wiązka lasera została przerwana (1 - wiązka została wykryta)
  while(digitalRead(Sensor) == 1){
  }
  //Jeśli zawodnik przekroczył linię startu - rozpoczyna się zliczanie czasu, DIODA ma kolor czerwony
  start = millis();
  digitalWrite (Led_Green, LOW);
  digitalWrite (Led_Blue, LOW);
  digitalWrite (Led_Red, HIGH);

  //Opóźnienie po przerwaniu wiązki, aby program działał poprawnie
  while(digitalRead(Sensor) == 0){ 
    delay(1000);
  }
  
  //Sprawdzanie czy wiązka lasera została przerwana po raz drugi
  while(digitalRead(Sensor) == 1){
  }

  //Zakończenie zliczania czasu, ustawienie DIODY na kolor niebieski
  finish = millis();
  digitalWrite (Led_Green, LOW);
  digitalWrite (Led_Blue, HIGH);
  digitalWrite (Led_Red, LOW);

  //Wyznaczenie z ilu cyfr składa się wynik
  n = finish - start;
  while(n != 0){
    n = n/10;
    i++;
  }

  //Ustawienie pozycji na WYŚWIETLACZU, w zależności od liczby cyfr
  switch (i) { 
    case 1: LED.setDisplayArea8(8); break;
    case 2: LED.setDisplayArea8(7,8); break;
    case 3: LED.setDisplayArea8(6,7,8); break;
    case 4: LED.setDisplayArea8(5,6,7,8); break;
    case 5: LED.setDisplayArea8(4,5,6,7,8); break;
    case 6: LED.setDisplayArea8(3,4,5,6,7,8); break;
    case 7: LED.setDisplayArea8(2,3,4,5,6,7,8); break;
    default: LED.setDisplayArea8(1,2,3,4,5,6,7,8); break;
  }  

  //Obliczenie czasu (wyniku zawodnika) oraz wypisanie go na WYŚWIETLACZU
  duration = finish - start;
  LED.print8(finish - start);
  LED.displayOn();

  //Nieskończona pętla - trwa dopóki automatyczny stoper nie zostanie zresetowany
  while(1){
    //Wywołanie funkcji WIFI 
    WIFI();
  }
}

//Funkcja odpowiadająca za wysłanie wyniku na urządzenie połączone ze stoperem za pomocą WiFi
void WIFI(){

  WiFiClient client = server.available();

  if (client) { 
    String currentLine = "";
    while (client.connected()) { 
      if (client.available()) {
        char c = client.read();
        if (c == '\n') { 

          if (currentLine.length() == 0) {

            //Zawartość HTTP
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.println("<!DOCTYPE html>");
            client.println("<html><body><script>$(document).ready(function() {setInterval('refreshPage()', 1000);});function refreshPage() { location.reload(); }</script><center>");
            client.println("<h1>AUTOMATYCZNY STOPER</h1>");
            //Wypisanie wyniku na stronie
            client.println("<h2>Czas: " + duration + " [ms]</h2>"); 
            client.println("</center></body></html>");

            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') { 
          currentLine += c; 
        }
      }
    }
    client.stop();
  }
  }
