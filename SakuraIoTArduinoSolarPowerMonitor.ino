#include <TracerSolarChargeController.h>
#include <SakuraIO.h>

#define LED_PIN 10

TracerSolarChargeController chargeController(&Serial1);
SakuraIO_I2C sakuraio;

int sumCount = 0;
float sumPanelVoltage = 0;
float sumBatteryChargeAmpere = 0;
float sumBatteryVoltage = 0;
float sumChargeAmpere = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("start");
  pinMode(LED_PIN, OUTPUT);
  chargeController.begin();
  //for(;;){
  //  if( (sakuraio.getConnectionStatus() & 0x80) == 0x80 ) break;
  //  Serial.print(".");
  //  delay(1000);
  //}
  Serial.println("finished setup");
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  Serial.println("hoge");

  if (chargeController.update()) {
    chargeController.printInfo(&Serial);
    sumPanelVoltage += chargeController.panelVoltage;
    sumBatteryVoltage += chargeController.batteryVoltage;
    sumChargeAmpere += chargeController.chargeAmpere;
    sumCount ++;
  }
  //sumPanelVoltage += 30;
  //sumBatteryVoltage += 22;
  //sumChargeAmpere += 1.25;
  //sumCount ++;

  if (sumCount >= 6) {
    Serial.println("send to sakura");
    float panelVoltage = sumPanelVoltage / sumCount;
    float batteryVoltage = sumBatteryVoltage / sumCount;
    float chargeAmpere = sumChargeAmpere / sumCount;
    float chargeWatt = batteryVoltage * chargeAmpere;

    sakuraioSendChargeInfo(panelVoltage, batteryVoltage, chargeAmpere, chargeWatt);

    sumPanelVoltage = 0;
    sumBatteryVoltage = 0;
    sumChargeAmpere = 0;
    sumCount = 0;
  }

  delay(10000);
}

void sakuraioSendChargeInfo(float panelVoltage, float batteryVoltage, float chargeAmpere, float chargeWatt) {
  // Tx Queue
  sakuraio.enqueueTx((uint8_t)0, (float) panelVoltage);
  sakuraio.enqueueTx((uint8_t)1, (float) chargeAmpere);
  sakuraio.enqueueTx((uint8_t)2, (float) batteryVoltage);
  sakuraio.enqueueTx((uint8_t)3, (float) chargeWatt);
  sakuraio.send();
}

