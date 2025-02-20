/*This code was created by H.I Electronic Tech channel
   I hope this code helps you in your projects
   if you want to support me, you could put a Like Share and subscribe to my channel
   here the youtube video if you have any question put it in the comments
   https://youtu.be/dXZiFx6RP6s

   SDA   D2
   SCK   D5
   MOSI  D7
   MISO  D6
   RST   D1
*/

#include <ESP8266WiFi.h>     //Include Esp library
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>        //include RFID library


#define SS_PIN D2 //RX slave select
#define RST_PIN D1
#define RedLed D3
#define GreenLed D4
#define relay D0
#define button D8

int btn_val=LOW;

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

/* Set these to your desired credentials. */
const char *ssid = "Bakayaro";  //ENTER YOUR WIFI SETTINGS
const char *password = "tanyabewok";

//Web/Server address to read/write from
const char *host = "192.168.0.5:8080";   //IP address of server

String getData , Link;
String CardID = "";

void setup() {
  delay(1000);
  Serial.begin(115200);
  SPI.begin();  // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  Serial.println("");
  Serial.println("");
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details

  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot

  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting to ");
  Serial.print(ssid);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.println("Connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

  pinMode(relay, OUTPUT);
  pinMode(RedLed, OUTPUT);
  pinMode(GreenLed, OUTPUT);
  pinMode(button, INPUT_PULLUP);

  digitalWrite(button, LOW);
  digitalWrite(relay, LOW);

}

void loop() {

  if (digitalRead(button) == HIGH){
    digitalWrite(RedLed, LOW);
    digitalWrite(GreenLed, HIGH);
    Serial.println("Button Pressed, Door Opened");
    digitalWrite(relay, HIGH);
    delay(5000);
    Serial.println("Door Closed");
    digitalWrite(relay, LOW);
    digitalWrite(GreenLed, LOW);
    delay(1000);
  }
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    Serial.print("Reconnecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("Connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());  //IP address assigned to your ESP
  }
  //look for new card
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;//got to start of loop if there is no card present
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    delay(5000);
    return;//if read card serial(0) returns 1, the uid struct contians the ID of the read card.
    digitalWrite(GreenLed, LOW);
    delay(5000);
    digitalWrite(GreenLed, HIGH);
  }

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    CardID += mfrc522.uid.uidByte[i];
  }

  HTTPClient http;    //Declare object of class HTTPClient

  //GET Data
  getData = "?CardID=" + CardID;  //Note "?" added at front
  Link = "http://192.168.0.5:8080/loginsystem/postdemo.php" + getData;

  http.begin(Link);

  int httpCode = http.GET();            //Send the request
  delay(10);
  String payload = http.getString();    //Get the response payload

  Serial.println("");
  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload
  Serial.println(CardID);     //Print Card ID
  
  

  if (payload == "login") {
    digitalWrite(RedLed, LOW);
    digitalWrite(GreenLed, HIGH);
    Serial.println("Door Opened");
    digitalWrite(relay, HIGH);
    delay(5000);
    Serial.println("Door Closed");
    digitalWrite(relay, LOW);
    digitalWrite(GreenLed, LOW);
  }
  else if (payload == "successful" || payload == "Cardavailable1") {
    digitalWrite(GreenLed, LOW);
    digitalWrite(RedLed, HIGH);
    delay(1000);
  }
  else if (payload == "NotAllow") {
    for (int j = 0; j < 5; j++) {
      digitalWrite(GreenLed, LOW);
      digitalWrite(RedLed, LOW);
      delay(200);
      digitalWrite(GreenLed, HIGH);
      digitalWrite(RedLed, HIGH);
    }
  }
  
  delay(1000);

  CardID = "";
  getData = "";
  Link = "";
  http.end();  //Close connection
  Serial.println("Conn closed");


  digitalWrite(RedLed, LOW);
  digitalWrite(GreenLed, LOW);
}
//=======================================================================
