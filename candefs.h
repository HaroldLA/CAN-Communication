//                  _,;;.,_
//         ,-;;,_  ;,',,,'''@
//      ,;;``  `'\\|//``-:;,.
//    @`         ;^^^;      `'@
//               :@ @:
//               \ u /
//      ,=,------)^^^(------,=,
//      '-'-----/=====\-----'-'
//              \_____/
//              /_____\
//          `\ \\_____/_
//            \//_____\/|
//            |        ||
//            |CAN-NET |'
//            :________:`
//
//	Beware the canner !!

/************************************************************************/
/*      S Y M B O L   D E F I N I T I O N S                             */
/************************************************************************/

#define TRUE            1
#define FALSE           0
#define REMOTE_FRAME	1
#define DATA_FRAME		0
 
/*......................................................................*/
/*      V A L U E S   F O R   C A N   C O N T R O L L E R               */
/*......................................................................*/
/*		Les VARIABLES en MAJUSCULES correspondent ? des bits REELS		*/
/*		Les VARIABLES en MINUSCULES sont propre au programme			*/
/************************************************************************/

#define CONTROL         0x00            /* select control register         */
#define AUTO_ACC_CODE   0x24            /* select acceptance code register */
#define AUTO_TX_BUFFER  0x2A            /* select tx_buffer with auto_inc. */
#define AUTO_RX_BUFFER  0x34            /* select rx_buffer with auto_inc. */
#define DMA_EN			0x80			/* enable dma transfert */

#define TRANSMIT_REQUEST 0x01           /* command: start CAN transmission */
#define TRANSMIT_ABORT  0x02            /* command: abort CAN transmission */
#define OVERRUN_CLEAR   0x08            /* command: reset overun condition */
#define RELEASE_RX_BUF  0x04            /* command: release receive buffer */
#define RESET_REQUEST   0x01            /* command: force CAN contr in RST */

/*......................................................................*/
/*      C O N F I G U R A T I O N   V A L U E S   F O R   C A N         */
/*......................................................................*/

#define ACCEPT_CODE     0x62			/* address (62h -> ? )				*/
#define ACCEPT_MASK     0xFF           	/* mask acceptance of  messages		*/
#define BUS_TIM_0       0x00            /* 1 Mbit/s	(1C pour 500)			*/
#define BUS_TIM_1       0x14            /* last 23                          */
#define OUT_CONTROL     0xFA            /* value for CAN output control		*/
#define DIFFERENTIAL    0xC0            /* enable RX0, RX1 CAN inputs		*/
#define CAN_INT_ENABLE  0x1E            /* enable CAN rx, tx, error int.	*/


/************************************************************************/
/*      S P E C I A L   F U N C T I O N   R E G I S T E R   B I T S     */
/************************************************************************/

sbit    CAN_BUS_STATUS  = 0xDF;         /* CAN: bus_on / bus_off status */
sbit    CAN_ERROR_STAT  = 0xDE;         /* CAN: error warning status    */
sbit    CAN_TX_COMPLETE = 0xDB;         /* CAN: transmit complete status*/
sbit    CAN_TX_BUFFER   = 0xDA;         /* CAN: tx buffer accessable    */
