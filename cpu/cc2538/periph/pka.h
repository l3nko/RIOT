/*
 * pka.h
 *
 *  Created on: Dec 18, 2015
 *      Author: l3nko
 */

#ifndef PKA_H_
#define PKA_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define PKA_STATUS_SUCCESS             0 // Success
#define PKA_STATUS_FAILURE             1 // Failure
#define PKA_STATUS_INVALID_PARAM       2 // Invalid parameter
#define PKA_STATUS_BUF_UNDERFLOW       3 // Buffer underflow
#define PKA_STATUS_RESULT_0            4 // Result is all zeros
#define PKA_STATUS_A_GR_B              5 // Big number compare return status if
                                         // the first big num is greater than
                                         // the second.
#define PKA_STATUS_A_LT_B              6 // Big number compare return status if
                                         // the first big num is less than the
                                         // second.
#define PKA_STATUS_OPERATION_INPRG     7 // PKA operation is in progress.
#define PKA_STATUS_OPERATION_NOT_INPRG 8 // No PKA operation is in progress.


typedef struct
{
  uint32_t* pui32X;	// Pointer to value of the x co-ordinate of the ec point.
  uint32_t* pui32Y;	// Pointer to value of the y co-ordinate of the ec point.
} ecc_point_t;

typedef struct
{
	const char*       name;				// Name of the curve.
	const uint8_t     ui8Size;			// Size of the curve in 32-bit word.
	const uint32_t*   pui32Prime;		// The prime that defines the field of the curve.
	const uint32_t*   pui32N;			// Order of the curve.
	const uint32_t*   pui32A;			// Co-efficient a of the equation.
	const uint32_t*   pui32B;			// co-efficient b of the equation.
	const uint32_t*   pui32Gx;			// x co-ordinate value of the generator point.
	const uint32_t*   pui32Gy;			// y co-ordinate value of the generator point.
} ecc_curve_info_t;


//uint8_t pka_get_op_status(void);

/***** BIG NUM ******/
uint8_t pka_bignum_mod_start(uint32_t* pui32BNum, uint8_t ui8BNSize,
                  uint32_t* pui32Modulus, uint8_t ui8ModSize,
                  uint32_t* pui32ResultVector);
uint8_t pka_bignum_mod_result(uint32_t* pui32ResultBuf,uint8_t ui8Size,
                      uint32_t ui32ResVectorLoc);

uint8_t pka_bignum_cmp_start(uint32_t* pui32BNum1, uint32_t* pui32BNum2, uint8_t ui8Size);
uint8_t pka_bignum_cmp_result(void);

uint8_t pka_bignum_invMod_start(uint32_t* pui32BNum, uint8_t ui8BNSize,
                     uint32_t* pui32Modulus, uint8_t ui8Size,
                     uint32_t* pui32ResultVector);
uint8_t pka_bignum_invMod_result(uint32_t* pui32ResultBuf, uint8_t ui8Size,
                         uint32_t ui32ResVectorLoc);

uint8_t pka_bignum_numMul_start(uint32_t* pui32Xplicand, uint8_t ui8XplicandSize,
                       uint32_t* pui32Xplier, uint8_t ui8XplierSize,
                       uint32_t* pui32ResultVector);
uint8_t pka_bignum_numMul_result(uint32_t* pui32ResultBuf, uint32_t* pui32Len,
                       uint32_t ui32ResVectorLoc);

uint8_t pka_bignum_numAdd_start(uint32_t* pui32BN1, uint8_t ui8BN1Size,
                  uint32_t* pui32BN2, uint8_t ui8BN2Size,
                  uint32_t* pui32ResultVector);
uint8_t pka_bignum_numAdd_result(uint32_t* pui32ResultBuf, uint32_t* pui32Len,
                      uint32_t ui32ResVectorLoc);


/***** ECC ******/
uint8_t pka_ecc_mul_start(uint32_t* pui32Scalar, ecc_point_t* ptEcPt,
				ecc_curve_info_t* ptCurve, uint32_t* pui32ResultVector);
uint8_t pka_ecc_mul_result(ecc_point_t* ptOutEcPt, uint32_t ui32ResVectorLoc);

uint8_t pka_ecc_mulGen_start(uint32_t* pui32Scalar, ecc_curve_info_t* ptCurve,
                     uint32_t* pui32ResultVector);
uint8_t pka_ecc_mulGen_result(ecc_point_t* ptOutEcPt, uint32_t ui32ResVectorLoc);

uint8_t pka_ecc_add_start(ecc_point_t* ptEcPt1, ecc_point_t* ptEcPt2, ecc_curve_info_t* ptCurve,
               uint32_t* pui32ResultVector);
uint8_t pka_ecc_add_result(ecc_point_t* ptOutEcPt, uint32_t ui32ResVectorLoc);


#ifdef __cplusplus
}
#endif

#endif /* PKA_H_ */
