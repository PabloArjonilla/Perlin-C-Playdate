#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"
#include "perlin.h"
#include "math.h"

static int update(void* userdata);

PlaydateAPI* pd;

typedef struct {
    uint32_t x;
    uint32_t y;
    int drawDistance;
    float fov;
} camera_t;

camera_t camera = {
    .x = 20000,
    .y = 20000,
    .drawDistance = 800,
    .fov = M_PI/4
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
float speed = 0.1f;


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
    //pd->system->logToConsole("X:%d - Y:%d ------ MS:%d",positionX, positionY);
  
    if (current & kButtonUp) {
        camera.y -= speed * deltaTimeMS;
    } else if (current & kButtonDown) {
        camera.y += speed * deltaTimeMS;
    }
    
    if (current & kButtonLeft) {
        camera.x -= speed * deltaTimeMS;
    } else if (current & kButtonRight) {
        camera.x += speed * deltaTimeMS;
    }

}


static void projectView(uint32_t cameraX, uint32_t cameraY, int drawDistance, float fov){
    
    float distance = drawDistance * tanf(fov);
    
    float plx = - distance;
    float ply = distance;
    
    float prx = distance;
    float pry = distance;
    
    //pd->system->logToConsole("x=%f, y=%f, distance=%f, plx=%f, ply=%f, prx=%f, pry=%f", cameraX, cameraY, distance,  plx, ply, prx, pry);
    
    for (int i = 0; i < 400; i++)
    {
        float deltaX = (plx + ((prx - plx) / 400 * i)) / drawDistance;
        float deltaY = (ply + ((pry - ply) / 400 * i)) / drawDistance;
        
        //pd->system->logToConsole("deltaX = %f, deltaY = %f", deltaX, deltaY);
        
        float rx = cameraX;
        float ry = cameraY;
        
        int maxHeight = 239;
        
        for (int z = 1; z < drawDistance; z++)
        {
            rx += deltaX;
            ry -= deltaY;
            
            float height = perlin2d(rx, ry, 0.005, 2);
            float color = perlin2d(rx, ry, 0.02, 1);
            
            float screenHeight = ((239 * height/ z) * 90) + 50 ;
            
            if(screenHeight<0){
                screenHeight = 0;
            }else if (screenHeight > 239){
                screenHeight = 239;
            }
            
            //pd->system->logToConsole("Current height is %f, and screen height is %d", height , screenHeight);
            
            if(screenHeight<maxHeight){
                
                for(int y = screenHeight; y < maxHeight; y++){
                    
                    int byteIndex = 52 * y + i / 8; //52 Rowbytes
                    int bitIndex = 7 - i % 8;
                    
                    if(color>mtx9c[y%4][i%4]-0.1f){
                        screenBufferData[byteIndex] &= ~(1 << bitIndex);
                    }else{
                        screenBufferData[byteIndex] |= (1 << bitIndex);
                    }
                    
                }
                
                maxHeight = screenHeight;
            }
        }
        
        for(int y = 0; y < maxHeight; y++){
    
            int byteIndex = 52 * y + i / 8;
            int bitIndex = 7 - i % 8;
            screenBufferData[byteIndex] |= (1 << bitIndex);
        }
    }
}

static int update(void* userdata){
    //Check player location
    
    deltaTimeMS = pd->system->getCurrentTimeMilliseconds() - lastFrame;
    lastFrame = pd->system->getCurrentTimeMilliseconds();
    
    moveCamera();
    projectView(camera.x,camera.y, camera.drawDistance, camera.fov);

    pd->graphics->drawBitmap(screenBuffer, 0, 0, 0);
    pd->system->drawFPS(10,10);
    pd->system->resetElapsedTime();
    return 1;
}

