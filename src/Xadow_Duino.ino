#include <Wire.h>
#include <TimerOne.h>
#include <Adafruit_NeoPixel.h>

#define DEBUG

const int npDataPin = 5;    // RGB_PIN
const int npMaxNum = 14;    // RGB_MAX_NUM
const int npGroundPin = 13; // RGB_POWER_PIN
const int buttonPin = 7;    // RGB_MODE_PIN
#define RGB_INT1_PIN    0
#define RGB_INT2_PIN    1
#define RGB_SCL_PIN     3   // unused?
#define RGB_SDA_PIN     2   // unused?

#define COLOR_PIXEL_ADDR  35    // I2C Address of Xadow Duino
#define BUFFER_SIZE  20         // I2C message buffer size

#define RGB_SET_WORK            0x80
#define RGB_SET_MODE            0x81

enum pixelMode {
  RGB_POWER_OFF,    // 0 
  RGB_POWER_ON,
  RGB_MONOCHROME,
  RGB_MARQUEE,
  RGB_RAINBOW,
  RGB_SPARKLE
};

#define RGB_INTERVAL    100

uint8_t I2C_buffer[BUFFER_SIZE];
uint8_t buffer[BUFFER_SIZE];
uint8_t Char_nu=0;  //how many byte received

uint8_t scan_data[4] = {0x00,0x00,0x00,COLOR_PIXEL_ADDR};
uint8_t scan_count = 0;
boolean flag_data_receive = 0;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(npMaxNum, npDataPin, NEO_GRB + NEO_KHZ800);

typedef struct {
  uint8_t rValue;
  uint8_t gValue;
  uint8_t bValue;
} RGBValue;
RGBValue rgbValue[npMaxNum] = {0};

const uint8_t RED[126] PROGMEM= {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,238,221,204,188,171,154,137,119,102,85,
68,51,34,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,17,35,52,
35,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,17,34,51,68,85,102,119,137,154,171,188,204,221,238,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
};
const uint8_t GREEN[126] PROGMEM= {0,17,34,51,68,85,102,119,136,153,170,187,204,221,238,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,238,221,204,188,170,154,136,120,102,86,68,52,34,18,0,0,0,0,
0,0,0,18,34,52,68,86,102,120,136,154,170,188,204,221,238,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,238,221,204,187,170,153,136,119,102,85,68,51,34,17,
};
const uint8_t BLUE[126] PROGMEM= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,18,34,52,68,86,102,120,136,154,170,188,
204,221,238,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,238,221,204,188,170,154,136,120,102,86,68,52,34,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
/*
const uint8_t RED[126] = {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,238,221,204,188,171,154,137,119,102,85,
68,51,34,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,17,35,52,
35,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,17,34,51,68,85,102,119,137,154,171,188,204,221,238,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
};
const uint8_t GREEN[126] = {0,17,34,51,68,85,102,119,136,153,170,187,204,221,238,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,238,221,204,188,170,154,136,120,102,86,68,52,34,18,0,0,0,0,
0,0,0,18,34,52,68,86,102,120,136,154,170,188,204,221,238,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,238,221,204,187,170,153,136,119,102,85,68,51,34,17,
};
const uint8_t BLUE[126] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,18,34,52,68,86,102,120,136,154,170,188,
204,221,238,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,238,221,204,188,170,154,136,120,102,86,68,52,34,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
*/
uint32_t sys_time = 0;
uint32_t time_max = 0;

uint8_t curPixelMode = RGB_SPARKLE;    // current WS2812 Display Mode 

uint32_t marqueeNumMax = 0;
uint32_t marqueeNum = 0;
uint32_t marqueeRGB = 0;
uint8_t marqueeRGBNumMax = 0;
uint8_t marqueeRGBNum = 0;
uint8_t marqueeTimeFlag = 0;

uint32_t rainbowNumMax = 0;
uint32_t rainbowNum = 0;
uint8_t rainbowRGBNumMax = 0;
uint8_t rainbowRGBNum = 0;
uint8_t rainbowTimeFlag = 0;

boolean shouldHandleButton = true;      // button needs handling flag 0 = true
uint8_t modeFlag = 0;                   // new WS2812 Display Mode 


void setup() {
    Wire.begin(COLOR_PIXEL_ADDR);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);
#ifdef DEBUG    
    Serial.begin(115200);               // start serial for output
    Serial.println("Xadow Duino debug.");
#endif;
    //Timer1.initialize(100000);        // 0.1 seconds in microseconds
    Timer1.initialize(500000);          // half second in microseconds
    Timer1.attachInterrupt(timerIsr);

    pinMode(npGroundPin, OUTPUT);
    pinMode(npDataPin, OUTPUT);
    pinMode(buttonPin, INPUT);
    pinMode(RGB_INT1_PIN, OUTPUT);
    pinMode(RGB_INT2_PIN, INPUT);
    
    digitalWrite(npGroundPin, HIGH);
    digitalWrite(npDataPin, HIGH);
    digitalWrite(RGB_INT1_PIN, HIGH);
    
    pixels.begin();
    pixels.setBrightness(64);
    delay(1000);
    
    setPixelsPower(true);
}

void loop() {
    uint32_t rgbData = 0;
    uint16_t timeData = 0;
    
    if (flag_data_receive) {
        modeFlag = 0;
        Timer1.stop();
        flag_data_receive = 0;
        copyArray(I2C_buffer,buffer,BUFFER_SIZE);
        
        switch(buffer[0]) {
            case RGB_SET_WORK:
                rgbData |= buffer[2];
                rgbData <<= 8;
                rgbData |= buffer[3];
                rgbData <<= 8;
                rgbData |= buffer[4];
#ifdef DEBUG    
                Serial.print("rgbData is 0x");Serial.print(rgbData,HEX);Serial.print("\r\n");
#endif; 
                setPixelsColor(buffer[1], rgbData);
                break;
            case RGB_SET_MODE:
                if (buffer[1] == RGB_MONOCHROME || buffer[1] == RGB_MARQUEE) {
                    rgbData |= buffer[3];
                    rgbData <<= 8;
                    rgbData |= buffer[4];
                    rgbData <<= 8;
                    rgbData |= buffer[5];
                    
                    timeData |= buffer[6];
                    timeData <<= 8;
                    timeData |= buffer[7];
                } else if(buffer[1] == RGB_RAINBOW) {
                    timeData |= buffer[3];
                    timeData <<= 8;
                    timeData |= buffer[4];
                }                             
#ifdef DEBUG                
                Serial.print("rgbData is 0x");Serial.print(rgbData,HEX);Serial.print("\r\n");
                Serial.print("timeData is ");Serial.print(timeData);Serial.print("\r\n");
#endif;                
                setPixelsMode(buffer[1], buffer[2], rgbData, timeData);
                break;
            default:
                break;
        }
    }
    // handle on board mode button presses
    if (shouldHandleButton == true) {
        shouldHandleButton = false;
        if (digitalRead(buttonPin) == 0) { // button released
            modeFlag ++;
            Timer1.stop();
            if (modeFlag > 4){
              modeFlag = 0;
            }
#ifdef DEBUG  
            Serial.print("Button press! curPixelMode is ");
            Serial.println(modeFlag);
#endif;
            if (modeFlag == 0) {
                setPixelsMode(RGB_POWER_OFF, 0, 0, 0);
            } else if (modeFlag == 1) {
                ws2812Display(npMaxNum, 0);
                setPixelsMode(RGB_MONOCHROME, npMaxNum, 0xff00ff, 1);  // RGB: 255, 0, 255
            } else if (modeFlag == 2) {
                ws2812Display(npMaxNum, 0);
                setPixelsMode(RGB_MARQUEE, npMaxNum, 0xffff00, 9);     // RGB: 255, 255, 0
            } else if (modeFlag == 3) {
                setPixelsMode(RGB_RAINBOW, npMaxNum, 0, 10);
            } else if (modeFlag == 4) {
                setPixelsMode(RGB_SPARKLE, npMaxNum, 0x0000ff, 10);     // RGB: 0, 0, 255
            }
        }
    }
    if (digitalRead(buttonPin)) { // button pressed
        shouldHandleButton = true;
    }
    delay(25);
}

void copyArray(uint8_t source[],uint8_t destination[],uint8_t length) {
	for(uint8_t i = 0; i < length;i ++) {
		destination[i] = source[i];
	}
}

void clearBuffer(uint8_t buff[],uint8_t size) {
    for (uint8_t i = 0;i < size;i++) {
        buff[i]=0x00;
    }
}

// Fills ucNum neopixels with ulRGB color
void ws2812Display(uint8_t ucNum, uint32_t ulRGB) {   
    uint8_t i;

    if (ucNum > npMaxNum) {
        ucNum = npMaxNum;
    }
    for(i=0;i<ucNum;i++) {
        pixels.setPixelColor(i, pixels.Color((ulRGB >> 16) & 0xff,(ulRGB >> 8) & 0xff,ulRGB & 0xff));
    }
    digitalWrite(npGroundPin, HIGH);
    pixels.show();
    
    for(i=0;i<ucNum;i++) {
        rgbValue[i].rValue = ulRGB >> 16;
        rgbValue[i].gValue = ulRGB >> 8;
        rgbValue[i].bValue = ulRGB;
    }
}

void setPixelsPower(boolean shouldBeOn) {
    if (shouldBeOn == true) {
        digitalWrite(npDataPin, LOW);
        digitalWrite(npGroundPin, HIGH);
    } else { // off
        digitalWrite(npDataPin, HIGH);
        digitalWrite(npGroundPin, LOW);
    }
}

void setPixelsColor(uint8_t ucNum,uint32_t ulRGB) {
    uint8_t i;
    uint32_t grbTemp;
    
    if(ucNum > (npMaxNum - 1)) return;
    
    rgbValue[ucNum].rValue = ulRGB >> 16;
    rgbValue[ucNum].gValue = ulRGB >> 8;
    rgbValue[ucNum].bValue = ulRGB;
    
    for(i=0;i<npMaxNum;i++) {
        grbTemp |= rgbValue[i].rValue;
        grbTemp <<= 8;
        grbTemp |= rgbValue[i].gValue;
        grbTemp <<= 8;
        grbTemp |= rgbValue[i].bValue;
        pixels.setPixelColor(i, pixels.Color(rgbValue[i].rValue, rgbValue[i].gValue, rgbValue[i].bValue));
    }
    digitalWrite(npGroundPin, HIGH);
    pixels.show();
}

void setPixelsMode(uint8_t ucMode, uint8_t ucNum, uint32_t ulRGB, uint16_t uiTime) {
    uint32_t data = 0;
    
    switch(ucMode) {
        case RGB_POWER_ON:
            sys_time = 0;
            setPixelsPower(true);
            break;
        case RGB_POWER_OFF:
            sys_time = 0;
            ws2812Display(npMaxNum, 0);
            setPixelsPower(false);
            break;
        case RGB_MONOCHROME:
            curPixelMode = RGB_MONOCHROME;
            sys_time = 0;
            if (uiTime == 0) {
                ws2812Display(npMaxNum,ulRGB);
                return;
            }
            ws2812Display(npMaxNum, 0);
            ws2812Display(ucNum,ulRGB);
            time_max = uiTime * 10;
            setPixelsPower(true);
            Timer1.resume();
#ifdef DEBUG
            Serial.print("RGB_MONOCHROME time_max is ");Serial.print(time_max);Serial.print("\r\n");
#endif;  
            break;
        case RGB_MARQUEE:
            curPixelMode = RGB_MARQUEE;
            sys_time = 0;
            marqueeRGBNum = 0;
            
            if (uiTime == 0) {
                marqueeTimeFlag = 1;
            } else {
                marqueeTimeFlag = 0;
            }
            marqueeNumMax = uiTime * 10;
            if (ucNum > npMaxNum) {
                marqueeRGBNumMax = npMaxNum;
            } else {
                marqueeRGBNumMax = ucNum;
            }
            marqueeRGB = ulRGB;
            sys_time = 0;
            ws2812Display(npMaxNum, 0);
            setPixelsColor(0,ulRGB);
            setPixelsPower(true);
            Timer1.resume();
#ifdef DEBUG
            Serial.print("RGB_MARQUEE marqueeNumMax is ");Serial.print(marqueeNumMax);Serial.print("\r\n");
#endif; 
            break;
        case RGB_RAINBOW:
            curPixelMode = RGB_RAINBOW;
            sys_time = 0;
            rainbowRGBNum = 0;
            
            if (uiTime == 0) {
                rainbowTimeFlag = 1;
            } else {
                rainbowTimeFlag = 0;
            }
            rainbowNumMax = uiTime * 10;
            if (ucNum > npMaxNum) {
                rainbowRGBNumMax = npMaxNum;
            } else {
                rainbowRGBNumMax = ucNum;
            }
            ws2812Display(npMaxNum, 0);
            data |= rgbValue[0].rValue;
            data <<= 8;
            data |= rgbValue[0].gValue;
            data <<= 8;
            data |= rgbValue[0].bValue;
            setPixelsColor(0, data);
            setPixelsPower(true);
            //Timer1.attachInterrupt(timerIsr);
            Timer1.resume();
#ifdef DEBUG
            Serial.print("RGB_MARQUEE rainbowNumMax is ");Serial.print(rainbowNumMax);Serial.print("\r\n");
#endif; 
            break;
        case RGB_SPARKLE:
            curPixelMode = RGB_SPARKLE;
            setPixelsPower(true);
            Timer1.resume();
            break;
        default:
            break;
    }
}

void receiveEvent(int howMany) {
	clearBuffer(I2C_buffer,BUFFER_SIZE);
	Char_nu = 0;
	while(0<Wire.available()) {
		I2C_buffer[Char_nu]=Wire.read();
		Char_nu++;
	}
#ifdef DEBUG
	Serial.println("the I2C_buffer  is ");
	for(unsigned char i =0;i<Char_nu;i++) {
        Serial.print("0x");
        Serial.print(I2C_buffer[i],HEX);
        Serial.print(" ");
	}
    Serial.print("\r\n");
#endif;
	flag_data_receive = 1;
}

void requestEvent() {
    Wire.write(scan_data[scan_count++]);
    if (scan_count >= 4) {
        scan_count = 0;
    }
}

void timerIsr() {
    uint8_t i;
    uint32_t data;
    
    sys_time ++;
// #ifdef DEBUG
//     Serial.print("sys_time is ");Serial.print(sys_time);Serial.print("\r\n");
// #endif;  
    if (curPixelMode == RGB_MONOCHROME) {
        if (sys_time > time_max) {
            sys_time = 0;
            curPixelMode = 0;
            time_max = 0;
            ws2812Display(npMaxNum,0);
            Timer1.stop();
            setPixelsPower(false);
        }
    } else if (curPixelMode == RGB_MARQUEE) {
        for(i=0;i<npMaxNum;i++) {
            rgbValue[i].rValue = 0;
            rgbValue[i].gValue = 0;
            rgbValue[i].bValue = 0;
        }
        setPixelsColor(marqueeRGBNum,marqueeRGB);
        
        marqueeRGBNum++;
        if (marqueeRGBNum >= marqueeRGBNumMax) {
            marqueeRGBNum = 0;
        }
        if(marqueeTimeFlag == 0) {
            marqueeNum ++;
            if (marqueeNum >= marqueeNumMax) {
                curPixelMode = 0;
                time_max = 0;
                marqueeNum = 0;
                marqueeNumMax = 0;
                marqueeRGB = 0;
                marqueeRGBNum = 0;
                Timer1.stop();
                ws2812Display(npMaxNum,0);
                setPixelsPower(false);
            }
        }
    } else if (curPixelMode == RGB_RAINBOW) {
        for (i=0;i<rainbowRGBNumMax;i++)  {
            rgbValue[i].rValue = pgm_read_byte(&RED[rainbowRGBNum+i]);
            rgbValue[i].gValue = pgm_read_byte(&GREEN[rainbowRGBNum+i]);
            rgbValue[i].bValue = pgm_read_byte(&BLUE[rainbowRGBNum+i]);
            /*
            rgbValue[i].rValue = RED[rainbowRGBNum+i];
            rgbValue[i].gValue = GREEN[rainbowRGBNum+i];
            rgbValue[i].bValue = BLUE[rainbowRGBNum+i];
            */
        }
        data |= rgbValue[0].rValue;
        data <<= 8;
        data |= rgbValue[0].gValue;
        data <<= 8;
        data |= rgbValue[0].bValue;
        setPixelsColor(0, data);
        
        rainbowRGBNum ++;

        if (rainbowRGBNum >= (126 - rainbowRGBNumMax)) {
            rainbowRGBNum = 0;
        }
        if (rainbowTimeFlag == 0) {
            rainbowNum ++;
            if(rainbowNum >= rainbowNumMax) {
                curPixelMode = 0;
                time_max = 0;
                rainbowNum = 0;
                rainbowNumMax = 0;
                rainbowRGBNum = 0;
                Timer1.stop();
                ws2812Display(npMaxNum,0);
                setPixelsPower(false);
            }
        }
    } else if (curPixelMode == RGB_SPARKLE) {
        int pixel = random(pixels.numPixels());
        uint32_t color = pixels.getPixelColor(pixel);
        if (color == 0) {
            //fadeIn(pixel, mainColor);
            color = pixels.Color(255, 0, 255); // magenta
            uint8_t red = color >> 16;
            uint8_t green = color >> 8;
            uint8_t blue = color;

            // fade in 5 steps
            for (int x=0; x < 5; x++) {
                int r = red * (x+1); r /= 5;
                int g = green * (x+1); g /= 5;
                int b = blue * (x+1); b /= 5;

                pixels.setPixelColor(pixel, pixels.Color(r, g, b));
                pixels.show();
                delay(40);
            }
        } else {
            //fadeOut(pixel);
            uint32_t color = pixels.getPixelColor(pixel);
            uint8_t red = color >> 16;
            uint8_t green = color >> 8;
            uint8_t blue = color;

            // fade out in 5 steps
            for (int x=5; x >= 0; x--) {
                int r = red * x; r /= 5;
                int g = green * x; g /= 5;
                int b = blue * x; b /= 5;

                pixels.setPixelColor(pixel, pixels.Color(r, g, b));
                pixels.show();
                delay(40);
            }
        }
        //pixels.show();
    }
}
