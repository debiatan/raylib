/**********************************************************************************************
*
*   raylib.text
*
*   Basic functions to load SpriteFonts and draw Text
*
*   Copyright (c) 2014 Ramon Santamaria (Ray San - raysan@raysanweb.com)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include "raylib.h"

#include <stdlib.h>       // Declares malloc() and free() for memory management
#include <string.h>       // String management functions (just strlen() is used)
#include <stdarg.h>       // Used for functions with variable number of parameters (FormatText())
#include <stdio.h>        // Standard input / output lib

#include "rlgl.h"         // raylib OpenGL abstraction layer to OpenGL 1.1, 3.3+ or ES2
#include "utils.h"        // Required for function GetExtendion()

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define FONT_FIRST_CHAR         32
#define MAX_FONTCHARS          128
#define MAX_FORMATTEXT_LENGTH   50

#define BIT_CHECK(a,b) ((a) & (1<<(b)))

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef unsigned char byte;

// SpriteFont one Character (Glyph) data
typedef struct Character {
    int value;        //char value = ' '; (int)value = 32;
    int x;
    int y;
    int w;
    int h;
} Character;

//----------------------------------------------------------------------------------
// Global variables
//----------------------------------------------------------------------------------
static SpriteFont defaultFont;        // Default font provided by raylib
// NOTE: defaultFont is loaded on InitWindow and disposed on CloseWindow [module: core]

//----------------------------------------------------------------------------------
// Other Modules Functions Declaration (required by text)
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
static bool PixelIsMagenta(Color p);                // Check if a pixel is magenta
static int ParseImageData(Color *imgDataPixel, int imgWidth, int imgHeight, Character **charSet);    // Parse image pixel data to obtain character set measures
static int GetNextPOT(int num);                     // Calculate next power-of-two value for a given value
static SpriteFont LoadRBMF(const char *fileName);   // Load a rBMF font file (raylib BitMap Font)

extern void LoadDefaultFont(void);
extern void UnloadDefaultFont(void);

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
extern void LoadDefaultFont(void)
{
    defaultFont.numChars = 96;              // We know our default font has 94 chars

    Image image;
    image.width = 128;                      // We know our default font image is 128 pixels width
    image.height = 64;                      // We know our default font image is 64 pixels height

    // Default font is directly defined here (data generated from a sprite font image)
    // This way, we reconstruct SpriteFont without creating large global variables
    // This data is automatically allocated to Stack and automatically deallocated at the end of this function
    int defaultFontData[256] = {
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00200020, 0x0001b000, 0x00000000, 0x00000000, 0x8ef92520, 0x00020a00, 0x7dbe8000, 0x1f7df45f,
        0x4a2bf2a0, 0x0852091e, 0x41224000, 0x10041450, 0x2e292020, 0x08220812, 0x41222000, 0x10041450, 0x10f92020, 0x3efa084c, 0x7d22103c, 0x107df7de,
        0xe8a12020, 0x08220832, 0x05220800, 0x10450410, 0xa4a3f000, 0x08520832, 0x05220400, 0x10450410, 0xe2f92020, 0x0002085e, 0x7d3e0281, 0x107df41f,
        0x00200000, 0x8001b000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xc0000fbe, 0xfbf7e00f, 0x5fbf7e7d, 0x0050bee8, 0x440808a2, 0x0a142fe8, 0x50810285, 0x0050a048,
        0x49e428a2, 0x0a142828, 0x40810284, 0x0048a048, 0x10020fbe, 0x09f7ebaf, 0xd89f3e84, 0x0047a04f, 0x09e48822, 0x0a142aa1, 0x50810284, 0x0048a048,
        0x04082822, 0x0a142fa0, 0x50810285, 0x0050a248, 0x00008fbe, 0xfbf42021, 0x5f817e7d, 0x07d09ce8, 0x00008000, 0x00000fe0, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x000c0180,
        0xdfbf4282, 0x0bfbf7ef, 0x42850505, 0x004804bf, 0x50a142c6, 0x08401428, 0x42852505, 0x00a808a0, 0x50a146aa, 0x08401428, 0x42852505, 0x00081090,
        0x5fa14a92, 0x0843f7e8, 0x7e792505, 0x00082088, 0x40a15282, 0x08420128, 0x40852489, 0x00084084, 0x40a16282, 0x0842022a, 0x40852451, 0x00088082,
        0xc0bf4282, 0xf843f42f, 0x7e85fc21, 0x3e0900bf, 0x00000000, 0x00000004, 0x00000000, 0x000c0180, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x04000402, 0x41482000, 0x00000000, 0x00000800,
        0x04000404, 0x4100203c, 0x00000000, 0x00000800, 0xf7df7df0, 0x514bef85, 0xbefbefbe, 0x04513bef, 0x14414500, 0x494a2885, 0xa28a28aa, 0x04510820,
        0xf44145f0, 0x474a289d, 0xa28a28aa, 0x04510be0, 0x14414510, 0x494a2884, 0xa28a28aa, 0x02910a00, 0xf7df7df0, 0xd14a2f85, 0xbefbe8aa, 0x011f7be0,
        0x00000000, 0x00400804, 0x20080000, 0x00000000, 0x00000000, 0x00600f84, 0x20080000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0xac000000, 0x00000f01, 0x00000000, 0x00000000, 0x24000000, 0x00000901, 0x00000000, 0x00000000, 0x24000000, 0x00000901, 0x00000000, 0x00000000,
        0x24fa28a2, 0x00000901, 0x00000000, 0x00000000, 0x2242252a, 0x00000952, 0x00000000, 0x00000000, 0x2422222a, 0x00000929, 0x00000000, 0x00000000,
        0x2412252a, 0x00000901, 0x00000000, 0x00000000, 0x24fbe8be, 0x00000901, 0x00000000, 0x00000000, 0xac020000, 0x00000f01, 0x00000000, 0x00000000,
        0x0003e000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000 };

    int charsHeight = 10;
    int charsDivisor = 1;    // Every char is separated from the consecutive by a 1 pixel divisor, horizontally and vertically

    int charsWidth[96] = { 3, 1, 4, 6, 5, 7, 6, 2, 3, 3, 5, 5, 2, 4, 1, 7, 5, 2, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 3, 4, 3, 6,
                           7, 6, 6, 6, 6, 6, 6, 6, 6, 3, 5, 6, 5, 7, 6, 6, 6, 6, 6, 6, 7, 6, 7, 7, 6, 6, 6, 2, 7, 2, 3, 5,
                           2, 5, 5, 5, 5, 5, 4, 5, 5, 1, 2, 5, 2, 5, 5, 5, 5, 5, 5, 5, 4, 5, 5, 5, 5, 5, 5, 3, 1, 3, 4, 4 };

    // Re-construct image from defaultFontData and generate OpenGL texture
    //----------------------------------------------------------------------
    image.pixels = (Color *)malloc(image.width * image.height * sizeof(Color));

    for (int i = 0; i < image.width * image.height; i++) image.pixels[i] = BLANK;        // Initialize array

    int counter = 0;        // Font data elements counter

    // Fill imgData with defaultFontData (convert from bit to pixel!)
    for (int i = 0; i < image.width * image.height; i += 32)
    {
        for (int j = 31; j >= 0; j--)
        {
            if (BIT_CHECK(defaultFontData[counter], j)) image.pixels[i+j] = WHITE;
        }

        counter++;

        if (counter > 256) counter = 0;         // Security check...
    }

    defaultFont.texture = CreateTexture(image, false); // Convert loaded image to OpenGL texture
    UnloadImage(image);

    // Reconstruct charSet using charsWidth[], charsHeight, charsDivisor, numChars
    //------------------------------------------------------------------------------
    defaultFont.charSet = (Character *)malloc(defaultFont.numChars * sizeof(Character));    // Allocate space for our character data
                                                                                            // This memory should be freed at end! --> Done on CloseWindow()
    int currentLine = 0;
    int currentPosX = charsDivisor;
    int testPosX = charsDivisor;

    for (int i = 0; i < defaultFont.numChars; i++)
    {
        defaultFont.charSet[i].value = FONT_FIRST_CHAR + i;  // First char is 32
        defaultFont.charSet[i].x = currentPosX;
        defaultFont.charSet[i].y = charsDivisor + currentLine * (charsHeight + charsDivisor);
        defaultFont.charSet[i].w = charsWidth[i];
        defaultFont.charSet[i].h = charsHeight;

        testPosX += (defaultFont.charSet[i].w + charsDivisor);

        if (testPosX >= defaultFont.texture.width)
        {
            currentLine++;
            currentPosX = 2 * charsDivisor + charsWidth[i];
            testPosX = currentPosX;

            defaultFont.charSet[i].x = charsDivisor;
            defaultFont.charSet[i].y = charsDivisor + currentLine * (charsHeight + charsDivisor);
        }
        else currentPosX = testPosX;
    }

    TraceLog(INFO, "Default font loaded successfully");
}

extern void UnloadDefaultFont(void)
{
    rlDeleteTextures(defaultFont.texture.id);
    free(defaultFont.charSet);
}

// Get the default font, useful to be used with extended parameters
SpriteFont GetDefaultFont()
{
    return defaultFont;
}

// Load a SpriteFont image into GPU memory
SpriteFont LoadSpriteFont(const char *fileName)
{
    SpriteFont spriteFont;

    // Check file extension
    if (strcmp(GetExtension(fileName),"rbmf") == 0) spriteFont = LoadRBMF(fileName);
    else
    {
        Image image = LoadImage(fileName);

        // At this point we have a pixel array with all the data...

        TraceLog(INFO, "[%s] SpriteFont image loaded: %i x %i", fileName, image.width, image.height);

        // Process bitmap Font pixel data to get measures (Character array)
        // spriteFont.charSet data is filled inside the function and memory is allocated!
        int numChars = ParseImageData(image.pixels, image.width, image.height, &spriteFont.charSet);

        TraceLog(INFO, "[%s] SpriteFont data parsed correctly", fileName);
        TraceLog(INFO, "[%s] SpriteFont num chars detected: %i", fileName, numChars);

        spriteFont.numChars = numChars;

        // Convert image font to POT image before conversion to texture
        // NOTE: Not required, we skip this step
/*
        // Just add the required amount of pixels at the right and bottom sides of image...
        int potWidth = GetNextPOT(image.width);
        int potHeight = GetNextPOT(image.height);

        // Check if POT texture generation is required (if texture is not already POT)
        if ((potWidth != image.width) || (potHeight != image.height))
        {
            Color *imgDataPixelPOT = NULL;

            // Generate POT array from NPOT data
            imgDataPixelPOT = (Color *)malloc(potWidth * potHeight * sizeof(Color));

            for (int j = 0; j < potHeight; j++)
            {
                for (int i = 0; i < potWidth; i++)
                {
                    if ((j < image.height) && (i < image.width)) imgDataPixelPOT[j*potWidth + i] = image.pixels[j*image.width + i];
                    else imgDataPixelPOT[j*potWidth + i] = MAGENTA;
                }
            }

            TraceLog(WARNING, "SpriteFont texture converted to POT: %ix%i", potWidth, potHeight);

            free(image.pixels);

            image.pixels = imgDataPixelPOT;
            image.width = potWidth;
            image.height = potHeight;
        }
*/
        spriteFont.texture = CreateTexture(image, false); // Convert loaded image to OpenGL texture
        UnloadImage(image);
    }

    return spriteFont;
}

// Unload SpriteFont from GPU memory
void UnloadSpriteFont(SpriteFont spriteFont)
{
    rlDeleteTextures(spriteFont.texture.id);
    free(spriteFont.charSet);
}

// Draw text (using default font)
// NOTE: fontSize work like in any drawing program but if fontSize is lower than font-base-size, then font-base-size is used
// NOTE: chars spacing is proportional to fontSize
void DrawText(const char *text, int posX, int posY, int fontSize, Color color)
{
    Vector2 position = { (float)posX, (float)posY };

    int defaultFontSize = 10;   // Default Font chars height in pixel

    if (fontSize < defaultFontSize) fontSize = defaultFontSize;

    int spacing = fontSize / defaultFontSize;

    DrawTextEx(defaultFont, text, position, fontSize, spacing, color);
}

// Draw text using SpriteFont
// NOTE: If font size is lower than base size, base size is used
// NOTE: chars spacing is NOT proportional to fontSize
void DrawTextEx(SpriteFont spriteFont, const char *text, Vector2 position, int fontSize, int spacing, Color tint)
{
    int length = strlen(text);
    int positionX = (int)position.x;
    float scaleFactor;

    Character c;

    if (fontSize <= spriteFont.charSet[0].h) scaleFactor = 1.0f;
    else scaleFactor = (float)fontSize / spriteFont.charSet[0].h;

    rlEnableTexture(spriteFont.texture.id);

    rlBegin(RL_QUADS);
        for(int i = 0; i < length; i++)
        {
            c = spriteFont.charSet[(int)text[i] - FONT_FIRST_CHAR];

            rlColor4ub(tint.r, tint.g, tint.b, tint.a);
            rlNormal3f(0.0f, 0.0f, 1.0f);                  // Normal Pointing Towards Viewer

            rlTexCoord2f((float)c.x / spriteFont.texture.width, (float)c.y / spriteFont.texture.height);
            rlVertex2f(positionX, position.y);

            rlTexCoord2f((float)c.x / spriteFont.texture.width, (float)(c.y + c.h) / spriteFont.texture.height);
            rlVertex2f(positionX, position.y + (c.h) * scaleFactor);

            rlTexCoord2f((float)(c.x + c.w) / spriteFont.texture.width, (float)(c.y + c.h) / spriteFont.texture.height);
            rlVertex2f(positionX + (c.w) * scaleFactor, position.y + (c.h) * scaleFactor);

            rlTexCoord2f((float)(c.x + c.w) / spriteFont.texture.width, (float)c.y / spriteFont.texture.height);
            rlVertex2f(positionX + (c.w) * scaleFactor, position.y);

            positionX += ((spriteFont.charSet[(int)text[i] - FONT_FIRST_CHAR].w) * scaleFactor + spacing);
        }
    rlEnd();

    rlDisableTexture();
}

// Formatting of text with variables to 'embed'
const char *FormatText(const char *text, ...)
{
    static char buffer[MAX_FORMATTEXT_LENGTH];

    va_list args;
    va_start(args, text);
    vsprintf(buffer, text, args);
    va_end(args);

    return buffer;
}

// Measure string width for default font
int MeasureText(const char *text, int fontSize)
{
    Vector2 vec;

    int defaultFontSize = 10;   // Default Font chars height in pixel
    if (fontSize < defaultFontSize) fontSize = defaultFontSize;
    int spacing = fontSize / defaultFontSize;

    vec = MeasureTextEx(defaultFont, text, fontSize, spacing);

    return (int)vec.x;
}

// Measure string size for SpriteFont
Vector2 MeasureTextEx(SpriteFont spriteFont, const char *text, int fontSize, int spacing)
{
    int len = strlen(text);
    int textWidth = 0;
    float scaleFactor;

    for (int i = 0; i < len; i++)
    {
        textWidth += spriteFont.charSet[(int)text[i] - FONT_FIRST_CHAR].w;
    }

    if (fontSize <= spriteFont.charSet[0].h) scaleFactor = 1.0f;
    else scaleFactor = (float)fontSize / spriteFont.charSet[0].h;

    Vector2 vec;
    vec.x = (float)textWidth * scaleFactor + (len - 1) * spacing; // Adds chars spacing to measure
    vec.y = (float)spriteFont.charSet[0].h * scaleFactor;

    return vec;
}

// Returns the base size for a SpriteFont (chars height)
int GetFontBaseSize(SpriteFont spriteFont)
{
    return spriteFont.charSet[0].h;
}

// Shows current FPS on top-left corner
// NOTE: Uses default font
void DrawFPS(int posX, int posY)
{
    char buffer[20];

    // NOTE: We are rendering fps every second for better viewing on high framerates
    // TODO: Not working properly on ANDROID and RPI

    static float fps = 0.0f;
    static int counter = 0;
    static int refreshRate = 20;

    if (counter < refreshRate)
    {
        counter++;
    }
    else
    {
        fps = GetFPS();
        refreshRate = fps;
        counter = 0;
    }

    sprintf(buffer, "%2.0f FPS", fps);
    DrawText(buffer, posX, posY, 20, LIME);
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------

// Check if a pixel is magenta
static bool PixelIsMagenta(Color p)
{
    return ((p.r == 255) && (p.g == 0) && (p.b == 255) && (p.a == 255));
}

// Parse image pixel data to obtain character set measures
static int ParseImageData(Color *imgDataPixel, int imgWidth, int imgHeight, Character **charSet)
{
    int charSpacing = 0;
    int lineSpacing = 0;

    int x = 0;
    int y = 0;

    Character tempCharSet[MAX_FONTCHARS];    // We allocate a temporal array for charData, once we get the actual charNumber we copy data to a sized array.

    for(y = 0; y < imgHeight; y++)
    {
        for(x = 0; x < imgWidth; x++)
        {
            if (!PixelIsMagenta(imgDataPixel[y*imgWidth + x])) break;
        }
        if (!PixelIsMagenta(imgDataPixel[y*imgWidth + x])) break;
    }

    charSpacing = x;
    lineSpacing = y;

    int charHeight = 0;
    int j = 0;

    while(!PixelIsMagenta(imgDataPixel[(lineSpacing + j)*imgWidth + charSpacing])) j++;

    charHeight = j;

    // Check array values to get characters: value, x, y, w, h
    int index = 0;
    int lineToRead = 0;
    int xPosToRead = charSpacing;

    while((lineSpacing + lineToRead * (charHeight + lineSpacing)) < imgHeight)
    {
        while((xPosToRead < imgWidth) &&
              !PixelIsMagenta((imgDataPixel[(lineSpacing + (charHeight+lineSpacing)*lineToRead)*imgWidth + xPosToRead])))
        {
            tempCharSet[index].value = FONT_FIRST_CHAR + index;
            tempCharSet[index].x = xPosToRead;
            tempCharSet[index].y = lineSpacing + lineToRead * (charHeight + lineSpacing);
            tempCharSet[index].h = charHeight;

            int charWidth = 0;

            while(!PixelIsMagenta(imgDataPixel[(lineSpacing + (charHeight+lineSpacing)*lineToRead)*imgWidth + xPosToRead + charWidth])) charWidth++;

            tempCharSet[index].w = charWidth;

            index++;

            xPosToRead += (charWidth + charSpacing);
        }

        lineToRead++;
        xPosToRead = charSpacing;
    }

    // We got tempCharSet populated with char data and the number of chars (index)
    // Now we move temp data to real charSet (passed as parameter to the function)
    (*charSet) = (Character *)malloc(index * sizeof(Character));        // BE CAREFUL! This memory should be freed!

    for (int i = 0; i < index; i++) (*charSet)[i] = tempCharSet[i];

    return index;
}

// Calculate next power-of-two value for a given num
static int GetNextPOT(int num)
{
    if (num != 0)
    {
        num--;
        num |= (num >> 1);     // Or first 2 bits
        num |= (num >> 2);     // Or next 2 bits
        num |= (num >> 4);     // Or next 4 bits
        num |= (num >> 8);     // Or next 8 bits
        num |= (num >> 16);    // Or next 16 bits
        num++;
    }

    return num;
}

// Load a rBMF font file (raylib BitMap Font)
static SpriteFont LoadRBMF(const char *fileName)
{
    // rBMF Info Header (16 bytes)
    typedef struct {
        char id[4];             // rBMF file identifier
        char version;           // rBMF file version
                                //      4 MSB --> main version
                                //      4 LSB --> subversion
        char firstChar;         // First character in the font
                                // NOTE: Depending on charDataType, it could be useless
        short imgWidth;         // Image width - always POT (power-of-two)
        short imgHeight;        // Image height - always POT (power-of-two)
        short numChars;         // Number of characters contained
        short charHeight;       // Characters height - the same for all characters
        char compType;          // Compression type:
                                //      4 MSB --> image data compression
                                //      4 LSB --> chars data compression
        char charsDataType;     // Char data type provided
    } rbmfInfoHeader;

    SpriteFont spriteFont;
    Image image;

    rbmfInfoHeader rbmfHeader;
    unsigned int *rbmfFileData;
    unsigned char *rbmfCharWidthData;

    int charsDivisor = 1;    // Every char is separated from the consecutive by a 1 pixel divisor, horizontally and vertically

    FILE *rbmfFile = fopen(fileName, "rb");        // Define a pointer to bitmap file and open it in read-binary mode

    fread(&rbmfHeader, sizeof(rbmfInfoHeader), 1, rbmfFile);

    TraceLog(INFO, "[%s] Loading rBMF file, size: %ix%i, numChars: %i, charHeight: %i", fileName, rbmfHeader.imgWidth, rbmfHeader.imgHeight, rbmfHeader.numChars, rbmfHeader.charHeight);

    spriteFont.numChars = (int)rbmfHeader.numChars;

    image.width = (int)rbmfHeader.imgWidth;
    image.height = (int)rbmfHeader.imgHeight;

    int numPixelBits = rbmfHeader.imgWidth * rbmfHeader.imgHeight / 32;

    rbmfFileData = (unsigned int *)malloc(numPixelBits * sizeof(unsigned int));

    for(int i = 0; i < numPixelBits; i++) fread(&rbmfFileData[i], sizeof(unsigned int), 1, rbmfFile);

    rbmfCharWidthData = (unsigned char *)malloc(spriteFont.numChars * sizeof(unsigned char));

    for(int i = 0; i < spriteFont.numChars; i++) fread(&rbmfCharWidthData[i], sizeof(unsigned char), 1, rbmfFile);

    // Re-construct image from rbmfFileData
    //-----------------------------------------
    image.pixels = (Color *)malloc(image.width * image.height * sizeof(Color));

    for (int i = 0; i < image.width * image.height; i++) image.pixels[i] = BLANK;        // Initialize array

    int counter = 0;        // Font data elements counter

    // Fill image data (convert from bit to pixel!)
    for (int i = 0; i < image.width * image.height; i += 32)
    {
        for (int j = 31; j >= 0; j--)
        {
            if (BIT_CHECK(rbmfFileData[counter], j)) image.pixels[i+j] = WHITE;
        }

        counter++;
    }

    TraceLog(INFO, "[%s] Image reconstructed correctly, now converting it to texture", fileName);

    spriteFont.texture = CreateTexture(image, false);
    UnloadImage(image);     // Unload image data

    TraceLog(INFO, "[%s] Starting charSet reconstruction", fileName);

    // Reconstruct charSet using rbmfCharWidthData, rbmfHeader.charHeight, charsDivisor, rbmfHeader.numChars
    spriteFont.charSet = (Character *)malloc(spriteFont.numChars * sizeof(Character));     // Allocate space for our character data

    int currentLine = 0;
    int currentPosX = charsDivisor;
    int testPosX = charsDivisor;

    for (int i = 0; i < spriteFont.numChars; i++)
    {
        spriteFont.charSet[i].value = (int)rbmfHeader.firstChar + i;
        spriteFont.charSet[i].x = currentPosX;
        spriteFont.charSet[i].y = charsDivisor + currentLine * ((int)rbmfHeader.charHeight + charsDivisor);
        spriteFont.charSet[i].w = (int)rbmfCharWidthData[i];
        spriteFont.charSet[i].h = (int)rbmfHeader.charHeight;

        testPosX += (spriteFont.charSet[i].w + charsDivisor);

        if (testPosX > spriteFont.texture.width)
        {
            currentLine++;
            currentPosX = 2 * charsDivisor + (int)rbmfCharWidthData[i];
            testPosX = currentPosX;

            spriteFont.charSet[i].x = charsDivisor;
            spriteFont.charSet[i].y = charsDivisor + currentLine * (rbmfHeader.charHeight + charsDivisor);
        }
        else currentPosX = testPosX;
    }

    TraceLog(INFO, "[%s] rBMF file loaded correctly as SpriteFont", fileName);

    fclose(rbmfFile);

    free(rbmfFileData);                // Now we can free loaded data from RAM memory
    free(rbmfCharWidthData);

    return spriteFont;
}

// Generate a sprite font from TTF data (font size required)
static SpriteFont GenerateFromTTF(const char *fileName, int fontSize)
{
    SpriteFont font;

    // TODO: Load TTF and generate bitmap font and chars data

    return font;
}