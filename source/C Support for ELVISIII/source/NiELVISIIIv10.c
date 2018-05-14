/**
 * ELVIS III NI FPGA Interface C API Wrapper source file.
 *
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 */

#include <stdio.h>
#include <time.h>
#include "NiELVISIIIv10.h"

/**
 * Global ELVIS III NiFpga Session.
 * A valid session handle is created by calling the NiELVISIIIv10_Open() function.
 * This session should be used when calling any of the NiFpga functions after
 * the NiELVISIIIv10_Open() function has been called.
 */
NiFpga_Session NiELVISIIIv10_session;

/**
 * Define the location of the bit file. The bit file must be located in
 * this folder.
 */
#define NiELVISIIIv10_DefaultFolder "/var/local/natinst/bitfiles/"
#define NiELVISIIIv10_BitfilePath NiELVISIIIv10_DefaultFolder NiELVISIIIv10_Bitfile

/**
 * Simple error handling for the cases where the function returns on error.
 */
void NiELVISIIIv10_ReturnIfNotSuccess(NiFpga_Status status, char *message)
{
	if (NiELVISIIIv10_IsNotSuccess((status)))
	{
		NiELVISIIIv10_PrintStatus((status));
	    printf((message));
	    return;
	}
}

/**
 * Simple error handling for the cases where the function returns on error.
 */
double NiELVISIIIv10_ReturnValueIfNotSuccess(NiFpga_Status status, double value, char *message)
{
	if (NiELVISIIIv10_IsNotSuccess((status)))
	{
	    NiELVISIIIv10_PrintStatus((status));
	    printf((message));
	    return (value);
	}
	return OK;
}

/**
 * Simple error handling for the cases where the function returns on error.
 */
NiFpga_Status NiELVISIIIv10_ReturnStatusIfNotSuccess(NiFpga_Status status, char *message)
{
	if (NiELVISIIIv10_IsNotSuccess((status)))
	{
	    NiELVISIIIv10_PrintStatus((status));
	    printf((message));
	    return (status);
	}
	return OK;
}

/**
 * Print the value of status if there is an error or warning.
 *
 * Messages are printed to stdout. Error codes are prefixed with "ERROR:" and
 * warning codes are prefixed with "WARNING:". If the status is equal to
 * NiFpga_Status_Success then nothing is printed.
 *
 * @param[in]  status  The NiFpga_Status for which to print the value
 */
void NiELVISIIIv10_PrintStatus(NiFpga_Status status) {
	if (status < NiFpga_Status_Success) {
		printf("ERROR: %d\n", status);
	}
	else if (status > NiFpga_Status_Success) {
		printf("WARNING: %d\n", status);
	}
}

/**
 * Opens a session to the ELVIS III FPGA Personality.
 *
 * This function ensures that the NiFpga library is loaded and that the correct
 * ELVIS III personality bitfile is programmed to the FPGA, started, and is
 * running. The ELVIS III personality bitfile (.lvbitx file) for your target
 * must be in the same directory as where the executable is run otherwise the
 * function will fail.
 *
 * @warning  This function is not thread-safe.
 *           It should be called before all other function calls and only once
 *           per application.
 *
 * @return  NiFpga_Status which indicates if the operation was successful.
 */
NiFpga_Status NiELVISIIIv10_Open()
{
	NiFpga_Status status;

	/**
	 * Initialize the NiFpga Library.
	 */
	status = NiFpga_Initialize();
	if (NiELVISIIIv10_IsNotSuccess(status))
	{
		NiELVISIIIv10_PrintStatus(status);
		printf("Could not load NiFpga library!\n");
		return status;
	}

	/**
	 * Open the appropriate FPGA bitfile.
	 */
	status = NiFpga_Open(NiELVISIIIv10_BitfilePath, NiELVISIIIv10_Signature, "RIO0", NiFpga_OpenAttribute_NoRun, &NiELVISIIIv10_session);
    if (NiELVISIIIv10_IsNotSuccess(status))
    {
		NiELVISIIIv10_PrintStatus(status);
		printf("Could not Open FPGA!\n");
		if (status == NiFpga_Status_BitfileReadError)
		{
			printf("Ensure the bitfile %s exists\n", NiELVISIIIv10_BitfilePath);
		}
		return status;
    }

	/**
	 * Ensure that the FPGA code is reset and in a known state.
	 */
	status = NiFpga_Reset(NiELVISIIIv10_session);
	if (NiELVISIIIv10_IsNotSuccess(status))
	{
		NiELVISIIIv10_PrintStatus(status);
		printf("Could not Reset FPGA!\n");
		return status;
	}

	/**
	 * Start the FPGA code.
	 */
	status = NiFpga_Run(NiELVISIIIv10_session, 0);
	if (NiELVISIIIv10_IsNotSuccess(status))
	{
		NiELVISIIIv10_PrintStatus(status);
		printf("Could not Run FPGA!\n");
		return status;
	}

	return NiFpga_Status_Success;
}

/**
 * Closes the session to the ELVIS III FPGA Personality.
 *
 * This function resets the FPGA (unless there is another open session) and
 * unloads the NiFpga library.
 *
 * @warning  This function is not thread-safe.
 *           It should be called after all other function calls and only once
 *           per application.
 */
NiFpga_Status NiELVISIIIv10_Close()
{
	NiFpga_Status status;

	/**
	 * Close and Reset the FPGA
	 */
	status = NiFpga_Close(NiELVISIIIv10_session, 0);
	if (NiELVISIIIv10_IsNotSuccess(status))
	{
		NiELVISIIIv10_PrintStatus(status);
		printf("Could not Close FPGA!\n");
		return status;
	}

	/**
	 * Unload the NiFpga library
	 */
	status = NiFpga_Finalize();
	if (NiELVISIIIv10_IsNotSuccess(status))
	{
		NiELVISIIIv10_PrintStatus(status);
		printf("Could not unload NiFpga library!\n");
		return status;
	}

	return status;
}
