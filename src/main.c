#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"
#include "perlin.h"

PlaydateAPI* pd;

//Rendering
LCDBitmap* screenBuffer;
uint8_t* screenBufferData;
int scroll = 0;
float mtx9c[4][4] = {{8.0/9, 4.0/9, 7.0/9, 3.0/9},{2.0/9, 6.0/9, 1.0/9, 5.0/9}, {7.0/9, 3.0/9, 8.0/9, 4.0/9},{1.0/9, 5.0/9, 2.0/9, 6.0/9}};

//Movement
unsigned int lastFrame;
int deltaTimeMS;
uint32_t positionX = 2000000;
uint32_t positionY = 2000000;
float speed = 0.1f;


static int update(void* userdata);

int eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg)
{
    (void)arg;
    
    if ( event == kEventInit )
    {
        pd = playdate;
        pd->system->setUpdateCallback(update, pd);
        screenBuffer = pd->graphics->newBitmap(400, 240, kColorWhite);
        pd->graphics->getBitmapData(screenBuffer, NULL, NULL, NULL, NULL, &screenBufferData);
        pd->display->setRefreshRate(0);
    }
    return 0;
}

static void moveCamera(void){
    
    PDButtons current;
    pd->system->getButtonState(&current, NULL, NULL);

    //LOG
    //pd->system->logToConsole("X:%d - Y:%d ------ MS:%d",positionX, positionY, deltaTimeMS);
    
    if ( current & kButtonUp ) {
        positionY -= speed * deltaTimeMS;
    } else if ( current & kButtonDown ) {
        positionY += speed * deltaTimeMS;
    }
    if ( current & kButtonLeft ) {
        positionX -= speed * deltaTimeMS;
    } else if ( current & kButtonRight ) {
        positionX += speed * deltaTimeMS;
    }
    
}


static void updateBuffer(void){
    for (int y = 0; y < 240; y+=1)
    {
        for (int x = 0; x < 400; x+=1)
        {
            {
                int byteIndex = 52 * y + x / 8; //52 Rowbytes
                int bitIndex = 7 - x % 8;
                
                //pd->system->logToConsole("%f ----",perlin2d(x+scroll, y+scroll, 0.1, 4), "", "");
                //pd->system->logToConsole("%f ----",mtx9c[0][0], "", "");
                
                if(perlin2d(x+positionX, y+positionY, 0.01, 4)>mtx9c[y%4][x%4]){
                    screenBufferData[byteIndex] &= ~(1 << bitIndex);
                }else{
                    screenBufferData[byteIndex] |= (1 << bitIndex);
                }
            }
        }
    }
}

static int update(void* userdata){
    //Check player location
    
    
    deltaTimeMS = pd->system->getCurrentTimeMilliseconds() - lastFrame;
    lastFrame = pd->system->getCurrentTimeMilliseconds();
    
    moveCamera();
    updateBuffer();
    
    pd->graphics->drawBitmap(screenBuffer, 0, 0, 0);
    pd->system->drawFPS(10,10);
    pd->system->resetElapsedTime();
    return 1;
}

