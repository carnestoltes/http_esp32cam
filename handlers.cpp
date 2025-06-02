#include "WifiCam.hpp"
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <StreamString.h>
#include <uri/UriBraces.h>
#include <WiFi.h>

static const char FRONTPAGE[] = R"EOT(
<!doctype html>
<title>ESP32 Cam</title>
<style>
table, th, td { border: 1px solid #000; border-collapse: collapse; padding: 0.4rem; text-align: center; }
a { text-decoration: none; }
</style>
<body>
<h1>ESP32 Cam</h1>
<table>
<thead>
<tr><th>320x240</th><th>640x480</th></tr>
</thead>
<tbody>
<tr>
<td><a href="/capture/320x240">Capturar</a></td>
<td><a href="/capture/640x480">Capturar</a></td>
</tr>
</tbody>
</table>
</body>
)EOT";

const char* serverUrl = "";  // Put the URL of server

static void captureAndSendHTTP(String resolution) {
  long width = resolution.substring(0, resolution.indexOf("x")).toInt();
  long height = resolution.substring(resolution.indexOf("x") + 1).toInt();

  auto r = esp32cam::Camera.listResolutions().find(width, height);
  if (!r.isValid()) {
    Serial.println("Invalid resolution");
    return;
  }

  if (!esp32cam::Camera.update([=](esp32cam::Settings& settings) { settings.resolution = r; }, 500)) {
    Serial.println("Cannot apply the new resolution ...");
    return;
  }

  auto frame = esp32cam::capture();
  if (!frame) {
    Serial.println("Capturing a image failure ...");
    return;
  }

  // Send image for http
  WiFiClient client;
  HTTPClient http;
  http.begin(client,serverUrl);
//Need define an structure for the missage because you are using a multer funcionality 
String boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW"; 
String bodyStart = "--" + boundary + "\r\n"
                   "Content-Disposition: form-data; name=\"imagen\"; filename=\"capture.jpg\"\r\n"
                   "Content-Type: image/jpeg\r\n\r\n";

String bodyEnd = "\r\n--" + boundary + "--\r\n";

http.addHeader("Content-Type", "multipart/form-data; boundary=" + boundary);
  
http.setTimeout(10000); // Adjust the window for send the package, mandatory 


// A buffer for send the image 
size_t totalLength = bodyStart.length() + frame->size() + bodyEnd.length();
uint8_t *postBuffer = new uint8_t[totalLength];

// Copy data into buffer
memcpy(postBuffer, bodyStart.c_str(), bodyStart.length());
memcpy(postBuffer + bodyStart.length(), frame->data(), frame->size());
memcpy(postBuffer + bodyStart.length() + frame->size(), bodyEnd.c_str(), bodyEnd.length());

// Send the sentence created
int httpResponseCode = http.POST(postBuffer, totalLength);
Serial.printf("HTTP Response code: %d\n", httpResponseCode);

// Clear the buffer
delete[] postBuffer;

http.end();
}

void addRequestHandlers() {
  server.on("/", HTTP_GET, [] {
    server.send(200, "text/html", FRONTPAGE);
  });

  server.on(UriBraces("/capture/{}x{}"), HTTP_GET, [] {
    String resolution = server.pathArg(0) + "x" + server.pathArg(1);
    captureAndSendHTTP(resolution);
    server.send(200, "text/plain", "Image send via HTTP.");
  });
}
