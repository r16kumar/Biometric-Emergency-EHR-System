/*
 * Embedded HealthSys - Final Production Firmware
 * Hardware: ESP32 DevKit V1 (30-Pin), R307S, SH1106, 4x4 Keypad
 * Features: Dashboard UI, Smart Enroll, Cloud Sync, Paper-Mode QR
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Adafruit_Fingerprint.h>
#include <ArduinoJson.h>
#include <U8g2lib.h>
#include <Keypad.h>
#include <Wire.h>
#include <QRCode.h>

// --- 1. CONFIGURATION ---
#define WIFI_SSID "BVCOE_ESP"
#define WIFI_PASS "hellobvcoe"

const char *DATABASE_URL = "https://emergencyhealthsystem-8c910-default-rtdb.asia-southeast1.firebasedatabase.app";
const char *WEB_APP_URL = "https://ehs-minor-project.netlify.app/index.html";

// --- 2. PIN DEFINITIONS ---
#define FINGER_RX 16
#define FINGER_TX 17

// Keypad Map
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'}, // A = Enroll
    {'4', '5', '6', 'B'}, // B = Search
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'} // * = Hidden Wipe
};
// Validated Pinout for 30-Pin DevKit
byte rowPins[ROWS] = {32, 33, 25, 26};
byte colPins[COLS] = {27, 14, 13, 23};
// --- 3. OBJECTS ---
HardwareSerial mySerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
QRCode qrcode;
// --- 4. UI FUNCTIONS ---

void drawHeader()
{
    u8g2.setFont(u8g2_font_helvB08_tr);
    u8g2.drawStr(2, 10, "EHS SYSTEM");
// WiFi Status Indicator
    u8g2.drawCircle(120, 6, 4, U8G2_DRAW_ALL);
if (WiFi.status() == WL_CONNECTED)
    {
        u8g2.drawDisc(120, 6, 2, U8G2_DRAW_ALL);
}
    u8g2.drawHLine(0, 14, 128);
}

void drawMainMenu()
{
    u8g2.clearBuffer();
    drawHeader();
// Left Card: Enroll
    u8g2.drawRFrame(4, 20, 58, 40, 4);
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(18, 45, "A");
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(12, 56, "Enroll");
// Right Card: Search
    u8g2.drawRFrame(66, 20, 58, 40, 4);
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(88, 45, "B");
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(74, 56, "Search");
u8g2.sendBuffer();
}

void drawStatus(String title, String line1, String line2 = "")
{
    u8g2.clearBuffer();
    drawHeader();
// Title Bar
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawBox(0, 15, 128, 13); // Black background
    u8g2.setDrawColor(0);
// White text
    u8g2.setCursor(2, 25);
    u8g2.print(title);

    u8g2.setDrawColor(1); // Reset to Normal
    u8g2.setFont(u8g2_font_helvB08_tr);
// Center text
    int w1 = u8g2.getStrWidth(line1.c_str());
    u8g2.setCursor((128 - w1) / 2, 45);
    u8g2.print(line1);
int w2 = u8g2.getStrWidth(line2.c_str());
    u8g2.setCursor((128 - w2) / 2, 60);
    u8g2.print(line2);

    u8g2.sendBuffer();
}

// *** PRODUCTION FIX: PAPER MODE QR ***
// *** PAPER MODE QR: Black Dots on White Background ***
void displayQRCode(String url)
{
    u8g2.clearBuffer();
// 1. Generate QR Data
    uint8_t qrcodeData[qrcode_getBufferSize(3)];
    qrcode_initText(&qrcode, qrcodeData, 3, 0, url.c_str());
// 2. Dimensions
    const int scale = 2;
    const int quiet = 4;
// Quiet zone padding
    const int qrPx = qrcode.size * scale;
const int bgWidth = qrPx + (quiet * 2);
    const int offsetX = 2;
const int offsetY = (64 - qrPx) / 2;

    // --- MAGIC FIX STARTS HERE ---

    // 3. Draw a GLOWING WHITE BOX (The "Paper")
    u8g2.setDrawColor(1);
// Color 1 = ON (White)
    u8g2.drawBox(offsetX, 0, bgWidth, 64);
// 4. Draw BLACK SQUARES on top (The "Ink")
    u8g2.setDrawColor(0);
// Color 0 = OFF (Black)

    int startX = offsetX + quiet;
for (uint8_t y = 0; y < qrcode.size; y++)
    {
        for (uint8_t x = 0; x < qrcode.size; x++)
        {
            // If the QR logic says "Draw a dot here"...
            if (qrcode_getModule(&qrcode, x, y))
            {
                
// ...we actually turn OFF the pixels (Black)
                u8g2.drawBox(startX + (x * scale), offsetY + (y * scale), scale, scale);
}
        }
    }

    // --- MAGIC FIX ENDS HERE ---

    // 5. Draw Instructions on the Right (White text on Black background)
    u8g2.setDrawColor(1);
// Switch back to White text

    int textX = offsetX + bgWidth + 4;

    u8g2.setFont(u8g2_font_6x10_tr);
u8g2.drawStr(textX, 15, "SCAN ME");

    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2.drawStr(textX, 30, "To Open");
    u8g2.drawStr(textX, 40, "Web Record");

    u8g2.setFont(u8g2_font_4x6_tr);
    u8g2.drawStr(textX, 58, "[Press Key]");

    u8g2.sendBuffer();
}

void displayPatientText(int id, String name, String allergy, String contact)
{
    u8g2.clearBuffer();
    drawHeader();

    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.setCursor(0, 28);
    u8g2.print("ID: ");
    u8g2.print(id);
u8g2.setCursor(0, 39);
    u8g2.print("Nm: ");
    u8g2.print(name.substring(0, 14));
    u8g2.setCursor(0, 50);
    u8g2.print("Al: ");
    u8g2.print(allergy.substring(0, 14));
    u8g2.setCursor(0, 61);
    u8g2.print("Ph: ");
    u8g2.print(contact);

    u8g2.sendBuffer();
}

// --- 5. CLOUD LOGIC ---

int getCloudCounter(String counterName, int defaultStart)
{
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
String path = String(DATABASE_URL) + "/system/" + counterName + ".json";
    http.begin(client, path);
    int code = http.GET();
    int val = defaultStart;
if (code > 0)
    {
        String payload = http.getString();
if (payload != "null")
            val = payload.toInt();
}
    http.end();
    return (val < defaultStart) ? defaultStart : val;
}

void incrementCloudCounter(String counterName, int currentVal)
{
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
String path = String(DATABASE_URL) + "/system/" + counterName + ".json";
    http.begin(client, path);
    http.addHeader("Content-Type", "application/json");
    http.PUT(String(currentVal + 1));
    http.end();
}

void uploadMap(int slot, int patientID)
{
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
String path = String(DATABASE_URL) + "/fingerprint_map/" + String(slot) + ".json";
    http.begin(client, path);
    http.addHeader("Content-Type", "application/json");
    http.PUT(String(patientID));
    http.end();
}

void createPatientPlaceholder(int patientID)
{
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
String path = String(DATABASE_URL) + "/patients/" + String(patientID) + ".json";
    http.begin(client, path);
    http.addHeader("Content-Type", "application/json");
    JsonDocument doc;
    doc["status"] = "Pending Registration";
doc["enrolled_via"] = "ESP32-Biometric";
    String jsonStr;
    serializeJson(doc, jsonStr);
    http.PUT(jsonStr);
    http.end();
}

// --- 6. CORE LOGIC ---

void handleFoundPatient(int slot)
{
    drawStatus("SEARCHING", "Slot Found: " + String(slot));
WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;

    String mapPath = String(DATABASE_URL) + "/fingerprint_map/" + String(slot) + ".json";
    http.begin(client, mapPath);
int code = http.GET();
    String payload = http.getString();
    http.end();

    if (code <= 0 || payload == "null")
    {
        drawStatus("ERROR", "No Cloud Record", "For this Finger");
delay(2000);
        return;
    }

    int patientID = payload.toInt();

    // Match Found Menu
    u8g2.clearBuffer();
    drawHeader();
    u8g2.setFont(u8g2_font_helvB08_tr);
u8g2.drawStr(25, 30, "ID FOUND!");
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.setCursor(10, 45);
    u8g2.print("1: View Text");
    u8g2.setCursor(10, 60);
    u8g2.print("2: View QR");
    u8g2.sendBuffer();

    char key = 0;
while (key != '1' && key != '2')
    {
        key = keypad.getKey();
delay(50);
    }

    if (key == '1')
    {
        drawStatus("LOADING", "Fetching Data...");
String dataPath = String(DATABASE_URL) + "/patients/" + String(patientID) + ".json";
        http.begin(client, dataPath);
        code = http.GET();
if (code > 0)
        {
            String json = http.getString();
JsonDocument doc;
            deserializeJson(doc, json);
            String name = doc["name"] | "Unknown";
            String allergy = doc["allergy"] | "N/A";
String contact = doc["contact"] | "N/A";
            displayPatientText(patientID, name, allergy, contact);
}
        else
        {
            drawStatus("ERROR", "Fetch Failed");
}
        http.end();
    }
    else if (key == '2')
    {
        drawStatus("GENERATING", "Creating QR...");
String qrLink = String(WEB_APP_URL) + "?id=" + String(patientID);
        displayQRCode(qrLink);
    }

    keypad.waitForKey();
// Wait to exit
}

bool getFingerImage(int slot)
{
    int p = -1;
    int timeout = 0;
drawStatus("SCANNING", "Place Finger", "on Sensor");
    while (p != FINGERPRINT_OK && timeout < 80)
    {
        p = finger.getImage();
if (p == FINGERPRINT_NOFINGER)
        {
            delay(50);
timeout++;
        }
    }
    if (p != FINGERPRINT_OK)
    {
        drawStatus("ERROR", "Timeout or", "Bad Image");
delay(1500);
        return false;
    }
    drawStatus("PROCESSING", "Converting", "Image...");
    p = finger.image2Tz(slot);
if (p != FINGERPRINT_OK)
    {
        drawStatus("ERROR", "Conversion", "Failed");
        delay(1500);
return false;
    }
    return true;
}

// --- SEQUENCES ---

void enrollSequence()
{
    // 1. Scan 1
    if (!getFingerImage(1))
        return;
// 2. Check Duplicates
    drawStatus("CHECKING", "Scanning", "Database...");
    int p = finger.fingerSearch();
if (p == FINGERPRINT_OK)
    {
        drawStatus("DUPLICATE", "ID Exists", "Fetching...");
        delay(1500);
handleFoundPatient(finger.fingerID);
        return;
    }

    // 3. Allocate
    drawStatus("SYNCING", "Reserving", "Cloud ID...");
int slot = getCloudCounter("next_slot", 1);
    int patientID = getCloudCounter("next_patient_id", 100);
if (slot > 1000)
    {
        drawStatus("ERROR", "Memory Full");
        delay(2000);
        return;
}

    // 4. Scan 2
    drawStatus("STEP 2", "Remove Finger");
    delay(1500);
while (finger.getImage() != FINGERPRINT_NOFINGER)
        ;

    drawStatus("STEP 2", "Place Same", "Finger Again");
while (finger.getImage() == FINGERPRINT_NOFINGER)
        ;
if (!getFingerImage(2))
        return;

    // 5. Store
    drawStatus("SAVING", "Writing to", "Memory...");
p = finger.createModel();
    if (p == FINGERPRINT_OK)
    {
        finger.storeModel(slot);
uploadMap(slot, patientID);
        createPatientPlaceholder(patientID);
        incrementCloudCounter("next_slot", slot);
        incrementCloudCounter("next_patient_id", patientID);

        // 6. Success -> QR
        drawStatus("SUCCESS", "ID: " + String(patientID), "Generating QR...");
delay(1500);
        String qrLink = String(WEB_APP_URL) + "?id=" + String(patientID);
        displayQRCode(qrLink);
        keypad.waitForKey();
}
    else
    {
        drawStatus("ERROR", "Mismatch", "Try Again");
delay(2000);
    }
}

void searchSequence()
{
    if (getFingerImage(1))
    {
        int p = finger.fingerSearch();
if (p == FINGERPRINT_OK)
        {
            handleFoundPatient(finger.fingerID);
}
        else
        {
            drawStatus("RESULT", "Not Found", "in Database");
delay(2000);
        }
    }
}

void wipeSequence()
{
    u8g2.clearBuffer();
    u8g2.drawBox(0, 0, 128, 64);
    u8g2.setDrawColor(0);
    u8g2.setFont(u8g2_font_ncenB10_tr);
u8g2.drawStr(10, 20, "ADMIN MENU");
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(10, 40, "Wipe All Data?");
    u8g2.drawStr(10, 55, "#: YES    *: NO");
    u8g2.sendBuffer();
u8g2.setDrawColor(1);

    char key = keypad.waitForKey();
    if (key == '#')
    {
        drawStatus("WIPING", "Please Wait...");
finger.emptyDatabase();
        drawStatus("SUCCESS", "Sensor Cleared");
    }
    else
    {
        drawStatus("CANCELLED", "Safe.");
}
    delay(2000);
}

// --- SETUP & LOOP ---

void setup()
{
    Serial.begin(115200);
    u8g2.begin();

    drawStatus("BOOTING", "Initializing", "Hardware...");
mySerial.begin(57600, SERIAL_8N1, FINGER_RX, FINGER_TX);
    finger.begin(57600);

    if (!finger.verifyPassword())
    {
        drawStatus("FAILURE", "Fingerprint", "Sensor Missing");
while (1)
            ;
}

    drawStatus("NETWORK", "Connecting", "to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    int dots = 0;
while (WiFi.status() != WL_CONNECTED)
    {
        u8g2.drawStr(60 + (dots * 5), 50, ".");
u8g2.sendBuffer();
        dots = (dots + 1) % 3;
        delay(300);
    }

    drawStatus("READY", "System Online");
    delay(1000);
}

void loop()
{
    drawMainMenu();
    char key = keypad.getKey();
if (key)
    {
        if (key == 'A' || key == '1')
            enrollSequence();
if (key == 'B' || key == '2')
            searchSequence();
if (key == '*')
            wipeSequence();
    }
}