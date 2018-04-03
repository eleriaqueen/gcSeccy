#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <wiikeyboard/keyboard.h>
#include <unistd.h>

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

void *Initialise();

bool quitapp=false;

#define PSO_NAME_SIZE_MAX 12
#define PSO_LEGACY_VALUE 5

char* pso_sectionid[] = { "Pinkal", "Redria", "Oran", "Yellowboze", "Whitill", "Viridia", "Greenill", "Skyly", "Bluefull", "Purplenum" };

void countdwn(unsigned int count);
unsigned int is_str_ascii(char* str);
unsigned int pso_strcpt(char* input_str, unsigned int cval);

char b[13] = {0};
unsigned int c = 0;

void keyPress_cb( char sym) {


	if ((sym == KS_Return) && (c > 0))
	{
		printf(" = ");
		
		secId = pso_sectionid[pso_strcpt(b, PSO_LEGACY_VALUE)];
			
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
			 
			printf("%s\n", secId);
			printf("\x1b[39m"); // Restore foreground color
		
		c = 0;
		memset(b, 0, sizeof(b));
	}
	else if ((sym >= KS_space) && (sym <= KS_asciitilde) && (c < PSO_NAME_SIZE_MAX) )
	{
		b[c] = sym;
		putchar(sym);
		c ++;
	}
	else if ( sym == KS_BackSpace)
	{
		if (c > 0) 
		{
			printf("\b \b");
			c --;
			b[c] = '\0';
		}
	}
	else if ( sym == KS_Escape) quitapp = true;

}

int main(int argc, char **argv) {

	xfb = Initialise();
	
	if (KEYBOARD_Init(keyPress_cb) == 0) ;
	else 
	{
		printf("Keyboard initialisation failed\n");
		printf("Quitting in:\n");
		countdwn(3);
		return 1;
	}
	
	printf("---------------------------\n");
	printf("- Section ID Tool for Wii -\n");
	printf("---------------------------\n\n");

	while(1) {

		PAD_ScanPads();

		u32 buttonsDown = PAD_ButtonsDown(0);

		getchar();
		
		if ((buttonsDown & WPAD_BUTTON_HOME) | quitapp)
		{
			printf("Quitting in:\n");
			countdwn(3);
			exit(0);
		}
		
		VIDEO_WaitVSync();
	}

	return 0;
}

void * Initialise() {

	void *framebuffer;

	VIDEO_Init();
	PAD_Init();

	// Let libOGC figure out the video mode
	rmode = VIDEO_GetPreferredMode(NULL); //Last mode used

	framebuffer = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	console_init(framebuffer,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(framebuffer);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	return framebuffer;
}

void countdwn(unsigned int count)
{	
	for (unsigned int i = (count); i > 0; i--)
	{
		printf("%u...\n", i);
		sleep(1);
	}
}

unsigned int pso_strcpt(char* input_str, unsigned int cval)
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
