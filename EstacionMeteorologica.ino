const char* token = "YOUR TOKEN";
const char* ssid = "YOUR WIFI";
const char* password = "YOUR WIFI PASSWORD";

#define ARDUINO_IDE
#define LANGUAGE es
#define DOWN 18

#include "src/Translations/es_ES.h"
//#include "src/Translations/ca_ES.h"
#ifdef CLION_IDE
    #include </Users/sergio.zambrano/Documents/Arduino/libraries/RTClib/RTClib.h>
    #include </Users/sergio.zambrano/Documents/Arduino/libraries/DFRobot_SHT20-master/DFRobot_SHT20.h>
    #include </Users/sergio.zambrano/Documents/Arduino/libraries/Adafruit_VEML6075_Library/Adafruit_VEML6075.h>
    #include </Users/sergio.zambrano/Documents/Arduino/libraries/Adafruit_BMP280_Library/Adafruit_BMP280.h>
    #include </Users/sergio.zambrano/Library/Arduino15/packages/esp32/hardware/esp32/1.0.2/libraries/WiFi/src/WiFi.h>
    #include </Users/sergio.zambrano/Library/Arduino15/packages/esp32/hardware/esp32/1.0.2/libraries/HTTPClient/src/HTTPClient.h>
    #include </Users/sergio.zambrano/Library/Arduino15/packages/esp32/hardware/esp32/1.0.2/libraries/BluetoothSerial/src/BluetoothSerial.h>
#endif
#ifdef ARDUINO_IDE
    #include <RTClib.h>
    #include <DFRobot_SHT20.h>
    #include <Adafruit_VEML6075.h>
    #include <Adafruit_BMP280.h>
    #include <WiFi.h>
    #include <HTTPClient.h>
    #include "BluetoothSerial.h"
#endif

RTC_DS3231 rtc;
DFRobot_SHT20 sht20;
Adafruit_VEML6075 uv = Adafruit_VEML6075();
Adafruit_BMP280 bmp;
BluetoothSerial Bluetooth;
long cycle = 0;

bool hasRtc = false;
bool hasHumidity = false;
bool hasUltraviolet = false;
bool hasPressure = false;
bool hasSdCard = false;

DateTime date;
float humidity;
float temperature1;
float uva;
float uvb;
float uvi;
float pressure;
float altitude;
float temperature2;

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(DOWN, INPUT_PULLUP);

    if (rtc.begin()) {
        hasRtc = true;
    }

    // Si se ha perdido la corriente, fijar fecha y hora
    //if (rtc.lostPower()) {
    //    // Fijar a fecha y hora de compilacion
    //    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //
    //    // Fijar a fecha y hora específica. En el ejemplo, 21 de Enero de 2016 a las 03:00:00
    //    // rtc.adjust(DateTime(2016, 1, 21, 3, 0, 0));
    //}

    sht20.initSHT20();
    delay(100);
    sht20.checkSHT20();
    if (sht20.readHumidity() > 0.0 && sht20.readTemperature() > 0.0) {
        hasHumidity = true;
    }

    if (uv.begin()) {
        hasUltraviolet = true;
        uv.setIntegrationTime(VEML6075_100MS);
        uv.setHighDynamic(true);
        uv.setForcedMode(false);
        uv.setCoefficients(2.22, 1.33,  // UVA_A and UVA_B coefficients
                           2.95, 1.74,  // UVB_C and UVB_D coefficients
                           0.001461, 0.002591); // UVA and UVB responses
    }

    if (bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID)) {
        hasPressure = true;
        bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                        Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                        Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                        Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                        Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
    }

    WiFi.begin(ssid, password);
    Bluetooth.begin("Estacion_meteorologica");
}

void readSensors()
{
    if (hasRtc) {
        date = rtc.now();
    }

    if (hasHumidity) {
        humidity = sht20.readHumidity();
        temperature1 = sht20.readTemperature();
    }

    if (hasUltraviolet) {
        uva = uv.readUVA();
        uvb = uv.readUVB();
        uvi = uv.readUVI();
    }

    if (hasPressure) {
        pressure = bmp.readPressure();
        altitude = bmp.readAltitude(1013.25);
        temperature2 = bmp.readTemperature();
    }
}

void printSerialDate(DateTime date)
{
    Serial.print("Date: ");
    Serial.print(date.year(), DEC);
    Serial.print('/');
    Serial.print(date.month(), DEC);
    Serial.print('/');
    Serial.print(date.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[date.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(date.hour(), DEC);
    Serial.print(':');
    Serial.print(date.minute(), DEC);
    Serial.print(':');
    Serial.print(date.second(), DEC);
    Serial.print(" ");
}

void printSerialHumidity(float humidity, float temperature)
{
    Serial.print("Temperature: ");
    Serial.print(temperature, 1);
    Serial.print("C");
    Serial.print(" Humidity:");
    Serial.print(humidity, 1);
    Serial.print("%");
    Serial.print(" ");
}

void printSerialUVA(float uva, float uvb, float uvi)
{
    Serial.print("UVA: ");
    Serial.print(uva);
    Serial.print(" UVB: ");
    Serial.print(uvb);
    Serial.print(" UVINDEX: ");
    Serial.print(uvi);
    Serial.print(" ");
}

void printSerialPressure(float temperature, float pressure, float altitude)
{
    Serial.print(F("Temperature: "));
    Serial.print(temperature);
    Serial.print("C");

    Serial.print(F(" Pressure: "));
    Serial.print(pressure);
    Serial.print(" Pa");

    Serial.print(F(" Altitude: "));
    Serial.print(altitude);
    Serial.print("m");

    Serial.print(" ");
}

void printSerialData()
{
    if (hasRtc) {
        printSerialDate(date);
    }

    if (hasHumidity) {
        printSerialHumidity(humidity, temperature1);
    }

    if (hasUltraviolet) {
        printSerialUVA(uva, uvb, uvi);
    }

    if (hasPressure) {
        printSerialPressure(temperature2, pressure, altitude);
    }
}

void printWifiData()
{
    if ((WiFi.status() != WL_CONNECTED)) {
        Serial.println("wifi not connected");
        return;
    }

    String url = "https://api.thingspeak.com/update?api_key=" + token;
    char texto[20];

    sprintf(texto, "&field1=%0.2f", humidity);
    url = url + texto;
    sprintf(texto, "&field2=%0.2f", temperature1);
    url = url + texto;
    sprintf(texto, "&field3=%0.2f", uva);
    url = url + texto;
    sprintf(texto, "&field4=%0.2f", uvb);
    url = url + texto;
    sprintf(texto, "&field5=%0.2f", uvi);
    url = url + texto;
    sprintf(texto, "&field6=%0.2f", temperature2);
    url = url + texto;
    sprintf(texto, "&field7=%0.2f", pressure);
    url = url + texto;
    sprintf(texto, "&field8=%0.2f", altitude);
    url = url + texto;

    HTTPClient http;
    http.begin(url);

    int httpCode = http.GET();
    if (httpCode > 0) {
        String payload = http.getString();
    }

    http.end();
}

void printBluetoothData()
{
    Bluetooth.print("Temperature1: ");
    Bluetooth.print(temperature1);
    Bluetooth.println(" grados");
    Bluetooth.print("Temperature2: ");
    Bluetooth.print(temperature2);
    Bluetooth.println(" grados");
    Bluetooth.print("Humidity:     ");
    Bluetooth.print(humidity);
    Bluetooth.println(" %");
    Bluetooth.print("UVA:          ");
    Bluetooth.print(uva);
    Bluetooth.println();
    Bluetooth.print("UVB:          ");
    Bluetooth.print(uvb);
    Bluetooth.println();
    Bluetooth.print("UVI:          ");
    Bluetooth.print(uvi);
    Bluetooth.println();
    Bluetooth.print("Pression:     ");
    Bluetooth.print(pressure);
    Bluetooth.println(" Pa");
    Bluetooth.print("Altitude:     ");
    Bluetooth.print(altitude);
    Bluetooth.println(" metros");
    Bluetooth.println();
}

void loop()
{
    delay(1000);
    cycle++;
    if (cycle % 3 == 0) {
      digitalWrite(LED_BUILTIN, HIGH);
      readSensors();
      printSerialData();
      printBluetoothData();
      Serial.println();
      digitalWrite(LED_BUILTIN, LOW);
    }
    if (cycle % 20 == 0) {
      Serial.println("wifi data");
      digitalWrite(LED_BUILTIN, HIGH);
      printWifiData();
      digitalWrite(LED_BUILTIN, LOW);
    }
}
