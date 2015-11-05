#define VERSION_STR "0.0.1"

#if defined(_WIN32)
#if !defined __BORLANDC__
#include "StdAfx.h"        // Precompiled Header for WIN32
#endif
#endif // defined(_WIN32)
#include "lpc21isp.h"   // if using propriatory serial port communication (customize attached lpc21isp.h)
#include "lpcterm.h"

#define MAX_BUF_LEN 20000

// ret 0 if eof
int readLine(FILE* fd, char* buf) {
	for (int i = 0;; i++) {
		char c = 0;
		int size = fread(&c, 1, 1, fd);
		if (c == '\n' && i == 0) {
			i--;
			continue;
		}
		if (i == MAX_BUF_LEN - 2 || size == 0 || c == '\n') {
			buf[i++] = '\n';
			buf[i] = '\0';
			return size != 0;
		}
		if (c == '_')
			c = '\n';
		buf[i] = c;
	}
}

void SendComPortWithDelay(ISP_ENVIRONMENT* isp, char* buffer) {
	char sendbuf[2];
	sendbuf[1] = '\0';
	for (int i = 0;; i++) {
		sendbuf[0] = buffer[i];
		if (!sendbuf[0])
			break;
		SendComPort(isp, sendbuf);
		Sleep(100);
	}
}

void test(ISP_ENVIRONMENT* IspEnvironment) {
	char buffer[MAX_BUF_LEN];
//	char* buffer;
//	char recvbuf[MAX_BUF_LEN];
	
	for (;;) {
		
		FILE* fdtestfile = fopen("bin.txt", "r");
		
		for (;;) {
			if (!readLine(fdtestfile, buffer))
				break;
			
			int n = 0;
			for (int i = 0; i < 8; i++) {
				n |= buffer[i] == '1' ? 1 << (7 - i) : 0;
			}
			
			sprintf(buffer, "MATLED SHOW %02X\n", n);
			printf("%s", buffer);
			//		SendComPortWithDelay(IspEnvironment, buffer);
			SendComPort(IspEnvironment, buffer);
			
			/*
			ReceiveComPort(IspEnvironment, recvbuf, sizeof(recvbuf) - 1, &realsize, 0, 5000);          // Check for received characters
			if (!realsize) {
				printf("error!\n");
				break;
			}
			*/
			/*
			printf("%s", recvbuf);
			readLine(fdtestfile, buffer);
			if (strcmp(buffer, recvbuf)) {
				printf("error!\n");
				printf("!= %s %d %d\n", buffer, strlen(buffer), strlen(recvbuf));
				break;
			}
			*/
			Sleep(100);
		}
		fclose(fdtestfile);
	}
	return;
}



int PerformActions(ISP_ENVIRONMENT *IspEnvironment)
{
//    DebugPrintf(2, "serial version " VERSION_STR "\n");

    OpenSerialPort(IspEnvironment);   /* Open the serial port to the microcontroller. */

//    ResetTarget(IspEnvironment, PROGRAM_MODE);

    ClearSerialPortBuffers(IspEnvironment);

    // Pass control to Terminal which will provide a terminal if one was asked for
	// User asked for terminal emulation, provide a really dumb terminal.
    test(IspEnvironment);

    CloseSerialPort(IspEnvironment);  /*  All done, close the serial port to the      */

    return 0;
}


/***************************** main *************************************/
/**  main. Everything starts from here.
\param [in] argc the number of arguments.
\param [in] argv an array of pointers to the arguments.
*/

#if !defined COMPILE_FOR_LPC21

#if defined INTEGRATED_IN_WIN_APP
int AppDoProgram(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    ISP_ENVIRONMENT IspEnvironment;

    // Initialize debug level
    debug_level = 2;

    // Initialize ISP Environment
    memset(&IspEnvironment, 0, sizeof(IspEnvironment));       // Clear the IspEnviroment to a known value
    IspEnvironment.micro       = NXP_ARM;                     // Default Micro
    IspEnvironment.FileFormat  = FORMAT_HEX;                  // Default File Format
    IspEnvironment.ProgramChip = TRUE;                        // Default to Programming the chip
    IspEnvironment.nQuestionMarks = 100;
    IspEnvironment.DoNotStart = 0;
    IspEnvironment.BootHold = 0;
//    ReadArguments(&IspEnvironment, argc, argv);               // Read and parse the command line
	
	IspEnvironment.serial_port = "/dev/tty.SLAB_USBtoUART";
	IspEnvironment.baud_rate = "115200";
	
    return PerformActions(&IspEnvironment);                   // Do as requested !
}

#endif // !defined COMPILE_FOR_LPC21
