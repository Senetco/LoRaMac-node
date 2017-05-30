#ifndef __COMPLIANCE_H__
#define __COMPLIANCE_H__

#include "board.h"
#include "LoRaMac.h"

#define COMPLIANCE_TX_DUTYCYCLE 5000 // 5sec

typedef struct ComplianceTest_s
{
    bool     Running;
    uint8_t  State;
    uint16_t DownLinkCounter;
    uint16_t DownLinkTimeoutCounter;
    bool     DownLinkReceived;
    bool     LinkCheck;
    uint8_t  DemodMargin;
    uint8_t  NbGateways;
} ComplianceTest_t;

extern ComplianceTest_t ComplianceTest;

void ProcessComplianceTestFrame( McpsIndication_t *McpsIndication );
void RunComplianceTest( void );
void DisableComplianceTest( void );

#endif // __COMPLIANCE_H__
