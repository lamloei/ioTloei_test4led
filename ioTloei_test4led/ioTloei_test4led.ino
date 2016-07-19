#include <ESP8266WiFi.h>
#include <MicroGear.h>

int state = 0;
int wifi_delay = 500;

char ssid[20];
char password[20];
char netpie_appid[20];
char netpie_appkey[40];
char netpie_appsecret[60];
char netpie_appalias[20];

#define D4 2
#define D5 14
#define D6 12
#define D7 13

WiFiClient client;
MicroGear microgear(client);

void microgear_sendState() {
  String a = "/"+String(netpie_appalias)+"/state";
  char b[40];
  a.toCharArray(b,a.length()+1);

  if (digitalRead(D4)==HIGH)  { microgear.publish(b,"1"); }
  if (digitalRead(D4)==LOW)   { microgear.publish(b,"2"); }
  if (digitalRead(D5)==HIGH)  { microgear.publish(b,"3"); }
  if (digitalRead(D5)==LOW)   { microgear.publish(b,"4"); }
  if (digitalRead(D6)==HIGH)  { microgear.publish(b,"5"); }
  if (digitalRead(D6)==LOW)   { microgear.publish(b,"6"); }
  if (digitalRead(D7)==HIGH)  { microgear.publish(b,"7"); }
  if (digitalRead(D7)==LOW)   { microgear.publish(b,"8"); }
}

void microgear_updateIO() {
  Serial.print("Incoming message --> ");
  Serial.println(String(state));

  if (state==1) { digitalWrite(D4,HIGH); }
  if (state==2) { digitalWrite(D4,LOW); }
  if (state==3) { digitalWrite(D5,HIGH); }
  if (state==4) { digitalWrite(D5,LOW); }
  if (state==5) { digitalWrite(D6,HIGH); }
  if (state==6) { digitalWrite(D6,LOW); }
  if (state==7) { digitalWrite(D7,HIGH); }
  if (state==8) { digitalWrite(D7,LOW); }
}

void microgear_onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  String a;
  char m = *(char *)msg;
  if (m=='?') {
    microgear_sendState();
  } else {
    for (int i=0; i<msglen; i++) {
      a += String((char)msg[i]);
    }
    state = a.toInt();
    microgear_updateIO();
  }
}

void microgear_onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
}

void setup() {
/**** parameter ****/
strcpy(ssid,"<WIFI SSID>");
strcpy(password,"<WIFI PASSWORD>");
strcpy(netpie_appid,"<APPID>");
strcpy(netpie_appkey,"<KEY>");
strcpy(netpie_appsecret,"<SECRET>");
strcpy(netpie_appalias,"<ALIAS>");
/********/
  digitalWrite(D4,HIGH);
  digitalWrite(D5,HIGH);
  digitalWrite(D6,HIGH);
  digitalWrite(D7,HIGH);
  pinMode(D4,OUTPUT);
  pinMode(D5,OUTPUT);
  pinMode(D6,OUTPUT);
  pinMode(D7,OUTPUT);

  Serial.begin(115200);
  while(!Serial) {}
  Serial.println("Starting...");

  if (WiFi.begin((const char*)ssid,(const char*)password)) {
      while (WiFi.status() != WL_CONNECTED) {
          delay(wifi_delay);
          Serial.print(".");
      }
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  microgear.on(MESSAGE,microgear_onMsghandler);
  microgear.on(CONNECTED,microgear_onConnected);
  microgear.init(netpie_appkey,netpie_appsecret,netpie_appalias);
  microgear.connect(netpie_appid);
}

void loop() {
  if (microgear.connected()) {
    microgear.loop();
  }
  else {
    Serial.println("connection lost, reconnect...");
    microgear.connect(netpie_appid);
  }
}
