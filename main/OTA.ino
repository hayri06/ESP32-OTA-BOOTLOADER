void execOTA() {
  bin = bootloaderName;
  Serial.print("Connecting to: ");
  Serial.println(String(host).c_str());
  // Connect to S3
  if (clientUp.connect(host.c_str(), port)) {
    // Connection Succeed.
    // Fecthing the bin
    Serial.print("Fetching Bin: ");
    Serial.println(String(bin).c_str());

    // Get the contents of the bin file
    clientUp.print(String("GET ") + bin + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Cache-Control: no-cache\r\n" +
                   "Connection: close\r\n\r\n");

    // Check what is being sent
    //    Serial.print(String("GET ") + bin + " HTTP/1.1\r\n" +
    //                 "Host: " + host + "\r\n" +
    //                 "Cache-Control: no-cache\r\n" +
    //                 "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (clientUp.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println("clientUp Timeout !");
        clientUp.stop();
        return;
      }
    }
    // Once the response is available,
    // check stuff

    while (clientUp.available()) {
      // read line till /n
      String line = clientUp.readStringUntil('\n');
      // remove space, to check if the line is end of headers
      line.trim();

      // if the the line is empty,
      // this is end of headers
      // break the while and feed the
      // remaining `clientUp` to the
      // Update.writeStream();
      if (!line.length()) {
        //headers ended
        break; // and get the OTA started
      }

      // Check if the HTTP Response is 200
      // else break and Exit Update
      if (line.startsWith("HTTP/1.1")) {
        if (line.indexOf("200") < 0) {
          Serial.println("Got a non 200 status code from server. Exiting OTA Update.");
          break;
        }
      }

      // extract headers here
      // Start with content length
      if (line.startsWith("Content-Length: ")) {
        contentLength = atol((getHeaderValue(line, "Content-Length: ")).c_str());
        Serial.print("Got ");
        Serial.print(String(contentLength).c_str());
        Serial.println(" bytes from server");
      }

      // Next, the content type
      if (line.startsWith("Content-Type: ")) {
        String contentType = getHeaderValue(line, "Content-Type: ");
        //Serial.println("Got " + contentType + " payload.");
        if (contentType == "application/octet-stream") {
          isValidContentType = true;
        }
      }
    }
  } else {
    // Connect to S3 failed
    // May be try?
    // Probably a choppy network?
    Serial.print("Connection to ");
    Serial.print(String(host).c_str());
    Serial.println(" failed. Please check your setup");
    // retry??
    execOTA();
  }

  // Check what is the contentLength and if content type is `application/octet-stream`
  //Serial.println("contentLength : " + String(contentLength) + ", isValidContentType : " + String(isValidContentType));

  // check contentLength and content type
  if (contentLength && isValidContentType) {
    // Check if there is enough to OTA Update
    bool canBegin = Update.begin(contentLength);
    delay(100);

    // If yes, begin
    if (canBegin) {
      Serial.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");
      // No activity would appear on the Serial monitor
      // So be patient. This may take 2 - 5mins to complete
      delay(100);

      size_t written = Update.writeStream(clientUp);
      delay(100);

      if (written == contentLength) {
        Serial.print("Written : ");
        Serial.print(String(written).c_str());
        Serial.println(" Succesfully");
      } else {
        Serial.print("Written only : ");
        Serial.print(String(written).c_str());
        Serial.print("/");
        Serial.print(String(contentLength).c_str());
        Serial.println(". Retry?");

        // retry??
        execOTA();
      }

      if (Update.end()) {
        Serial.println("OTA done!");
        if (Update.isFinished()) {
          Serial.println("Update successfully completed. Rebooting.");
          ESP.restart();
        } else {
          Serial.println("Update not finished? Something went wrong!");
        }
      } else {
        Serial.print("Error Occurred. Error #: ");
        Serial.println(String(Update.getError()).c_str());
      }
    } else {
      // not enough space to begin OTA
      // Understand the partitions and
      // space availability
      Serial.println("Not enough space to begin OTA");
      clientUp.flush();
    }
  } else {
    Serial.println("There was no content in the response");
    clientUp.flush();
  }
}
