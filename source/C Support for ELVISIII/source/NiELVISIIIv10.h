/**
 * ELVIS III NI FPGA Interface C API Wrapper header file.
 *
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 */

#ifndef NiELVISIIIv10_h_
#define NiELVISIIIv10_h_

#include "NiELVISIIIv10_Typedefs.h"

#define OK 1

#if NiFpga_Cpp
extern "C" {
#endif

/**
 * Simple error handling for the cases where the function returns on error.
 */
void NiELVISIIIv10_ReturnIfNotSuccess(NiFpga_Status status, char *message);

/**
 * Simple error handling for the cases where the function returns on error.
 */
double NiELVISIIIv10_ReturnValueIfNotSuccess(NiFpga_Status status, double value, char *message);

/**
 * Simple error handling for the cases where the function returns on error.
 */
NiFpga_Status NiELVISIIIv10_ReturnStatusIfNotSuccess(NiFpga_Status status, char *message);

/**
 * Tests whether a status is not equal to NiFpga_Status_Success.
 *
 * Both errors and warnings will return NiFpga_True. If you only care about
 * checking errors and not warnings then use NiFpga_IsNotError.
 *
 * @param[in]  status  The NiFpga_Status to check for an error
 * @return  NiFpga_Bool which indicates if the status is an error or warning
 * @see NiFpga_IsNotError
 */
static NiFpga_Inline NiFpga_Bool NiELVISIIIv10_IsNotSuccess(NiFpga_Status status)
{
	return (status != NiFpga_Status_Success);
}

/**
 * Print the value of status if there is an error or warning.
 */
void NiELVISIIIv10_PrintStatus(NiFpga_Status status);

/**
 * Opens a session to the ELVIS III FPGA Personality.
 */
NiFpga_Status NiELVISIIIv10_Open();

/**
 * Closes the session to the ELVIS III FPGA Personality.
 */
NiFpga_Status NiELVISIIIv10_Close();

#if NiFpga_Cpp
}
#endif

#endif /* NiELVISIIIv10_h_ */
