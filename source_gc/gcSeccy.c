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
void printheader();

extern void __SYS_ReadROM(void *buf,u32 len,u32 offset);

int main(int argc, char **argv) {
	char buf[13];
	memset(buf, 0, sizeof(buf));
	char letter = 'A';
	unsigned int c = 0;
	char *secId;
	
	xfb = Initialise();
	
	printheader();

	printf("%c", letter);
	while(1) {
		
		PAD_ScanPads();
		u32 buttonsDown = PAD_ButtonsDown(0);
		
		if ((buttonsDown & PAD_BUTTON_A) && (c < 12))
		{
			printf("%c", letter);
			buf[c] = letter;
			c++;
		}
		
		if (((buttonsDown & PAD_BUTTON_Y) || (buttonsDown & PAD_BUTTON_X)) && (c > 0))
		{
			printf("\b = ");
			
			secId = pso_sectionid[pso_strcpt(buf, PSO_LEGACY_VALUE)];
			
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
			printf("%c", letter);
			c = 0;
			memset(buf, 0, sizeof(buf));
		}
		
		if ((buttonsDown & PAD_BUTTON_UP) && (letter < 126))
		{
			letter++;
			printf("\b%c", letter);
		}
		if ((buttonsDown & PAD_BUTTON_DOWN) && (letter > 32))
		{
			letter--;
			printf("\b%c", letter);
		}
		
		if (buttonsDown & PAD_BUTTON_START)
		{
			iprintf("\x1b[2J");
			printheader();
			c = 0;
			memset(buf, 0, sizeof(buf));
			printf("%c", letter);
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

void printheader()
{
	printf("--------------------------------\n");
	printf("-    Section ID Tool for GC    -\n");
	printf("--------------------------------\n\n");
	printf("Press [Up] and [Down] to go     \n");
	printf("    through an ASCII table.     \n\n");
	printf("[A] memorises a character.      \n\n");
	printf("Pressing [X] or [Y] will display\n");
	printf("    the ID PSO would give you   \n");
	printf("    when creating a character   \n");
	printf("    with the same name.         \n\n");
	printf("[Start] clears the screen.      \n\n");
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