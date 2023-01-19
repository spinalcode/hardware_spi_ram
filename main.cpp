#define screenWidth 400

int sx=0;
int sy=0;

#include <Pokitto.h>

#include "globals.h"
#include "font.h"
#include "buttonhandling.h"

#define MISO_pin P1_21
#define MOSI_pin P1_22
#define SCLK_pin P1_20
#define CS_pin P1_5

SPI spi(MOSI_pin, MISO_pin, SCLK_pin); // mosi, miso, sclk
DigitalInOut cs(CS_pin);

#include "spi_ram.h"
#include "screen.h"


char tempText[64];

// print text
void myPrint(char x, char y, const char* text) {
    uint8_t numChars = strlen(text);
    uint8_t x1 = 0;//2+x+28*y;
    for (uint8_t t = 0; t < numChars; t++) {
        uint8_t character = text[t] - 32;
        Pokitto::Display::drawSprite(x+((x1++)*8), y, font88[character]);
    }
}

int main(){
    using PC=Pokitto::Core;
    using PD=Pokitto::Display;
    using PB=Pokitto::Buttons;
    using PS=Pokitto::Sound;

    PC::begin();
    PD::invisiblecolor = 0;
    PD::adjustCharStep = 1;
    PD::adjustLineStep = 0;
    PD::setFont(font3x5);


    // Setup the spi for 8 bit data, high steady state clock,
    // second edge capture, with a 20MHz clock rate
    spi.format(8, 3);
    spi.frequency(20000000);

    init_RAM();
    // load a larger than screen image to RAM, this one takes up nearly all of it.
    sendToRAM(0, &background1[0], 400*300);
    Pokitto::Display::load565Palette(background1_pal); // load a palette the same way as any other palette in any other screen mode

    PD::lineFillers[0] = myBGFiller; // A custom filler to draw from SRAM HAT to screen
    
    while( PC::isRunning() ){

        if(!PC::update()) continue;
        updateButtons();

        if(_Left[HELD] && sx>0) sx--;
        if(_Right[HELD] && sx<180) sx++;
        if(_Up[HELD] && sy>0) sy--;
        if(_Down[HELD] && sy<124) sy++;

        sprintf(tempText,"FPS:%d",fpsCount);
        myPrint(0,0,tempText);

        fpsCounter++;

        if(PC::getTime() >= lastMillis+1000){
            PD::clear();
            lastMillis = PC::getTime();
            fpsCount = fpsCounter;
            fpsCounter = 0;
        }

    }
    
    return 0;
}
