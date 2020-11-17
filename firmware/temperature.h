#ifndef TEMPERATURE_H
#define TEMPERATURE_H

MAX6675 thermocouple(PIN_TC_CLK, PIN_TC_CS, PIN_TC_DO);

/**
   readThermocouple
*/
void readThermocouple() {

  uint8_t lcdState = digitalState(ILI9341_CS_PIN);
  digitalHigh(ILI9341_CS_PIN);
  digitalLow(PIN_TC_CS);
  delay(1);
  double reading = thermocouple.readCelsius() * TEMP_COMPENSATION;

  if (reading == NAN) {
    tcStat = 1;
  } else {
    temperature = reading;
    tcStat = 0;
  }
  digitalHigh(PIN_TC_CS);
  if (lcdState == 0) digitalLow(ILI9341_CS_PIN);
  else digitalHigh(ILI9341_CS_PIN);
}

#endif // TEMPERATURE_H
