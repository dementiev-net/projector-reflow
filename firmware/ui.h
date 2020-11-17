#ifndef UI_H
#define UI_H

#define MENU_TEXT_XPOS 5
#define MENU_BAR_XPOS 3

#define TEMPERATURE_WINDOW 1.2 // n times the profile's maximum temperature

PDQ_ILI9341 tft; // PDQ: create LCD object (using pins in "PDQ_ILI9341_config.h")

// menu
Menu::Engine MenuEngine;
const uint8_t menuItemHeight = 28;
const uint8_t menuItemsVisible = 8;
bool menuUpdateRequest = true;
bool initialProcessDisplay = false;
// track menu item state to improve render preformance
typedef struct {
  const Menu::Item_t *mi;
  uint8_t pos;
  bool current;
} LastItemState_t;

LastItemState_t currentlyRenderedItems[menuItemsVisible];

// encoder
ClickEncoder Encoder(PIN_ENC_A, PIN_ENC_B, PIN_ENC_BTN, ENC_STEPS_PER_NOTCH, IS_ENC_ACTIVE);
int16_t encMovement;
int16_t encAbsolute;
int16_t encLastAbsolute = -1;

float pxPerSec;
float pxPerC;
uint16_t xOffset; // used for wraparound on x axis

/**
   setupTFT
*/
void setupTFT() {

  FastPin<ILI9341_RST_PIN>::setOutput();
  FastPin<ILI9341_RST_PIN>::hi();
  FastPin<ILI9341_RST_PIN>::lo();
  delay(1);
  FastPin<ILI9341_RST_PIN>::hi();
  tft.begin();
  tft.setTextWrap(false);
  tft.setTextSize(1);
  tft.setRotation(1);
  tft.fillScreen(ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
}

/**
   displaySplash
*/
void displaySplash() {

  tft.fillScreen(ILI9341_WHITE);
  tft.setTextColor(ILI9341_RED);
  // splash screen
  tft.setCursor(2, 30);
  tft.setTextSize(4);
  tft.print("Reflow");
  tft.setCursor(tft.width() - 240, 68);
  tft.print("Controller");
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(52, 67);
  tft.print("v"); tft.print(ver);
  tft.setCursor(0, 199);
  tft.print("(c) 2012-2013 Ed Simmons");
  tft.setCursor(0, 209);
  tft.print("(c) 2014 Karl Pitrich <karl@pitrich.com>");
  tft.setCursor(0, 219);
  tft.print("(c) 2017 David Sanz Kirbis");
  tft.setCursor(0, 229);
  tft.print("(c) 2019 PHPScript <info@phpscript.ru>");
  delay(3000);
}

/**
   displayError
*/
void displayError(int error) {

  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE, ILI9341_RED);
  tft.fillScreen(ILI9341_RED);
  tft.setCursor(10, 10);

  if (error < 9) {
    tft.println("Thermocouple Error");
    tft.setCursor(10, 30);
    switch (error) {
      case 0b001:
        tft.println("Open Circuit");
        break;
      case 0b010:
        tft.println("GND Short");
        break;
      case 0b100:
        tft.println("VCC Short");
        break;
    }
    tft.setCursor(10, 60);
    tft.println("Power off,");
    tft.setCursor(10, 75);
    tft.println("check connections");
  } else {
    tft.println("Temperature");
    tft.setCursor(10, 30);
    tft.println("following error");
    tft.setCursor(10, 45);
    tft.print("during ");
    tft.println((error == 10) ? "heating" : "cooling");
  }

  tone(PIN_BEEPER, BEEP_FREQ, 2000); //Error Beep
  while (1) { //  stop
    ;
  }
}

/**
   alignRightPrefix
*/
void alignRightPrefix(uint16_t v) {

  if (v < 1e2) tft.print(' ');
  if (v < 1e1) tft.print(' ');
}

/**
   displayThermocoupleData
*/
void displayThermocoupleData(uint8_t xpos, uint8_t ypos) {

  tft.setCursor(xpos, ypos);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  // temperature
  tft.setTextSize(4);
  alignRightPrefix((int)temperature);
  switch (tcStat) {
    case 0:
      tft.print((uint32_t)temperature);
      tft.print("\367C");
      break;
    case 1:
      tft.print("---");
      break;
  }
}

/*
   clearLastMenuItemRenderState
*/
void clearLastMenuItemRenderState() {

  // memset(&currentlyRenderedItems, 0xff, sizeof(LastItemState_t) * menuItemsVisible);
  for (uint8_t i = 0; i < menuItemsVisible; i++) {
    currentlyRenderedItems[i].mi = NULL;
    currentlyRenderedItems[i].pos = 0xff;
    currentlyRenderedItems[i].current = false;
  }
}

extern const Menu::Item_t miRampUpRate, miRampDnRate, miSoakTime,
       miSoakTempA, miSoakTempB, miPeakTime, miPeakTemp,
       miLoadProfile, miSaveProfile,
       miPidSettingP, miPidSettingI, miPidSettingD,
       miFanSettings;
/**
   menuExit
*/
bool menuExit(const Menu::Action_t a) {

  clearLastMenuItemRenderState();
  MenuEngine.lastInvokedItem = &Menu::NullItem;
  menuUpdateRequest = false;
  return false;
}

/**
   menuDummy
*/
bool menuDummy(const Menu::Action_t a) {
  return true;
}

/**
   printDouble
*/
void printDouble(double val, uint8_t precision = 1) {
  ftoa(buf, val, precision);
  tft.print(buf);
}

/**
   getItemValuePointer
*/
void getItemValuePointer(const Menu::Item_t *mi, double **d, int16_t **i) {

  if (mi == &miRampUpRate)  *d = &activeProfile.rampUpRate;
  if (mi == &miRampDnRate)  *d = &activeProfile.rampDownRate;
  if (mi == &miSoakTime)    *i = &activeProfile.soakDuration;
  if (mi == &miSoakTempA)    *i = &activeProfile.soakTempA;
  if (mi == &miSoakTempB)    *i = &activeProfile.soakTempB;
  if (mi == &miPeakTime)    *i = &activeProfile.peakDuration;
  if (mi == &miPeakTemp)    *i = &activeProfile.peakTemp;
  if (mi == &miPidSettingP) *d = &heaterPID.Kp;
  if (mi == &miPidSettingI) *d = &heaterPID.Ki;
  if (mi == &miPidSettingD) *d = &heaterPID.Kd;
  if (mi == &miFanSettings) *i = &fanAssistSpeed;
}

/**
   isPidSetting
*/
bool isPidSetting(const Menu::Item_t *mi) {
  return mi == &miPidSettingP || mi == &miPidSettingI || mi == &miPidSettingD;
}

/**
   isRampSetting
*/
bool isRampSetting(const Menu::Item_t *mi) {
  return mi == &miRampUpRate || mi == &miRampDnRate;
}

/**
   getItemValueLabel
*/
bool getItemValueLabel(const Menu::Item_t *mi, char *label) {

  int16_t *iValue = NULL;
  double  *dValue = NULL;
  char *p;

  getItemValuePointer(mi, &dValue, &iValue);
  if (isRampSetting(mi) || isPidSetting(mi)) {
    p = label;
    ftoa(p, *dValue, (isPidSetting(mi)) ? 2 : 1); // need greater precision with pid values
    p = label;
    if (isRampSetting(mi)) {
      while (*p != '\0') p++;
      *p++ = 0xf7; *p++ = 'C'; *p++ = '/'; *p++ = 's';
      *p = '\0';
    }
  } else {
    if (mi == &miPeakTemp || mi == &miSoakTempA || mi == &miSoakTempB ) {
      itostr(label, *iValue, "\367C");
    }
    if (mi == &miPeakTime || mi == &miSoakTime) {
      itostr(label, *iValue, "s");
    }
    if (mi == &miFanSettings) {
      itostr(label, *iValue, "%");
    }
  }
  return dValue || iValue;
}

/**
   menu_editNumericalValue
*/
bool menu_editNumericalValue(const Menu::Action_t action) {

  if (action == Menu::actionDisplay) {
    bool initial = currentState != Edit;
    currentState = Edit;
    tft.setTextSize(2);
    if (initial) {
      tft.setTextColor(ILI9341_BLUE, ILI9341_WHITE);
      tft.setCursor(MENU_TEXT_XPOS, 220);
      tft.print("Edit & click to save.");
      Encoder.setAccelerationEnabled(true);
    }
    for (uint8_t i = 0; i < menuItemsVisible; i++) {
      if (currentlyRenderedItems[i].mi == MenuEngine.currentItem) {
        uint8_t y = currentlyRenderedItems[i].pos * menuItemHeight + 2;
        if (initial) {
          tft.fillRect(115 + MENU_TEXT_XPOS, y - 1, 100, menuItemHeight - 2, ILI9341_RED);
        }
        tft.setCursor(120 + MENU_TEXT_XPOS, y + 4);
        break;
      }
    }
    tft.setTextColor(ILI9341_WHITE, ILI9341_RED);
    int16_t *iValue = NULL;
    double  *dValue = NULL;
    getItemValuePointer(MenuEngine.currentItem, &dValue, &iValue);
    if (isRampSetting(MenuEngine.currentItem) || isPidSetting(MenuEngine.currentItem)) {
      double tmp;
      double factor = (isPidSetting(MenuEngine.currentItem)) ? 100 : 10;
      if (initial) {
        tmp = *dValue;
        tmp *= factor;
        encAbsolute = (int16_t)tmp;
      }
      else {
        tmp = encAbsolute;
        tmp /= factor;
        *dValue = tmp;
      }
    }
    else {
      if (initial) encAbsolute = *iValue;
      else *iValue = encAbsolute;
    }
    getItemValueLabel(MenuEngine.currentItem, buf);
    tft.print(buf);
    tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  }
  if (action == Menu::actionParent || action == Menu::actionTrigger) {
    clearLastMenuItemRenderState();
    menuUpdateRequest = true;
    MenuEngine.lastInvokedItem = &Menu::NullItem;
    if (currentState == Edit) { // leave edit mode, return to menu
      if (isPidSetting(MenuEngine.currentItem)) {
        savePID();
      }
      else if (MenuEngine.currentItem == &miFanSettings) {
        saveFanSpeed();
      }
      // don't autosave profile, so that one can do "save as" without overwriting the current profile
      currentState = Settings;
      Encoder.setAccelerationEnabled(false);
      return false;
    }
    return true;
  }
}

/**
   factoryReset
*/
void factoryReset() {

  makeDefaultProfile();
  tft.fillScreen(ILI9341_BLUE);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(2);
  tft.setCursor(20, 210);
  tft.print("Resetting...");
  // then save the same profile settings into all slots
  for (uint8_t i = 0; i < maxProfiles; i++) {
    saveParameters(i);
  }
  fanAssistSpeed = FACTORY_FAN_ASSIST_SPEED;
  saveFanSpeed();
  heaterPID.Kp =  FACTORY_KP;// 0.60;
  heaterPID.Ki =  FACTORY_KI; //0.01;
  heaterPID.Kd = FACTORY_KD; //19.70;
  savePID();
  activeProfileId = 0;
  saveLastUsedProfile();
  delay(500);
}

/**
   menu_factoryReset
*/
bool menu_factoryReset(const Menu::Action_t action) {

  if (action == Menu::actionDisplay) {
    bool initial = currentState != Edit;
    currentState = Edit;
    if (initial) {
      tft.setTextColor(ILI9341_RED, ILI9341_WHITE);
      tft.setTextSize(2);
      tft.setCursor(29, 200);
      tft.print("Click to confirm");
      tft.setCursor(29, 218);
      tft.setTextSize(1);
      tft.print("Doubleclick to exit");
    }
  }
  if (action == Menu::actionTrigger) { // do it
    factoryReset();
    tft.fillScreen(ILI9341_WHITE);
    MenuEngine.navigate(MenuEngine.getParent());
    return false;
  }
  if (action == Menu::actionParent) {
    if (currentState == Edit) { // leave edit mode only, returning to menu
      currentState = Settings;
      clearLastMenuItemRenderState();
      return false;
    }
  }
}

/**
   memoryFeedbackScreen
*/
void memoryFeedbackScreen(uint8_t profileId, bool loading) {

  tft.fillScreen(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(10, 50);
  tft.print(loading ? "Loading" : "Saving");
  tft.print(" profile ");
  tft.setTextSize(4);
  tft.setTextColor(ILI9341_RED);
  tft.setCursor(200, 36);
  tft.print(profileId);
}

/**
   saveProfile
*/
void saveProfile(unsigned int targetProfile, bool quiet = false);

/**
   loadProfile
*/
void loadProfile(unsigned int targetProfile) {

  memoryFeedbackScreen(activeProfileId, true);
  bool ok = loadParameters(targetProfile);
  activeProfileId = targetProfile;
  saveLastUsedProfile();
  delay(1000);
}

/**
   menu_saveLoadProfile
*/
bool menu_saveLoadProfile(const Menu::Action_t action) {

  int h;
  bool isLoad = MenuEngine.currentItem == &miLoadProfile;
  if (isLoad) h = 94; else h = 122;
  if (action == Menu::actionDisplay) {
    bool initial = currentState != Edit;
    currentState = Edit;
    if (initial) {
      tft.fillRect(27, h, 240, 51, ILI9341_WHITE);
      tft.drawRoundRect(27, h, 240, 51, 2, ILI9341_BLUE);
    }
    tft.setTextColor(ILI9341_BLUE, ILI9341_WHITE);
    tft.setTextSize(2);
    if (initial) {
      encAbsolute = activeProfileId;
      tft.setCursor(35, h + 32);
      tft.print("Doubleclick to exit");
    }
    if (encAbsolute > maxProfiles) encAbsolute = maxProfiles;
    if (encAbsolute <  0) encAbsolute =  0;
    tft.setCursor(35, h + 4);
    tft.print("Click to ");
    tft.print((isLoad) ? "load " : "save ");
    tft.setTextColor(ILI9341_WHITE, ILI9341_RED);
    tft.print(encAbsolute);
  }
  if (action == Menu::actionTrigger) {
    (isLoad) ? loadProfile(encAbsolute) : saveProfile(encAbsolute);
    tft.fillScreen(ILI9341_WHITE);
    MenuEngine.navigate(MenuEngine.getParent());
    return false;
  }
  if (action == Menu::actionParent) {
    if (currentState == Edit) { // leave edit mode only, returning to menu
      currentState = Settings;
      clearLastMenuItemRenderState();
      return false;
    }
  }
}

/**
   toggleAutoTune
*/
void toggleAutoTune();

/**
   menu_cycleStart
*/
bool menu_cycleStart(const Menu::Action_t action) {

  if (action == Menu::actionDisplay) {
    startCycleZeroCrossTicks = zeroCrossTicks;
    menuExit(action);
    currentState = RampToSoak;
    initialProcessDisplay = false;
    menuUpdateRequest = false;
  }
  return true;
}

/**
   renderMenuItem
*/
void renderMenuItem(const Menu::Item_t *mi, uint8_t pos) {

  //ScopedTimer tm("  render menuitem");
  bool isCurrent = MenuEngine.currentItem == mi;
  uint8_t y = pos * menuItemHeight + 2;
  if (currentlyRenderedItems[pos].mi == mi
      && currentlyRenderedItems[pos].pos == pos
      && currentlyRenderedItems[pos].current == isCurrent)
  {
    return; // don't render the same item in the same state twice
  }
  tft.setCursor(MENU_TEXT_XPOS, y + 4);
  tft.setTextSize(2);
  // menu cursor bar
  tft.fillRect(MENU_BAR_XPOS, y - 2, tft.width() - 6, menuItemHeight, isCurrent ? ILI9341_BLUE : ILI9341_WHITE);
  if (isCurrent) tft.setTextColor(ILI9341_YELLOW, ILI9341_BLUE);
  else tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.print(MenuEngine.getLabel(mi));
  // show values if in-place editable items
  if (getItemValueLabel(mi, buf)) {
    tft.print(' '); tft.print(buf); tft.print("   ");
  }
  // mark items that have children
  if (MenuEngine.getChild(mi) != &Menu::NullItem) {
    tft.print(" \x10   "); // 0x10 -> filled right arrow
  }
  currentlyRenderedItems[pos].mi = mi;
  currentlyRenderedItems[pos].pos = pos;
  currentlyRenderedItems[pos].current = isCurrent;
}

/**
   drawInitialProcessDisplay
*/
void drawInitialProcessDisplay() {

  const uint16_t h =  tft.height() - 105;
  const uint16_t w = tft.width() - 24;
  const uint8_t yOffset =  30; // space not available for graph
  double tmp;
  initialProcessDisplay = true;

  tft.fillScreen(ILI9341_WHITE);
  tft.fillRect(0, 0, tft.width(), menuItemHeight, ILI9341_BLUE);
  tft.setCursor(4, 6);
  tft.print("Profile ");
  tft.print(activeProfileId);
  tmp = h / (activeProfile.peakTemp * TEMPERATURE_WINDOW) * 100.0;
  pxPerC = tmp;
  double estimatedTotalTime = 0;//60 * 12;
  // estimate total run time for current profile
  estimatedTotalTime = activeProfile.soakDuration + activeProfile.peakDuration;
  estimatedTotalTime += (activeProfile.peakTemp - temperature) / (float)activeProfile.rampUpRate;
  estimatedTotalTime += (activeProfile.peakTemp - temperature) / (float)activeProfile.rampDownRate;
  estimatedTotalTime *= 1.1; // add some spare
  tmp = w / estimatedTotalTime ;
  pxPerSec = (float)tmp;
  // 50°C grid
  int16_t t = (uint16_t)(activeProfile.peakTemp * TEMPERATURE_WINDOW);
  tft.setTextColor(tft.Color565(0xa0, 0xa0, 0xa0));
  tft.setTextSize(2);
  for (uint16_t tg = 0; tg < t; tg += 50) {
    uint16_t l = h - (tg * pxPerC / 100) + yOffset;
    tft.drawFastHLine(0, l + 38, tft.width(), tft.Color565(0xe0, 0xe0, 0xe0));
    tft.setCursor(tft.width() - 54, l + 23);
    alignRightPrefix((int)tg);
    tft.print((int)tg);
    tft.print("\367");
  }
}

/*
   updateProcessDisplay
*/
void updateProcessDisplay() {

  const uint16_t h = tft.height() - 105;
  const uint16_t w = tft.width() - 24;
  const uint8_t yOffset =  70; // space not available for graph
  uint16_t dx, dy;
  uint8_t y = 6;
  double tmp;

  // header & initial view
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLUE);
  tft.setTextSize(2);
  if (!initialProcessDisplay) {
    drawInitialProcessDisplay();
  }
  // elapsed time
  uint16_t elapsed = (zeroCrossTicks - startCycleZeroCrossTicks) / (float)(TICKS_PER_SEC);
  tft.setCursor(tft.width() - 60, y);
  alignRightPrefix(elapsed);
  tft.print(elapsed);
  tft.print("s");
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLUE);
  y += menuItemHeight + 2;
  displayThermocoupleData(1, y);
  tft.setTextSize(2);
  // current state
  y -= 2;
  tft.setCursor(tft.width() - 130, y);
  tft.setTextColor(ILI9341_BLACK, ILI9341_GREEN);
  switch (currentState) {
#define casePrintState(state) case state: tft.print(#state); break;
      casePrintState(RampToSoak);
      casePrintState(Soak);
      casePrintState(RampUp);
      casePrintState(Peak);
      casePrintState(RampDown);
      casePrintState(CoolDown);
      casePrintState(Complete);
    default: tft.print((uint8_t)currentState); break;
  }
  tft.print("        "); // lazy: fill up space
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  // set point
  y += 20;
  tft.setCursor(tft.width() - 130, y);
  tft.print("Sp:");
  alignRightPrefix((int)Setpoint);
  printDouble(Setpoint);
  tft.print("\367C  ");
  // draw temperature curves
  if (xOffset >= elapsed) {
    xOffset = 0;
  }
  do { // x with wrap around
    dx = (uint16_t)((elapsed - xOffset) * pxPerSec);
    if (dx > w) {
      xOffset = elapsed;
    }
  } while (dx > w);
  // temperature setpoint
  dy = h - ((uint16_t)Setpoint * pxPerC / 100) + yOffset;
  tft.drawPixel(dx, dy, ILI9341_BLUE);
  // actual temperature
  dy = h - ((uint16_t)temperature * pxPerC / 100) + yOffset;
  tft.drawPixel(dx, dy, ILI9341_RED);
  // bottom line
  y = 220;
  // set values
  tft.setCursor(1, y);
  tft.print("\xef");
  alignRightPrefix((int)heaterValue);
  tft.print((int)heaterValue);
  tft.print('%');
  tft.print(" \x2a");
  alignRightPrefix((int)fanValue);
  tft.print((int)fanValue);
  tft.print('%');
  tft.print(" \x12 "); // alternative: \x7f
  printDouble(rampRate);
  tft.print("\367C/s    ");
}

// Name, Label, Next, Previous, Parent, Child, Callback
MenuItem(miExit, "", Menu::NullItem, Menu::NullItem, Menu::NullItem, miCycleStart, menuExit);
MenuItem(miCycleStart,  "Start Cycle",  miEditProfile, Menu::NullItem, miExit, Menu::NullItem, menu_cycleStart);
MenuItem(miEditProfile, "Edit Profile", miLoadProfile, miCycleStart,   miExit, miRampUpRate, menuDummy);
MenuItem(miRampUpRate, "Ramp up  ",   miSoakTempA,      Menu::NullItem, miEditProfile, Menu::NullItem, menu_editNumericalValue);
MenuItem(miSoakTempA,   "Soak temp A", miSoakTempB,      miRampUpRate,   miEditProfile, Menu::NullItem, menu_editNumericalValue);
MenuItem(miSoakTempB,   "Soak temp B", miSoakTime,      miSoakTempA,   miEditProfile, Menu::NullItem, menu_editNumericalValue);
MenuItem(miSoakTime,   "Soak time", miPeakTemp,      miSoakTempB,     miEditProfile, Menu::NullItem, menu_editNumericalValue);
MenuItem(miPeakTemp,   "Peak temp", miPeakTime,      miSoakTime,     miEditProfile, Menu::NullItem, menu_editNumericalValue);
MenuItem(miPeakTime,   "Peak time", miRampDnRate,    miPeakTemp,     miEditProfile, Menu::NullItem, menu_editNumericalValue);
MenuItem(miRampDnRate, "Ramp down", Menu::NullItem,  miPeakTime,     miEditProfile, Menu::NullItem, menu_editNumericalValue);
MenuItem(miLoadProfile,  "Load Profile",  miSaveProfile,  miEditProfile, miExit, Menu::NullItem, menu_saveLoadProfile);
MenuItem(miSaveProfile,  "Save Profile",  miFanSettings,  miLoadProfile, miExit, Menu::NullItem, menu_saveLoadProfile);
MenuItem(miFanSettings,  "Fan Speed",  miPidSettings,  miSaveProfile, miExit, Menu::NullItem, menu_editNumericalValue);
MenuItem(miPidSettings,  "PID Settings",  miFactoryReset, miFanSettings, miExit, miPidSettingP,  menuDummy);
MenuItem(miPidSettingP,  "Heater Kp",  miPidSettingI, Menu::NullItem, miPidSettings, Menu::NullItem, menu_editNumericalValue);
MenuItem(miPidSettingI,  "Heater Ki",  miPidSettingD, miPidSettingP,  miPidSettings, Menu::NullItem, menu_editNumericalValue);
MenuItem(miPidSettingD,  "Heater Kd",  Menu::NullItem, miPidSettingI, miPidSettings, Menu::NullItem, menu_editNumericalValue);
MenuItem(miFactoryReset, "Factory Reset", Menu::NullItem, miPidSettings, miExit, Menu::NullItem, menu_factoryReset);

#endif // UI_H
