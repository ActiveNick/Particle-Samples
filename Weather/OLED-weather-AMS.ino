// This #include statement was automatically added by the Particle IDE.
#include "SparkFunMicroOLED/SparkFunMicroOLED.h"

// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_DHT/Adafruit_DHT.h"

// This #include statement was automatically added by the Particle IDE.
#include "SparkTime/SparkTime.h"

// This #include statement was automatically added by the Particle IDE.
#include "HttpClient/HttpClient.h"

// Other libraries
#include "math.h"

#define DHTPIN 2    // what pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11		// DHT 11 
//#define DHTTYPE DHT22		// DHT 22 (AM2302)
//#define DHTTYPE DHT21		// DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht(DHTPIN, DHTTYPE);

// Declare a MicroOLED object. If no parameters are supplied, default pins are
// used, which will work for the Photon Micro OLED Shield (RST=D7, DC=D6, CS=A2)
MicroOLED oled;

char Org[] = "Mobility42";
char Disp[] = "Photon1";
char Locn[] = "Princeton, NJ";

String AzureMobileService = "mobiledemosjs.azure-mobile.net";
String AzureMobileServiceTable = "weathersensors";
char AzureMobileServiceKey[40] = "caiorjBnqOYgOmYdvBzRZQcxAtQFch58";
char deviceName[40] = "SparkCore"; 

UDP UDPClient;
SparkTime rtc;
HttpClient http;

void setup()
{
    rtc.begin(&UDPClient, "north-america.pool.ntp.org");
    rtc.setTimeZone(-5); // GMT offset
    Serial.begin(9600);
    
    // Temperature sensor initialization
    dht.begin();
    
    oled.begin();     // Initialize the OLED
    oled.clear(ALL);  // Clear the display's internal memory
    oled.display();   // Display what's in the buffer (splashscreen)
    delay(1000);      // Delay 1000 ms
    oled.clear(PAGE); // Clear the buffer.
    
    printTitle("Weather!", 0);
    
    //delay(10000);
}

 
void loop()
{
    delay(5000);
    
     // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a 
    // very slow sensor)
	float h = dht.getHumidity();
    // Read temperature as Celsius
    float t = dht.getTempCelcius();
    // Read temperature as Farenheit
	float f = dht.getTempFarenheit();
	
	oled.clear(PAGE);            // Clear the display
    oled.setCursor(0, 0);        // Set cursor to top-left
    oled.setFontType(0);         // Smallest font
    oled.print("WEATHER");         // Print "A0"
    //oled.setFontType(0);         // 7-segment font
    //oled.print("");               // Print a0 reading
    oled.setCursor(0, 16);       // Set cursor to top-middle-left
    oled.setFontType(0);         // Repeat
    oled.print("TEMP:");
    oled.setFontType(0);
    oled.print(f);
    oled.setCursor(0, 32);
    oled.setFontType(0);
    oled.print("HUMI:");
    oled.setFontType(0);
    oled.print(h);
    oled.display();
    
    unsigned long currentTime;
    currentTime = rtc.now();
    
    String timeNowString = rtc.ISODateUTCString(currentTime);
    char timeNowChar[sizeof(timeNowString)]; 
    strcpy(timeNowChar, timeNowString.c_str());
    
    char payload[200];
    //snprintf(payload, sizeof(payload), "{ \"temp\": \"76\", \"hmdt\": \"32\", \"subject\": \"wthr\", \"dspl\": \"%s\", \"time\": \"%s\" }", deviceName, timeNowChar);
    snprintf(payload, sizeof(payload), "{ \"sensor\":\"weather\", \"unit\":\"F\",\"location\":\"%s\",\"measure\":\"Temp\",\"org\":\"%s\",\"value\": %f,\"device\":\"%s\", \"time\": \"%s\" }", Locn, Org, f, Disp, timeNowChar);
    
    http_header_t headers[] = {
        { "X-ZUMO-APPLICATION", AzureMobileServiceKey },
        { "Cache-Control", "no-cache" },
        { NULL, NULL } // NOTE: Always terminate headers with NULL
    };
    
    http_request_t request;
    http_response_t response;
    
    request.hostname = AzureMobileService;
    request.port = 80;
    request.path = "/tables/" + AzureMobileServiceTable;
    request.body = payload;

    http.post(request, response, headers);
    Serial.print("Application>\tResponse status: ");
    Serial.println(response.status);

    Serial.print("Application>\tHTTP Response Body: ");
    Serial.println(response.body);

}

// Center and print a small title
// This function is quick and dirty. Only works for titles one
// line long.
void printTitle(String title, int font)
{
  int middleX = oled.getLCDWidth() / 2;
  int middleY = oled.getLCDHeight() / 2;

  oled.clear(PAGE);
  oled.setFontType(font);
  // Try to set the cursor in the middle of the screen
  oled.setCursor(middleX - (oled.getFontWidth() * (title.length()/2)),
                 middleY - (oled.getFontWidth() / 2));
  // Print the title:
  oled.print(title);
  oled.display();
  delay(1500);
  oled.clear(PAGE);
}