/**
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 */

#include <stdio.h>

#include "NiELVISIIIv10.h"

/**
 * Overview:
 * ELVIS III main function. This template contains basic code to open the ELVIS III
 * FPGA session.
 *
 * Code in ELVIS III example projects is designed for reuse. You can copy source
 * files from the example project to use in this template.
 *
 * Note:
 * The Eclipse project defines the preprocessor symbol for the ELVIS III.
 */
int main(int argc, char **argv)
{
	NiFpga_Status status;

	/*
	 * Open the ELVIS III NiFpga Session.
	 * This function MUST be called before all other functions. After this call
	 * is complete the ELVIS III target will be ready to be used.
	 */
	status = NiELVISIIIv10_Open();
	if (NiELVISIIIv10_IsNotSuccess(status))
	{
	    return status;
	}

	/*
	 * Your application code goes here.
	 */

	/*
	 * Close the ELVISIII NiFpga Session.
	 * This function MUST be called after all other functions.
	 */
	status = NiELVISIIIv10_Close();

	/*
	 * Returns 0 if successful.
	 */
	return status;
}
