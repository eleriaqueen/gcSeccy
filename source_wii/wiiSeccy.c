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
unsigned int count = 0;

void keyPress_cb( char sym) {


	if ((sym == KS_Return) && (count > 0))
	{
		printf(" = %s\n", pso_sectionid[pso_strcpt(b, PSO_LEGACY_VALUE)]);
		count = 0;
		memset(b, 0, sizeof(b));
	}
	else if ((sym >= KS_space) && (sym <= KS_asciitilde) && (count < PSO_NAME_SIZE_MAX) )
	{
		b[count] = sym;
		putchar(sym);
		count ++;
	}
	else if ( sym == KS_BackSpace)
	{
		if (count > 0) 
		{
			printf("\b \b");
			count --;
			b[count] = '\0';
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