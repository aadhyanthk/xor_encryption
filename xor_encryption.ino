#define ZERO_LED_PIN 13  // LED for binary 0
#define ONE_LED_PIN 8   // LED for binary 1

void setup() {
    Serial.begin(9600);  // Recommended baud rate for Arduino UNO
    pinMode(ZERO_LED_PIN, OUTPUT);
    pinMode(ONE_LED_PIN, OUTPUT);
    digitalWrite(ZERO_LED_PIN, LOW);
    digitalWrite(ONE_LED_PIN, LOW);
}

// XOR encryption function
String xorEncrypt(String text, String key) {
    String result = "";
    for (int i = 0; i < text.length(); i++) {
        for (int j = 7; j >= 0; j--) {
          result +=  (((text[i] >> j) & 1) ^ ((key[i] >> j) & 1)) ? '1' : '0';
        }
        result += ' ';
    }
    return result;
}

//binary string
String xorDecrypt(String text, String key) {
    String result = "";
    int tmp = 0;

    Serial.print("Decrypted: ");

    for (int i = 0; i < text.length(); i++) {
        if (text[i] == ' ' || text[i] == '\0') {
            result += (char) tmp;
            Serial.print((char)tmp);
            
            tmp = 0;
            continue; // skip spaces
        }
        else {
            tmp = (tmp << 1) | ((text[i] - '0') ^ (key[i] - '0'));
        }
    }
    Serial.println();

    return result;
}


// Function to adjust password length
String adjustPassword(String key, int length) {
    String newKey = "";
    for (int i = 0; i < length; i++) {
        newKey += key[i % key.length()];
    }
    return newKey;
}

// Function to convert a string into binary
String toBinaryString(String text) {
    String binary = "";
    for (char c : text) {
        for (int i = 7; i >= 0; i--) {
            binary += ((c >> i) & 1) ? '1' : '0';
        }
        binary += " ";  // Space between characters
    }
    return binary;
}

// Function to blink LEDs based only on ciphertext binary
void displayBinary(char c) {
        bool bit = (c == '1');

        if (bit) {
            digitalWrite(ONE_LED_PIN, HIGH);  
            digitalWrite(ZERO_LED_PIN, LOW); 
            Serial.print(1);
        } else {
            digitalWrite(ONE_LED_PIN, LOW);
            digitalWrite(ZERO_LED_PIN, HIGH);
            Serial.print(0);  
        }
        
        delay(50);  // ON duration
        digitalWrite(ONE_LED_PIN, LOW);  
        digitalWrite(ZERO_LED_PIN, LOW);  
        delay(50);  // OFF duration
}

void loop() {
    if (Serial.available()) {
        delay(100);  // Ensure full input is received

        // Read plaintext input
        String plaintext = Serial.readStringUntil('\n');
        plaintext.trim();

        // Wait for password input
        while (!Serial.available());
        delay(100);  // Ensure stability in input
        String password = Serial.readStringUntil('\n');
        password.trim();

        // Adjust password length to match plaintext
        String correctedPassword = adjustPassword(password, plaintext.length());

        // Perform XOR encryption
        String encryptedText = xorEncrypt(plaintext, correctedPassword);

        // Display plaintext and password on Serial Monitor
        Serial.println("Plaintext: " + plaintext);
        Serial.println("Password: " + correctedPassword);
        Serial.println("Binary of Plaintext:  " + toBinaryString(plaintext));
        Serial.println("Binary of Password:   " + toBinaryString(correctedPassword));
        Serial.print("Binary of Ciphertext: ");

        // Blink LEDs based only on ciphertext binary
        /*String encryptedString = "";
        int tmp = 0;
        for (char c : encryptedText) {
            if (c == ' ' || c == '\0') {
              encryptedString += (char) tmp;
              tmp = 0;
              Serial.print(c);
              delay(50);
            }
            else {
              tmp = (tmp << 1) | (c - '0');
              displayBinary(c);
            }
        }*/
        String encryptedString = "";
        int tmp = 0;
        for (int i = 0; i < encryptedText.length(); i += 9) {
          tmp = 0;
          for (int j = i; j < i + 8; j++) {
            char c = encryptedText[j];
            tmp = (tmp << 1) | (encryptedText[j] - '0');
            displayBinary(encryptedText[j]);
          }
          Serial.print(' ');
          encryptedString += (char) tmp;
        }
        Serial.println();
        Serial.println("Ciphertext: " + encryptedString);

        //decrypt
        String decryptedText = xorDecrypt(encryptedText, toBinaryString(correctedPassword));
        if (decryptedText == plaintext) {
          Serial.println("Successful Encryption and Decryption!");
        }
        else {
          Serial.println("Concat Error!");
        }
    }
}

