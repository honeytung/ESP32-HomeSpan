/* 
 * Adafruit Temperature and Humidity Sensor DHT22 
 */
#include "DHT.h"

#define DHTPIN 4            // DHT Pin 
#define DHTTYPE DHT22       // DHT Type, DHT22 in this project

DHT dht(DHTPIN, DHTTYPE);


/* 
 * Adafruit Monochrome 0.91" 128x32 I2C OLED Display 
 */
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128        // OLED display width, in pixels
#define SCREEN_HEIGHT 32        // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library
// On an esp32S:    D21(SDA), D22(SCL)
#define OLED_RESET     35       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C     // < See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int tempDisplay = 0;            // Integer to store initial temperature value for display
int humidDisplay = 0;           // Integer to store initial humidity value for display
char totalLine[20] = {' '};     // String to store temperature and humidity data for drawing to display
int lineLength = 10;            // Length of line printed on OLED display


/*  Function:       displaySetup
 *  Input:          None
 *  Output:         None
 *  Description:    Setup the SSD1306 OLED display according to example from Adafruit
 *                  Modified 2022/12/14 from Adafruit Example Code SSD1306 I2C
 */
void displaySetup(void) {
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("Error: SSD1306 allocation failed!"));
        for(;;); // Don't proceed, loop forever
    }

    // Show initial display buffer contents on the screen
    display.display();
}


/*  Function:       sensorSetup
 *  Input:          None
 *  Output:         None
 *  Description:    Setup the Temperature and Humidity Sensor DHT22 according to example from Adafruit
 *                  Modified 2022/12/14 from Adafruit Example Code DHT Sensor Library
 */
void sensorSetup(void) {
    dht.begin();
}


/*  Function:       drawLine
 *  Input:          char[]
 *  Output:         None
 *  Description:    Prints a line of characters to 0.91" OLED Display
 *                  Modified 2022/12/14 from Adafruit Example Code SSD1306 I2C
 */
void drawLine(char line[]) {
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    delay(1000);
    display.clearDisplay();

    display.setTextSize(3);                 // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);    // Draw white text
    display.setCursor(0, 0);                // Start at top-left corner
    display.cp437(true);                    // Use full 256 char 'Code Page 437' font

    // Note: Library will draw what it can and the rest will be clipped
    for (int16_t i = 0; i < lineLength; i++) {
        if (line[i] == '\n') {
            display.write(' ');
        }
        else {
            display.write(line[i]);
        }
    }

    display.display();
    delay(1000);
    Serial.println(F("Display"));
}


/*  Function:       readTemperature
 *  Input:          None
 *  Output:         Temperature in float
 *  Description:    Returns the current temperature in celcius from DHT22 sensor and rounds the value for OLED printing
 */
float readTemperature(void) {
    float temperature = dht.readTemperature();

    if (isnan(temperature)) {
        Serial.println(F("Error: Failed to read Temperature!"));
        temperature = 23;
        tempDisplay = 23;

        return temperature;    
    }

    tempDisplay = round(temperature);
    return temperature;
}


/*  Function:       readHumidity
 *  Input:          None
 *  Output:         Humidity in float
 *  Description:    Returns the current humidity in percentage from DHT22 sensor and rounds the value for OLED printing
 */
float readHumidity(void) {
    float humidity = dht.readHumidity();

    if (isnan(humidity)) {
        Serial.println(F("Error: Failed to read Humidity!"));
        humidity = 23;
        humidDisplay = 23;

        return humidity;    
    }

    humidDisplay = round(humidity);
    return humidity;
}


/*  Struct:         DEV_TempSensor
 *  Description:    Service route for DHT22 temperature readings, DHT22 and SSD1306 OLED display must be initialized
 *                  first before executing
 *                  Modified 2022/12/14 from HomeSpan example 12: Service Loops
 *                  https://github.com/HomeSpan/HomeSpan/tree/master/examples/12-ServiceLoops
 */
struct DEV_TempSensor : Service::TemperatureSensor {        
    SpanCharacteristic *temp;                                       
  
    DEV_TempSensor() : Service::TemperatureSensor() {
        float temperature = readTemperature();

        sprintf(totalLine, "%iC %i%%\n", tempDisplay, humidDisplay);
        drawLine(totalLine);

        temp = new Characteristic::CurrentTemperature(temperature); 
        temp -> setRange(-40, 80);     
        
        Serial.print("Temperature Sensor Initialization Completed!");          
        Serial.print("\n");

    }

    void loop() {
        if (temp->timeVal() > 5000) {
            float temperature = readTemperature();

            sprintf(totalLine, "%iC %i%%\n", tempDisplay, humidDisplay);
            drawLine(totalLine);
      
            temp->setVal(temperature);
      
            LOG1("Temperature Update: ");
            LOG1(temperature);
            LOG1("\n");            
        }
    }
};
      

/*  Struct:         DEV_HumiditySensor
 *  Description:    Service route for DHT22 humidity readings, DHT22 and SSD1306 OLED display must be initialized
 *                  first before executing
 *                  Modified 2022/12/14 from HomeSpan example 12: Service Loops
 *                  https://github.com/HomeSpan/HomeSpan/tree/master/examples/12-ServiceLoops
 */
struct DEV_HumiditySensor : Service::HumiditySensor {
    SpanCharacteristic *relativeHumidity;                    
    
    DEV_HumiditySensor() : Service::HumiditySensor() {
        float humidity = readHumidity();

        sprintf(totalLine, "%iC %i%%\n", tempDisplay, humidDisplay);
        drawLine(totalLine);

        relativeHumidity = new Characteristic::CurrentRelativeHumidity(humidity);

        Serial.print("Humidity Sensor Initialization Completed!"); 
        Serial.print("\n");

    }

    void loop() {
        if (relativeHumidity->timeVal() > 5000) {
            float humidity = readHumidity();

            sprintf(totalLine, "%iC %i%%\n", tempDisplay, humidDisplay);
            drawLine(totalLine);

            relativeHumidity->setVal(humidity);       
        }
    }
};