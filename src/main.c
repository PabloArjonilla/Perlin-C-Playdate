#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"
#include "perlin.h"

static int update(void* userdata);

PlaydateAPI* pd;

typedef struct {
    uint32_t x;
    uint32_t y;
    uint8_t drawDistance;
    float fov;
} camera_t;

camera_t camera = {
    .x = 20000,
    .y = 20000,
    .drawDistance = 400,
    .fov = 45.0
};

//Rendering
LCDBitmap* screenBuffer;
uint8_t* screenBufferData;
int scroll = 0;
float mtx9c[4][4] = {
    {8.0/9, 4.0/9, 7.0/9, 3.0/9},
    {2.0/9, 6.0/9, 1.0/9, 5.0/9},
    {7.0/9, 3.0/9, 8.0/9, 4.0/9},
    {1.0/9, 5.0/9, 2.0/9, 6.0/9}
};

//Delta time
unsigned int lastFrame;
int deltaTimeMS;


//Movement
uint32_t positionX = 2000;
uint32_t positionY = 2000;
float speed = 0.001f;



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


static void projectView(uint32_t cameraX, uint32_t cameraY, uint8_t drawDistance, float fov){
    
    float distance = drawDistance * fov;
    
    float plx = - distance;
    float ply = distance;
    
    float prx = distance;
    float pry = distance;
    
    for (int i = 0; i < 400; i++)
    {
        float deltaX = (plx + (prx-plx)/400*i)/drawDistance;
        float deltaY = (ply + (pry-ply)/400*i)/drawDistance;
        
        //pd->system->logToConsole("deltaX = %f, deltaY = %f", deltaX, deltaY);
    
        float rx = cameraX;
        float ry = cameraY;
        
        float maxHeight = 239;
        
        for (int z = 1; z < camera.drawDistance; z++)
        {
            rx += deltaX;
            ry -= deltaY;
            
            float height = perlin2d(rx, ry, 0.01, 1);
            float screenHeight = perlin2d(rx, ry, 0.01, 1)*239;
            

            //pd->system->logToConsole("rx = %f --- ry= %f...... height:%f",rx, ry, height);
     
            if(screenHeight<maxHeight){
                
                for(int y = screenHeight; y < maxHeight; y++){

                    int byteIndex = 52 * y + i / 8; //52 Rowbytes
                    int bitIndex = 7 - i % 8;
                    
                    screenBufferData[byteIndex] &= ~(1 << bitIndex);

                    if(height>mtx9c[y%4][i%4]){
                        screenBufferData[byteIndex] &= ~(1 << bitIndex);
                    }else{
                        screenBufferData[byteIndex] |= (1 << bitIndex);
                    }
                    
                }
                
                maxHeight = height;
            }
        }
    }
}

static void updateBuffer(void){
    for (int y = 0; y < 240; y++)
    {
        for (int x = 0; x < 400; x++)
        {
            {
                int byteIndex = 52 * y + x / 8; //52 Rowbytes
                int bitIndex = 7 - x % 8;
                
                //pd->system->logToConsole("%f ----",perlin2d(x+scroll, y+scroll, 0.1, 4), "", "");
                //pd->system->logToConsole("%f ----",mtx9c[0][0], "", "");
                
                if(perlin2d(x+positionX, y+positionY, 0.001, 4)>mtx9c[y%4][x%4]){
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
    
    //moveCamera();
    projectView(camera.x,camera.y, camera.drawDistance, camera.fov);
    //updateBuffer();
    
    pd->graphics->drawBitmap(screenBuffer, 0, 0, 0);
    pd->system->drawFPS(10,10);
    pd->system->resetElapsedTime();
    return 1;
}

