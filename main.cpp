/**

* Author: Jiaxin Lin

* Assignment: Simple 2D Scene

* Date due: 10/05/2026

* I pledge that I have completed this assignment without

* collaborating with anyone else, in conformance with the

* NYU School of Engineering Policies and Procedures on

* Academic Misconduct.

**/


#include "cs3113.h"
#include <math.h>

// Global Constants
constexpr int   SCREEN_WIDTH  = 1000,
                SCREEN_HEIGHT = 800,
                FPS           = 60,
                SIZE          = 80;

// Sun global constants
constexpr float SUN_MOVE_SPEED   = 10.0f;
constexpr float SQUARE_LIMIT = 20.0f;
constexpr float SUN_BASE_SIZE   = 200.0f;
constexpr float SUN_PULSE_AMP   = 10.0f;
constexpr float SUN_PULSE_SPEED = 2.0f;

// Earth global constants
constexpr float EARTH_ORBIT_X = 300.0f;
constexpr float EARTH_ORBIT_Y = 200.0f;
constexpr float EARTH_ORBIT_SPEED = 0.2f;
constexpr float EARTH_ROTATION_SPEED = 30.0f;

// Moon global constants
constexpr float MOON_ORBIT_RADIUS = 80.0f;
constexpr float MOON_ORBIT_SPEED  = 2.0f;

// Meteor global constants
constexpr float METEOR_INTERVAL = 5.0f;
constexpr float METEOR_SPEED_X  = 1500.0f;
constexpr float METEOR_SPEED_Y  = 700.0f;
constexpr float METEOR_SIZE_W   = 120.0f;
constexpr float METEOR_SIZE_H   = 120.0f;

constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
constexpr Vector2 BASE_SIZE   = { static_cast<float>(SIZE), static_cast<float>(SIZE) };

// Image downloaded from Google Image
constexpr char Sun_FP[]    = "Sun.png";
constexpr char Earth_FP[]  = "Earth.png";
constexpr char Moon_FP[]   = "Moon.png";
constexpr char Meteor_FP[] = "Meteor.png";

// Global Variables
AppStatus gAppStatus     = RUNNING;
float     gPulseTime     = 0.0f;
Vector2 gPosition = {
    ORIGIN.x - SQUARE_LIMIT,
    ORIGIN.y - SQUARE_LIMIT
};
Texture2D gSunTexture;
Vector2   gScale         = BASE_SIZE;
float     gPreviousTicks = 0.0f;
int gDirection = 0;
float gSunSize = SUN_BASE_SIZE;

// Earth global Variable
Texture2D gEarthTexture;
float gEarthOrbitAngle = 0.0f;
float gEarthRotation = 0.0f;
Vector2 gEarthPosition = ORIGIN;

// Moon global variable
Texture2D gMoonTexture;
float gMoonOrbitAngle = 0.0f;
Vector2 gMoonPosition = ORIGIN;

// Meteor global variable
Texture2D gMeteorTexture;
bool gMeteorActive = false;
float gMeteorTimer = 0.0f;
Vector2 gMeteorPosition = { 0.0f, 0.0f };

// Background global variable
float gBackgroundTime = 0.0f;
constexpr float BG_CHANGE_SPEED = 0.15f;


// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

// Function Definitions
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Textures");

    gSunTexture = LoadTexture(Sun_FP);
    gEarthTexture = LoadTexture(Earth_FP);
    gMoonTexture = LoadTexture(Moon_FP);
    gMeteorTexture = LoadTexture(Meteor_FP);

    SetTargetFPS(FPS);
}

void processInput()
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{
    // Delta time
    float ticks = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;
    
    // Sun orbit as a square
    float left   = ORIGIN.x - SQUARE_LIMIT;
    float right  = ORIGIN.x + SQUARE_LIMIT;
    float top    = ORIGIN.y - SQUARE_LIMIT;
    float bottom = ORIGIN.y + SQUARE_LIMIT;

    if (gDirection == 0)
    {
        gPosition.x += SUN_MOVE_SPEED * deltaTime;

        if (gPosition.x >= right)
        {
            gPosition.x = right;
            gDirection = 1;
        }
    }
    else if (gDirection == 1)
    {
        gPosition.y += SUN_MOVE_SPEED * deltaTime;

        if (gPosition.y >= bottom)
        {
            gPosition.y = bottom;
            gDirection = 2;
        }
    }
    else if (gDirection == 2)
    {
        gPosition.x -= SUN_MOVE_SPEED * deltaTime;

        if (gPosition.x <= left)
        {
            gPosition.x = left;
            gDirection = 3;
        }
    }
    else if (gDirection == 3)
    {
        gPosition.y -= SUN_MOVE_SPEED * deltaTime;

        if (gPosition.y <= top)
        {
            gPosition.y = top;
            gDirection = 0;
        }
    }
    
    // Sun palsing
    gPulseTime += SUN_PULSE_SPEED * deltaTime;
    gSunSize = SUN_BASE_SIZE + SUN_PULSE_AMP * sin(gPulseTime);
    
    // Earth orbit
    gEarthOrbitAngle += EARTH_ORBIT_SPEED * deltaTime;

    gEarthPosition.x =
        gPosition.x + EARTH_ORBIT_X * cos(gEarthOrbitAngle);

    gEarthPosition.y =
        gPosition.y + EARTH_ORBIT_Y * sin(gEarthOrbitAngle);

    // Earth rotation
    gEarthRotation += EARTH_ROTATION_SPEED * deltaTime;
    
    // Moon orbit around Earth
    gMoonOrbitAngle += MOON_ORBIT_SPEED * deltaTime;

    gMoonPosition.x =
        gEarthPosition.x + MOON_ORBIT_RADIUS * cos(gMoonOrbitAngle);

    gMoonPosition.y =
        gEarthPosition.y + MOON_ORBIT_RADIUS * sin(gMoonOrbitAngle);
    
    // Background change
    gBackgroundTime += BG_CHANGE_SPEED * deltaTime;
    
    // Meteor logic
    gMeteorTimer += deltaTime;

    if (!gMeteorActive && gMeteorTimer >= METEOR_INTERVAL)
    {
        gMeteorActive = true;
        gMeteorTimer = 0.0f;

        gMeteorPosition.x = SCREEN_WIDTH + METEOR_SIZE_W / 2.0f;
        gMeteorPosition.y = 80.0f;
    }

    if (gMeteorActive)
    {
        gMeteorPosition.x -= METEOR_SPEED_X * deltaTime;
        gMeteorPosition.y += METEOR_SPEED_Y * deltaTime;

        if (gMeteorPosition.x < -METEOR_SIZE_W ||
            gMeteorPosition.y > SCREEN_HEIGHT + METEOR_SIZE_H)
        {
            gMeteorActive = false;
        }
    }
}

void render()
{
    BeginDrawing();

    float bgFactor = (sin(gBackgroundTime - PI / 2.0f) + 1.0f) / 2.0f;

    Color backgroundColor = {
        static_cast<unsigned char>(30 * bgFactor),
        static_cast<unsigned char>(20 * bgFactor),
        static_cast<unsigned char>(80 * bgFactor),
        255
    };

    ClearBackground(backgroundColor);

    Rectangle sunTextureArea = {
        0.0f,
        0.0f,
        static_cast<float>(gSunTexture.width),
        static_cast<float>(gSunTexture.height)
    };

    Rectangle earthTextureArea = {
        0.0f,
        0.0f,
        static_cast<float>(gEarthTexture.width),
        static_cast<float>(gEarthTexture.height)
    };

    Rectangle moonTextureArea = {
        0.0f,
        0.0f,
        static_cast<float>(gMoonTexture.width),
        static_cast<float>(gMoonTexture.height)
    };

    Rectangle sunDestinationArea = {
        gPosition.x,
        gPosition.y,
        gSunSize,
        gSunSize
    };
    
    Rectangle earthDestinationArea = {
        gEarthPosition.x,
        gEarthPosition.y,
        gScale.x,
        gScale.y
    };
    
    Rectangle moonDestinationArea = {
        gMoonPosition.x,
        gMoonPosition.y,
        gScale.x / 2.0f,
        gScale.y / 2.0f
    };
    
    Vector2 sunObjectOrigin = {
        sunDestinationArea.width / 2.0f,
        sunDestinationArea.height / 2.0f
    };
    
    Vector2 objectOrigin = {
        gScale.x / 2.0f,
        gScale.y / 2.0f
    };
    
    Vector2 moonOrigin = {
        moonDestinationArea.width / 2.0f,
        moonDestinationArea.height / 2.0f
    };

    DrawTexturePro(
        gSunTexture,
        sunTextureArea,
        sunDestinationArea,
        sunObjectOrigin,
        0.0f,
        WHITE
    );

    DrawTexturePro(
        gEarthTexture,
        earthTextureArea,
        earthDestinationArea,
        objectOrigin,
        gEarthRotation,
        WHITE
    );

    DrawTexturePro(
        gMoonTexture,
        moonTextureArea,
        moonDestinationArea,
        moonOrigin,
        0.0f,
        WHITE
    );
    
    if (gMeteorActive)
    {
        Rectangle meteorTextureArea = {
            0.0f,
            0.0f,
            static_cast<float>(gMeteorTexture.width),
            static_cast<float>(gMeteorTexture.height)
        };

        Rectangle meteorDestinationArea = {
            gMeteorPosition.x,
            gMeteorPosition.y,
            METEOR_SIZE_W,
            METEOR_SIZE_H
        };

        Vector2 meteorOrigin = {
            meteorDestinationArea.width / 2.0f,
            meteorDestinationArea.height / 2.0f
        };

        DrawTexturePro(
            gMeteorTexture,
            meteorTextureArea,
            meteorDestinationArea,
            meteorOrigin,
            0.0f,
            WHITE
        );
    }

    EndDrawing();
}

void shutdown()
{
    UnloadTexture(gSunTexture);
    UnloadTexture(gEarthTexture);
    UnloadTexture(gMoonTexture);
    UnloadTexture(gMeteorTexture);
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}
