#ifndef EEPROM_HELPERS_H
#define EEPROM_HELPERS_H

// EEPROM offsets
const uint16_t offsetFanSpeed   = maxProfiles * sizeof(Profile_t) + 1; // one byte
const uint16_t offsetProfileNum = maxProfiles * sizeof(Profile_t) + 2; // one byte
const uint16_t offsetPidConfig  = maxProfiles * sizeof(Profile_t) + 3; // sizeof(PID_t)

/**
   savePID
*/
bool savePID() {

  do {} while (!(eeprom_is_ready()));
  eeprom_write_block(&heaterPID, (void *)offsetPidConfig, sizeof(PID_t));
  return true;
}

/**
   loadPID
*/
bool loadPID() {

  do {} while (!(eeprom_is_ready()));
  eeprom_read_block(&heaterPID, (void *)offsetPidConfig, sizeof(PID_t));
  return true;
}

/**
   saveFanSpeed
*/
void saveFanSpeed() {

  EEPROM.write(offsetFanSpeed, (uint8_t)fanAssistSpeed & 0xff);
  delay(250);
}

/**
   loadFanSpeed
*/
void loadFanSpeed() {

  fanAssistSpeed = EEPROM.read(offsetFanSpeed) & 0xff;
}

/*
   saveLastUsedProfile
*/
void saveLastUsedProfile() {

  EEPROM.write(offsetProfileNum, (uint8_t)activeProfileId & 0xff);
}

/**
   loadParameters
*/
bool loadParameters(uint8_t profile) {

  uint16_t offset = profile * sizeof(Profile_t);

  do {} while (!(eeprom_is_ready()));
  eeprom_read_block(&activeProfile, (void *)offset, sizeof(Profile_t));
  return activeProfile.checksum == crc8((uint8_t *)&activeProfile, sizeof(Profile_t) - sizeof(uint8_t));
}

/**
   loadLastUsedProfile
*/
void loadLastUsedProfile() {

  activeProfileId = EEPROM.read(offsetProfileNum) & 0xff;
  loadParameters(activeProfileId);
}

/**
   saveParameters
*/
bool saveParameters(uint8_t profile) {

  uint16_t offset = profile * sizeof(Profile_t);

  activeProfile.checksum = crc8((uint8_t *)&activeProfile, sizeof(Profile_t) - sizeof(uint8_t));
  do {} while (!(eeprom_is_ready()));
  eeprom_write_block(&activeProfile, (void *)offset, sizeof(Profile_t));
  return true;
}

#endif EEPROM_HELPERS_H
