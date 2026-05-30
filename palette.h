#pragma once

#define rgb565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))

#define COLOR_BLACK 0x0000           ///<   0,   0,   0 #000000
#define COLOR_NAVY 0x000F            ///<   0,   0, 123 #00007b
#define COLOR_DARKGREEN 0x03E0       ///<   0, 125,   0 #007d00
#define COLOR_DARKCYAN 0x03EF        ///<   0, 125, 123 #007d7b
#define COLOR_MAROON 0x7800          ///< 123,   0,   0 #7b0000
#define COLOR_PURPLE 0x780F          ///< 123,   0, 123
#define COLOR_OLIVE 0x7BE0           ///< 123, 125,   0
#define COLOR_LIGHTGREY 0xC618       ///< 198, 195, 198
#define COLOR_DARKGREY 0x7BEF        ///< 123, 125, 123 #7b7d7b
#define COLOR_BLUE 0x001F            ///<   0,   0, 255
#define COLOR_GREEN 0x07E0           ///<   0, 255,   0
#define COLOR_CYAN 0x07FF            ///<   0, 255, 255 #00ffff
#define COLOR_RED 0xF800             ///< 255,   0,   0
#define COLOR_MAGENTA 0xF81F         ///< 255,   0, 255 #ff00ff
#define COLOR_YELLOW 0xFFE0          ///< 255, 255,   0 #ffff00
#define COLOR_WHITE 0xFFFF           ///< 255, 255, 255 #ffffff
#define COLOR_ORANGE 0xFD20          ///< 255, 165,   0
#define COLOR_GREENYELLOW 0xAFE5     ///< 173, 255,  41
#define COLOR_PINK 0xFC18            ///< 255, 130, 198
#define COLOR_BROWN 0x9A60           ///< 150, 75,    0
#define COLOR_GOLD 0xFEA0            ///< 255, 215,   0 #ffd700
#define COLOR_SILVER 0xC618          ///< 192, 192, 192 #c0c0c0
#define COLOR_SKYBLUE 0x867D         ///< 135, 206, 235 #87ceeb
#define COLOR_VIOLET 0xEC1D          ///< 238, 130, 238 #ee82ee
#define COLOR_PEARL 0xEF18           ///< 237, 228, 199 #ede4c7
#define COLOR_WATERMELON_RED 0xB9C9  ///< 190, 55, 70   #be3746
#define COLOR_PEWTER_BLUE 0x7DB6     ///< 127, 183, 180 #7fb7b4
#define COLOR_PURPLE_TAUPE 0x49EA    ///< 73, 62, 78    #493e4e
#define COLOR_FOREST_GREEN 0x6D2D    ///< 109, 167, 108 #6da76c
#define COLOR_INDIGO 0x2A0D          ///< 38, 63, 103   #263F67
#define COLOR_SUNRAY 0xDDCB          ///< 221, 187, 89  #DDBB59
#define COLOR_LIGHT_TAUPE 0xB46D     ///< 180, 141, 108 #b48d6c
#define COLOR_FELDGRAU 0x43CB        ///< 67, 88, 89    #435859
#define COLOR_CEDAR_CHEST 0xBB09     ///< 193, 98, 78   #C1624E
#define COLOR_DARK_CHARCOAL 0x31A6   ///< 51, 51, 51    #333333
#define COLOR_SONIC_SILVER 0x73AE    ///< 119, 119, 119 #777777
#define COLOR_ASPARAGUS 0x8D4C       ///< 143, 171, 97  #8FAB61
#define COLOR_SEA_SERPENT 0x3DF9     ///< 59, 192, 211  #3BC0D3
#define COLOR_GRAY 0xBDD7            ///< 187, 187, 187 #BBBBBB