// #include statement for the DHT11 weather sensor library
#include "Adafruit_DHT/Adafruit_DHT.h"

// #include statement for the Sparkfun Photon OLED Shield 
#include "SparkFunMicroOLED/SparkFunMicroOLED.h"
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

char Org[] = "VSLive";
char Disp[] = "Photon";
char Locn[] = "Brooklyn, NY";

void setup() {
    
    // Temperature sensor initialization
    dht.begin();
    delay(1000);
    
    oled.begin();     // Initialize the OLED
    oled.clear(ALL);  // Clear the display's internal memory
    oled.display();   // Display what's in the buffer (splashscreen)
    delay(1000);      // Delay 1000 ms
    oled.clear(PAGE); // Clear the buffer.
    
    printTitle("Weather!", 0);
}

void loop() {

    delay(2000);
    
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
    
    char payload[300];
    snprintf(payload, sizeof(payload), "{ \"s\":\"wthr\", \"u\":\"F\",\"l\":\"%s\",\"m\":\"Temperature\",\"o\":\"%s\",\"v\": %f,\"d\":\"%s\" }", Locn, Org, f, Disp);
    Spark.publish("weatherdata", payload);
    
    delay(5000);
    
    snprintf(payload, sizeof(payload), "{ \"s\":\"wthr\", \"u\":\"%%\",\"l\":\"%s\",\"m\":\"Humidity\",\"o\":\"%s\",\"v\": %f,\"d\":\"%s\" }", Locn, Org, h, Disp);
    Spark.publish("weatherdata", payload);

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