#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

static int update(void* userdata);

int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg)
{
    (void)arg;
    
    if ( event == kEventInit )
    {
        pd->system->setUpdateCallback(update, pd);
    }
    
    return 0;
}


static int update(void* userdata)
{
    PlaydateAPI* pd = userdata;
    
    LCDBitmap* bitmap = pd->graphics->newBitmap(400, 240, kColorWhite);
    
    int rowbytes;
    uint8_t* data;
    
    pd->graphics->getBitmapData(bitmap, NULL, NULL, &rowbytes, NULL, &data);
    
 
    for (int y = 0; y < 240; y+=2)
    {
        for (int x = 0; x < 400; x+=2)
        {
            int byteIndex = rowbytes * y + x / 8;
            int bitIndex = x % 8;
            data[byteIndex] &= ~(1 << bitIndex);
        }
    }

    // Clear the screen
    pd->graphics->clear(kColorWhite);
    
    // Create a checkered pattern bitmap
    
    // Draw the checkered pattern bitmap on the screen
    pd->graphics->drawBitmap(bitmap, 0, 0, 0);
    
    // Free the checkered pattern bitmap
    pd->graphics->freeBitmap(bitmap);
    
    return 1;
    
}

