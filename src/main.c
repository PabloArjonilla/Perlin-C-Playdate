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
    float angle;
    int height;
    int drawDistance;
    float fov;
} camera_t;

camera_t camera = {
    .x = 20000,
    .y = 20000,
    .angle = M_PI/3.5,
    .height = 100,
    .drawDistance = 1200,
    .fov = M_PI/3.5
};

//Rendering
LCDBitmap* screenBuffer;
uint8_t* screenBufferData;
int scroll = 0;


float mtx17c[4][4] ={
    {16.0/17,  8.0/17, 14.0/17,  6.0/17},
    {4.0/17, 12.0/17,  2.0/17, 10.0/17},
    {13.0/17,  5.0/17, 15.0/17,  7.0/17},
    {1.0/17,  9.0/17,  3.0/17, 11.0/17}
};

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
float speedMovement = 0.1f;
float speedRotation = 0.001f;


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
    //pd->system->logToConsole("Angle:%f",camera.angle);
    
    if (current & kButtonUp) {
        camera.x += speedMovement * deltaTimeMS * cos(camera.angle);
        camera.y -= speedMovement * deltaTimeMS * sin(camera.angle);
    } else if (current & kButtonDown) {
        camera.x -= speedMovement * deltaTimeMS * cos(camera.angle);
        camera.y += speedMovement * deltaTimeMS * sin(camera.angle);
   
    }
    
    if (current & kButtonLeft) {
        camera.angle -= speedRotation * deltaTimeMS;
    } else if (current & kButtonRight) {
        camera.angle += speedRotation * deltaTimeMS;
    }
    
}


static void projectView(uint32_t cameraX, uint32_t cameraY, int cameraHeight, float cameraAngle, int drawDistance, float fov){
    
    float sinAngle = sin(cameraAngle);
    float cosAngle = cos(cameraAngle);
    
    float distance = drawDistance * tanf(fov);
    
    float plx = cosAngle * distance + sinAngle * distance;
    float ply = sinAngle * distance - cosAngle * distance;
    
    float prx = cosAngle * distance - sinAngle * distance;;
    float pry = sinAngle * distance + cosAngle * distance;
;
    
    //pd->system->logToConsole("x=%f, y=%f, distance=%f, plx=%f, ply=%f, prx=%f, pry=%f", cameraX, cameraY, distance,  plx, ply, prx, pry);
    
    for (int i = 0; i < 400; i++)
    {
        
        float deltaX = (plx + ((prx - plx) / 400 * i)) / drawDistance;
        float deltaY = (ply + ((pry - ply) / 400 * i)) / drawDistance;
        
        //pd->system->logToConsole("deltaX = %f, deltaY = %f", deltaX, deltaY);
        
        float rx = cameraX;
        float ry = cameraY;
        
        int maxHeight = 239;
        
        for (int z = 50; z < drawDistance; z+=2)
        {
            rx += deltaX;
            ry -= deltaY;
            
            float height = perlin2d(rx, ry, 0.03, 1);
            float color = perlin2d(rx, ry, 0.01, 1);
            
            if(height>0.6f){
                color = 1.0f;
            }
            
            float screenHeight = ((239 * height/ z) * 120) + cameraHeight ;
            
            if(screenHeight<0){
                screenHeight = 0;
            }else if (screenHeight > 239){
                screenHeight = 239;
            }
            
            //pd->system->logToConsole("Current height is %f, and screen height is %d", height , screenHeight);
            
            if(screenHeight<maxHeight){
        
                for(int y = screenHeight; y < maxHeight; y++){
                    
                    if(y < 119){
                        continue;
                    }
                    
                    int byteIndex = 52 * y + i / 8; //52 Rowbytes
                    int bitIndex = 7 - i % 8;
                    
                    if(color>mtx17c[y%4][i%4]-0.1f){
                        screenBufferData[byteIndex] &= ~(1 << bitIndex);
                    }else{
                        screenBufferData[byteIndex] |= (1 << bitIndex);
                    }
                }
                
                for(int y = (239 - screenHeight); y >= (239-maxHeight); y--){

                    if(y > 119){
                        continue;
                    }

                    int byteIndex = 52 * y + i / 8; //52 Rowbytes
                    int bitIndex = 7 - i % 8;

                    if(color>mtx17c[y%4][i%4]-0.1f){
                        screenBufferData[byteIndex] &= ~(1 << bitIndex);
                    }else{
                        screenBufferData[byteIndex] |= (1 << bitIndex);
                    }
                }
                
                maxHeight = screenHeight;
            }
        }
        
//        for(int y = 0; y < maxHeight; y++){
//
//            int byteIndex = 52 * y + i / 8;
//            int bitIndex = 7 - i % 8;
//            screenBufferData[byteIndex] |= (1 << bitIndex);
//            screenBufferData[byteIndex] &= ~(1 << bitIndex);
//        }
    }
}

static int update(void* userdata){
    //Check player location
    
    deltaTimeMS = pd->system->getCurrentTimeMilliseconds() - lastFrame;
    lastFrame = pd->system->getCurrentTimeMilliseconds();
    
    moveCamera();
    projectView(camera.x,camera.y, camera.height, camera.angle, camera.drawDistance, camera.fov);
    
    pd->graphics->drawBitmap(screenBuffer, 0, 0, 0);
    pd->system->drawFPS(10,10);
    pd->system->resetElapsedTime();
    return 1;
}

