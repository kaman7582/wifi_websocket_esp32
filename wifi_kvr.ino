#if 0
#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h>

const char * ssid="esp32";
const char * pws="11111111";

//WifiServer  serv(80);

#define my_output(fmt,args...) Serial.printf(fmt,##args)

WebSocketsServer webSocket = WebSocketsServer(80);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) 
{
    	switch(type) 
    {
		    case WStype_DISCONNECTED:
			    my_output("[WSc] Disconnected!\n");
			  break;

		    case WStype_CONNECTED: 
			    my_output("[WSc] Connected to url: %s\n", payload);
          webSocket.sendTXT(num,"Connected");
			    break;

		    case WStype_TEXT:
			    Serial.printf("[WSc] get text: %s\n", payload);
			    break;
          
		    case WStype_BIN:
			    Serial.printf("[WSc] get binary length: %u\n", length);
			    break;
	}
}

void setup() {
  // put your setup code here, to run once:
  int status=0;
  Serial.begin(115200);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);
  WiFi.softAP(ssid, pws);
  Serial.println();
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

int cli_con=0;

void loop() {
  // put your main code here, to run repeatedly:
  webSocket.loop();
  if(WiFi.softAPgetStationNum()>0 && cli_con == 0)
  {
      Serial.println("a client links:");
      Serial.println(WiFi.softAPgetStationNum());
      cli_con = 1;
  }

}
#endif


#include <Arduino.h>

#include <WiFi.h>
//#include <WiFiMulti.h>
//#include <WiFiClientSecure.h>

#include <WebSocketsServer.h>


WebSocketsServer webSocket = WebSocketsServer(81);
const char * ssid="lovechenxue";
const char * pws="11111111";

#define USE_SERIAL Serial

void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
	const uint8_t* src = (const uint8_t*) mem;
	USE_SERIAL.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
	for(uint32_t i = 0; i < len; i++) {
		if(i % cols == 0) {
			USE_SERIAL.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
		}
		USE_SERIAL.printf("%02X ", *src);
		src++;
	}
	USE_SERIAL.printf("\n");
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) 
    {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: 
          {
                IPAddress ip = webSocket.remoteIP(num);
                USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
				        // send message to client
				        webSocket.sendTXT(num, "Connected");
          }
            break;
        case WStype_TEXT:
            USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);

            // send message to client
            webSocket.sendTXT(num, payload);

            // send data to all connected clients
            // webSocket.broadcastTXT("message here");
            break;
        case WStype_BIN:
            USE_SERIAL.printf("[%u] get binary length: %u\n", num, length);
            hexdump(payload, length);

            // send message to client
            // webSocket.sendBIN(num, payload, length);
            break;
    		case WStype_ERROR:			
    		case WStype_FRAGMENT_TEXT_START:
    		case WStype_FRAGMENT_BIN_START:
    		case WStype_FRAGMENT:
    		case WStype_FRAGMENT_FIN:
    			break;
    }

}

void setup() {
    // USE_SERIAL.begin(921600);
    USE_SERIAL.begin(115200);

    //Serial.setDebugOutput(true);
    USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        USE_SERIAL.flush();
       // delay(1000);
    }

    //WiFiMulti.addAP("SSID", "11111111");

    //while(WiFiMulti.run() != WL_CONNECTED) {
      //  delay(100);
    //}
    WiFi.softAP(ssid, pws);
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

void loop() {
    webSocket.loop();
}
