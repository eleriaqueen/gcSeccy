#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>

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

extern void __SYS_ReadROM(void *buf,u32 len,u32 offset);

int main(int argc, char **argv) {
	char letter = 'a';
	unsigned int c = 0;
	int flicker = 0;
	
	xfb = Initialise();
	
	printf("---------------------------\n");
	printf("- Section ID Tool for GC  -\n");
	printf("---------------------------\n\n");
	
	while(1) {
		if (flicker >= 0)
		{
			printf("\b%c", letter);
			flicker ++;
			
			
		}
		else if (flicker < 0)
		{
			printf("\b_");
			flicker--;
		}
		if (flicker >= 30) flicker = -1;
		if (flicker < -20) flicker = 0;
		
		PAD_ScanPads();
		u32 buttonsDown = PAD_ButtonsDown(0);
		if ((buttonsDown & PAD_BUTTON_A) && (c < 12) && (flicker >= 0))
		{
			printf("%c", letter);
			b[c] = letter;
			c++;
		}
		else if ((buttonsDown & PAD_BUTTON_A) && (c < 12) && (flicker < 0))
		{
			printf("\b%c_", letter);
			b[c] = letter;
			c++;
		}
		
		if ((buttonsDown & PAD_BUTTON_START) && (c > 0))
		{
			printf("\b = %s\n", pso_sectionid[pso_strcpt(b, PSO_LEGACY_VALUE)]);
			c = 0;
		}
		
		if ((buttonsDown & PAD_BUTTON_UP) && (letter < 126))
		{
			letter++;
			printf("\b%c", letter);
			flicker = 0;
		}
		if ((buttonsDown & PAD_BUTTON_DOWN) && (letter > 32))
		{
			letter--;
			printf("\b%c", letter);
			flicker = 0;
		}
		
		VIDEO_WaitVSync();
	}

	return 0;
}

void * Initialise() {

	void *framebuffer;

	VIDEO_Init();
	PAD_Init();
	
	static char IPLInfo [ 256 ];
	__SYS_ReadROM(IPLInfo,256,0);
	
	PAD_ScanPads();
	// L Trigger held down ignores the fact that there's a component cable plugged in.
	if(VIDEO_HaveComponentCable() && !(PAD_ButtonsDown(0) & PAD_TRIGGER_L)) {
		if((strstr(IPLInfo,"PAL")!=NULL)) 
		{
			rmode = &TVEurgb60Hz480Prog; //Progressive 480p
		}
		else
		{
			rmode = &TVNtsc480Prog; //Progressive 480p
		}
	}
	else {
		//try to use the IPL region
		if(strstr(IPLInfo,"PAL")!=NULL) 
		{
				rmode = &TVPal576IntDfScale;         //PAL
		}
		else if(strstr(IPLInfo,"NTSC")!=NULL) 
		{
				rmode = &TVNtsc480IntDf;        //NTSC
		}
		else
		{
				rmode = VIDEO_GetPreferredMode(NULL); //Last mode used
		}
	}

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