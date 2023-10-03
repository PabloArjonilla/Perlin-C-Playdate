#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"
#include "perlin.h"

PlaydateAPI* pd;

LCDBitmap* screenBuffer;
int rowbytes;
uint8_t* screenBufferData;
int scroll = 0;

static int update(void* userdata);

#define SCREEN_WIDTH = 400
#define SCREEN_HEIGHT = 240

int eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg)
{
    (void)arg;
    
    if ( event == kEventInit )
    {
        pd = playdate;
        pd->system->setUpdateCallback(update, pd);
        screenBuffer = pd->graphics->newBitmap(400, 240, kColorWhite);
        pd->graphics->getBitmapData(screenBuffer, NULL, NULL, &rowbytes, NULL, &screenBufferData);
        pd->display->setRefreshRate(1);
    }
    return 0;
}


static void updateBuffer(int scroll){
    for (int y = 0; y < 240; y+=1)
    {
        for (int x = 0; x < 400; x+=1)
        {
            {
                int byteIndex = rowbytes * y + x / 8;
                int bitIndex = 7 - x % 8;
     
               //pd->system->logToConsole("%f ---- %f",perlin2d(x+scroll, y+scroll, 0.1, 4), noise2d(x+scroll, y+scroll)/256 , "", "");
                if(perlin2d(x+scroll, y+scroll, 0.1, 4)>0.5f){
                    screenBufferData[byteIndex] &= ~(1 << bitIndex);
                }else{
                    screenBufferData[byteIndex] |= (1 << bitIndex);
                }
            }
        }
    }
}


//BLACK screenBufferData[byteIndex] &= ~(1 << bitIndex);
//WHITE screenBufferData[byteIndex] |= (1 << bitIndex);
static int update(void* userdata){
    //Check player location
    
    updateBuffer(++scroll);
    
    pd->graphics->drawBitmap(screenBuffer, 0, 0, 0);
    pd->system->drawFPS(10,10);
    return 1;
}

