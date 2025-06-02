# http_esp32cam
This project is a reduced version of architecture client/server HTTP for send an image using **esp32cam** via POST to the server. Client (esp32) connect to LAN, and offer a simple html page for select between two resolution .jpeg for send it.

**Note:** The IP of the client esp32, displays into serial terminal.

This project is base on the example of: (yoursunny){https://github.com/yoursunny/esp32cam}

## Client ESP32cam configuration

Into **.ino** file, fill this fields with your LAN

static const char* WIFI_SSID = "";
static const char* WIFI_PASS = "";

Into **.cpp** file, you should configure the directory where save the images, the header if you use multer for split the payload and a timeout for give a message enough time for send it. 

const char* serverUrl = "";

http.addHeader("Content-Type", "multipart/form-data; boundary=" + boundary);

http.setTimeout(10000); 

## Server.js

In other hand, exist in this project a file name "server.js" that a minimalist server HTTP for receive the image send it and storage in the folder of server did you desired.

You should modify a couple of lines that appear following this line.

The directory where store the images

const uploadDir = '';
