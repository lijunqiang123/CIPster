/*******************************************************************************
 * Copyright (c) 2009, Rockwell Automation, Inc.
 * All rights reserved.
 *
 ******************************************************************************/
#ifndef OPENER_ENCAP_H_
#define OPENER_ENCAP_H_

#include "typedefs.h"

/** @file encap.h
 * @brief This file contains the public interface of the encapsulation layer
 */

/**  @defgroup ENCAP CIPster Ethernet encapsulation layer
 * The Ethernet encapsulation layer handles provides the abstraction between the Ethernet and the CIP layer.
 */

//** defines **

#define ENCAPSULATION_HEADER_LENGTH 24

//* @brief Ethernet/IP standard port
static const int kOpenerEthernetPort = 0xAF12;

/** @brief definition of status codes in encapsulation protocol
 * All other codes are either legacy codes, or reserved for future use
 *
 */
enum EncapsulationProtocolErrorCode
{
    kEncapsulationProtocolSuccess = 0x0000,
    kEncapsulationProtocolInvalidCommand = 0x0001,
    kEncapsulationProtocolInsufficientMemory = 0x0002,
    kEncapsulationProtocolIncorrectData = 0x0003,
    kEncapsulationProtocolInvalidSessionHandle = 0x0064,
    kEncapsulationProtocolInvalidLength = 0x0065,
    kEncapsulationProtocolUnsupportedProtocol = 0x0069
};


//** structs **
struct  EncapsulationData
{
    CipUint     command_code;
    CipUint     data_length;
    CipUdint    session_handle;
    CipUdint    status;
    CipOctet    sender_context[8];      ///< length of 8, according to the specification
    CipUdint    options;
    EipUint8*   buf_start;              ///< Pointer to the communication buffer used for this message
    EipUint8*   buf_pos;                ///< The current position in the communication buffer during the decoding process
};


struct  EncapsulationInterfaceInformation
{
    EipUint16   type_code;
    EipUint16   length;
    EipUint16   encapsulation_protocol_version;
    EipUint16   capability_flags;
    EipInt8     name_of_service[16];
};

//** global variables (public) **

//** public functions **
/** @ingroup ENCAP
 * @brief Initialize the encapsulation layer.
 */
void EncapsulationInit();

/** @ingroup ENCAP
 * @brief Shutdown the encapsulation layer.
 *
 * This means that all open sessions including their sockets are closed.
 */
void EncapsulationShutDown();

/** @ingroup ENCAP
 * @brief Handle delayed encapsulation message responses
 *
 * Certain encapsulation message requests require a delayed sending of the response
 * message. This functions checks if messages need to be sent and performs the
 * sending.
 */
void ManageEncapsulationMessages();

#endif // OPENER_ENCAP_H_
