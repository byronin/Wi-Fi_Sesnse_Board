#include "Adafruit_Sensor.h"  //- https://github.com/adafruit/Adafruit_Sensor
#include "Wire.h"
#include "Adafruit_MPU6050.h" //- https://github.com/adafruit/Adafruit_MPU6050
#include "Adafruit_BMP280.h"  //- https://github.com/adafruit/Adafruit_BMP280_Library
#include "DHT.h"              //- https://github.com/adafruit/DHT-sensor-library
#include "Adafruit_VL53L0X.h" //- https://github.com/adafruit/Adafruit_VL53L0X

Adafruit_MPU6050 mpu;

Adafruit_BMP280 bmp;

#define DHTPIN 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

Adafruit_VL53L0X lox ;
VL53L0X_RangingMeasurementData_t measure;

void setup() {
  Serial.begin(115200);

  board_begin();

}
void loop() {
  
  Serial.println(get_mm()); // vl53l0x distance level
  Serial.println(get_dht('h')); // humidity % level
  Serial.println(get_dht('t')); // temperature °C level from DHT
  Serial.println(get_dht('f')); // temperature °F level
  Serial.println(get_bmp('p')); // pressure hPa level
  Serial.println(get_bmp('a')); // altitude m level
  Serial.println(get_bmp('t')); // temperature °C level from BMP
  Serial.println(get_mpu('x')); // x acceleration
  Serial.println(get_mpu('y')); // y acceleration
  Serial.println("");
  delay(1000);
  
}



float get_mpu(char mpu_par) {
  float rtrn_val;
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  
  if (mpu_par == 'x') {
    rtrn_val = a.acceleration.x ;
  } else if (mpu_par == 'y') {
    rtrn_val = a.acceleration.y ;
  }
  return rtrn_val;

}

float get_bmp(char bmp_par) {
  float rtrn_val;
  if (bmp_par == 'p') {
    rtrn_val = bmp.readPressure() ;
  } else if (bmp_par == 'a') {
    rtrn_val = bmp.readAltitude(1013.25) ;
  } else if (bmp_par == 't') {
    rtrn_val = bmp.readTemperature();
  }
  return rtrn_val;
}


float get_dht(char dht_par) {

  float rtrn_val;

  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  if (dht_par == 'h') {
    rtrn_val = h ;
  } else if (dht_par == 't') {
    rtrn_val = t ;
  } else if (dht_par == 'f') {
    rtrn_val = f ;
  }
  return rtrn_val;
}


int get_mm() {
  int sens_mm ;
  lox.rangingTest(&measure, false);
  sens_mm =  measure.RangeMilliMeter;
  if (measure.RangeStatus != 4 && sens_mm != 8191) {

  } else {
    sens_mm = -1;
  }
  if (sens_mm < 700) {
    digitalWrite(15, HIGH);
  } else {
    digitalWrite(15, LOW);
  }
  if (sens_mm < 450) {
    digitalWrite(13, HIGH);
  } else {
    digitalWrite(13, LOW);
  }
  if (sens_mm < 300) {
    digitalWrite(12, HIGH);
  } else {
    digitalWrite(12, LOW);
  }
  if (sens_mm < 125) {
    digitalWrite(16, HIGH);
  } else {
    digitalWrite(16, LOW);
  }
  if (sens_mm == -1) {
    digitalWrite(16, LOW);
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);
    digitalWrite(15, LOW);
  }
  return sens_mm ;
}


void board_begin() {
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050");
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  delay(100);

  unsigned status;
  status = bmp.begin(BMP280_ADDRESS_ALT);
  if (!status) {
    Serial.println("Failed to find BMP280");
  }
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  delay(100);

  dht.begin();
  delay(100);

  pinMode(15, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(16, OUTPUT);
  if (!lox.begin()) {
    Serial.println("Failed to find VL53L0X");
  }
}
