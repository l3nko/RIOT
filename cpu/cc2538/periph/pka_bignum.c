/*
 * pka.c
 *
 *  Created on: Dec 18, 2015
 *      Author: l3nko
 */

#include "cc2538.h"
#include "periph_cpu.h"
#include "pka.h"
#include "hw_pka.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define HWREG(x)	(*((volatile unsigned long *)(x)))

#if ENABLE_DEBUG
	#define ASSERT(cond) \
		if (!(cond)) \
		{ \
			DEBUG(#cond, __FILE__, __LINE__, 0); \
		}
#else
	#define ASSERT(cond)
#endif

////PKAGetOpsStatus
//uint8_t pka_get_op_status(void)
//{
//	if((HWREG(PKA_FUNCTION) & PKA_FUNCTION_RUN) != 0)
//	{
//		return (PKA_STATUS_OPERATION_INPRG);
//	}
//	else
//	{
//		return (PKA_STATUS_OPERATION_NOT_INPRG);
//	}
//}

//PKABigNumModStart
uint8_t pka_bignum_mod_start(uint32_t* pui32BNum, uint8_t ui8BNSize,
                  uint32_t* pui32Modulus, uint8_t ui8ModSize,
                  uint32_t* pui32ResultVector)
{
	uint8_t extraBuf;
	uint32_t offset;
	int i;

	//
	// Check the arguments.
	//
	ASSERT(NULL != pui32BNum);
	ASSERT(NULL != pui32Modulus);
	ASSERT(NULL != pui32ResultVector);

	//
	// make sure no operation is in progress.
	//
	if((HWREG(PKA_FUNCTION) & PKA_FUNCTION_RUN) != 0)
	{
		return (PKA_STATUS_OPERATION_INPRG);
	}

	//
	// calculate the extra buffer requirement.
	//
	extraBuf = 2 + ui8ModSize % 2;

	offset = 0;

	//
	// Update the A ptr with the offset address of the PKA RAM location
	// where the number will be stored.
	//
	HWREG( (PKA_APTR) ) = offset >>2;

	//
	// Load the number in PKA RAM
	//
	for(i = 0; i < ui8BNSize; i++)
	{
		HWREG((PKA_RAM_BASE + offset + 4*i)) = pui32BNum[i];
	}

	//
	// determine the offset for the next data input.
	//
	offset += 4 * (i + ui8BNSize % 2);

	//
	// Update the B ptr with the offset address of the PKA RAM location
	// where the divisor will be stored.
	//
	HWREG( (PKA_BPTR) ) = offset >> 2;

	//
	// Load the divisor in PKA RAM.
	//
	for(i = 0; i < ui8ModSize;  i++)
	{
		HWREG((PKA_RAM_BASE + offset + 4*i)) = pui32Modulus[i];
	}

	//
	// determine the offset for the next data.
	//
	offset += 4 * (i + extraBuf);

	//
	// Copy the result vector address location.
	//
	*pui32ResultVector = PKA_RAM_BASE + offset;

	//
	// Load C ptr with the result location in PKA RAM
	//
	HWREG( (PKA_CPTR) ) = offset >> 2;

	//
	// Load A length registers with Big number length in 32 bit words.
	//
	HWREG( (PKA_ALENGTH) ) = ui8BNSize;

	//
	// Load B length registers  Divisor length in 32-bit words.
	//
	HWREG( (PKA_BLENGTH) ) = ui8ModSize;

	//
	// Start the PKCP modulo operation by setting the PKA Function register.
	//
	HWREG( (PKA_FUNCTION) ) = (PKA_FUNCTION_RUN | PKA_FUNCTION_MODULO);

	return (PKA_STATUS_SUCCESS);
}

//PKABigNumModGetResult
uint8_t pka_bignum_mod_result(uint32_t* pui32ResultBuf,uint8_t ui8Size,
                      uint32_t ui32ResVectorLoc)
{
	uint32_t regMSWVal;
	uint32_t len;
	int i;

	//
	// Check the arguments.
	//
	ASSERT(NULL != pui32ResultBuf);
	ASSERT((ui32ResVectorLoc > PKA_RAM_BASE) &&
		   (ui32ResVectorLoc < (PKA_RAM_BASE + PKA_RAM_SIZE)));

	//
	// verify that the operation is complete.
	//
	if((HWREG(PKA_FUNCTION) & PKA_FUNCTION_RUN) != 0)
	{
		return (PKA_STATUS_OPERATION_INPRG);
	}

	//
	//  Get the MSW register value.
	//
	regMSWVal = HWREG(PKA_DIVMSW);

	//
	// Check to make sure that the result vector is not all zeroes.
	//
	if(regMSWVal & PKA_DIVMSW_RESULT_IS_ZERO)
	{
		return (PKA_STATUS_RESULT_0);
	}

	//
	// Get the length of the result.
	//
	len = ((regMSWVal & PKA_DIVMSW_MSW_ADDRESS_M) + 1) -
		  ((ui32ResVectorLoc - PKA_RAM_BASE) >> 2);

	//
	// If the size of the buffer provided is less than the result length than
	// return error.
	//
	if(ui8Size < len)
	{
		return (PKA_STATUS_BUF_UNDERFLOW);
	}

	//
	// copy the result from vector C into the pResult.
	//
	for(i = 0; i < len; i++)
	{
		pui32ResultBuf[i]= HWREG( (ui32ResVectorLoc + 4*i) );
	}

	return (PKA_STATUS_SUCCESS);
}

//PKABigNumCmpStart
uint8_t pka_bignum_cmp_start(uint32_t* pui32BNum1, uint32_t* pui32BNum2, uint8_t ui8Size)
{
    uint32_t offset;
    int i;

    //
    // Check the arguments.
    //
    ASSERT(NULL != pui32BNum1);
    ASSERT(NULL != pui32BNum2);

    offset = 0;

    //
    // Make sure no operation is in progress.
    //
    if((HWREG(PKA_FUNCTION) & PKA_FUNCTION_RUN) != 0)
    {
        return (PKA_STATUS_OPERATION_INPRG);
    }

    //
    // Update the A ptr with the offset address of the PKA RAM location
    // where the first big number will be stored.
    //
    HWREG( (PKA_APTR) ) = offset >> 2;

    //
    // Load the first big number in PKA RAM.
    //
    for(i = 0; i < ui8Size; i++)
    {
        HWREG( (PKA_RAM_BASE + offset + 4*i) ) = pui32BNum1[i];
    }

    //
    // Determine the offset in PKA RAM for the next pointer.
    //
    offset += 4 * (i + ui8Size % 2);

    //
    // Update the B ptr with the offset address of the PKA RAM location
    // where the second big number will be stored.
    //
    HWREG( (PKA_BPTR) ) = offset >> 2;

    //
    // Load the second big number in PKA RAM.
    //
    for(i = 0; i < ui8Size;  i++)
    {
        HWREG( (PKA_RAM_BASE + offset + 4*i) ) = pui32BNum2[i];
    }

    //
    // Load length registers in 32 bit word size.
    //
    HWREG( (PKA_ALENGTH) ) = ui8Size;

    //
    // Set the PKA Function register for the Compare operation
    // and start the operation.
    //
    HWREG( (PKA_FUNCTION) ) = (PKA_FUNCTION_RUN | PKA_FUNCTION_COMPARE);

    return (PKA_STATUS_SUCCESS);
}

//PKABigNumCmpGetResult
uint8_t pka_bignum_cmp_result(void)
{
    uint8_t status;

    //
    // verify that the operation is complete.
    //
    if((HWREG(PKA_FUNCTION) & PKA_FUNCTION_RUN) != 0)
    {
        status = PKA_STATUS_OPERATION_INPRG;
        return (status);
    }

    //
    // Check the COMPARE register.
    //
    switch(HWREG(PKA_COMPARE))
    {
        case PKA_COMPARE_A_EQUALS_B:
            status = PKA_STATUS_SUCCESS;
            break;

        case PKA_COMPARE_A_GREATER_THAN_B:
            status = PKA_STATUS_A_GR_B;
            break;

        case PKA_COMPARE_A_LESS_THAN_B:
            status = PKA_STATUS_A_LT_B;
            break;

        default:
            status = PKA_STATUS_FAILURE;
            break;
    }

    return (status);
}

//PKABigNumInvModStart
uint8_t pka_bignum_invMod_start(uint32_t* pui32BNum, uint8_t ui8BNSize,
                     uint32_t* pui32Modulus, uint8_t ui8Size,
                     uint32_t* pui32ResultVector)
{
    uint32_t offset;
    int i;

    //
    // Check the arguments.
    //
    ASSERT(NULL != pui32BNum);
    ASSERT(NULL != pui32Modulus);
    ASSERT(NULL != pui32ResultVector);

    offset = 0;

    //
    // Make sure no operation is in progress.
    //
    if((HWREG(PKA_FUNCTION) & PKA_FUNCTION_RUN) != 0)
    {
        return (PKA_STATUS_OPERATION_INPRG);
    }

    //
    // Update the A ptr with the offset address of the PKA RAM location
    // where the number will be stored.
    //
    HWREG( (PKA_APTR) ) = offset >>2;

    //
    // Load the \e pui32BNum number in PKA RAM.
    //
    for(i = 0; i < ui8BNSize; i++)
    {
        HWREG( (PKA_RAM_BASE + offset + 4*i) ) = pui32BNum[i];
    }

    //
    // Determine the offset for next data.
    //
    offset += 4 * (i + ui8BNSize % 2);

    //
    // Update the B ptr with the offset address of the PKA RAM location
    // where the modulus will be stored.
    //
    HWREG( (PKA_BPTR) ) = offset >> 2;

    //
    // Load the \e pui32Modulus divisor in PKA RAM.
    //
    for(i = 0; i < ui8Size;  i++)
    {
        HWREG( (PKA_RAM_BASE + offset + 4*i) ) = pui32Modulus[i];
    }

    //
    // Determine the offset for result data.
    //
    offset += 4 * (i + ui8Size % 2);

    //
    // Copy the result vector address location.
    //
    *pui32ResultVector = PKA_RAM_BASE + offset;

    //
    // Load D ptr with the result location in PKA RAM.
    //
    HWREG( (PKA_DPTR) ) = offset >> 2;

    //
    // Load the respective length registers.
    //
    HWREG( (PKA_ALENGTH) ) = ui8BNSize;
    HWREG( (PKA_BLENGTH) ) = ui8Size;

    //
    // set the PKA function to InvMod operation and the start the operation.
    //
    HWREG( (PKA_FUNCTION) ) = 0x0000F000;

    return (PKA_STATUS_SUCCESS);
}

//PKABigNumInvModGetResult
uint8_t pka_bignum_invMod_result(uint32_t* pui32ResultBuf, uint8_t ui8Size,
                         uint32_t ui32ResVectorLoc)
{
    uint32_t regMSWVal;
    uint32_t len;
    int i;

    //
    // Check the arguments.
    //
    ASSERT(NULL != pui32ResultBuf);
    ASSERT((ui32ResVectorLoc > PKA_RAM_BASE) &&
           (ui32ResVectorLoc < (PKA_RAM_BASE + PKA_RAM_SIZE)));

    //
    // Verify that the operation is complete.
    //
    if((HWREG(PKA_FUNCTION) & PKA_FUNCTION_RUN) != 0)
    {
        return (PKA_STATUS_OPERATION_INPRG);
    }

    //
    // Get the MSW register value.
    //
    regMSWVal = HWREG(PKA_MSW);

    //
    // Check to make sure that the result vector is not all zeroes.
    //
    if(regMSWVal & PKA_MSW_RESULT_IS_ZERO)
    {
        return (PKA_STATUS_RESULT_0);
    }

    //
    // Get the length of the result
    //
    len = ((regMSWVal & PKA_MSW_MSW_ADDRESS_M) + 1) -
          ((ui32ResVectorLoc - PKA_RAM_BASE) >> 2);

    //
    // Check if the provided buffer length is adequate to store the result
    // data.
    //
    if(ui8Size < len)
    {
        return (PKA_STATUS_BUF_UNDERFLOW);
    }

    //
    // Copy the result from vector C into the \e pui32ResultBuf.
    for(i = 0; i < len; i++)
    {
        pui32ResultBuf[i]= HWREG( (ui32ResVectorLoc + 4*i) );
    }

    return (PKA_STATUS_SUCCESS);
}

//PKABigNumMultiplyStart
uint8_t pka_bignum_numMul_start(uint32_t* pui32Xplicand, uint8_t ui8XplicandSize,
                       uint32_t* pui32Xplier, uint8_t ui8XplierSize,
                       uint32_t* pui32ResultVector)
{
    uint32_t offset;
    int i;

    //
    // Check for the arguments.
    //
    ASSERT(NULL != pui32Xplicand);
    ASSERT(NULL != pui32Xplier);
    ASSERT(NULL != pui32ResultVector);

    offset = 0;

    //
    // Make sure no operation is in progress.
    //
    if((HWREG(PKA_FUNCTION) & PKA_FUNCTION_RUN) != 0)
    {
        return (PKA_STATUS_OPERATION_INPRG);
    }

    //
    // Update the A ptr with the offset address of the PKA RAM location
    // where the multiplicand will be stored.
    //
    HWREG( (PKA_APTR) ) = offset >> 2;

    //
    // Load the multiplicand in PKA RAM.
    //
    for(i = 0; i < ui8XplicandSize; i++)
    {
        HWREG((PKA_RAM_BASE + offset + 4*i)) = *pui32Xplicand;
        pui32Xplicand++;
    }

    //
    // Determine the offset for the next data.
    //
    offset += 4 * (i + (ui8XplicandSize % 2));

    //
    // Update the B ptr with the offset address of the PKA RAM location
    // where the multiplier will be stored.
    //
    HWREG( (PKA_BPTR) ) = offset >> 2;

    //
    // Load the multiplier in PKA RAM.
    //
    for(i = 0; i < ui8XplierSize; i++)
    {
        HWREG( (PKA_RAM_BASE + offset + 4*i) ) = *pui32Xplier;
        pui32Xplier++;
    }

    //
    // Determine the offset for the next data.
    //
    offset += 4 * (i + (ui8XplierSize % 2));

    //
    // Copy the result vector address location.
    //
    *pui32ResultVector = PKA_RAM_BASE + offset;

    //
    // Load C ptr with the result location in PKA RAM.
    //
    HWREG( (PKA_CPTR) ) = offset >> 2;

    //
    // Load the respective length registers.
    //
    HWREG( (PKA_ALENGTH) ) = ui8XplicandSize;
    HWREG( (PKA_BLENGTH) ) = ui8XplierSize;

    //
    // Set the PKA function to the multiplication and start it.
    //
    HWREG( (PKA_FUNCTION) ) = (PKA_FUNCTION_RUN | PKA_FUNCTION_MULTIPLY);

    return (PKA_STATUS_SUCCESS);
}

//PKABigNumMultGetResult
uint8_t pka_bignum_numMul_result(uint32_t* pui32ResultBuf, uint32_t* pui32Len,
                       uint32_t ui32ResVectorLoc)
{
    uint32_t regMSWVal;
    uint32_t len;
    int i;

    //
    // Check for arguments.
    //
    ASSERT(NULL != pui32ResultBuf);
    ASSERT(NULL != pui32Len);
    ASSERT((ui32ResVectorLoc > PKA_RAM_BASE) &&
           (ui32ResVectorLoc < (PKA_RAM_BASE + PKA_RAM_SIZE)));

    //
    // Verify that the operation is complete.
    //
    if((HWREG(PKA_FUNCTION) & PKA_FUNCTION_RUN) != 0)
    {
        return (PKA_STATUS_OPERATION_INPRG);
    }

    //
    // Get the MSW register value.
    //
    regMSWVal = HWREG(PKA_MSW);

    //
    // Check to make sure that the result vector is not all zeroes.
    //
    if(regMSWVal & PKA_MSW_RESULT_IS_ZERO)
    {
        return (PKA_STATUS_RESULT_0);
    }

    //
    // Get the length of the result.
    //
    len = ((regMSWVal & PKA_MSW_MSW_ADDRESS_M) + 1) -
          ((ui32ResVectorLoc - PKA_RAM_BASE) >> 2);

    //
    // Make sure that the length of the supplied result buffer is adequate
    // to store the resultant.
    //
    if(*pui32Len < len)
    {
        return (PKA_STATUS_BUF_UNDERFLOW);
    }

    //
    // Copy the resultant length.
    //
    *pui32Len = len;

    //
    // Copy the result from vector C into the pResult.
    //
    for(i = 0; i < *pui32Len; i++)
    {
        pui32ResultBuf[i]= HWREG( (ui32ResVectorLoc + 4*i) );
    }

    return (PKA_STATUS_SUCCESS);
}


//PKABigNumAddStart
uint8_t pka_bignum_numAdd_start(uint32_t* pui32BN1, uint8_t ui8BN1Size,
                  uint32_t* pui32BN2, uint8_t ui8BN2Size,
                  uint32_t* pui32ResultVector)
{
    uint32_t offset;
    int i;

    //
    // Check for arguments.
    //
    ASSERT(NULL != pui32BN1);
    ASSERT(NULL != pui32BN2);
    ASSERT(NULL != pui32ResultVector);

    offset = 0;

    //
    // Make sure no operation is in progress.
    //
    if((HWREG(PKA_FUNCTION) & PKA_FUNCTION_RUN) != 0)
    {
        return (PKA_STATUS_OPERATION_INPRG);
    }

    //
    // Update the A ptr with the offset address of the PKA RAM location
    // where the big number 1 will be stored.
    //
    HWREG( (PKA_APTR) ) = offset >> 2;

    //
    // Load the big number 1 in PKA RAM.
    //
    for(i = 0; i < ui8BN1Size; i++)
    {
        HWREG((PKA_RAM_BASE + offset + 4*i)) = pui32BN1[i];
    }

    //
    // Determine the offset in PKA RAM for the next data.
    //
    offset += 4 * (i + (ui8BN1Size % 2));

    //
    // Update the B ptr with the offset address of the PKA RAM location
    // where the big number 2 will be stored.
    //
    HWREG( (PKA_BPTR) ) = offset >> 2;

    //
    // Load the big number 2 in PKA RAM.
    //
    for(i = 0; i < ui8BN2Size; i++)
    {
        HWREG((PKA_RAM_BASE + offset + 4*i)) = pui32BN2[i];
    }

    //
    // Determine the offset in PKA RAM for the next data.
    //
    offset += 4 * (i + (ui8BN2Size % 2));

    //
    // Copy the result vector address location.
    //
    *pui32ResultVector = PKA_RAM_BASE + offset;

    //
    // Load C ptr with the result location in PKA RAM.
    //
    HWREG( (PKA_CPTR) ) = offset >> 2;

    //
    // Load respective length registers.
    //
    HWREG( (PKA_ALENGTH) ) = ui8BN1Size;
    HWREG( (PKA_BLENGTH) ) = ui8BN2Size;

    //
    // Set the function for the add operation and start the operation.
    //
    HWREG( (PKA_FUNCTION) ) = (PKA_FUNCTION_RUN | PKA_FUNCTION_ADD);

    return (PKA_STATUS_SUCCESS);
}

//PKABigNumAddGetResult
uint8_t pka_bignum_numAdd_result(uint32_t* pui32ResultBuf, uint32_t* pui32Len,
                      uint32_t ui32ResVectorLoc)
{
    uint32_t regMSWVal;
    uint32_t len;
    int i;

    //
    // Check for the arguments.
    //
    ASSERT(NULL != pui32ResultBuf);
    ASSERT(NULL != pui32Len);
    ASSERT((ui32ResVectorLoc > PKA_RAM_BASE) &&
           (ui32ResVectorLoc < (PKA_RAM_BASE + PKA_RAM_SIZE)));

    //
    // Verify that the operation is complete.
    //
    if((HWREG(PKA_FUNCTION) & PKA_FUNCTION_RUN) != 0)
    {
        return (PKA_STATUS_OPERATION_INPRG);
    }

    //
    // Get the MSW register value.
    //
    regMSWVal = HWREG(PKA_MSW);

    //
    // Check to make sure that the result vector is not all zeroes.
    //
    if(regMSWVal & PKA_MSW_RESULT_IS_ZERO)
    {
        return (PKA_STATUS_RESULT_0);
    }

    //
    // Get the length of the result.
    //
    len = ((regMSWVal & PKA_MSW_MSW_ADDRESS_M) + 1) -
          ((ui32ResVectorLoc - PKA_RAM_BASE) >> 2);

    //
    // Make sure that the supplied result buffer is adequate to store the
    // resultant data.
    //
    if(*pui32Len < len)
    {
        return (PKA_STATUS_BUF_UNDERFLOW);
    }

    //
    // Copy the length.
    //
    *pui32Len = len;

    //
    // Copy the result from vector C into the provided buffer.
    for(i = 0; i < *pui32Len; i++)
    {
        pui32ResultBuf[i] = HWREG( (ui32ResVectorLoc +  4*i) );
    }

    return (PKA_STATUS_SUCCESS);
}








