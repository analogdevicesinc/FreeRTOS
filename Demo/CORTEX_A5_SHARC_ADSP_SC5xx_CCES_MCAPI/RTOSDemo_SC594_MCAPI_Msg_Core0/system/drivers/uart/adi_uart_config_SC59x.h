/******************************************************************************
 * @file:    adi_uart_config_SC59x.h
 * @brief:   UART driver static configuration
 * @version: $Revision: 62861 $
 * @date:    $Date: 2019-11-27 09:36:45 -0500 (Wed, 27 Nov 2019) $
 *****************************************************************************

 Copyright (c) 2019-2020 Analog Devices.  All Rights Reserved.

 This software is proprietary and confidential.  By using this software you agree
 to the terms of the associated Analog Devices License Agreement.
 
*******************************************************************************/

/**
 * @file      adi_uart_config_SC59x.h
 *
 * @brief     UART driver static configuration
 *
 * @details   Header File which has the static configuration for UART instances.
 *
 */

/** @addtogroup UART_Driver UART Device Driver
 *  @{
 */

/** @addtogroup UART_Driver_Static_Configuration UART Device Driver Static Configuration
 *  @{

  Static Configuration: Using this configuration application can configure the control register
  parameters upfront which can be set to default before UART data transfer starts. This configuration 
  has macros which can be configured through adi_uart_config.h file which by default is available
  with UART driver. Example project can maintain a copy of it locally for custom configuration.

  Default Static Configuration for UART(0/1/2): 
  The default configuration for UART:
        UART baud rate : 9600 bps
        word length    : 8 bits
        stop bits      : 1 bit
        parity         : None

  Specifies the UART(X)_CLK register Divisor\n
    ADI_UART(X)_CLOCK_DIVISOR                 (0x32Eul)  --baud rate - 9600 bps \n
    ADI_UART(X)_CLOCK_EDBO                    (0x0ul)------power on reset value \n

  Specifies the UART(X)_Control register Configuration\n
    ADI_UART(X)_CTL_WORD_LENGTH               (0x3ul)------word length - 8 bits\n
    ADI_UART(X)_CTL_HALF_STOP_BIT             (0x0ul)------1 stop bit \n
    ADI_UART(X)_CTL_ENABLE_PARITY             (0x0ul)------parity set to none \n
    ADI_UART(X)_CTL_PARITY_SELECTION          (0x0ul)------power on reset value \n
    ADI_UART(X)_CTL_STOP_BIT                  (0x0ul)------power on reset value \n
    ADI_UART(X)_CTL_MODE_ENABLE               (0x0ul)------power on reset value \n
    ADI_UART(X)_CTL_STICKY_PARITY             (0x0ul)------power on reset value \n
    ADI_UART(X)_CTL_SET_BREAK                 (0x0ul)------power on reset value \n
    ADI_UART(X)_CTL_FLOW_CONTROL_POLARITY     (0x0ul)------power on reset value \n
    ADI_UART(X)_CTL_MANUAL_SEND_REQ           (0x0ul)------power on reset value \n
    ADI_UART(X)_CTL_TRANSMITTER_OFF           (0x0ul)------power on reset value \n
    ADI_UART(X)_CTL_AUTOMATIC_CTS             (0x0ul)------power on reset value \n
    ADI_UART(X)_CTL_AUTOMATIC_RTS             (0x0ul)------power on reset value \n
    ADI_UART(X)_CTL_RxFIFO_IRQ_THRESHOLD      (0x0ul)------power on reset value \n
    ADI_UART(X)_CTL_RxFIFO_RTS_THRESHOLD      (0x0ul)------power on reset value \n
 */

#ifndef __ADI_UART_CONFIG_SC59x_H__
#define __ADI_UART_CONFIG_SC59x_H__

#include <sys/platform.h>
#include <stdint.h>

/**************************************************************************************************************/
/*! Specifies the number of active UART devices accordingly the memory allotted will be optimized */
#if !defined(__ADSP21593__) && !defined(__ADSP21591__) && !defined(__ADSP21593W__) && !defined(__ADSP21591W__)
#define ADI_UART_NUM_DEVICES                   4ul
#else
#define ADI_UART_NUM_DEVICES                   3ul
#endif
/**************************************************************************************************************/

/**************************************************************************************************************/
/*! select/unselect UART0 Instance */
#define ADI_UART0_INSTANCE                     1ul

/*! select/unselect UART1 Instance */
#define ADI_UART1_INSTANCE                     1ul

/*! select/unselect UART2 Instance */
#define ADI_UART2_INSTANCE                     1ul

#if !defined(__ADSP21593__) && !defined(__ADSP21591__) && !defined(__ADSP21593W__) && !defined(__ADSP21591W__)
/*! select/unselect UART3 Instance */
#define ADI_UART3_INSTANCE                     1ul
#endif
/**************************************************************************************************************/

/**************************************************************************************************************/
/*! Specifies the UART0_CLK Divisor value */
#define ADI_UART0_CLOCK_DIVISOR                (0x32Eul)

/*! Specifies the UART0_CLK Prescaler value */
#define ADI_UART0_CLOCK_EDBO                   (0x0ul)

/* Sets the UART0 Clock value which can be derived from  UART BaudRate = SCLK/(16pow(1-EDBO) * UART_CLK.DIV) */
#define ADI_UART0_CLOCK_VALUE                  ((ADI_UART0_CLOCK_DIVISOR<<BITP_UART_CLK_DIV)|\
                                                (ADI_UART0_CLOCK_EDBO<<BITP_UART_CLK_EDBO))
/**************************************************************************************************************/

/**************************************************************************************************************/
/*!
   Word length Select\n
   (0) -  5 Bits word length\n
   (1) -  6 Bits word length\n
   (2) -  7 Bits word length\n
   (3) -  8 Bits word length\n
*/
#define ADI_UART0_CTL_WORD_LENGTH              (0x3ul)

/*!
   Half stop bit selection\n
   (0) -  Send no half stop bit\n
   (1) -  Send one half stop bit\n
   Only for word length with 5 bits, 1.5 Stop bits can be used\n
*/
#define ADI_UART0_CTL_HALF_STOP_BIT            (0x0ul)

/*!
   Parity Enable is used to control of the parity bit\n
   (0) -  Parity will not be transmitted or checked\n
   (1) -  Parity will be transmitted and checked\n
*/
#define ADI_UART0_CTL_ENABLE_PARITY            (0x0ul)

/*!
   Parity Select is required only if parity is enabled\n
   (0) -  Odd parity will be transmitted and checked\n
   (1) -  Even parity will be transmitted and checked\n
*/
#define ADI_UART0_CTL_PARITY_SELECTION         (0x0ul)

/*!
   Stop bit selection\n
   (0) -  Send 1 stop bit\n
   (1) -  Send 2 stop bits\n
*/
#define ADI_UART0_CTL_STOP_BIT                 (0x0ul)

/*!
   UART mode of operation\n
   (0) -  UART mode\n
   (1) -  MDB mode\n
*/
#define ADI_UART0_CTL_MODE_ENABLE              (0x0ul)

/*!
   Stick Parity is Used to force parity to defined values\n
   (0) -  Hardware calculates the parity based on data bits\n
   (1) -  Force parity to defined value\n
*/
#define ADI_UART0_CTL_STICKY_PARITY            (0x0ul)

/*!
   Set Break enables UART_Tx pin to be used as a flag\n
   (0) -  UART_Tx pin not forced\n
   (1) -  UART_Tx pin is forced to zero\n
*/
#define ADI_UART0_CTL_SET_BREAK                (0x0ul)

/*!
   Flow control pin polarity\n
   (0) -  Active low CTS/RTS\n
   (1) -  Active High CTS/RTS\n
*/
#define ADI_UART0_CTL_FLOW_CONTROL_POLARITY    (0x0ul)

/*!
   Manual request to send\n
   (0) -  UART deasserts signaling, not ready to receive\n
   (1) -  UART asserts signaling, ready to receive\n
*/
#define ADI_UART0_CTL_MANUAL_SEND_REQ          (0x0ul)

/*!
   Transmitter off  prevents the data from going to TSR(shift register) from THR(Tx FIFO)\n
   (0) -  Transmission is ON if Automatic CTS is zero\n
   (1) -  Transmission is OFF if Automatic CTS is zero\n
*/
#define ADI_UART0_CTL_TRANSMITTER_OFF          (0x0ul)

/*!
   Automatic CTS uses Hardware to control the handshaking for Tx\n
   (0) -  Disable automatic Tx handshaking protocol\n
   (1) -  Enable automatic Tx handshaking protocol\n
*/
#define ADI_UART0_CTL_AUTOMATIC_CTS            (0x0ul)

/*!
   Automatic RTS makes use of Hardware such that UART_RTS pin is controlled with minimal latency\n
   (0) -  Disable automatic Rx handshaking protocol\n
   (1) -  Enable automatic Rx handshaking protocol\n
*/
#define ADI_UART0_CTL_AUTOMATIC_RTS            (0x0ul)

/*!
   Receive FIFO IRQ Threshold\n
   (0) -  Sets receive FIFO IRQ threshold for count >= 4 bytes\n
   (1) -  Sets receive FIFO IRQ threshold for count >= 7 bytes\n
*/
#define ADI_UART0_CTL_RxFIFO_IRQ_THRESHOLD     (0x0ul)

/*!
   Receive FIFO RTS Threshold\n
   (0) -  Deassert RTS if Rx FIFO count > 4 and assert if count <= 4\n
   (1) -  Deassert RTS if Rx FIFO count > 7 and assert if count <= 7\n
*/
#define ADI_UART0_CTL_RxFIFO_RTS_THRESHOLD     (0x0ul)

/* Specifies the UART0 Control register Configuration*/
#define ADI_UART0_CTL                          ((ADI_UART0_CTL_WORD_LENGTH<<BITP_UART_CTL_WLS)|\
                                                (ADI_UART0_CTL_HALF_STOP_BIT<<BITP_UART_CTL_STBH)|\
                                                (ADI_UART0_CTL_ENABLE_PARITY<<BITP_UART_CTL_PEN)|\
                                                (ADI_UART0_CTL_PARITY_SELECTION<<BITP_UART_CTL_EPS)|\
                                                (ADI_UART0_CTL_STOP_BIT<<BITP_UART_CTL_STB)|\
                                                (ADI_UART0_CTL_MODE_ENABLE<<BITP_UART_CTL_MOD)|\
                                                (ADI_UART0_CTL_STICKY_PARITY<<BITP_UART_CTL_STP)|\
                                                (ADI_UART0_CTL_SET_BREAK<<BITP_UART_CTL_SB) |\
                                                (ADI_UART0_CTL_FLOW_CONTROL_POLARITY<<BITP_UART_CTL_FCPOL)|\
                                                (ADI_UART0_CTL_MANUAL_SEND_REQ<<BITP_UART_CTL_MRTS)|\
                                                (ADI_UART0_CTL_TRANSMITTER_OFF<<BITP_UART_CTL_XOFF)|\
                                                (ADI_UART0_CTL_AUTOMATIC_CTS<<BITP_UART_CTL_ACTS) |\
                                                (ADI_UART0_CTL_AUTOMATIC_RTS<<BITP_UART_CTL_ARTS)|\
                                                (ADI_UART0_CTL_RxFIFO_IRQ_THRESHOLD<<BITP_UART_CTL_RFIT)|\
                                                (ADI_UART0_CTL_RxFIFO_RTS_THRESHOLD<<BITP_UART_CTL_RFRT))
/************************************************************************************************************/

/************************************************************************************************************/
/*! Specifies the UART1_CLK Divisor value */
#define ADI_UART1_CLOCK_DIVISOR                 (0x32Eul)

/*! Specifies the UART1_CLK Prescaler value */
#define ADI_UART1_CLOCK_EDBO                    (0x0ul)

/* Sets the UART1 Clock value which can be derived from  UART BaudRate = SCLK/(16pow(1-EDBO) * UART_CLK.DIV) */
#define ADI_UART1_CLOCK_VALUE                   ((ADI_UART1_CLOCK_DIVISOR<<BITP_UART_CLK_DIV)|\
                                                 (ADI_UART1_CLOCK_EDBO<<BITP_UART_CLK_EDBO))
/************************************************************************************************************/

/************************************************************************************************************/
/*!
   Word length Select\n
   (0) -  5 Bits word length\n
   (1) -  6 Bits word length\n
   (2) -  7 Bits word length\n
   (3) -  8 Bits word length\n
*/
#define ADI_UART1_CTL_WORD_LENGTH               (0x3ul)

/*!
   Half stop bit selection\n
   (0) -  Send no half stop bit\n
   (1) -  Send one half stop bit\n
   Only for word length with 5 bits, 1.5 Stop bits can be used\n
*/
#define ADI_UART1_CTL_HALF_STOP_BIT             (0x0ul)

/*!
   Parity Enable is used to control of the parity bit\n
   (0) -  Parity will not be transmitted or checked\n
   (1) -  Parity will be transmitted and checked\n
*/
#define ADI_UART1_CTL_ENABLE_PARITY             (0x0ul)

/*!
   Parity Select is required only if parity is enabled\n
   (0) -  Odd parity will be transmitted and checked\n
   (1) -  Even parity will be transmitted and checked\n
*/
#define ADI_UART1_CTL_PARITY_SELECTION          (0x0ul)

/*!
   Stop bit selection\n
   (0) -  Send 1 stop bit\n
   (1) -  Send 2 stop bits\n
*/
#define ADI_UART1_CTL_STOP_BIT                  (0x0ul)

/*!
   UART mode of operation\n
   (0) -  UART mode\n
   (1) -  MDB mode\n
*/
#define ADI_UART1_CTL_MODE_ENABLE               (0x0ul)

/*!
   Stick Parity is Used to force parity to defined values\n
   (0) -  Hardware calculates the parity based on data bits\n
   (1) -  Force parity to defined value\n
*/
#define ADI_UART1_CTL_STICKY_PARITY             (0x0ul)

/*!
   Set Break enables UART_Tx pin to be used as a flag\n
   (0) -  UART_Tx pin not forced\n
   (1) -  UART_Tx pin is forced to zero\n
*/
#define ADI_UART1_CTL_SET_BREAK                 (0x0ul)

/*!
   Flow control pin polarity\n
   (0) -  Active low CTS/RTS\n
   (1) -  Active High CTS/RTS\n
*/
#define ADI_UART1_CTL_FLOW_CONTROL_POLARITY     (0x0ul)

/*!
   Manual request to send\n
   (0) -  UART deasserts signaling, not ready to receive\n
   (1) -  UART asserts signaling, ready to receive\n
*/
#define ADI_UART1_CTL_MANUAL_SEND_REQ           (0x0ul)

/*!
   Transmitter off  prevents the data from going to TSR(shift register) from THR(Tx FIFO)\n
   (0) -  Transmission is ON if Automatic CTS is zero\n
   (1) -  Transmission is OFF if Automatic CTS is zero\n
*/
#define ADI_UART1_CTL_TRANSMITTER_OFF           (0x0ul)

/*!
   Automatic CTS uses Hardware to control the handshaking for Tx\n
   (0) -  Disable automatic Tx handshaking protocol\n
   (1) -  Enable automatic Tx handshaking protocol\n
*/
#define ADI_UART1_CTL_AUTOMATIC_CTS             (0x0ul)

/*!
   Automatic RTS makes use of Hardware such that UART_RTS pin is controlled with minimal latency\n
   (0) -  Disable automatic Rx handshaking protocol\n
   (1) -  Enable automatic Rx handshaking protocol\n
*/
#define ADI_UART1_CTL_AUTOMATIC_RTS             (0x0ul)

/*!
   Receive FIFO IRQ Threshold.\n
   (0) -  Sets receive FIFO IRQ threshold for count >= 4 bytes.\n
   (1) -  Sets receive FIFO IRQ threshold for count >= 7 bytes.\n
*/
#define ADI_UART1_CTL_RxFIFO_IRQ_THRESHOLD      (0x0ul)

/*!
   Receive FIFO RTS Threshold.\n
   (0) -  Deassert RTS if Rx FIFO count > 4 and assert if count <= 4.\n
   (1) -  Deassert RTS if Rx FIFO count > 7 and assert if count <= 7.\n
*/
#define ADI_UART1_CTL_RxFIFO_RTS_THRESHOLD      (0x0ul)

/* Specifies the UART1 Control register Configuration*/
#define ADI_UART1_CTL                           ((ADI_UART1_CTL_WORD_LENGTH<<BITP_UART_CTL_WLS)|\
                                                 (ADI_UART1_CTL_HALF_STOP_BIT<<BITP_UART_CTL_STBH)|\
                                                 (ADI_UART1_CTL_ENABLE_PARITY<<BITP_UART_CTL_PEN)|\
                                                 (ADI_UART1_CTL_PARITY_SELECTION<<BITP_UART_CTL_EPS)|\
                                                 (ADI_UART1_CTL_STOP_BIT<<BITP_UART_CTL_STB)|\
                                                 (ADI_UART1_CTL_MODE_ENABLE<<BITP_UART_CTL_MOD)|\
                                                 (ADI_UART1_CTL_STICKY_PARITY<<BITP_UART_CTL_STP)|\
                                                 (ADI_UART1_CTL_SET_BREAK<<BITP_UART_CTL_SB)|\
                                                 (ADI_UART1_CTL_FLOW_CONTROL_POLARITY<<BITP_UART_CTL_FCPOL)|\
                                                 (ADI_UART1_CTL_MANUAL_SEND_REQ<<BITP_UART_CTL_MRTS)|\
                                                 (ADI_UART1_CTL_TRANSMITTER_OFF<<BITP_UART_CTL_XOFF)|\
                                                 (ADI_UART1_CTL_AUTOMATIC_CTS<<BITP_UART_CTL_ACTS)|\
                                                 (ADI_UART1_CTL_AUTOMATIC_RTS<<BITP_UART_CTL_ARTS)|\
                                                 (ADI_UART1_CTL_RxFIFO_IRQ_THRESHOLD<<BITP_UART_CTL_RFIT)|\
                                                 (ADI_UART1_CTL_RxFIFO_RTS_THRESHOLD<<BITP_UART_CTL_RFRT))

/************************************************************************************************************/

/**************************************************************************************************************/
/*! Specifies the UART2_CLK Divisor value */
#define ADI_UART2_CLOCK_DIVISOR                (0x32Eul)

/*! Specifies the UART2_CLK Prescaler value */
#define ADI_UART2_CLOCK_EDBO                   (0x0ul)

/* Sets the UART2 Clock value which can be derived from  UART BaudRate = SCLK/(16pow(1-EDBO) * UART_CLK.DIV) */
#define ADI_UART2_CLOCK_VALUE                  ((ADI_UART2_CLOCK_DIVISOR<<BITP_UART_CLK_DIV)|\
                                                (ADI_UART2_CLOCK_EDBO<<BITP_UART_CLK_EDBO))
/**************************************************************************************************************/

/**************************************************************************************************************/
/*!
   Word length Select\n
   (0) -  5 Bits word length\n
   (1) -  6 Bits word length\n
   (2) -  7 Bits word length\n
   (3) -  8 Bits word length\n
*/
#define ADI_UART2_CTL_WORD_LENGTH              (0x3ul)

/*!
   Half stop bit selection\n
   (0) -  Send no half stop bit\n
   (1) -  Send one half stop bit\n
   Only for word length with 5 bits, 1.5 Stop bits can be used\n
*/
#define ADI_UART2_CTL_HALF_STOP_BIT            (0x0ul)

/*!
   Parity Enable is used to control of the parity bit\n
   (0) -  Parity will not be transmitted or checked\n
   (1) -  Parity will be transmitted and checked\n
*/
#define ADI_UART2_CTL_ENABLE_PARITY            (0x0ul)

/*!
   Parity Select is required only if parity is enabled\n
   (0) -  Odd parity will be transmitted and checked\n
   (1) -  Even parity will be transmitted and checked\n
*/
#define ADI_UART2_CTL_PARITY_SELECTION         (0x0ul)

/*!
   Stop bit selection\n
   (0) -  Send 1 stop bit\n
   (1) -  Send 2 stop bits\n
*/
#define ADI_UART2_CTL_STOP_BIT                 (0x0ul)

/*!
   UART mode of operation\n
   (0) -  UART mode\n
   (1) -  MDB mode\n
*/
#define ADI_UART2_CTL_MODE_ENABLE              (0x0ul)

/*!
   Stick Parity is Used to force parity to defined values\n
   (0) -  Hardware calculates the parity based on data bits\n
   (1) -  Force parity to defined value\n
*/
#define ADI_UART2_CTL_STICKY_PARITY            (0x0ul)

/*!
   Set Break enables UART_Tx pin to be used as a flag\n
   (0) -  UART_Tx pin not forced\n
   (1) -  UART_Tx pin is forced to zero\n
*/
#define ADI_UART2_CTL_SET_BREAK                (0x0ul)

/*!
   Flow control pin polarity\n
   (0) -  Active low CTS/RTS\n
   (1) -  Active High CTS/RTS\n
*/
#define ADI_UART2_CTL_FLOW_CONTROL_POLARITY    (0x0ul)

/*!
   Manual request to send\n
   (0) -  UART deasserts signaling, not ready to receive\n
   (1) -  UART asserts signaling, ready to receive\n
*/
#define ADI_UART2_CTL_MANUAL_SEND_REQ          (0x0ul)

/*!
   Transmitter off  prevents the data from going to TSR(shift register) from THR(Tx FIFO)\n
   (0) -  Transmission is ON if Automatic CTS is zero\n
   (1) -  Transmission is OFF if Automatic CTS is zero\n
*/
#define ADI_UART2_CTL_TRANSMITTER_OFF          (0x0ul)

/*!
   Automatic CTS uses Hardware to control the handshaking for Tx\n
   (0) -  Disable automatic Tx handshaking protocol\n
   (1) -  Enable automatic Tx handshaking protocol\n
*/
#define ADI_UART2_CTL_AUTOMATIC_CTS            (0x0ul)

/*!
   Automatic RTS makes use of Hardware such that UART_RTS pin is controlled with minimal latency\n
   (0) -  Disable automatic Rx handshaking protocol\n
   (1) -  Enable automatic Rx handshaking protocol\n
*/
#define ADI_UART2_CTL_AUTOMATIC_RTS            (0x0ul)

/*!
   Receive FIFO IRQ Threshold\n
   (0) -  Sets receive FIFO IRQ threshold for count >= 4 bytes\n
   (1) -  Sets receive FIFO IRQ threshold for count >= 7 bytes\n
*/
#define ADI_UART2_CTL_RxFIFO_IRQ_THRESHOLD     (0x0ul)

/*!
   Receive FIFO RTS Threshold\n
   (0) -  Deassert RTS if Rx FIFO count > 4 and assert if count <= 4\n
   (1) -  Deassert RTS if Rx FIFO count > 7 and assert if count <= 7\n
*/
#define ADI_UART2_CTL_RxFIFO_RTS_THRESHOLD     (0x0ul)

/* Specifies the UART2 Control register Configuration*/
#define ADI_UART2_CTL                          ((ADI_UART2_CTL_WORD_LENGTH<<BITP_UART_CTL_WLS)|\
                                                (ADI_UART2_CTL_HALF_STOP_BIT<<BITP_UART_CTL_STBH)|\
                                                (ADI_UART2_CTL_ENABLE_PARITY<<BITP_UART_CTL_PEN)|\
                                                (ADI_UART2_CTL_PARITY_SELECTION<<BITP_UART_CTL_EPS)|\
                                                (ADI_UART2_CTL_STOP_BIT<<BITP_UART_CTL_STB)|\
                                                (ADI_UART2_CTL_MODE_ENABLE<<BITP_UART_CTL_MOD)|\
                                                (ADI_UART2_CTL_STICKY_PARITY<<BITP_UART_CTL_STP)|\
                                                (ADI_UART2_CTL_SET_BREAK<<BITP_UART_CTL_SB) |\
                                                (ADI_UART2_CTL_FLOW_CONTROL_POLARITY<<BITP_UART_CTL_FCPOL)|\
                                                (ADI_UART2_CTL_MANUAL_SEND_REQ<<BITP_UART_CTL_MRTS)|\
                                                (ADI_UART2_CTL_TRANSMITTER_OFF<<BITP_UART_CTL_XOFF)|\
                                                (ADI_UART2_CTL_AUTOMATIC_CTS<<BITP_UART_CTL_ACTS) |\
                                                (ADI_UART2_CTL_AUTOMATIC_RTS<<BITP_UART_CTL_ARTS)|\
                                                (ADI_UART2_CTL_RxFIFO_IRQ_THRESHOLD<<BITP_UART_CTL_RFIT)|\
                                                (ADI_UART2_CTL_RxFIFO_RTS_THRESHOLD<<BITP_UART_CTL_RFRT))
/************************************************************************************************************/


#if !defined(__ADSP21593__) && !defined(__ADSP21591__) && !defined(__ADSP21593W__) && !defined(__ADSP21591W__)
/************************************************************************************************************/
/*! Specifies the UART3_CLK Divisor value */
#define ADI_UART3_CLOCK_DIVISOR                 (0x32Eul)

/*! Specifies the UART3_CLK Prescaler value */
#define ADI_UART3_CLOCK_EDBO                    (0x0ul)

/* Sets the UART3 Clock value which can be derived from  UART BaudRate = SCLK/(16pow(1-EDBO) * UART_CLK.DIV) */
#define ADI_UART3_CLOCK_VALUE                   ((ADI_UART3_CLOCK_DIVISOR<<BITP_UART_CLK_DIV)|\
                                                 (ADI_UART3_CLOCK_EDBO<<BITP_UART_CLK_EDBO))
/************************************************************************************************************/

/************************************************************************************************************/
/*!
   Word length Select\n
   (0) -  5 Bits word length\n
   (1) -  6 Bits word length\n
   (2) -  7 Bits word length\n
   (3) -  8 Bits word length\n
*/
#define ADI_UART3_CTL_WORD_LENGTH               (0x3ul)

/*!
   Parity Enable is used to control of the parity bit\n
   (0) -  Parity will not be transmitted or checked\n
   (1) -  Parity will be transmitted and checked\n
*/
#define ADI_UART3_CTL_HALF_STOP_BIT             (0x0ul)

/*!
   Parity Enable is used to control of the parity bit\n
   (0) -  Parity will not be transmitted or checked\n
   (1) -  Parity will be transmitted and checked\n
*/
#define ADI_UART3_CTL_ENABLE_PARITY             (0x0ul)

/*!
   Parity Select is required only if parity is enabled\n
   (0) -  Odd parity will be transmitted and checked\n
   (1) -  Even parity will be transmitted and checked\n
*/
#define ADI_UART3_CTL_PARITY_SELECTION          (0x0ul)

/*!
   Stop bit selection\n
   (0) -  Send 1 stop bit\n
   (1) -  Send 2 stop bits\n
*/
#define ADI_UART3_CTL_STOP_BIT                  (0x0ul)

/*!
   UART mode of operation\n
   (0) -  UART mode\n
   (1) -  MDB mode\n
*/
#define ADI_UART3_CTL_MODE_ENABLE               (0x0ul)

/*!
   Stick Parity is Used to force parity to defined values\n
   (0) -  Hardware calculates the parity based on data bits\n
   (1) -  Force parity to defined value\n
*/
#define ADI_UART3_CTL_STICKY_PARITY             (0x0ul)

/*!
   Set Break enables UART_Tx pin to be used as a flag\n
   (0) -  UART_Tx pin not forced\n
   (1) -  UART_Tx pin is forced to zero\n
*/
#define ADI_UART3_CTL_SET_BREAK                 (0x0ul)

/*!
   Flow control pin polarity\n
   (0) -  Active low CTS/RTS\n
   (1) -  Active High CTS/RTS\n
*/
#define ADI_UART3_CTL_FLOW_CONTROL_POLARITY     (0x0ul)

/*!
   Manual request to send\n
   (0) -  UART deasserts signaling, not ready to receive\n
   (1) -  UART asserts signaling, ready to receive\n
*/
#define ADI_UART3_CTL_MANUAL_SEND_REQ           (0x0ul)

/*!
   Transmitter off  prevents the data from going to TSR(shift register) from THR(Tx FIFO)\n
   (0) -  Transmission is ON if Automatic CTS is zero\n
   (1) -  Transmission is OFF if Automatic CTS is zero\n
*/
#define ADI_UART3_CTL_TRANSMITTER_OFF           (0x0ul)

/*!
   Automatic CTS uses Hardware to control the handshaking for Tx\n
   (0) -  Disable automatic Tx handshaking protocol\n
   (1) -  Enable automatic Tx handshaking protocol\n
*/
#define ADI_UART3_CTL_AUTOMATIC_CTS             (0x0ul)

/*!
   Automatic RTS makes use of Hardware such that UART_RTS pin is controlled with minimal latency\n
   (0) -  Disable automatic Rx handshaking protocol\n
   (1) -  Enable automatic Rx handshaking protocol\n
*/
#define ADI_UART3_CTL_AUTOMATIC_RTS             (0x0ul)

/*!
   Receive FIFO IRQ Threshold.\n
   (0) -  Sets receive FIFO IRQ threshold for count >= 4 bytes.\n
   (1) -  Sets receive FIFO IRQ threshold for count >= 7 bytes.\n
*/
#define ADI_UART3_CTL_RxFIFO_IRQ_THRESHOLD      (0x0ul)

/*!
   Receive FIFO RTS Threshold.\n
   (0) -  Deassert RTS if Rx FIFO count > 4 and assert if count <= 4.\n
   (1) -  Deassert RTS if Rx FIFO count > 7 and assert if count <= 7.\n
*/
#define ADI_UART3_CTL_RxFIFO_RTS_THRESHOLD      (0x0ul)

/* Specifies the UART3 Control register Configuration*/
#define ADI_UART3_CTL                           ((ADI_UART3_CTL_WORD_LENGTH<<BITP_UART_CTL_WLS)|\
                                                 (ADI_UART3_CTL_HALF_STOP_BIT<<BITP_UART_CTL_STBH)|\
                                                 (ADI_UART3_CTL_ENABLE_PARITY<<BITP_UART_CTL_PEN)|\
                                                 (ADI_UART3_CTL_PARITY_SELECTION<<BITP_UART_CTL_EPS)|\
                                                 (ADI_UART3_CTL_STOP_BIT<<BITP_UART_CTL_STB)|\
                                                 (ADI_UART3_CTL_MODE_ENABLE<<BITP_UART_CTL_MOD)|\
                                                 (ADI_UART3_CTL_STICKY_PARITY<<BITP_UART_CTL_STP)|\
                                                 (ADI_UART3_CTL_SET_BREAK<<BITP_UART_CTL_SB)|\
                                                 (ADI_UART3_CTL_FLOW_CONTROL_POLARITY<<BITP_UART_CTL_FCPOL)|\
                                                 (ADI_UART3_CTL_MANUAL_SEND_REQ<<BITP_UART_CTL_MRTS)|\
                                                 (ADI_UART3_CTL_TRANSMITTER_OFF<<BITP_UART_CTL_XOFF)|\
                                                 (ADI_UART3_CTL_AUTOMATIC_CTS<<BITP_UART_CTL_ACTS)|\
                                                 (ADI_UART3_CTL_AUTOMATIC_RTS<<BITP_UART_CTL_ARTS)|\
                                                 (ADI_UART3_CTL_RxFIFO_IRQ_THRESHOLD<<BITP_UART_CTL_RFIT)|\
                                                 (ADI_UART3_CTL_RxFIFO_RTS_THRESHOLD<<BITP_UART_CTL_RFRT))

/************************************************************************************************************/
#endif /* LQFP parts check */

#endif /* __ADI_UART_CONFIG_SC59x_H__ */

/*@}*/

/*@}*/
