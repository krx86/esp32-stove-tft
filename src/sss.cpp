void loop() {
    server.handleClient();
    telnet.loop();
    esp_bluedroid_disable;
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_9, 0);
    ArduinoOTA.handle();

    unsigned long currentMillis = millis();

    // ... (rest of your existing code before the display logic) ...

    // Create sprites outside the if/else blocks (only once)
    if (!bckg.created()) bckg.createSprite(320, 240);
    if (!text.created()) text.createSprite(200, 30);
    if (!text2.created()) text2.createSprite(200, 30);
    if (!text3.created()) text3.createSprite(200, 30);
    if (!text4.created()) text4.createSprite(130, 40);
    if (!img.created()) img.createSprite(200, 200);
    if (!img2.created()) img2.createSprite(220, 30);
    if (!text5.created()) text5.createSprite(200, 30);
    if (!bckg2.created()) bckg2.createSprite(110, 170);
    if (!text6.created()) text6.createSprite(200, 30);
    
    // Clear main screen when switching to settings
      if (old_setti == false && setti == true) {

        bckg2.deleteSprite();
        text.deleteSprite();
        text2.deleteSprite();
        text3.deleteSprite();
        text4.deleteSprite();
        img.deleteSprite();
        img2.deleteSprite();

      }
       // Clear settings screen when switching to main
       if (old_setti == true && setti == false) {
       bckg.deleteSprite();
       text.deleteSprite();
       text2.deleteSprite();
       text3.deleteSprite();
       text5.deleteSprite();
       text6.deleteSprite();
       img2.deleteSprite();
      }

    text4.fillSprite(TFT_WHITE);

    if (kludas > 20) {
        // Kļūdu attēlošana
        img.fillSprite(TFT_WHITE);
        img.pushImage(0, 0, 200, 200, kluda);
        text4.setTextColor(TFT_BLACK);
        messageDamp = "Error";
        tone(buzzerPort, buzzerRefillFrequency);
        delay(buzzerRefillDelay);
        noTone(buzzerPort);
        delay(buzzerRefillDelay);
    }
    else {
        text4.setTextColor(TFT_BLACK);
    }

    // Pārbauda un pārslēdz "setti" režīmu
    if (pot_raw >= 155 && pot_raw <= 175) {
        delay(500);
        setti = !setti;
        delay(100);
        tft.fillScreen(TFT_WHITE);
    }

    if (setti) {

        // Iestatījumu ekrāns
        img2.fillSprite(TFT_TRANSPARENT);
        bckg.fillSprite(TFT_WHITE);
        bckg.pushImage(0, 0, 320, 240, setting);
        text.fillSprite(TFT_WHITE);
        text.setTextColor(TFT_BLACK);
        text5.fillSprite(TFT_WHITE);
        text5.setTextColor(TFT_BLACK);
        text2.fillSprite(TFT_WHITE);
        text2.setTextColor(TFT_BLACK);
        text3.fillSprite(TFT_WHITE);
        text3.setTextColor(TFT_BLACK);
        text6.fillSprite(TFT_WHITE);
        text6.setTextColor(TFT_BLACK);

        // Zīmē tekstus
        text5.setFreeFont(FSSB12);
        text5.drawString("TargetTemp.  " + String(targetTempC), 0, 0, GFXFF);
        text5.pushToSprite(&bckg, 120, 10);

        text2.setFreeFont(FSSB12);
        text2.drawString("koificients  " + String(kP), 0, 0, GFXFF);
        text2.pushToSprite(&bckg, 120, 60);

        text3.setFreeFont(FSSB12);
        text3.drawString("Min. temp  " + String(temperatureMin), 0, 0, GFXFF);
        text3.pushToSprite(&bckg, 120, 110);

        text.setFreeFont(FSSB12);
        text.drawString("refillTrigger  " + String(refillTrigger), 0, 0, GFXFF);
        text.pushToSprite(&bckg, 120, 160);

        text6.setFreeFont(FSSB12);

        if (suknis == false) {
            text6.drawString("Suknis OFF  ", 0, 0, GFXFF);
        }
        else {
            text6.drawString("Suknis ON  ", 0, 0, GFXFF);
        }
        text6.pushToSprite(&bckg, 120, 210);

        img2.drawRect(0, 0, 200, 30, 10);
        img2.pushToSprite(&bckg, 116, krx, TFT_TRANSPARENT);
        bckg.pushSprite(0, 0);

        old_setti = setti;
    }
    else {
        // Noklusējuma ekrāns
        bckg2.fillSprite(TFT_WHITE);
        int y = 105 - temperature;
        img2.fillSprite(TFT_WHITE);
        text.fillSprite(TFT_WHITE);
        text.setTextWrap(true);
        text.setTextColor(TFT_RED);
        text3.fillSprite(TFT_WHITE);
        text3.setTextColor(TFT_BLACK);
        text2.fillSprite(TFT_WHITE);
        text2.setTextColor(TFT_BLACK);
        bckg2.pushImage(0, 0, 110, 170, arrow);
        img2.fillRect(0, 0, 17, y, TFT_BLACK);
        img2.pushToSprite(&bckg2, 63, 14, TFT_WHITE);
        bckg2.pushSprite(0, 10, TFT_BLACK);
        text.setFreeFont(&FreeSansBold42pt7b);
        text.drawString(String(temperature) + " c", 0, 0, GFXFF);
        text.pushSprite(120, 60);
        text2.setFreeFont(FF35);
        text2.drawString(String(damper) + " %", 0, 0, GFXFF);
        text2.pushSprite(190, 190);
        text3.drawString(String(targetTempC) + "°c   Target.", 0, 0, 4);
        text3.pushSprite(120, 20);
        img.pushSprite(90, 0, TFT_BLACK);
        text4.setFreeFont(FF23);
        text4.drawString(messageDamp, 0, 0, GFXFF);
        text4.pushSprite(20, 188);
         old_setti = setti;
    }

   // ... (rest of your existing code after the display logic) ...
}
