//#include "/Users/sergio.zambrano/Library/Arduino15/packages/esp32/hardware/esp32/1.0.2/cores/esp32/Arduino.h"
//#include "/Users/sergio.zambrano/Library/Arduino15/packages/esp32/hardware/esp32/1.0.2/cores/esp32/HardwareSerial.h"
//#include "/Users/sergio.zambrano/Library/Arduino15/packages/esp32/hardware/esp32/1.0.2/variants/doitESP32devkitV1/pins_arduino.h"

#define DEBUG
#ifdef DEBUG
    #include </Users/sergio.zambrano/Documents/Arduino/libraries/RTClib/RTClib.h>
    #include </Users/sergio.zambrano/Documents/Arduino/libraries/DFRobot_SHT20-master/DFRobot_SHT20.h>
//    #include "src/Adafruit_BusIO/Adafruit_I2CDevice.h"
//    #include "src/Adafruit_BusIO/Adafruit_I2CRegister.h"
//    #include "src/Adafruit_BusIO/Adafruit_SPIDevice.h"
//    #include "src/Adafruit_BusIO/Adafruit_BusIO_Register.h"
    #include </Users/sergio.zambrano/Documents/Arduino/libraries/Adafruit_VEML6075_Library/Adafruit_VEML6075.h>
    #include </Users/sergio.zambrano/Documents/Arduino/libraries/Adafruit_BMP280_Library/Adafruit_BMP280.h>
    #include </Users/sergio.zambrano/Library/Arduino15/packages/esp32/hardware/esp32/1.0.2/libraries/BluetoothSerial/src/BluetoothSerial.h>
#else
    #include <RTClib.h>
    #include <DFRobot_SHT20.h>
    #include <Adafruit_VEML6075.h>
    #include <Adafruit_BMP280.h>
    #include "BluetoothSerial.h"
#endif

RTC_DS3231 rtc;
DFRobot_SHT20 sht20;
Adafruit_VEML6075 uv = Adafruit_VEML6075();
Adafruit_BMP280 bmp;
BluetoothSerial Bluetooth;

String daysOfTheWeek[7] = { "Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado" };
String monthsNames[12] = { "Enero", "Febrero", "Marzo", "Abril", "Mayo",  "Junio", "Julio","Agosto","Septiembre","Octubre","Noviembre","Diciembre" };
void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(LED_BUILTIN, OUTPUT);

    if (!rtc.begin()) {
        Serial.println(F("Couldn't find RTC"));
        while (1);
    }

    // Si se ha perdido la corriente, fijar fecha y hora
    if (rtc.lostPower()) {
        // Fijar a fecha y hora de compilacion
        //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

        // Fijar a fecha y hora específica. En el ejemplo, 21 de Enero de 2016 a las 03:00:00
        // rtc.adjust(DateTime(2016, 1, 21, 3, 0, 0));
    }

    sht20.initSHT20();
    delay(100);
    sht20.checkSHT20();

    if (! uv.begin()) {
        Serial.println("Failed to communicate with VEML6075 sensor, check wiring?");
    }
    uv.setIntegrationTime(VEML6075_100MS);
    uv.setHighDynamic(true);
    uv.setForcedMode(false);
    uv.setCoefficients(2.22, 1.33,  // UVA_A and UVA_B coefficients
                       2.95, 1.74,  // UVB_C and UVB_D coefficients
                       0.001461, 0.002591); // UVA and UVB responses

    if (!bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID)) {
        Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
        while (1);
    }
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                    Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

    Bluetooth.begin("Estacion_meteorologica");
}

void printDate(DateTime date)
{
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
    Serial.println();
}

void printHumidity()
{
    float humd = sht20.readHumidity();                  // Read Humidity
    float temp = sht20.readTemperature();               // Read Temperature
    Serial.print("Temperature:");
    Serial.print(temp, 1);
    Serial.print("C");
    Serial.print(" Humidity:");
    Serial.print(humd, 1);
    Serial.print("%");
    Serial.println();
}

void printUVA()
{
    Serial.print("UVA: ");
    Serial.print(uv.readUVA());
    Serial.print("  UVB: ");
    Serial.print(uv.readUVB());
    Serial.print("  UVINDEX: ");
    Serial.print(uv.readUVI());
    Serial.println();
}

void printPressure()
{
    Serial.print(F("Temperature = "));
    Serial.print(bmp.readTemperature());
    Serial.print(" *C . ");

    Serial.print(F("Pressure = "));
    Serial.print(bmp.readPressure());
    Serial.print(" Pa . ");

    Serial.print(F("Approx altitude = "));
    Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
    Serial.print(" m");

    Serial.println();
    Bluetooth.print("Temperatura: ");
    Bluetooth.print(bmp.readTemperature());
    Bluetooth.println();
}

void loop() {
    delay(3000);
    digitalWrite(LED_BUILTIN, HIGH);
    // Obtener fecha actual y mostrar por Serial
    DateTime now = rtc.now();
    printDate(now);
    printHumidity();
    printUVA();
    printPressure();
    Serial.println();
    digitalWrite(LED_BUILTIN, LOW);
}
