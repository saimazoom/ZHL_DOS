// Z88dk fastcall/calle/nominal in https://www.z88dk.org/wiki/doku.php?id=optimization

#ifdef C64 
	#define __FASTCALL__ __fastcall__
	#define __CALLEE__ 
#endif 

#ifdef CPC
	#define __FASTCALL__ 
	#define __CALLEE__ 
#endif 

#ifdef DOS
	#define __FASTCALL__ 
	#define __CALLEE__ 
#endif 


#define BYTE unsigned char
#define WORD unsigned int

#ifdef ZX
    #define INK_BLACK      0x00
    #define INK_BLUE       0x01
    #define INK_RED        0x02
    #define INK_MAGENTA    0x03
    #define INK_GREEN      0x04
    #define INK_CYAN       0x05
    #define INK_YELLOW     0x06
    #define INK_WHITE      0x07

    #define PAPER_BLACK    0x00
    #define PAPER_BLUE     0x08
    #define PAPER_RED      0x10
    #define PAPER_MAGENTA  0x18
    #define PAPER_GREEN    0x20
    #define PAPER_CYAN     0x28
    #define PAPER_YELLOW   0x30
    #define PAPER_WHITE    0x38

    #define BRIGHT         0x40
    #define FLASH          0x80
#endif

#ifdef C64
   // Disable the PESCII translation performed by CC65
   #pragma charmap (0,0);
   #pragma charmap (1,1);
   #pragma charmap (2,2);
   #pragma charmap (3,3);
   #pragma charmap (4,4);
   #pragma charmap (5,5);
   #pragma charmap (6,6);
   #pragma charmap (7,7);
   #pragma charmap (8,8);
   #pragma charmap (9,9);
   #pragma charmap (10,10);
   #pragma charmap (11,11);
   #pragma charmap (12,12);
   #pragma charmap (13,13);
   #pragma charmap (14,14);
   #pragma charmap (15,15);
   #pragma charmap (16,16);
   #pragma charmap (17,17);
   #pragma charmap (18,18);
   #pragma charmap (19,19);
   #pragma charmap (20,20);
   #pragma charmap (21,21);
   #pragma charmap (22,22);
   #pragma charmap (23,23);
   #pragma charmap (24,24);
   #pragma charmap (25,25);
   #pragma charmap (26,26);
   #pragma charmap (27,27);
   #pragma charmap (28,28);
   #pragma charmap (29,29);
   #pragma charmap (30,30);
   #pragma charmap (31,31);
   #pragma charmap (32,32);
   #pragma charmap (33,33);
   #pragma charmap (34,34);
   #pragma charmap (35,35);
   #pragma charmap (36,36);
   #pragma charmap (37,37);
   #pragma charmap (38,38);
   #pragma charmap (39,39);
   #pragma charmap (40,40);
   #pragma charmap (41,41);
   #pragma charmap (42,42);
   #pragma charmap (43,43);
   #pragma charmap (44,44);
   #pragma charmap (45,45);
   #pragma charmap (46,46);
   #pragma charmap (47,47);
   #pragma charmap (48,48);
   #pragma charmap (49,49);
   #pragma charmap (50,50);
   #pragma charmap (51,51);
   #pragma charmap (52,52);
   #pragma charmap (53,53);
   #pragma charmap (54,54);
   #pragma charmap (55,55);
   #pragma charmap (56,56);
   #pragma charmap (57,57);
   #pragma charmap (58,58);
   #pragma charmap (59,59);
   #pragma charmap (60,60);
   #pragma charmap (61,61);
   #pragma charmap (62,62);
   #pragma charmap (63,63);
   #pragma charmap (64,64);
   #pragma charmap (65,65);
   #pragma charmap (66,66);
   #pragma charmap (67,67);
   #pragma charmap (68,68);
   #pragma charmap (69,69);
   #pragma charmap (70,70);
   #pragma charmap (71,71);
   #pragma charmap (72,72);
   #pragma charmap (73,73);
   #pragma charmap (74,74);
   #pragma charmap (75,75);
   #pragma charmap (76,76);
   #pragma charmap (77,77);
   #pragma charmap (78,78);
   #pragma charmap (79,79);
   #pragma charmap (80,80);
   #pragma charmap (81,81);
   #pragma charmap (82,82);
   #pragma charmap (83,83);
   #pragma charmap (84,84);
   #pragma charmap (85,85);
   #pragma charmap (86,86);
   #pragma charmap (87,87);
   #pragma charmap (88,88);
   #pragma charmap (89,89);
   #pragma charmap (90,90);
   #pragma charmap (91,91);
   #pragma charmap (92,92);
   #pragma charmap (93,93);
   #pragma charmap (94,94);
   #pragma charmap (95,95);
   #pragma charmap (96,96);
   #pragma charmap (97,97);
   #pragma charmap (98,98);
   #pragma charmap (99,99);
   #pragma charmap (100,100);
   #pragma charmap (101,101);
   #pragma charmap (102,102);
   #pragma charmap (103,103);
   #pragma charmap (104,104);
   #pragma charmap (105,105);
   #pragma charmap (106,106);
   #pragma charmap (107,107);
   #pragma charmap (108,108);
   #pragma charmap (109,109);
   #pragma charmap (110,110);
   #pragma charmap (111,111);
   #pragma charmap (112,112);
   #pragma charmap (113,113);
   #pragma charmap (114,114);
   #pragma charmap (115,115);
   #pragma charmap (116,116);
   #pragma charmap (117,117);
   #pragma charmap (118,118);
   #pragma charmap (119,119);
   #pragma charmap (120,120);
   #pragma charmap (121,121);
   #pragma charmap (122,122);
   #pragma charmap (123,123);
   #pragma charmap (124,124);
   #pragma charmap (125,125);
   #pragma charmap (126,126);
   #pragma charmap (127,127);
   #pragma charmap (128,128);
   #pragma charmap (129,129);
   #pragma charmap (130,130);
   #pragma charmap (131,131);
   #pragma charmap (132,132);
   #pragma charmap (133,133);
   #pragma charmap (134,134);
   #pragma charmap (135,135);
   #pragma charmap (136,136);
   #pragma charmap (137,137);
   #pragma charmap (138,138);
   #pragma charmap (139,139);
   #pragma charmap (140,140);
   #pragma charmap (141,141);
   #pragma charmap (142,142);
   #pragma charmap (143,143);
   #pragma charmap (144,144);
   #pragma charmap (145,145);
   #pragma charmap (146,146);
   #pragma charmap (147,147);
   #pragma charmap (148,148);
   #pragma charmap (149,149);
   #pragma charmap (150,150);
   #pragma charmap (151,151);
   #pragma charmap (152,152);
   #pragma charmap (153,153);
   #pragma charmap (154,154);
   #pragma charmap (155,155);
   #pragma charmap (156,156);
   #pragma charmap (157,157);
   #pragma charmap (158,158);
   #pragma charmap (159,159);
   #pragma charmap (160,160);
   #pragma charmap (161,161);
   #pragma charmap (162,162);
   #pragma charmap (163,163);
   #pragma charmap (164,164);
   #pragma charmap (165,165);
   #pragma charmap (166,166);
   #pragma charmap (167,167);
   #pragma charmap (168,168);
   #pragma charmap (169,169);
   #pragma charmap (170,170);
   #pragma charmap (171,171);
   #pragma charmap (172,172);
   #pragma charmap (173,173);
   #pragma charmap (174,174);
   #pragma charmap (175,175);
   #pragma charmap (176,176);
   #pragma charmap (177,177);
   #pragma charmap (178,178);
   #pragma charmap (179,179);
   #pragma charmap (180,180);
   #pragma charmap (181,181);
   #pragma charmap (182,182);
   #pragma charmap (183,183);
   #pragma charmap (184,184);
   #pragma charmap (185,185);
   #pragma charmap (186,186);
   #pragma charmap (187,187);
   #pragma charmap (188,188);
   #pragma charmap (189,189);
   #pragma charmap (190,190);
   #pragma charmap (191,191);
   #pragma charmap (192,192);
   #pragma charmap (193,193);
   #pragma charmap (194,194);
   #pragma charmap (195,195);
   #pragma charmap (196,196);
   #pragma charmap (197,197);
   #pragma charmap (198,198);
   #pragma charmap (199,199);
   #pragma charmap (200,200);
   #pragma charmap (201,201);
   #pragma charmap (202,202);
   #pragma charmap (203,203);
   #pragma charmap (204,204);
   #pragma charmap (205,205);
   #pragma charmap (206,206);
   #pragma charmap (207,207);
   #pragma charmap (208,208);
   #pragma charmap (209,209);
   #pragma charmap (210,210);
   #pragma charmap (211,211);
   #pragma charmap (212,212);
   #pragma charmap (213,213);
   #pragma charmap (214,214);
   #pragma charmap (215,215);
   #pragma charmap (216,216);
   #pragma charmap (217,217);
   #pragma charmap (218,218);
   #pragma charmap (219,219);
   #pragma charmap (220,220);
   #pragma charmap (221,221);
   #pragma charmap (222,222);
   #pragma charmap (223,223);
   #pragma charmap (224,224);
   #pragma charmap (225,225);
   #pragma charmap (226,226);
   #pragma charmap (227,227);
   #pragma charmap (228,228);
   #pragma charmap (229,229);
   #pragma charmap (230,230);
   #pragma charmap (231,231);
   #pragma charmap (232,232);
   #pragma charmap (233,233);
   #pragma charmap (234,234);
   #pragma charmap (235,235);
   #pragma charmap (236,236);
   #pragma charmap (237,237);
   #pragma charmap (238,238);
   #pragma charmap (239,239);
   #pragma charmap (240,240);
   #pragma charmap (241,241);
   #pragma charmap (242,242);
   #pragma charmap (243,243);
   #pragma charmap (244,244);
   #pragma charmap (245,245);
   #pragma charmap (246,246);
   #pragma charmap (247,247);
   #pragma charmap (248,248);
   #pragma charmap (249,249);
   #pragma charmap (250,250);
   #pragma charmap (251,251);
   #pragma charmap (252,252);
   #pragma charmap (253,253);
   #pragma charmap (254,254);
   #pragma charmap (255,255);

// In C64 ink is in the HIGH nibble in the colormem, PAPER at the low nibble

    #define INK_BLACK      0x00
    #define INK_WHITE      0x10
    #define INK_RED        0x20
    #define INK_CYAN       0x30
    #define INK_MAGENTA    0x40
    #define INK_GREEN      0x50
    #define INK_BLUE       0x60
    #define INK_YELLOW     0x70
    #define INK_LIGHT_BROWN     0x80
    #define INK_BROWN      0x90
    #define INK_LIGHT_RED  0xA0
    #define INK_DARK_GREY  0xB0
    #define INK_GREY       0xC0
    #define INK_LIGHT_GREEN 0xD0
    #define INK_LIGHT_BLUE 0xE0
    #define INK_LIGHT_GREY 0xF0

    #define PAPER_BLACK      0x00
    #define PAPER_WHITE      0x01
    #define PAPER_RED        0x02
    #define PAPER_CYAN       0x03
    #define PAPER_MAGENTA    0x04
    #define PAPER_GREEN      0x05
    #define PAPER_BLUE       0x06
    #define PAPER_YELLOW     0x07
    #define PAPER_LIGHT_BROWN 0x08
    #define PAPER_BROWN      0x09
    #define PAPER_LIGHT_RED  0x0A
    #define PAPER_DARK_GREY  0x0B
    #define PAPER_GREY       0x0C
    #define PAPER_LIGHT_GREEN 0x0D
    #define PAPER_LIGHT_BLUE 0x0E
    #define PAPER_LIGHT_GREY 0x0F

// BRIGHT and FLASH are not supported in C64
    #define BRIGHT         0x00
    #define FLASH          0x00
#endif 


// The function being called (callee) is responsible for cleaning up the stack after finishing. 

#ifdef C64
   extern void setRamLayout ();
   extern void __FASTCALL__ splitScreen (BYTE scanline);
   extern void __FASTCALL__ HighResMode(); // 320x200 2 colors per cell
   extern void __FASTCALL__ clearVideoColorMem (BYTE color); 
   extern void __FASTCALL__ clearVideoRam (BYTE color); 
   extern void __FASTCALL__ clearVideoBitmap (BYTE color); // Bitmap memrory
#endif

extern void __CALLEE__ scrollArriba (BYTE fila_inicial, BYTE columna_inicial);
extern void __CALLEE__scrollArriba2 (BYTE linea_inicial, BYTE num, BYTE step);
extern void __CALLEE__drawRectangle (BYTE xorig, BYTE yorig, BYTE width, BYTE height);
extern void  clsScreen (BYTE effect); // Distintos efectos para borrar la pantalla...
extern void __CALLEE__ waitForAnyKey();

extern void __CALLEE__clearchar (BYTE x, BYTE y, BYTE color);

extern void __CALLEE__ __FASTCALL__ clearScreen (BYTE color);
extern void __CALLEE__ waitForNoKey();
extern void setRAMPage (BYTE banco);
extern void setRAMBack();

extern void drawSprite (BYTE *pointer, BYTE xorig, BYTE yorig, BYTE width, BYTE height);
extern WORD __CALLEE__ randomNumber(); 
extern unsigned char getKey();

extern void __CALLEE__ putPixel (BYTE x, BYTE y);
extern void __CALLEE__ drawLine (BYTE x0, BYTE y0, BYTE x1, BYTE y1);
extern void __CALLEE__ eraseLine (BYTE x0, BYTE y0, BYTE x1, BYTE y1);
extern void __FASTCALL__ paint_pic (unsigned char *bytestring);
extern void __CALLEE__ fill (BYTE x, BYTE y); 
extern void __CALLEE__ pfill (BYTE x, BYTE y, BYTE pattern); 

///////////////////////////////////////////////////////////
//                  DATA STRUCTURES                      //
///////////////////////////////////////////////////////////

struct fzx_state
{
   unsigned char            x;         // current x coordinate in pixels
   unsigned char            y;         // current y coordinate in pixels

};

///////////////////////////////////////////////////////////
//                      DRAW TEXT                        //
///////////////////////////////////////////////////////////
// Functions in printlib.asm
extern void fzx_setat(unsigned char x, unsigned char y);
extern void fzx_putc(unsigned char c);

extern void fzx_puts(char *s);
extern void fzx_write(unsigned int *buf, unsigned int len);

extern void __CALLEE__ print_string (BYTE x, BYTE y, unsigned char *texto);
extern void __CALLEE__ print_char (BYTE x, BYTE y, unsigned char texto);
extern void __CALLEE__ setAttr (BYTE x, BYTE Y, BYTE attr);


