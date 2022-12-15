/*********************************************************************************
 *  MIT License
 *  
 *  Copyright (c) 2020-2022 Gregg E. Berman
 *  
 *  https://github.com/HomeSpan/HomeSpan
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *  
 ********************************************************************************/

/*
 *  Temperature & Humidity Sensor with OLED Display and HomeKit
 *  Author: Harry Tung
 *          Modified from HomeSpan Example 12: Service Loops
 *  Date:   2022/12/14
 */

#include "HomeSpan.h" 
#include "DEV_Sensors.h" 

void setup() {
	Serial.begin(115200);

	displaySetup();										// Setup OLED Display
	sensorSetup();										// Setup DHT22 Sensor
  	delay(2000);

	homeSpan.begin(Category::Bridges,"HomeSpan Bridge");

  	new SpanAccessory();  
    	new Service::AccessoryInformation();
      	new Characteristic::Identify(); 
      
  	new SpanAccessory();
    	new Service::AccessoryInformation();
      		new Characteristic::Identify(); 
      		new Characteristic::Name("Temp Sensor");
    	new DEV_TempSensor();                       	// Create a Temperature Sensor

  	new SpanAccessory();
    	new Service::AccessoryInformation();
      		new Characteristic::Identify(); 
      		new Characteristic::Name("Humidity");  
    	new DEV_HumiditySensor();                   	// Create a Humidity Sensor
}

void loop(){
  	homeSpan.poll();
}
