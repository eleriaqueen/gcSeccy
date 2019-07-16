#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>

#ifdef __wii__
#include <wiiuse/wpad.h>
#endif

#include <unistd.h>

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

void *Initialise();

bool quitapp = false;

#define PSO_NAME_SIZE_MAX 12
#define PSO_LEGACY_VALUE 5

char *pso_sectionid[] = {"Pinkal", "Redria", "Oran", "Yellowboze", "Whitill", "Viridia", "Greenill", "Skyly", "Bluefull", "Purplenum"};

void countdown(unsigned int count);
unsigned int is_str_ascii(char *str);
unsigned int pso_strcpt(char *input_str, unsigned int cval);
void printheader();

extern void __SYS_ReadROM(void *buf, u32 len, u32 offset);

int main(int argc, char **argv)
{
	char buf[13];
	memset(buf, 0, sizeof(buf));
	char letter = 'A';
	unsigned int c = 0;
	char *secId;

	char *down = "\x1b[1B";
	char *up = "\x1b[1A";
	char *left = "\b";

#define LUPL() printf("%s%s%c%s", left, up, letter + 1, left) // Left, Up, PrintChar, Left
#define DDPU() printf("%s%s%c%s", down, down, letter - 1, up) // Down, Down, PrintChar, Up

#define LUSL() printf("%s%s %s", left, up, left) // Left, Up, PrintSpace, Left
#define DDSU() printf("%s%s %s", down, down, up) // Down, Down, PrintSpace, Up

#define CON_CLR() printf("\x1b[2J") // Console_Clear

	xfb = Initialise();

#ifdef __wii__
	// This function initialises attached controllers
	WPAD_Init();
#endif

	printheader();

	printf("\n              > %c", letter);

	LUPL();
	DDPU();

	while (1)
	{

		PAD_ScanPads();
		u32 buttonsDown = PAD_ButtonsDown(0);

		if ((buttonsDown & PAD_BUTTON_A) && (c < 12))
		{

			LUSL();
			DDSU();

			printf("%c", letter);

			if (letter < 126)
				LUPL();
			else
				LUSL();

			if (letter > 33)
				DDPU();
			else
				DDSU();
			buf[c] = letter;
			c++;
		}

		if ((buttonsDown & PAD_BUTTON_B) && (c > 0))
		{
			printf("%s%s %s%s", left, up, left, down);
			printf("%s %s%s", down, left, up);
			printf(" %s", left);

			if (letter < 126)
				LUPL();
			else
				LUSL();

			if (letter > 33)
				DDPU();
			else
				DDSU();

			printf("%s", left);
			printf("%c", letter);

			buf[c - 1] = '\0';
			c--;
		}

		if (((buttonsDown & PAD_BUTTON_Y) || (buttonsDown & PAD_BUTTON_X)) && (c > 0))
		{
			LUSL();
			DDSU();

			printf("\b = ");

			secId = pso_sectionid[pso_strcpt(buf, PSO_LEGACY_VALUE)];

			/*
			if (strstr(secId, "Pinkal"))
				printf("\x1b[35;1m");
			else if (strstr(secId, "Redria"))
				printf("\x1b[31;1m");
			else if (strstr(secId, "Oran"))
				printf("\x1b[33m");
			else if (strstr(secId, "Yellowboze"))
				printf("\x1b[33;1m");
			else if (strstr(secId, "Whitill"))
				printf("\x1b[37;1m");
			else if (strstr(secId, "Viridia"))
				printf("\x1b[32m");
			else if (strstr(secId, "Greenill"))
				printf("\x1b[32;1m");
			else if (strstr(secId, "Skyly"))
				printf("\x1b[36;1m");
			else if (strstr(secId, "Bluefull"))
				printf("\x1b[34m");
			else if (strstr(secId, "Purplenum"))
				printf("\x1b[35m");
			*/

			printf("%s\n\n\n\n%s%s", secId, left, up); // secId, 4 Newlines, then Left Up
			// printf("\x1b[39m"); // Restore foreground color
			printf("              > %c", letter);

			if (letter < 126)
				LUPL();
			else
				LUSL();

			if (letter > 33)
				DDPU();
			else
				DDSU();

			c = 0;
			memset(buf, 0, sizeof(buf));
		}

		if ((buttonsDown & PAD_BUTTON_UP) && (letter < 126))
		{
			letter++;
			printf("\b%c", letter);

			if (letter < 126)
				LUPL();
			else
				LUSL();

			DDPU();
		}
		if ((buttonsDown & PAD_BUTTON_DOWN) && (letter > 32))
		{
			letter--;
			printf("\b%c", letter);

			LUPL();

			if (letter > 33)
				DDPU();
			else
				DDSU();
		}

		if (buttonsDown & PAD_BUTTON_START)
		{
			CON_CLR();

			printheader();

			c = 0;
			memset(buf, 0, sizeof(buf));
			printf("\n              > %c", letter);
		}
		VIDEO_WaitVSync();

#ifdef __wii__
		if ((buttonsDown & WPAD_BUTTON_HOME) || (buttonsDown & PAD_TRIGGER_Z) || quitapp)
		{
			CON_CLR();
			printf("Shutdown in... 3...\n");
			sleep(1);
			countdown(2);
			exit(0);
		}
#else
		if ((buttonsDown & PAD_TRIGGER_Z))
		{
			CON_CLR();
			printf("Shutdown in... 3...\n");
			sleep(1);
			countdown(2);
			exit(0);
		}
#endif
	}

	return 0;
}

void *Initialise()
{

	void *framebuffer;

	VIDEO_Init();
	PAD_Init();

	PAD_ScanPads();

	rmode = VIDEO_GetPreferredMode(NULL);

	framebuffer = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	console_init(framebuffer, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);

	VIDEO_Configure(rmode);

	/*** Set the framebuffer to be displayed at next VBlank ***/
	VIDEO_SetNextFramebuffer(framebuffer);

	VIDEO_SetBlack(FALSE);

	/*** Update the video for next vblank ***/
	VIDEO_Flush();

	VIDEO_WaitVSync(); /*** Wait for VBL ***/
	if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();

	return framebuffer;
}

void printheader()
{
	// 80 characters
	// printf("123456789 123456789 123456789 123456789 123456789 123456789 123456789  123456789 \n");
	printf("              ---------------------------------------------------                \n");
	printf("              -              Section ID Tool for GC             -                \n");
	printf("              ---------------------------------------------------                \n");
	printf("                                                                                 \n");
	printf("              [A] memorises a letter/character.                                  \n");
	printf("              [B] deletes last input letter/character.                           \n");
	printf("              [X] and [Y] print your Section ID                                  \n");
	printf("              [Start] clears the screen.                                         \n");
	printf("              [Z] Exits the app.                                                 \n\n");
	
}

void countdown(unsigned int count)
{
	for (unsigned int i = (count); i > 0; i--)
	{
		printf("               %u...\n", i);
		sleep(1);
	}
}

unsigned int pso_strcpt(char *input_str, unsigned int cval)
{
	unsigned int sum = 0;
	char *c = input_str;
	while (*c)
	{
		sum += (*c);
		c++;
	}
	return ((sum + cval) % 10);
}
