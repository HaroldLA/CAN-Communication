/*----------------------------------------------------------------------*/
/*                                                                      */
/*      Source file:            code1.C                                 */
/*      Package:                basic                                   */
/*      Software version:       1.4                                     */
/*      Date:                   October 2020                            */
/*      Authors:                A. ELOUARDI & S. BOUAZIZ & Ch. JOUVRAY	*/
/*		      based on	Jens-Ulf Pehrs                          		*/
/*            Philips Semiconductors RHW-PCALH/AI			    		*/
/*                                                                      */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*      INCLUDES      D E F I N I T I O N S                             */
/************************************************************************/
#include <reg592.h>
#include <stdio.h>
#include <intrins.h>
#include "candefs.h"
#include "FonctionsBases2.h"
#include "libtp2.h"




/************************************************************************/
/************************************************************************/
/*							FONCTIONS									*/
/************************************************************************/
/************************************************************************/

/************************************************************************/
/*......................................................................*/
/*		Fonctions pour la configuration et l'initialisation de CAN		*/
/*......................................................................*/
/************************************************************************/

/************************************************************************/
/*      procedure:      can_config                                      */
/*      input:          ---                                             */
/*      output:         ---                                             */
/*      calls:          ---                                             */
/************************************************************************/

//La reference de tension n'est pas active ... ? voir !
unsigned char    can_config ()
{   unsigned char retour=0;
    EA = FALSE;
    can_com=0;
    do    {
        CANADR = CONTROL;
        CANDAT = RESET_REQUEST;
    }   while  ( !(CANDAT & RESET_REQUEST) );
    CANADR = AUTO_ACC_CODE;
    CANDAT = ACCEPT_CODE;
    CANDAT = ACCEPT_MASK;
    CANDAT = BUS_TIM_0;
    CANDAT = BUS_TIM_1;
    CANDAT = OUT_CONTROL;
    CANCON = DIFFERENTIAL;
    do    {
        CANADR = CONTROL;
        CANDAT = 0;
    }   while  ( CANDAT & RESET_REQUEST );
    CANDAT = CAN_INT_ENABLE;

    if  ( CAN_ERROR_STAT )    
	        retour=1; //printf("erreur_stat\n");
    PS1 = TRUE;
    ES1 = TRUE;
    EA  = TRUE;
	return retour;
}

/************************************************************************/
/*      procedure:      can_contr_init                                  */
/*      input:          ---                                             */
/*      output:         ---                                             */
/*      calls:          led_init, can_config, slio_init                 */
/************************************************************************/

void can_contr_init()
{
    can_config ();
}


/************************************************************************/
/*      procedure:      CANInitialisation                               */
/*      input:          ---                                             */
/*      output:         ---                                             */
/*      calls:          can_contr_init					                */
/************************************************************************/
 
void CANInitialisation()
{
	can_contr_init();
    can_init_request=FALSE;
	int_save = CANCON;
	can_tx_ready=1;
}





/************************************************************************/
/*......................................................................*/
/*			Fonction en cas de probl?me sur le CAN						*/
/*......................................................................*/
/************************************************************************/



/************************************************************************/
/*      procedure:      can_int_error                                   */
/*      input:          ---                                             */
/*      output:         ---                                             */
/*      calls:          ---                                             */
/*----------------------------------------------------------------------*/
/*      running on interrupt level, called from "can_int ()"            */
/************************************************************************/

void    can_int_error ()
{
    if  ( CAN_ERROR_STAT )  {
        can_event       = TRUE;
        if  ( CAN_BUS_STATUS )
            can_system_error = TRUE;
    }
    else  {
        can_event   = FALSE;
    }
}

/************************************************************************/
/*......................................................................*/
/*			Fonctions pour la r?ception de messages sur le CAN			*/
/*......................................................................*/
/************************************************************************/

/************************************************************************/
/*      procedure:      can_buf_full                                    */
/*      input:          ---                                             */
/*      output:         ---                                             */
/*      calls:          ---                                             */
/*----------------------------------------------------------------------*/
/*      running on interrupt level, called from "can_int ()"            */
/*----------------------------------------------------------------------*/
/*                                                                      */
/*                                                                      */
/************************************************************************/

void    can_buf_full ()
{
    
//    CANSTA = (char *)tab_rx;
    CANSTA = (unsigned char)(can_buffrx);
	CANADR = DMA_EN|AUTO_RX_BUFFER;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
    if  ( overrun_int )
        CANCON = RELEASE_RX_BUF | OVERRUN_CLEAR;
    else
        CANCON = RELEASE_RX_BUF;

	can_event   = TRUE;
    can_rx_ready =TRUE;
}

/************************************************************************/
/*......................................................................*/
/*			Fonctions pour l'?mission de messages sur le CAN			*/
/*......................................................................*/
/************************************************************************/

/************************************************************************/
/*      procedure:      send_message                                    */
/*      input:          data_table         				*/
/*      output:         ---                                             */
/*      calls:                                                          */
/************************************************************************/

unsigned char send_message ( unsigned char *tabtx ){

  int_save    = CANCON;
  if  ( CAN_TX_BUFFER )   {
    CANSTA = (char)tabtx;
    CANADR = DMA_EN|AUTO_TX_BUFFER;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
    can_event = TRUE;
    can_tx_ready = FALSE;
    can_tx_sending = TRUE; 
    CANCON = TRANSMIT_REQUEST|DIFFERENTIAL;        		/* start CAN transmit to SLIO */
	return 1;
  }
  else   {                                    			/* error exit, if transmit    */

  //ATTENTION ne pas faire ca car cela risque de planter
  // il faut rien faire dans ce cas et pr?venir qu'il n'y  pas 
  //d'?mission...... et emettre ? nouveau apres.
    CANCON           = TRANSMIT_ABORT|DIFFERENTIAL;     /*    not possible            */
    can_system_error = TRUE;
    can_init_request = TRUE;
    can_partner      = FALSE;
    can_tx_ready=can_tx_sending=FALSE;
	return 0;
    };
}

/************************************************************************/
/*      procedure:      can_tx_complete                                 */
/*      input:          ---                                             */
/*      output:         ---                                             */
/*      calls:          ---                                             */
/*----------------------------------------------------------------------*/
/*      running on interrupt level, called from "can_int ()"            */
/************************************************************************/

void    can_tx_complete ()
{
    can_event = TRUE;
	can_tx_ready = TRUE;
}


/************************************************************************/
/*......................................................................*/
/*			Fonction g?rant les interruptions sur le CAN				*/
/*......................................................................*/
/************************************************************************/
/*      procedure:      can_int                                         */
/*      input:          ---                                             */
/*      output:         ---                                             */
/*      calls:          ---                                             */
/*----------------------------------------------------------------------*/
/*      running on interrupt level, called from "can_int ()"            */
/************************************************************************/

void    can_int (void)  interrupt 5
{
    int_save = CANCON;
 
    if  ( error_int ){			// due ? une erreur
		can_int_error ();
	}
    if  ( receive_int )    							// due ? la r?ception d'un message
	{
		// on stocke la valeur du tps lors de la r?ception du message NON
		    CANSTA = (unsigned char)(can_buffrx);
			CANADR = DMA_EN|AUTO_RX_BUFFER;
			_nop_();
			_nop_();
			_nop_();
			_nop_();
    		if  ( overrun_int )
        		CANCON = RELEASE_RX_BUF | OVERRUN_CLEAR;
    		else
        		CANCON = RELEASE_RX_BUF;
			can_event   = TRUE;
    		can_rx_ready =TRUE;

	}
    if  ( transmit_int && CAN_TX_COMPLETE ){// due ? une transm- correcte et buffer d'emission vide
  		can_tx_complete ();
	}
}


/************************************************************************/
/*      procedure:      main                                            */
/*      input:          ---                                             */
/*      output:         ---                                             */
/*      calls:          CANInitialisation, LireBufferReception          */
/*                      CANEmissionMessage, CANREceptionMessage         */
/************************************************************************/

unsigned int countx;


/***************************   MAIN      *********************************/
void    main()
{
	//Initialisation du CAN
    CANInitialisation();
	printf("initialisation du systeme\n");

	while(1)
	{   
		/**** erreur donc demande d'initialisation *****/    
		if  ( can_system_error ){
 			printf("CAN ERREUR\n");
			can_init_request = TRUE;
		}
		/**** besoin d'une initialisation *****/
		if  ( can_init_request ){
 			printf("initialisation CAN\n");
		    CANInitialisation();
			can_init_request=FALSE;
    	}

		if ( can_rx_ready )
		{ 
		  can_rx_ready=0;
		  countx=countx+1;
		  printf("%u\n",countx);

		/*** INSERER ICI LE CODE POUR LA RECEPTION ***/
		}
		
		if(can_tx_ready){
		/***  INSERER ICI LE CODE POUR L'EMISSION  ***/		    	   		        
		}
		
	 }
}

