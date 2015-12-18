/*
 * pka_ecc.c
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

//PKAECCMultiplyStart
uint8_t pka_ecc_mul_start(uint32_t* pui32Scalar, ecc_point_t* ptEcPt,
			ecc_curve_info_t* ptCurve, uint32_t* pui32ResultVector)
{
    uint8_t extraBuf;
    uint32_t offset;
    int i;

    //
    // Check for the arguments.
    //
    ASSERT(NULL != pui32Scalar);
    ASSERT(NULL != ptEcPt);
    ASSERT(NULL != ptEcPt->pui32X);
    ASSERT(NULL != ptEcPt->pui32Y);
    ASSERT(NULL != ptCurve);
    ASSERT(ptCurve->ui8Size <= PKA_MAX_CURVE_SIZE_32_BIT_WORD);
    ASSERT(NULL != pui32ResultVector);

    offset = 0;

    //
    // Make sure no PKA operation is in progress.
    //
    if((HWREG(PKA_FUNCTION) & PKA_FUNCTION_RUN) != 0)
    {
        return (PKA_STATUS_OPERATION_INPRG);
    }

    //
    // Calculate the extra buffer requirement.
    //
    extraBuf = 2 + ptCurve->ui8Size % 2;

    //
    // Update the A ptr with the offset address of the PKA RAM location
    // where the scalar will be stored.
    //
    HWREG((PKA_APTR)) = offset >> 2;

    //
    // Load the scalar in PKA RAM.
    //
    for(i = 0; i < ptCurve->ui8Size; i++)
    {
        HWREG((PKA_RAM_BASE + offset + 4*i)) = *pui32Scalar++;
    }

    //
    // Determine the offset for the next data.
    //
    offset += 4 * (i + (ptCurve->ui8Size % 2));

    //
    // Update the B ptr with the offset address of the PKA RAM location
    // where the curve parameters will be stored.
    //
    HWREG((PKA_BPTR)) = offset >> 2;

    //
    // Write curve parameter 'p' as 1st part of vector B immediately
    // following vector A at PKA RAM
    //
    for(i = 0; i < ptCurve->ui8Size; i++)
    {
        HWREG((PKA_RAM_BASE + offset + 4*i)) =
            (uint32_t)ptCurve->pui32Prime[i];
    }

    //
    // Determine the offset for the next data.
    //
    offset += 4 * (i + extraBuf);

    //
    // Copy curve parameter 'a' in PKA RAM.
    //
    for(i = 0; i < ptCurve->ui8Size; i++)
    {
        HWREG((PKA_RAM_BASE + offset + 4*i)) = (uint32_t)ptCurve->pui32A[i];
    }

    //
    // Determine the offset for the next data.
    //
    offset += 4 * (i + extraBuf);

    //
    // Copy curve parameter 'b' in PKA RAM.
    //
    for(i = 0; i < ptCurve->ui8Size; i++)
    {
        HWREG((PKA_RAM_BASE + offset + 4*i)) = (uint32_t)ptCurve->pui32B[i];
    }

    //
    // Determine the offset for the next data.
    //
    offset += 4 * (i + extraBuf);

    //
    // Update the C ptr with the offset address of the PKA RAM location
    // where the Gx, Gy will be stored.
    //
    HWREG((PKA_CPTR)) = offset >> 2;

    //
    // Write elliptic curve point x co-ordinate value.
    //
    for(i = 0; i < ptCurve->ui8Size; i++)
    {
        HWREG((PKA_RAM_BASE + offset + 4*i)) = ptEcPt->pui32X[i];
    }

    //
    // Determine the offset for the next data.
    //
    offset += 4 * (i + extraBuf);

    //
    // Write elliptic curve point y co-ordinate value.
    //
    for(i = 0; i < ptCurve->ui8Size; i++)
    {
        HWREG((PKA_RAM_BASE + offset + 4*i)) = ptEcPt->pui32Y[i];
    }

    //
    // Determine the offset for the next data.
    //
    offset += 4 * (i + extraBuf);

    //
    // Update the result location.
    //
    *pui32ResultVector =  PKA_RAM_BASE + offset;

    //
    // Load D ptr with the result location in PKA RAM.
    //
    HWREG(PKA_DPTR) = offset >> 2;

    //
    // Load length registers.
    //
    HWREG(PKA_ALENGTH) = ptCurve->ui8Size;
    HWREG(PKA_BLENGTH) = ptCurve->ui8Size;

    //
    // set the PKA function to ECC-MULT and start the operation.
    //
    HWREG(PKA_FUNCTION) = 0x0000D000;

    return (PKA_STATUS_SUCCESS);
}


//PKAECCMultiplyGetResult
uint8_t pka_ecc_mul_result(ecc_point_t* ptOutEcPt, uint32_t ui32ResVectorLoc)
{
    int i;
    uint32_t addr;
    uint32_t regMSWVal;
    uint32_t len;

    //
    // Check for the arguments.
    //
    ASSERT(NULL != ptOutEcPt);
    ASSERT(NULL != ptOutEcPt->pui32X);
    ASSERT(NULL != ptOutEcPt->pui32Y);
    ASSERT((ui32ResVectorLoc > PKA_RAM_BASE) &&
           (ui32ResVectorLoc < (PKA_RAM_BASE + PKA_RAM_SIZE)));

    //
    // Verify that the operation is completed.
    //
    if((HWREG(PKA_FUNCTION) & PKA_FUNCTION_RUN) != 0)
    {
        return (PKA_STATUS_OPERATION_INPRG);
    }

    if(HWREG(PKA_SHIFT) == 0x00000000)
    {
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

        addr = ui32ResVectorLoc;

        //
        // copy the x co-ordinate value of the result from vector D into
        // the \e ptOutEcPt.
        //
        for(i = 0; i < len; i++)
        {
            ptOutEcPt->pui32X[i] = HWREG(addr + 4*i);
        }

        addr += 4 * (i + 2 + len % 2);

        //
        // copy the y co-ordinate value of the result from vector D into
        // the \e ptOutEcPt.
        //
        for(i = 0; i < len; i++)
        {
            ptOutEcPt->pui32Y[i] = HWREG(addr + 4*i);
        }

        return (PKA_STATUS_SUCCESS);
    }
    else
    {
        return (PKA_STATUS_FAILURE);
    }
}


//PKAECCMultGenPtStart
uint8_t pka_ecc_mulGen_start(uint32_t* pui32Scalar, ecc_curve_info_t* ptCurve,
                     uint32_t* pui32ResultVector)
{
    uint8_t extraBuf;
    uint32_t offset;
    int i;

    //
    // check for the arguments.
    //
    ASSERT(NULL != pui32Scalar);
    ASSERT(NULL != ptCurve);
    ASSERT(ptCurve->ui8Size <= PKA_MAX_CURVE_SIZE_32_BIT_WORD);
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
    // Calculate the extra buffer requirement.
    //
    extraBuf = 2 + ptCurve->ui8Size % 2;

    //
    // Update the A ptr with the offset address of the PKA RAM location
    // where the scalar will be stored.
    //
    HWREG(PKA_APTR) = offset >> 2;

    //
    // Load the scalar in PKA RAM.
    //
    for(i = 0; i < ptCurve->ui8Size; i++)
    {
        HWREG((PKA_RAM_BASE + offset + 4*i)) = *pui32Scalar++;
    }

    //
    // Determine the offset in PKA RAM for the next data.
    //
    offset += 4 * (i + (ptCurve->ui8Size % 2));

    //
    // Update the B ptr with the offset address of the PKA RAM location
    // where the curve parameters will be stored.
    //
    HWREG(PKA_BPTR) = offset >> 2;

    //
    // Write curve parameter 'p' as 1st part of vector B.
    //
    for(i = 0; i < ptCurve->ui8Size; i++)
    {
        HWREG((PKA_RAM_BASE + offset + 4*i)) =
            (uint32_t)ptCurve->pui32Prime[i];
    }

    //
    // Determine the offset in PKA RAM for the next data.
    //
    offset += 4 * (i + extraBuf);

    //
    // Write curve parameter 'a' in PKA RAM.
    //
    for(i = 0; i < ptCurve->ui8Size; i++)
    {
        HWREG((PKA_RAM_BASE + offset + 4*i)) = (uint32_t)ptCurve->pui32A[i];
    }

    //
    // Determine the offset in PKA RAM for the next data.
    //
    offset += 4 * (i + extraBuf);

    //
    // write curve parameter 'b' in PKA RAM.
    //
    for(i = 0; i < ptCurve->ui8Size; i++)
    {
        HWREG((PKA_RAM_BASE + offset + 4*i)) = (uint32_t)ptCurve->pui32B[i];
    }

    //
    // Determine the offset in PKA RAM for the next data.
    //
    offset += 4 * (i + extraBuf);

    //
    // Update the C ptr with the offset address of the PKA RAM location
    // where the Gx, Gy will be stored.
    //
    HWREG(PKA_CPTR) = offset >> 2;

    //
    // Write x co-ordinate value of the Generator point in PKA RAM.
    //
    for(i = 0; i < ptCurve->ui8Size; i++)
    {
        HWREG((PKA_RAM_BASE + offset + 4*i)) = (uint32_t)ptCurve->pui32Gx[i];
    }

    //
    // Determine the offset in PKA RAM for the next data.
    //
    offset += 4 * (i + extraBuf);

    //
    // Write y co-ordinate value of the Generator point in PKA RAM.
    //
    for(i = 0; i < ptCurve->ui8Size; i++)
    {
        HWREG((PKA_RAM_BASE + offset + 4*i)) = (uint32_t)ptCurve->pui32Gy[i];
    }

    //
    // Determine the offset in PKA RAM for the next data.
    //
    offset += 4 * (i + extraBuf);

    //
    // Update the result location.
    //
    *pui32ResultVector =  PKA_RAM_BASE + offset;

    //
    // Load D ptr with the result location in PKA RAM.
    //
    HWREG(PKA_DPTR) = offset >> 2;

    //
    // Load length registers.
    //
    HWREG(PKA_ALENGTH) = ptCurve->ui8Size;
    HWREG(PKA_BLENGTH) = ptCurve->ui8Size;

    //
    // Set the PKA function to ECC-MULT and start the operation.
    //
    HWREG( (PKA_FUNCTION) ) = 0x0000D000;

    return (PKA_STATUS_SUCCESS);
}


//PKAECCMultGenPtGetResult
uint8_t pka_ecc_mulGen_result(ecc_point_t* ptOutEcPt, uint32_t ui32ResVectorLoc)
{
    int i;
    uint32_t regMSWVal;
    uint32_t addr;
    uint32_t len;

    //
    // Check for the arguments.
    //
    ASSERT(NULL != ptOutEcPt);
    ASSERT(NULL != ptOutEcPt->pui32X);
    ASSERT(NULL != ptOutEcPt->pui32Y);
    ASSERT((ui32ResVectorLoc > PKA_RAM_BASE) &&
           (ui32ResVectorLoc < (PKA_RAM_BASE + PKA_RAM_SIZE)));

    //
    // Verify that the operation is completed.
    //
    if((HWREG(PKA_FUNCTION) & PKA_FUNCTION_RUN) != 0)
    {
        return (PKA_STATUS_OPERATION_INPRG);
    }

    if(HWREG(PKA_SHIFT) == 0x00000000)
    {
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

        addr = ui32ResVectorLoc;

        //
        // Copy the x co-ordinate value of the result from vector D into the
        // EC point.
        //
        for(i = 0; i < len; i++)
        {
            ptOutEcPt->pui32X[i] = HWREG( (addr + 4*i) );
        }

        addr += 4 * (i + 2 + len % 2);

        //
        // Copy the y co-ordinate value of the result from vector D into the
        // EC point.
        //
        for(i = 0; i < len; i++)
        {
            ptOutEcPt->pui32Y[i] = HWREG( (addr + 4*i) );
        }

        return (PKA_STATUS_SUCCESS);
    }
    else
    {
        return (PKA_STATUS_FAILURE);
    }
}

//PKAECCAddStart
uint8_t pka_ecc_add_start(ecc_point_t* ptEcPt1, ecc_point_t* ptEcPt2, ecc_curve_info_t* ptCurve,
               uint32_t* pui32ResultVector)
{
    uint8_t extraBuf;
    uint32_t offset;
    int i;

    //
    // Check for the arguments.
    //
    ASSERT(NULL != ptEcPt1);
    ASSERT(NULL != ptEcPt1->pui32X);
    ASSERT(NULL != ptEcPt1->pui32Y);
    ASSERT(NULL != ptEcPt2);
    ASSERT(NULL != ptEcPt2->pui32X);
    ASSERT(NULL != ptEcPt2->pui32Y);
    ASSERT(NULL != ptCurve);
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
    // Calculate the extra buffer requirement.
    //
    extraBuf = 2 + ptCurve->ui8Size % 2;

    //
    // Update the A ptr with the offset address of the PKA RAM location
    // where the first ecPt will be stored.
    //
    HWREG(PKA_APTR) = offset >> 2;

    //
    // Load the x co-ordinate value of the first EC point in PKA RAM.
    //
    for(i = 0; i < ptCurve->ui8Size; i++)
    {
        HWREG((PKA_RAM_BASE + offset + 4*i)) = ptEcPt1->pui32X[i];
    }

    //
    // Determine the offset in PKA RAM for the next data.
    //
    offset += 4 * (i + extraBuf);

    //
    // Load the y co-ordinate value of the first EC point in PKA RAM.
    //
    for(i = 0; i < ptCurve->ui8Size; i++)
    {
        HWREG((PKA_RAM_BASE + offset + 4*i)) = ptEcPt1->pui32Y[i];
    }

    //
    // Determine the offset in PKA RAM for the next data.
    //
    offset += 4 * (i + extraBuf);

    //
    // Update the B ptr with the offset address of the PKA RAM location
    // where the curve parameters will be stored.
    //
    HWREG(PKA_BPTR) = offset >> 2;

    //
    // Write curve parameter 'p' as 1st part of vector B
    //
    for(i = 0; i < ptCurve->ui8Size; i++)
    {
        HWREG((PKA_RAM_BASE + offset + 4*i)) =
            (uint32_t)ptCurve->pui32Prime[i];
    }

    //
    // Determine the offset in PKA RAM for the next data.
    //
    offset += 4 * (i + extraBuf);

    //
    // Write curve parameter 'a'.
    //
    for(i = 0; i < ptCurve->ui8Size; i++)
    {
        HWREG((PKA_RAM_BASE + offset + 4*i)) = (uint32_t)ptCurve->pui32A[i];
    }

    //
    // Determine the offset in PKA RAM for the next data.
    //
    offset += 4 * (i + extraBuf);

    //
    // Update the C ptr with the offset address of the PKA RAM location
    // where the ecPt2 will be stored.
    //
    HWREG(PKA_CPTR) = offset >> 2;

    //
    // Load the x co-ordinate value of the second EC point in PKA RAM.
    //
    for(i = 0; i < ptCurve->ui8Size; i++)
    {
        HWREG((PKA_RAM_BASE + offset + 4*i)) = ptEcPt2->pui32X[i];
    }

    //
    // Determine the offset in PKA RAM for the next data.
    //
    offset += 4 * (i + extraBuf);

    //
    // Load the y co-ordinate value of the second EC point in PKA RAM.
    //
    for(i = 0; i < ptCurve->ui8Size; i++)
    {
        HWREG((PKA_RAM_BASE + offset + 4*i)) = ptEcPt2->pui32Y[i];
    }

    //
    // Determine the offset in PKA RAM for the next data.
    //
    offset += 4 * (i + extraBuf);

    //
    // Copy the result vector location.
    //
    *pui32ResultVector = PKA_RAM_BASE + offset;

    //
    // Load D ptr with the result location in PKA RAM.
    //
    HWREG(PKA_DPTR) = offset >> 2;

    //
    // Load length registers.
    //
    HWREG(PKA_BLENGTH) = ptCurve->ui8Size;

    //
    // Set the PKA Function to ECC-ADD and start the operation.
    //
    HWREG( (PKA_FUNCTION) ) = 0x0000B000;

    return (PKA_STATUS_SUCCESS);
}

//PKAECCAddGetResult
uint8_t pka_ecc_add_result(ecc_point_t* ptOutEcPt, uint32_t ui32ResVectorLoc)
{
    uint32_t regMSWVal;
    uint32_t addr;
    int i;
    uint32_t len;

    //
    // Check for the arguments.
    //
    ASSERT(NULL != ptOutEcPt);
    ASSERT(NULL != ptOutEcPt->pui32X);
    ASSERT(NULL != ptOutEcPt->pui32Y);
    ASSERT((ui32ResVectorLoc > PKA_RAM_BASE) &&
           (ui32ResVectorLoc < (PKA_RAM_BASE + PKA_RAM_SIZE)));

    if((HWREG(PKA_FUNCTION) & PKA_FUNCTION_RUN) != 0)
    {
        return (PKA_STATUS_OPERATION_INPRG);
    }

    if(HWREG(PKA_SHIFT) == 0x00000000)
    {
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

        addr = ui32ResVectorLoc;

        //
        // Copy the x co-ordinate value of result from vector D into the
        // the output EC Point.
        //
        for(i = 0; i < len; i++)
        {
            ptOutEcPt->pui32X[i] = HWREG((addr + 4*i));
        }

        addr += 4 * (i + 2 + len % 2);

        //
        // Copy the y co-ordinate value of result from vector D into the
        // the output EC Point.
        //
        for(i = 0; i < len; i++)
        {
            ptOutEcPt->pui32Y[i] = HWREG((addr + 4*i));
        }

        return (PKA_STATUS_SUCCESS);
    }
    else
    {
        return (PKA_STATUS_FAILURE);
    }
}



