#include "ComplianceTest.h"

ComplianceTest_t ComplianceTest;

/*
 * Disable compliance mode if number of uplinks without a downlink exceeds this threshold
 */
#define COMPLIANCE_TEST_NO_RX_MAX 136 /* 132 + 4 for good measure */


void ProcessComplianceTestFrame( McpsIndication_t *McpsIndication )
{
    uint8_t *pData;

    if(ComplianceTest.Running == true)
    {
        ComplianceTest.DownLinkCounter++;
        ComplianceTest.DownLinkReceived = true;
        ComplianceTest.DownLinkTimeoutCounter = 0;
    }
    
    if((McpsIndication->RxData) && (McpsIndication->Port == 224))
    {
        pData = &McpsIndication->Buffer[0];
    
        if( ComplianceTest.Running == false )
        {
            // Check compliance test enable command (i)
            if( ( McpsIndication->BufferSize == 4 ) && 
                ( pData[0] == 0x01 ) && ( pData[1] == 0x01 ) && ( pData[2] == 0x01 ) && (pData[3] == 0x01 ) )
            {
                AppPort = 224;
                AppDataSize = 2;
                ComplianceTest.DownLinkCounter = 0;
                ComplianceTest.DownLinkReceived = true;
                ComplianceTest.LinkCheck = false;
                ComplianceTest.DemodMargin = 0;
                ComplianceTest.NbGateways = 0;
                ComplianceTest.Running = true;
                ComplianceTest.State = 1;       
    	        INFO_LOG("Compliance Application enabled");
            }
        }
        else
        {    
            ComplianceTest.State = pData[0];

    	    INFO_LOG("Compliance Application State: %d", ComplianceTest.State);

            switch( ComplianceTest.State )
            {
            case 0: // Check compliance test disable command (ii)
                DisableComplianceTest( );
                break;
            case 1: // (iii, iv)
                AppDataSize = 2;
                break;
            case 2: // Enable confirmed messages (v)
                IsTxConfirmed = true;
                ComplianceTest.State = 1;
                break;
            case 3:  // Disable confirmed messages (vi)
                IsTxConfirmed = false;
                ComplianceTest.State = 1;
                break;
            case 4: // (vii)
                AppDataSize = McpsIndication->BufferSize;
            
                if( AppDataSize > LORAWAN_APP_DATA_MAX_SIZE )
                    AppDataSize = LORAWAN_APP_DATA_MAX_SIZE;
            
                AppData[0] = 4;
                for( uint8_t i = 1; i < AppDataSize; i++ )
                {
                    AppData[i] = McpsIndication->Buffer[i] + 1; 
                }
                break;
            case 5: // (viii)
            {
                MlmeReq_t mlmeReq;
                mlmeReq.Type = MLME_LINK_CHECK;
                LoRaMacMlmeRequest( &mlmeReq );
                break;
            }
            case 6: // (viii)
            {
                // Disable compliance app
                DisableComplianceTest( );

                // Restart mac layer
                DeviceInit();
                break;
            }
            default:
                break;
            }
        } 
    }
}

void RunComplianceTest()
{
    if(ComplianceTest.DownLinkReceived == false)
        ComplianceTest.DownLinkTimeoutCounter++;
    else
        ComplianceTest.DownLinkReceived = false;


    INFO_LOG("Run Compliance Application (Timeout Counter=%d)", ComplianceTest.DownLinkTimeoutCounter);
    
    // Disable Compliance Test after too many uplinks without a downlink
    if(ComplianceTest.DownLinkTimeoutCounter >= COMPLIANCE_TEST_NO_RX_MAX)
    {
        DisableComplianceTest();
    }
    else if( ComplianceTest.LinkCheck == true )
    {
        ComplianceTest.LinkCheck = false;
        AppDataSize = 3;
        AppData[0] = 5;
        AppData[1] = ComplianceTest.DemodMargin;
        AppData[2] = ComplianceTest.NbGateways;
        ComplianceTest.State = 1;
    }
    else
    {
        switch( ComplianceTest.State )
        {        
        case 1:
            AppDataSize = 2;
            AppData[0] = ComplianceTest.DownLinkCounter >> 8;
            AppData[1] = ComplianceTest.DownLinkCounter;
            break;

        case 4:
            ComplianceTest.State = 1;
            break;

        default:
            break;
        }
    }
}

void DisableComplianceTest()
{
    IsTxConfirmed = LORAWAN_CONFIRMED_MSG_ON;
    AppPort = LORAWAN_APP_PORT;
    AppDataSize = LORAWAN_APP_DATA_SIZE;
    ComplianceTest.DownLinkCounter = 0;
    ComplianceTest.DownLinkTimeoutCounter = 0;
    ComplianceTest.Running = false;
    ComplianceTest.DownLinkReceived = false;
    
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_ADR;
    mibReq.Param.AdrEnable = LORAWAN_ADR_ON;
    LoRaMacMibSetRequestConfirm( &mibReq );

    INFO_LOG("Compliance Application disabled");
}
