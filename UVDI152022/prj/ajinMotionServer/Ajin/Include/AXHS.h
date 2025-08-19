/****************************************************************************
*****************************************************************************
**
** File Name
** ----------
**
** AXHS.H
**
** COPYRIGHT (c) AJINEXTEK Co., LTD
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
** Resource Define Header File
** 
**
*****************************************************************************
*****************************************************************************
**
** Source Change Indices
** ---------------------
** 
** (None)
**
**
*****************************************************************************
*****************************************************************************
**
** Website
** ---------------------
**
** http://www.ajinextek.com
**
*****************************************************************************
*****************************************************************************
*/

#ifndef __AXT_AXHS_H__
#define __AXT_AXHS_H__



// 베이스보드 정의
#ifndef __AXT_BASE_BOARD__
#define __AXT_BASE_BOARD__
typedef enum _AXT_BASE_BOARD
{
    AXT_UNKNOWN                                             = 0x00,    // Unknown Baseboard
    AXT_BIHR                                                = 0x01,    // ISA bus, Half size
    AXT_BIFR                                                = 0x02,    // ISA bus, Full size
    AXT_BPHR                                                = 0x03,    // PCI bus, Half size
    AXT_BPFR                                                = 0x04,    // PCI bus, Full size
    AXT_BV3R                                                = 0x05,    // VME bus, 3U size
    AXT_BV6R                                                = 0x06,    // VME bus, 6U size
    AXT_BC3R                                                = 0x07,    // cPCI bus, 3U size
    AXT_BC6R                                                = 0x08,    // cPCI bus, 6U size
    AXT_BEHR                                                = 0x09,    // PCIe bus, Half size
    AXT_BEFR                                                = 0x0A,    // PCIe bus, Full size
    AXT_BEHD                                                = 0x0B,    // PCIe bus, Half size, DB-32T
    AXT_FMNSH4D                                             = 0x52,    // ISA bus, Full size, DB-32T, SIO-2V03 * 2
    AXT_PCI_DI64R                                           = 0x43,    // PCI bus, Digital IN 64점
    AXT_PCIE_DI64R                                          = 0x44,    // PCIe bus, Digital IN 64점
    AXT_PCI_DO64R                                           = 0x53,    // PCI bus, Digital OUT 64점
    AXT_PCIE_DO64R                                          = 0x54,    // PCIe bus, Digital OUT 64점
    AXT_PCI_DB64R                                           = 0x63,    // PCI bus, Digital IN 32점, OUT 32점
    AXT_PCIE_DB64R                                          = 0x64,    // PCIe bus, Digital IN 32점, OUT 32점
    AXT_BPFR_COM                                            = 0x70,    // PCI bus, Full size, For serial function modules.
    AXT_PCIN204                                             = 0x82,    // PCI bus, Half size On-Board 2 Axis controller.
    AXT_BPHD                                                = 0x83,    // PCI bus, Half size, DB-32T
    AXT_PCIN404                                             = 0x84,    // PCI bus, Half size On-Board 4 Axis controller.    
    AXT_PCIN804                                             = 0x85,    // PCI bus, Half size On-Board 8 Axis controller.
    AXT_PCIEN804                                            = 0x86,    // PCIe bus, Half size On-Board 8 Axis controller.
    AXT_PCIEN404                                            = 0x87,    // PCIe bus, Half size On-Board 4 Axis controller.
    AXT_PCI_AIO1602HR                                       = 0x93,    // PCI bus, Half size, AI-16ch, AO-2ch AI16HR
    AXT_PCI_R1604                                           = 0xC1,    // PCI bus[PCI9030], Half size, RTEX based 16 axis controller
    AXT_PCI_R3204                                           = 0xC2,    // PCI bus[PCI9030], Half size, RTEX based 32 axis controller
    AXT_PCI_R32IO                                           = 0xC3,    // PCI bus[PCI9030], Half size, RTEX based IO only.
    AXT_PCI_REV2                                            = 0xC4,    // Reserved.
    AXT_PCI_R1604MLII                                       = 0xC5,    // PCI bus[PCI9030], Half size, Mechatrolink-II 16/32 axis controller.
    AXT_PCI_R0804MLII                                       = 0xC6,    // PCI bus[PCI9030], Half size, Mechatrolink-II 08 axis controller.
    AXT_PCI_Rxx00MLIII                                      = 0xC7,    // PCI Board, Mechatrolink III AXT, PCI Bus[PCI9030], Half size, Max.32 Slave module support
    AXT_PCIE_Rxx00MLIII                                     = 0xC8,    // PCI Express Board, Mechatrolink III AXT, PCI Bus[], Half size, Max.32 Slave module support
    AXT_PCP2_Rxx00MLIII                                     = 0xC9,    // PCI/104-Plus Board, Mechatrolink III AXT, PCI Bus[], Half size, Max.32 Slave module support
    AXT_PCI_R1604SIIIH                                      = 0xCA,    // PCI bus[PCI9030], Half size, SSCNET III / H 16/32 axis controller.
    AXT_PCI_R32IOEV                                         = 0xCB,    // PCI bus[PCI9030], Half size, RTEX based IO only. Economic version.
    AXT_PCIE_R0804RTEX                                      = 0xCC,    // PCIe bus, Half size, Half size, RTEX based 08 axis controller.
    AXT_PCIE_R1604RTEX                                      = 0xCD,    // PCIe bus, Half size, Half size, RTEX based 16 axis controller.
    AXT_PCIE_R2404RTEX                                      = 0xCE,    // PCIe bus, Half size, Half size, RTEX based 24 axis controller.
    AXT_PCIE_R3204RTEX                                      = 0xCF,    // PCIe bus, Half size, Half size, RTEX based 32 axis controller.
    AXT_PCIE_Rxx04SIIIH                                     = 0xD0,    // PCIe bus, Half size, SSCNET III / H 16(or 32)-axis(CAMC-QI based) controller.
    AXT_PCIE_Rxx00SIIIH                                     = 0xD1,    // PCIe bus, Half size, SSCNET III / H Max. 32-axis(DSP Based) controller.
    AXT_ETHERCAT_RTOSV5                                     = 0xD2,    // EtherCAT, RTOS(On Time), Version 5.29
    AXT_PCI_Nx04_A                                          = 0xD3,    // PCI bus, Half size On-Board x Axis controller For Rtos.
    AXT_PCI_R3200MLIII_IO                                   = 0xD4,    // PCI Board, Mechatrolink III AXT, PCI Bus[PCI9030], Half size, Max.32 IO only    controller
    AXT_PCIE_Rxx05MLIII                                     = 0xD5,    // PCIe bus, Half size, Mechatrolink III 32 axis(DSP Based) controller.
    AXT_PCIE_Rxx05SIIIH                                     = 0xD6,    // PCIe bus, Half size, Sscnet III / H  32 axis(DSP Based) controller.
    AXT_PCIE_Rxx05RTEX                                      = 0xD7,    // PCIe bus, Half size, RTEX 32 axis(DSP Based) controller.
    AXT_PCIE_Rxx05ECAT                                      = 0xD8,    // PCIe bus, Half size, ECAT(DSP Based) controller. (Max. 32 Axes)
    AXT_PCI_Rxx05MLIII                                      = 0xD9,    // PCI bus, Half size, Mechatrolink III 32 axis(DSP Based) controller.
    AXT_PCI_Rxx05SIIIH                                      = 0xDA,    // PCI bus, Half size, Sscnet III / H  32 axis(DSP Based) controller.
    AXT_PCI_Rxx05RTEX                                       = 0xDB,    // PCI bus, Half size, RTEX 32 axis(DSP Based) controller.
    AXT_PCI_Rxx05ECAT                                       = 0xDC,     // PCI bus, Half size, ECAT(DSP Based) controller.
	AXT_VIRTUAL_BOARD										= 0xDD,		// Virtual XX axis controller
	AXT_PCIE_Rxx05ECAT_E									= 0xDE,		// PCIe bus, Half size, ECAT(DSP Based) controller. (Max.64 Axes)
	AXT_EXDEV_BOARD											= 0xDF		//AXTEXDEV , 3rd party external device
} AXT_BASE_BOARD;                                                      
#endif                                                                 
                                                                       
// 모듈 정의                                                           
#ifndef __AXT_FUNC_MODULE__                                            
#define __AXT_FUNC_MODULE__
typedef enum _AXT_FUNC_MODULE
{
    AXT_SMC_2V01                                            = 0x01,    // CAMC-5M, 2 Axis
    AXT_SMC_2V02                                            = 0x02,    // CAMC-FS, 2 Axis
    AXT_SMC_1V01                                            = 0x03,    // CAMC-5M, 1 Axis
    AXT_SMC_1V02                                            = 0x04,    // CAMC-FS, 1 Axis
    AXT_SMC_2V03                                            = 0x05,    // CAMC-IP, 2 Axis
    AXT_SMC_4V04                                            = 0x06,    // CAMC-QI, 4 Axis
    AXT_SMC_R1V04A4                                         = 0x07,    // CAMC-QI, 1 Axis, For RTEX A4 slave only
    AXT_SMC_1V03                                            = 0x08,    // CAMC-IP, 1 Axis
    AXT_SMC_R1V04                                           = 0x09,    // CAMC-QI, 1 Axis, For RTEX SLAVE only(Pulse Output Module)
    AXT_SMC_R1V04MLIISV                                     = 0x0A,    // CAMC-QI, 1 Axis, For Sigma-X series.
    AXT_SMC_R1V04MLIIPM                                     = 0x0B,    // 2 Axis, For Pulse output series(JEPMC-PL2910).
    AXT_SMC_2V04                                            = 0x0C,    // CAMC-QI, 2 Axis
    AXT_SMC_R1V04A5                                         = 0x0D,    // CAMC-QI, 1 Axis, For RTEX A5N slave only
    AXT_SMC_R1V04MLIICL                                     = 0x0E,    // CAMC-QI, 1 Axis, For MLII Convex Linear only
    AXT_SMC_R1V04MLIICR                                     = 0x0F,    // CAMC-QI, 1 Axis, For MLII Convex Rotary only
    AXT_SMC_R1V04PM2Q                                       = 0x10,    // CAMC-QI, 2 Axis, For RTEX SLAVE only(Pulse Output Module)
    AXT_SMC_R1V04PM2QE                                      = 0x11,    // CAMC-QI, 4 Axis, For RTEX SLAVE only(Pulse Output Module)
    AXT_SMC_R1V04MLIIORI                                    = 0x12,    // CAMC-QI, 1 Axis, For MLII Oriental Step Driver only
    AXT_SMC_R1V04A6                                         = 0x13,    // CAMC-QI, 1 Axis, For RTEX A6N slave only
    AXT_SMC_R1V04SIIIHMIV                                   = 0x14,    // CAMC-QI, 1 Axis, For SSCNETIII/H MRJ4
    AXT_SMC_R1V04SIIIHMIV_R                                 = 0x15,    // CAMC-QI, 1 Axis, For SSCNETIII/H MRJ4
    AXT_SMC_R1V04SIIIHME                                    = 0x16,    // CAMC-QI, 1 Axis, For SSCNETIII/H MRJE
    AXT_SMC_R1V04SIIIHME_R                                  = 0x17,    // CAMC-QI, 1 Axis, For SSCNETIII/H MRJE
    AXT_SMC_R1V04MLIIS7                                     = 0x1D,    // CAMC-QI, 1 Axis, For ML-II/YASKWA Sigma7(SGD7S)
    AXT_SMC_R1V04MLIIISV                                    = 0x20,    // DSP, 1 Axis, For ML-3 SigmaV/YASKAWA only 
    AXT_SMC_R1V04MLIIIPM                                    = 0x21,    // DSP, 1 Axis, For ML-3 SLAVE/AJINEXTEK only(Pulse Output Module)
    AXT_SMC_R1V04MLIIISV_MD                                 = 0x22,    // DSP, 1 Axis, For ML-3 SigmaV-MD/YASKAWA only (Multi-Axis Control amp)
    AXT_SMC_R1V04MLIIIS7S                                   = 0x23,    // DSP, 1 Axis, For ML-3 Sigma7S/YASKAWA only
    AXT_SMC_R1V04MLIIIS7W                                   = 0x24,    // DSP, 2 Axis, For ML-3 Sigma7W/YASKAWA only(Dual-Axis control amp)
    AXT_SMC_R1V04MLIIIRS2                                   = 0x25,    // DSP, 1 Axis, For ML-3 RS2A/SANYO DENKI
    AXT_SMC_R1V04MLIIIS7_MD                                 = 0x26,    // DSP, 1 Axis, For ML-3 Sigma7-MD/YASKAWA only (Multi-Axis Control amp)
    AXT_SMC_R1V04MLIIIAZ                                    = 0x27,    // DSP, 4 Axis, For ML-3 AZD/ORIENTAL only (Multi-Axis Control amp)
    AXT_SMC_R1V04MLIIIPCON                                  = 0x28,    // DSP, 1 Axis, For ML-3 PCON/IAI only
	AXT_SMC_R1V04MLIIIVPH_HE								= 0x29,	   // DSP, 1 Axis, For ML-3 VPH-HE/NIKKIDENSO only
    AXT_SMC_R1V04PM2QSIIIH                                  = 0x60,    // CAMC-QI, 2Axis For SSCNETIII/H SLAVE only(Pulse Output Module)
    AXT_SMC_R1V04PM4QSIIIH                                  = 0x61,    // CAMC-QI, 4Axis For SSCNETIII/H SLAVE only(Pulse Output Module)
    AXT_SIO_RCNT2SIIIH                                      = 0x62,    // Counter slave module, Reversible counter, 2 channels, For SSCNETIII/H Only
    AXT_SIO_RDI32SIIIH                                      = 0x63,    // DI slave module, SSCNETIII AXT, IN 32-Channel
    AXT_SIO_RDO32SIIIH                                      = 0x64,    // DO slave module, SSCNETIII AXT, OUT 32-Channel
    AXT_SIO_RDB32SIIIH                                      = 0x65,    // DB slave module, SSCNETIII AXT, IN 16-Channel, OUT 16-Channel
    AXT_SIO_RAI16SIIIH                                      = 0x66,    // AI slave module, SSCNETIII AXT, Analog IN 16ch, 16 bit
    AXT_SIO_RAO08SIIIH                                      = 0x67,    // A0 slave module, SSCNETIII AXT, Analog OUT 8ch, 16 bit
    AXT_SMC_R1V04PM2QSIIIH_R                                = 0x68,    // CAMC-QI, 2Axis For SSCNETIII/H SLAVE only(Pulse Output Module) 
    AXT_SMC_R1V04PM4QSIIIH_R                                = 0x69,    // CAMC-QI, 4Axis For SSCNETIII/H SLAVE only(Pulse Output Module) 
    AXT_SIO_RCNT2SIIIH_R                                    = 0x6A,    // Counter slave module, Reversible counter, 2 channels, For SSCNETIII/H Only 
    AXT_SIO_RDI32SIIIH_R                                    = 0x6B,    // DI slave module, SSCNETIII AXT, IN 32-Channel 
    AXT_SIO_RDO32SIIIH_R                                    = 0x6C,    // DO slave module, SSCNETIII AXT, OUT 32-Channel 
    AXT_SIO_RDB32SIIIH_R                                    = 0x6D,    // DB slave module, SSCNETIII AXT, IN 16-Channel, OUT 16-Channel 
    AXT_SIO_RAI16SIIIH_R                                    = 0x6E,    // AI slave module, SSCNETIII AXT, Analog IN 16ch, 16 bit 
    AXT_SIO_RAO08SIIIH_R                                    = 0x6F,    // A0 slave module, SSCNETIII AXT, Analog OUT 8ch, 16 bit 
    AXT_SIO_RDI32MLIII                                      = 0x70,    // DI slave module, MechatroLink III AXT, IN 32-Channel NPN
    AXT_SIO_RDO32MLIII                                      = 0x71,    // DO slave module, MechatroLink III AXT, OUT 32-Channel  NPN
    AXT_SIO_RDB32MLIII                                      = 0x72,    // DB slave module, MechatroLink III AXT, IN 16-Channel, OUT 16-Channel  NPN
    AXT_SIO_RDI32MSMLIII                                    = 0x73,    // DI slave module, MechatroLink III M-SYSTEM, IN 32-Channel
    AXT_SIO_RDO32AMSMLIII                                   = 0x74,    // DO slave module, MechatroLink III M-SYSTEM, OUT 32-Channel
    AXT_SIO_RDI32PMLIII                                     = 0x75,    // DI slave module, MechatroLink III AXT, IN 32-Channel PNP
    AXT_SIO_RDO32PMLIII                                     = 0x76,    // DO slave module, MechatroLink III AXT, OUT 32-Channel  PNP
    AXT_SIO_RDB32PMLIII                                     = 0x77,    // DB slave module, MechatroLink III AXT, IN 16-Channel, OUT 16-Channel  PNP
    AXT_SIO_RDI16MLIII                                      = 0x78,    // DI slave module, MechatroLink III M-SYSTEM, IN 16-Channel
    AXT_SIO_UNDEFINEMLIII                                   = 0x79,    // IO slave module, MechatroLink III Any, Max. IN 480-Channel, Max. OUT 480-Channel
    AXT_SIO_RDI32MLIIISFA                                   = 0x7A,    // DI slave module, MechatroLink III AXT(SFA), IN 32-Channel NPN
    AXT_SIO_RDO32MLIIISFA                                   = 0x7B,    // DO slave module, MechatroLink III AXT(SFA), OUT 32-Channel  NPN
    AXT_SIO_RDB32MLIIISFA                                   = 0x7C,    // DB slave module, MechatroLink III AXT(SFA), IN 16-Channel, OUT 16-Channel  NPN
    AXT_SIO_RDB128MLIIIAI                                   = 0x7D,    // Intelligent I/O (Product by IAI), For MLII only
    AXT_SIO_RSIMPLEIOMLII                                   = 0x7E,    // Digital IN/Out xx점, Simple I/O series, MLII 전용.
    AXT_SIO_RDO16AMLIII                                     = 0x7F,    // DO slave module, MechatroLink III M-SYSTEM, OUT 16-Channel, NPN
    AXT_SIO_RDI16MLII                                       = 0x80,    // DISCRETE INPUT MODULE, 16 points (Product by M-SYSTEM), For MLII only
    AXT_SIO_RDO16AMLII                                      = 0x81,    // NPN TRANSISTOR OUTPUT MODULE, 16 points (Product by M-SYSTEM), For MLII only
    AXT_SIO_RDO16BMLII                                      = 0x82,    // PNP TRANSISTOR OUTPUT MODULE, 16 points (Product by M-SYSTEM), For MLII only 
    AXT_SIO_RDB96MLII                                       = 0x83,    // Digital IN/OUT(Selectable), MAX 96 points, For MLII only
    AXT_SIO_RDO32RTEX                                       = 0x84,    // Digital OUT  32점
    AXT_SIO_RDI32RTEX                                       = 0x85,    // Digital IN  32점
    AXT_SIO_RDB32RTEX                                       = 0x86,    // Digital IN/OUT  32점
    AXT_SIO_RDO32RTEXNT1_D1                                 = 0x87,    // Digital OUT 32점 IntekPlus 전용
    AXT_SIO_RDI32RTEXNT1_D1                                 = 0x88,    // Digital IN 32점 IntekPlus 전용
    AXT_SIO_RDB32RTEXNT1_D1                                 = 0x89,    // Digital IN/OUT 32점 IntekPlus 전용
    AXT_SIO_RDO16BMLIII                                     = 0x8A,    // DO slave module, MechatroLink III M-SYSTEM, OUT 16-Channel, PNP
    AXT_SIO_RDB32ML2NT1                                     = 0x8B,    // DB slave module, MechatroLink2 AJINEXTEK NT1 Series
    AXT_SIO_RDB128YSMLIII                                   = 0x8C,    // IO slave module, MechatroLink III Any, Max. IN 480-Channel, Max. OUT 480-Channel
    AXT_SIO_DI32_P                                          = 0x92,    // Digital IN  32점, PNP type(source type)
    AXT_SIO_DO32T_P                                         = 0x93,    // Digital OUT 32점, Power TR, PNT type(source type)
    AXT_SIO_RDB128MLII                                      = 0x94,    // Digital IN 64점 / OUT 64점, MLII 전용(JEPMC-IO2310)
    AXT_SIO_RDI32                                           = 0x95,    // Digital IN  32점, For RTEX only
    AXT_SIO_RDO32                                           = 0x96,    // Digital OUT 32점, For RTEX only
    AXT_SIO_DI32                                            = 0x97,    // Digital IN  32점
    AXT_SIO_DO32P                                           = 0x98,    // Digital OUT 32점
    AXT_SIO_DB32P                                           = 0x99,    // Digital IN 16점 / OUT 16점
    AXT_SIO_RDB32T                                          = 0x9A,    // Digital IN 16점 / OUT 16점, For RTEX only
    AXT_SIO_DO32T                                           = 0x9E,    // Digital OUT 16점, Power TR OUTPUT
    AXT_SIO_DB32T                                           = 0x9F,    // Digital IN 16점 / OUT 16점, Power TR OUTPUT
    AXT_SIO_RAI16RB                                         = 0xA0,    // A0h(160) : AI 16Ch, 16 bit, For RTEX only
    AXT_SIO_AI4RB                                           = 0xA1,    // A1h(161) : AI 4Ch, 12 bit
    AXT_SIO_AO4RB                                           = 0xA2,    // A2h(162) : AO 4Ch, 12 bit
    AXT_SIO_AI16H                                           = 0xA3,    // A3h(163) : AI 4Ch, 16 bit
    AXT_SIO_AO8H                                            = 0xA4,    // A4h(164) : AO 4Ch, 16 bit
    AXT_SIO_AI16HB                                          = 0xA5,    // A5h(165) : AI 16Ch, 16 bit (SIO-AI16HR(input module))
    AXT_SIO_AO2HB                                           = 0xA6,    // A6h(166) : AO 2Ch, 16 bit  (SIO-AI16HR(output module))
    AXT_SIO_RAI8RB                                          = 0xA7,    // A7h(167) : AI 8Ch, 16 bit, For RTEX only        
    AXT_SIO_RAO4RB                                          = 0xA8,    // A8h(168) : AO 4Ch, 16 bit, For RTEX only
    AXT_SIO_RAI4MLII                                        = 0xA9,    // A9h(169) : AI 4Ch, 16 bit, For MLII only
    AXT_SIO_RAO2MLII                                        = 0xAA,    // AAh(170) : AO 2Ch, 16 bit, For MLII only
    AXT_SIO_RAVCI4MLII                                      = 0xAB,    // DC VOLTAGE/CURRENT INPUT MODULE, 4 points (Product by M-SYSTEM), For MLII only
    AXT_SIO_RAVO2MLII                                       = 0xAC,    // DC VOLTAGE OUTPUT MODULE, 2 points (Product by M-SYSTEM), For MLII only
    AXT_SIO_RACO2MLII                                       = 0xAD,    // DC CURRENT OUTPUT MODULE, 2 points (Product by M-SYSTEM), For MLII only
    AXT_SIO_RATI4MLII                                       = 0xAE,    // THERMOCOUPLE INPUT MODULE, 4 points (Product by M-SYSTEM), For MLII only
    AXT_SIO_RARTDI4MLII                                     = 0xAF,    // RTD INPUT MODULE, 4 points (Product by M-SYSTEM), For MLII only
    AXT_SIO_RCNT2MLII                                       = 0xB0,    // Counter slave module, Reversible counter, 2 channels (Product by YASKWA), For MLII only
    AXT_SIO_CN2CH                                           = 0xB1,    // Counter Module, 2 channels, Remapped ID, Actual ID is (0xA8)
    AXT_SIO_RCNT2RTEX                                       = 0xB2,    // Counter slave module, Reversible counter, 2 channels, For RTEX Only
    AXT_SIO_RCNT2MLIII                                      = 0xB3,    // Counter slave module, MechatroLink III AXT, 2 ch, Trigger per channel
    AXT_SIO_RHPC4MLIII                                      = 0xB4,    // Counter slave module, MechatroLink III AXT, 4 ch
    AXT_SIO_RAI16RTEX                                       = 0xC0,    // ANALOG VOLTAGE INPUT(+- 10V) 16 Channel RTEX 
    AXT_SIO_RAO08RTEX                                       = 0xC1,    // ANALOG VOLTAGE OUTPUT(+- 10V) 08 Channel RTEX
    AXT_SIO_RAI8MLIII                                       = 0xC2,    // AI slave module, MechatroLink III AXT, Analog IN 8ch, 16 bit
    AXT_SIO_RAI16MLIII                                      = 0xC3,    // AI slave module, MechatroLink III AXT, Analog IN 16ch, 16 bit
    AXT_SIO_RAO4MLIII                                       = 0xC4,    // A0 slave module, MechatroLink III AXT, Analog OUT 4ch, 16 bit
    AXT_SIO_RAO8MLIII                                       = 0xC5,    // A0 slave module, MechatroLink III AXT, Analog OUT 8ch, 16 bit
    AXT_SIO_RAVO4MLII                                       = 0xC6,    // DC VOLTAGE OUTPUT MODULE, 4 points (Product by M-SYSTEM), For MLII only
    AXT_SIO_RAV04MLIII                                      = 0xC7,    // AO Slave module, MechatroLink III M-SYSTEM Voltage output module
    AXT_SIO_RAVI4MLIII                                      = 0xC8,    // AI Slave module, MechatroLink III M-SYSTEM Voltage/Current input module
    AXT_SIO_RAI16MLIIISFA                                   = 0xC9,    // AI slave module, MechatroLink III AXT(SFA), Analog IN 16ch, 16 bit
    AXT_SIO_RDB32MSMLIII                                    = 0xCA,    // DIO slave module, MechatroLink III M-SYSTEM, IN 16-Channel, OUT 16-Channel
    AXT_SIO_RAVI4MLII                                       = 0xCB,    // DC VOLTAGE/CURRENT INPUT MODULE, 4 points (Product by M-SYSTEM), For MLII only
    AXT_SIO_RMEMORY_MLIII                                   = 0xCC,    // Memory Access type module, MechatroLink III
    AXT_SIO_RAVCI8MLII                                      = 0xCE,    // DC VOLTAGE/CURRENT INPUT MODULE, 8 points (Product by M-SYSTEM), For MLII only
    AXT_SIO_RDB64MSMLIII                                    = 0xD0,    // DIO slave module, MechatroLink III M-SYSTEM, IN 32-Channel, OUT 32-Channel
    AXT_SIO_RDI16MSMLIII                                    = 0xD1,    // DI slave module, MechatroLink III M-SYSTEM, IN 16-Channel
    AXT_COM_234R                                            = 0xD3,    // COM-234R
    AXT_COM_484R                                            = 0xD4,    // COM-484R
    AXT_COM_234IDS                                          = 0xD5,    // COM-234IDS
    AXT_COM_484IDS                                          = 0xD6,    // COM-484IDS
    AXT_SIO_AO4F                                            = 0xD7,    // AO 4Ch, 16 bit
    AXT_SIO_AI8F                                            = 0xD8,    // AI 8Ch, 16 bit
    AXT_SIO_AI8AO4F                                         = 0xD9,    // AI 8Ch, AO 4Ch, 16 bit
    AXT_SIO_HPC4                                            = 0xDA,    // External Encoder module for 4Channel with Trigger function.(Position Based)
    AXT_SIO_LCM4                                            = 0xDB,    // External Encoder module for 4Channel with Trigger function.(Velocity Based)
    AXT_ECAT_MOTION                                         = 0xE1,    // EtherCAT Motion Module
    AXT_ECAT_DIO                                            = 0xE2,    // EtherCAT DIO Module 
    AXT_ECAT_AIO                                            = 0xE3,    // EtherCAT AIO Module
    AXT_ECAT_SERIAL                                         = 0xE4,    // EtherCAT Serial COM(RS232C) Module
    AXT_ECAT_COUPLER                                        = 0xE5,    // EtherCAT Coupler Module
    AXT_ECAT_CNT                                            = 0xE6,    // EtherCAT Count Module        
    AXT_ECAT_UNKNOWN                                        = 0xE7,    // EtherCAT Unknown Module
    AXT_ECAT_ETC                                            = 0xE8,    // EtherCAT ETC Module (already register Database)
    AXT_SMC_4V04_A                                          = 0xEA,    // Nx04_A
    AXT_VIRTUAL_MOTION                                      = 0xEB,    // Virtual Motion Module
    AXT_VIRTUAL_DIO                                         = 0xEC,    // Virtual DIO Module
    AXT_VIRTUAL_AIO                                         = 0xED,    // Virtual AIO Module
    AXT_EXDEV_DIO                                           = 0xF5,    //AXTEXDEV	3rd party External device DIO
    AXT_EXDEV_AIO                                           = 0xF6     //AXTEXDEV	3rd party External device AIO


} AXT_FUNC_MODULE;
#endif

#ifndef __AXT_FUNC_RESULT__
#define __AXT_FUNC_RESULT__
typedef enum _AXT_FUNC_RESULT
{
    AXT_RT_SUCCESS                                          = 0,       // API 함수 수행 성공
    AXT_RT_OPEN_ERROR                                       = 1001,    // 라이브러리 오픈 되지않음
    AXT_RT_OPEN_ALREADY                                     = 1002,    // 라이브러리 오픈 되어있고 사용 중임
    AXT_RT_NOT_INITIAL                                      = 1052,    // Serial module이 초기화되어있지 않음
    AXT_RT_NOT_OPEN                                         = 1053,    // 라이브러리 초기화 실패
    AXT_RT_NOT_SUPPORT_VERSION                              = 1054,    // 지원하지않는 하드웨어
    AXT_RT_LOCK_FILE_MISMATCH                               = 1055,    // Lock파일과 현재 Scan정보가 일치하지 않음 
    AXT_RT_MASTER_VERSION_MISMATCH                          = 1056,    // Library와 EtherCAT Master 버젼이 일치하지 않음
    AXT_RT_NOT_RUN_EZMANAGER                                = 1057,    // EzManager가 실행되지않음
    AXT_RT_NOT_FIND_BIN_FILE                                = 1058,    // BIN 파일을 찾을 수 없음
    AXT_RT_NOT_FIND_ENI_FILE                                = 1059,    // ENI 파일을 찾을 수 없음
    AXT_RT_NOT_FIND_CONFIG_FILE                             = 1060,    // Config 파일을 찾을 수 없음
    AXT_RT_RTOS_OPEN_ERROR                                  = 1061,    // RTOS Open 실패    
    AXT_RT_SLAVE_CONFIG_ERROR                               = 1062,    // RTOS Slave Config 실패     
    AXT_RT_SLAVE_OP_TIMEOUT_WARNING                         = 1063,    // Slave들이 OP 모드가 될 때까지 대기중 Timeout이 발생
    AXT_RT_SLAVE_NOT_OP                                     = 1064,    // OP 모드가 아닌 Slave가 존재함

    AXT_RT_RESCAN_NOT_EXIST_BOARD                           = 1060,    // 보드가 존재하지 않음
    AXT_RT_RESCAN_TIMEOUT                                   = 1061,    // Rescan 명령 후 대기 시간 초과

    AXT_RT_BAD_PARAMETER                                    = 1070,    // 사용자가 입력한 파라미터가 적절하지 않음

    AXT_RT_INVALID_HARDWARE                                 = 1100,    // 유효하지 않는 보드
    AXT_RT_INVALID_BOARD_NO                                 = 1101,    // 유효하지 않는 보드 번호
    AXT_RT_INVALID_MODULE_POS                               = 1102,    // 유효하지 않는 모듈 위치
    AXT_RT_INVALID_LEVEL                                    = 1103,    // 유효하지 않는 레벨
    AXT_RT_INVALID_VARIABLE                                 = 1104,    // 유효하지 않는 변수
    AXT_RT_INVALID_MODULE_NO                                = 1105,    // 유효하지 않는 모듈
    AXT_RT_INVALID_NO                                       = 1106,    // 유효하지 않는 번호

    AXT_RT_ERROR_VERSION_READ                               = 1151,    // 라이브러리 버전을 읽을수 없음
    AXT_RT_NETWORK_ERROR                                    = 1152,    // 하드웨어 네트워크 에러
    AXT_RT_NETWORK_LOCK_MISMATCH                            = 1153,    // 보드 Lock정보와 현재 Scan정보가 일치하지 않음

    AXT_RT_1ST_BELOW_MIN_VALUE                              = 1160,    // 첫번째 인자값이 최소값보다 더 작음
    AXT_RT_1ST_ABOVE_MAX_VALUE                              = 1161,    // 첫번째 인자값이 최대값보다 더 큼
    AXT_RT_2ND_BELOW_MIN_VALUE                              = 1170,    // 두번째 인자값이 최소값보다 더 작음
    AXT_RT_2ND_ABOVE_MAX_VALUE                              = 1171,    // 두번째 인자값이 최대값보다 더 큼
    AXT_RT_3RD_BELOW_MIN_VALUE                              = 1180,    // 세번째 인자값이 최소값보다 더 작음
    AXT_RT_3RD_ABOVE_MAX_VALUE                              = 1181,    // 세번째 인자값이 최대값보다 더 큼
    AXT_RT_4TH_BELOW_MIN_VALUE                              = 1190,    // 네번째 인자값이 최소값보다 더 작음
    AXT_RT_4TH_ABOVE_MAX_VALUE                              = 1191,    // 네번째 인자값이 최대값보다 더 큼
    AXT_RT_5TH_BELOW_MIN_VALUE                              = 1200,    // 다섯번째 인자값이 최소값보다 더 작음
    AXT_RT_5TH_ABOVE_MAX_VALUE                              = 1201,    // 다섯번째 인자값이 최대값보다 더 큼
    AXT_RT_6TH_BELOW_MIN_VALUE                              = 1210,    // 여섯번째 인자값이 최소값보다 더 작음
    AXT_RT_6TH_ABOVE_MAX_VALUE                              = 1211,    // 여섯번째 인자값이 최대값보다 더 큼
    AXT_RT_7TH_BELOW_MIN_VALUE                              = 1220,    // 일곱번째 인자값이 최소값보다 더 작음
    AXT_RT_7TH_ABOVE_MAX_VALUE                              = 1221,    // 일곱번째 인자값이 최대값보다 더 큼
    AXT_RT_8TH_BELOW_MIN_VALUE                              = 1230,    // 여덟번째 인자값이 최소값보다 더 작음
    AXT_RT_8TH_ABOVE_MAX_VALUE                              = 1231,    // 여덟번째 인자값이 최대값보다 더 큼
    AXT_RT_9TH_BELOW_MIN_VALUE                              = 1240,    // 아홉번째 인자값이 최소값보다 더 작음
    AXT_RT_9TH_ABOVE_MAX_VALUE                              = 1241,    // 아홉번째 인자값이 최대값보다 더 큼
    AXT_RT_10TH_BELOW_MIN_VALUE                             = 1250,    // 열번째 인자값이 최소값보다 더 작음
    AXT_RT_10TH_ABOVE_MAX_VALUE                             = 1251,    // 열번째 인자값이 최대값보다 더 큼
    AXT_RT_11TH_BELOW_MIN_VALUE                             = 1252,    // 열한번째 인자값이 최소값보다 더 작음
    AXT_RT_11TH_ABOVE_MAX_VALUE                             = 1253,    // 열한번째 인자값이 최대값보다 더 큼

    AXT_RT_AIO_OPEN_ERROR                                   = 2001,    // AIO 모듈 오픈실패
    AXT_RT_AIO_NOT_MODULE                                   = 2051,    // AIO 모듈 없음
    AXT_RT_AIO_NOT_EVENT                                    = 2052,    // AIO 이벤트 읽지 못함
    AXT_RT_AIO_INVALID_MODULE_NO                            = 2101,    // 유효하지않은 AIO모듈
    AXT_RT_AIO_INVALID_CHANNEL_NO                           = 2102,    // 유효하지않은 AIO채널번호
    AXT_RT_AIO_INVALID_USE                                  = 2106,    // AIO 함수 사용못함
    AXT_RT_AIO_INVALID_TRIGGER_MODE                         = 2107,    // 유효하지않는 트리거 모드
    AXT_RT_AIO_EXTERNAL_DATA_EMPTY                          = 2108,    // 외부 데이터 값이 없을 경우
    AXT_RT_AIO_INVALID_VALUE                                = 2109,    // 유효하지않는 값 설정
    AXT_RT_AIO_UPG_ALEADY_ENABLED                           = 2110,    // AO UPG 기능 사용중 설정됨                              

    AXT_RT_DIO_OPEN_ERROR                                   = 3001,    // DIO 모듈 오픈실패
    AXT_RT_DIO_NOT_MODULE                                   = 3051,    // DIO 모듈 없음
    AXT_RT_DIO_NOT_INTERRUPT                                = 3052,    // DIO 인터럽트 설정안됨
    AXT_RT_DIO_INVALID_MODULE_NO                            = 3101,    // 유효하지않는 DIO 모듈 번호
    AXT_RT_DIO_INVALID_OFFSET_NO                            = 3102,    // 유효하지않는 DIO OFFSET 번호
    AXT_RT_DIO_INVALID_LEVEL                                = 3103,    // 유효하지않는 DIO 레벨
    AXT_RT_DIO_INVALID_MODE                                 = 3104,    // 유효하지않는 DIO 모드
    AXT_RT_DIO_INVALID_VALUE                                = 3105,    // 유효하지않는 값 설정
    AXT_RT_DIO_INVALID_USE                                  = 3106,    // DIO 함수 사용못함
    AXT_RT_DIO_INVALID_LINK                                 = 3107,    // DIO Link가 유효하지 않음
    AXT_RT_DIO_INTERLOCK_NOT_ENABLED                        = 3108,    // DIO InterLock 유효하지 않음
    AXT_RT_DIO_INTERLOCK_NOT_SAME_BOARD                     = 3109,    // Destination Module과 Source Module일 동일한 보드내에 있지 않음

    AXT_RT_CNT_OPEN_ERROR                                   = 3201,    // CNT 모듈 오픈실패
    AXT_RT_CNT_NOT_MODULE                                   = 3251,    // CNT 모듈 없음
    AXT_RT_CNT_NOT_INTERRUPT                                = 3252,    // CNT 인터럽트 설정안됨
    AXT_RT_CNT_NOT_TRIGGER_ENABLE                           = 3253,    // CNT Trigger 출력 기능이 활성화되어 있지 않음
    AXT_RT_CNT_INVALID_MODULE_NO                            = 3301,    // 유효하지않는 CNT 모듈 번호
    AXT_RT_CNT_INVALID_CHANNEL_NO                           = 3302,    // 유효하지않는 채널 번호
    AXT_RT_CNT_INVALID_OFFSET_NO                            = 3303,    // 유효하지않는 CNT OFFSET 번호
    AXT_RT_CNT_INVALID_LEVEL                                = 3304,    // 유효하지않는 CNT 레벨
    AXT_RT_CNT_INVALID_MODE                                 = 3305,    // 유효하지않는 CNT 모드
    AXT_RT_CNT_INVALID_VALUE                                = 3306,    // 유효하지않는 값 설정
    AXT_RT_CNT_INVALID_USE                                  = 3307,    // CNT 함수 사용못함
    AXT_RT_CNT_CMD_EXE_TIMEOUT                              = 3308,    // CNT 모듈 데이터입력 시간초과 했을때
    AXT_RT_CNT_INVALID_VELOCITY                             = 3309,    // 유효하지않는 CNT 속도
    AXT_RT_PROTECTED_DURING_PWMENABLE                       = 3310,    // PWM Enable 되어 있는 상태에서 사용 못 함
    AXT_RT_CNT_INVALID_TABLEPOS                             = 3311,    // 유효하지 않은 CNT TABLE 번호 
    AXT_RT_CNT_DIMENSION_ERROR                              = 3312,    // 해당 Dimension 설정 상태에서는 사용할 수 없음
	AXT_RT_CNT_INVALID_RANGE                                = 3313,    // 유효하지않는 CNT Range(Lower ~ Upper)

    AXT_RT_COM_OPEN_ERROR                                   = 3401,    // COM 포트 오픈실패
    AXT_RT_COM_NOT_OPEN                                     = 3402,    // COM 포트 오픈되지 않음
    AXT_RT_COM_ALREADY_IN_USE                               = 3403,    // COM 포트 사용중
    AXT_RT_COM_NOT_MODULE                                   = 3451,    // COM 포트 없음
    AXT_RT_COM_NOT_INTERRUPT                                = 3452,    // COM 인터럽트 설정안됨
    AXT_RT_COM_INVALID_MODULE_NO                            = 3501,    // 유효하지않는 COM 모듈 번호
    AXT_RT_COM_INVALID_PORT_NO                              = 3502,    // 유효하지않는 채널 번호
    AXT_RT_COM_INVALID_OFFSET_NO                            = 3503,    // 유효하지않는 COM OFFSET 번호
    AXT_RT_COM_INVALID_LEVEL                                = 3504,    // 유효하지않는 COM 레벨
    AXT_RT_COM_INVALID_MODE                                 = 3505,    // 유효하지않는 COM 모드
    AXT_RT_COM_INVALID_VALUE                                = 3506,    // 유효하지않는 값 설정
    AXT_RT_COM_INVALID_USE                                  = 3507,    // COM 함수 사용못함
    AXT_RT_COM_INVALID_BAUDRATE                             = 3508,    // 유효하지않는 값 설정
	
    AXT_RT_MOTION_OPEN_ERROR                                = 4001,    // 모션 라이브러리 Open 실패
    AXT_RT_MOTION_NOT_MODULE                                = 4051,    // 시스템에 장착된 모션 모듈이 없음
    AXT_RT_MOTION_NOT_INTERRUPT                             = 4052,    // 인터럽트 결과 읽기 실패
    AXT_RT_MOTION_NOT_INITIAL_AXIS_NO                       = 4053,    // 해당 축 모션 초기화 실패
    AXT_RT_MOTION_NOT_IN_CONT_INTERPOL                      = 4054,    // 연속 보간 구동 중이 아닌 상태에서 연속보간 중지 명령을 수행 하였음
    AXT_RT_MOTION_NOT_PARA_READ                             = 4055,    // 원점 구동 설정 파라미터 로드 실패
    AXT_RT_MOTION_INVALID_AXIS_NO                           = 4101,    // 해당 축이 존재하지 않음
    AXT_RT_MOTION_INVALID_METHOD                            = 4102,    // 해당 축 구동에 필요한 설정이 잘못됨
    AXT_RT_MOTION_INVALID_USE                               = 4103,    // 'uUse' 인자값이 잘못 설정됨
    AXT_RT_MOTION_INVALID_LEVEL                             = 4104,    // 'uLevel' 인자값이 잘못 설정됨
    AXT_RT_MOTION_INVALID_BIT_NO                            = 4105,    // 범용 입출력 해당 비트가 잘못 설정됨
    AXT_RT_MOTION_INVALID_STOP_MODE                         = 4106,    // 모션 정지 모드 설정값이 잘못됨
    AXT_RT_MOTION_INVALID_TRIGGER_MODE                      = 4107,    // 트리거 설정 모드가 잘못 설정됨
    AXT_RT_MOTION_INVALID_TRIGGER_LEVEL                     = 4108,    // 트리거 출력 레벨 설정이 잘못됨
    AXT_RT_MOTION_INVALID_SELECTION                         = 4109,    // 'uSelection' 인자가 COMMAND 또는 ACTUAL 이외의 값으로 설정되어 있음
    AXT_RT_MOTION_INVALID_TIME                              = 4110,    // Trigger 출력 시간값이 잘못 설정되어 있음
    AXT_RT_MOTION_INVALID_FILE_LOAD                         = 4111,    // 모션 설정값이 저장된 파일이 로드가 안됨
    AXT_RT_MOTION_INVALID_FILE_SAVE                         = 4112,    // 모션 설정값을 저장하는 파일 저장에 실패함
    AXT_RT_MOTION_INVALID_VELOCITY                          = 4113,    // 모션 구동 속도값이 0으로 설정되어 모션 에러 발생
    AXT_RT_MOTION_INVALID_ACCELTIME                         = 4114,    // 모션 구동 가속 시간값이 0으로 설정되어 모션 에러 발생
    AXT_RT_MOTION_INVALID_PULSE_VALUE                       = 4115,    // 모션 단위 설정 시 입력 펄스값이 0보다 작은값으로 설정됨
    AXT_RT_MOTION_INVALID_NODE_NUMBER                       = 4116,    // 위치나 속도 오버라이드 함수가 모션 정지 중에 실행됨
    AXT_RT_MOTION_INVALID_TARGET                            = 4117,    // 다축 모션 정지 원인에 관한 플래그를 반환한다.

	AXT_RT_MOTION_SSTOPCMD_ALREADY_IN_EXECUTION				= 4150,    // 정지 명령으로 인한 감속 중에 정지 명령이 추가로 호출됨
    AXT_RT_MOTION_ERROR_IN_NONMOTION                        = 4151,    // 모션 구동중이어야 되는데 모션 구동중이 아닐 때
    AXT_RT_MOTION_ERROR_IN_MOTION                           = 4152,    // 모션 구동 중에 다른 모션 구동 함수를 실행함
    AXT_RT_MOTION_ERROR                                     = 4153,    // 다축 구동 정지 함수 실행 중 에러 발생함
    AXT_RT_MOTION_ERROR_GANTRY_ENABLE                       = 4154,    // 겐트리 enable이 되어있을 때
    AXT_RT_MOTION_ERROR_GANTRY_AXIS                         = 4155,    // 겐트리 축이 마스터채널(축) 번호(0 ~ (최대축수 - 1))가 잘못 들어갔을 때
    AXT_RT_MOTION_ERROR_MASTER_SERVOON                      = 4156,    // 마스터 축 서보온이 안되어있을 때
    AXT_RT_MOTION_ERROR_SLAVE_SERVOON                       = 4157,    // 슬레이브 축 서보온이 안되어있을 때
    AXT_RT_MOTION_INVALID_POSITION                          = 4158,    // 유효한 위치에 없을 때
    AXT_RT_ERROR_NOT_SAME_MODULE                            = 4159,    // 똑 같은 모듈내에 있지 않을경우
    AXT_RT_ERROR_NOT_SAME_BOARD                             = 4160,    // 똑 같은 보드내에 있지 아닐경우
    AXT_RT_ERROR_NOT_SAME_PRODUCT                           = 4161,    // 제품이 서로 다를경우
    AXT_RT_NOT_CAPTURED                                     = 4162,    // 위치가 저장되지 않을 때
    AXT_RT_ERROR_NOT_SAME_IC                                = 4163,    // 같은 칩내에 존재하지않을 때
    AXT_RT_ERROR_NOT_GEARMODE                               = 4164,    // 기어모드로 변환이 안될 때
    AXT_ERROR_CONTI_INVALID_AXIS_NO                         = 4165,    // 연속보간 축맵핑 시 유효한 축이 아닐 때
    AXT_ERROR_CONTI_INVALID_MAP_NO                          = 4166,    // 연속보간 맵핑 시 유효한 맵핑 번호가 아닐 때
    AXT_ERROR_CONTI_EMPTY_MAP_NO                            = 4167,    // 연속보간 맵핑 번호가 비워 있을 때
    AXT_RT_MOTION_ERROR_CACULATION                          = 4168,    // 계산상의 오차가 발생했을 때
    AXT_RT_ERROR_MOVE_SENSOR_CHECK                          = 4169,    // 연속보간 구동전 에러센서가(Alarm, EMG, Limit등) 감지된경우
                                                                       
    AXT_ERROR_HELICAL_INVALID_AXIS_NO                       = 4170,    // 헬리컬 축 맵핑 시 유효한 축이 아닐 때
    AXT_ERROR_HELICAL_INVALID_MAP_NO                        = 4171,    // 헬리컬 맵핑 시 유효한 맵핑 번호가 아닐 때
    AXT_ERROR_HELICAL_EMPTY_MAP_NO                          = 4172,    // 헬리컬 맵핑 번호가 비워 있을 때
    AXT_ERROR_HELICAL_ZPOS_DISTANCE_ZERO                    = 4173,    // 헬리컬 맵핑된 Z축의 이동량이 0일 때

    AXT_ERROR_SPLINE_INVALID_AXIS_NO                        = 4180,    // 스플라인 축 맵핑 시 유효한 축이 아닐 때
    AXT_ERROR_SPLINE_INVALID_MAP_NO                         = 4181,    // 스플라인 맵핑 시 유효한 맵핑 번호가 아닐 때
    AXT_ERROR_SPLINE_EMPTY_MAP_NO                           = 4182,    // 스플라인 맵핑 번호가 비워있을 때
    AXT_ERROR_SPLINE_NUM_ERROR                              = 4183,    // 스플라인 점숫자가 부적당할 때
    AXT_RT_MOTION_INTERPOL_VALUE                            = 4184,    // 보간할 때 입력 값이 잘못넣어졌을 때
    AXT_RT_ERROR_NOT_CONTIBEGIN                             = 4185,    // 연속보간 할 때 CONTIBEGIN함수를 호출하지 않을 때
    AXT_RT_ERROR_NOT_CONTIEND                               = 4186,    // 연속보간 할 때 CONTIEND함수를 호출하지 않을 때
                                                                       
    AXT_RT_MOTION_HOME_SEARCHING                            = 4201,    // 홈을 찾고 있는 중일 때 다른 모션 함수들을 사용할 때
    AXT_RT_MOTION_HOME_ERROR_SEARCHING                      = 4202,    // 홈을 찾고 있는 중일 때 외부에서 사용자나 혹은 어떤것에 의한  강제로 정지당할 때
    AXT_RT_MOTION_HOME_ERROR_START                          = 4203,    // 초기화 문제로 홈시작 불가할 때
    AXT_RT_MOTION_HOME_ERROR_GANTRY                         = 4204,    // 홈을 찾고 있는 중일 때 겐트리 enable 불가할 때
                                                                       
    AXT_RT_MOTION_READ_ALARM_WAITING                        = 4210,    // 서보팩으로부터 알람코드 결과를 기다리는 중
    AXT_RT_MOTION_READ_ALARM_NO_REQUEST                     = 4211,    // 서보팩에 알람코드 반환 명령이 내려지지않았을 때
    AXT_RT_MOTION_READ_ALARM_TIMEOUT                        = 4212,    // 서보팩 알람읽기 시간초과 했을때(1sec이상)
    AXT_RT_MOTION_READ_ALARM_FAILED                         = 4213,    // 서보팩 알람읽기에 실패 했을 때
    AXT_RT_MOTION_READ_ALARM_UNKNOWN                        = 4220,    // 알람코드가 알수없는 코드일 때
    AXT_RT_MOTION_READ_ALARM_FILES                          = 4221,    // 알람정보 파일이 정해진위치에 존재하지 않을 때
    AXT_RT_MOTION_READ_ALARM_NOT_DETECTED                   = 4222,    // 알람코드 Read 시, 알람이 발생하지 않았을 때
                                                                       
    AXT_RT_MOTION_POSITION_OUTOFBOUND                       = 4251,    // 설정한 위치값이 설정 최대값보다 크거나 최소값보다 작은값임
    AXT_RT_MOTION_PROFILE_INVALID                           = 4252,    // 구동 속도 프로파일 설정이 잘못됨
    AXT_RT_MOTION_VELOCITY_OUTOFBOUND                       = 4253,    // 구동 속도값이 최대값보다 크게 설정됨
    AXT_RT_MOTION_MOVE_UNIT_IS_ZERO                         = 4254,    // 구동 단위값이 0으로 설정됨
    AXT_RT_MOTION_SETTING_ERROR                             = 4255,    // 속도, 가속도, 저크, 프로파일 설정이 잘못됨
    AXT_RT_MOTION_IN_CONT_INTERPOL                          = 4256,    // 연속 보간 구동 중 구동 시작 또는 재시작 함수를 실행하였음 
    AXT_RT_MOTION_DISABLE_TRIGGER                           = 4257,    // 트리거 출력이 Disable 상태임
    AXT_RT_MOTION_INVALID_CONT_INDEX                        = 4258,    // 연속 보간 Index값 설정이 잘못됨
    AXT_RT_MOTION_CONT_QUEUE_FULL                           = 4259,    // 모션 칩의 연속 보간 큐가 Full 상태임
    AXT_RT_PROTECTED_DURING_SERVOON                         = 4260,    // 서보 온 되어 있는 상태에서 사용 못 함
    AXT_RT_HW_ACCESS_ERROR                                  = 4261,    // 메모리 Read / Write 실패
                                                                       
    AXT_RT_HW_DPRAM_CMD_WRITE_ERROR_LV1                     = 4262,    // DPRAM Comamnd Write 실패 Level1
    AXT_RT_HW_DPRAM_CMD_WRITE_ERROR_LV2                     = 4263,    // DPRAM Comamnd Write 실패 Level2
    AXT_RT_HW_DPRAM_CMD_WRITE_ERROR_LV3                     = 4264,    // DPRAM Comamnd Write 실패 Level3
    AXT_RT_HW_DPRAM_CMD_READ_ERROR_LV1                      = 4265,    // DPRAM Comamnd Read 실패 Level1
    AXT_RT_HW_DPRAM_CMD_READ_ERROR_LV2                      = 4266,    // DPRAM Comamnd Read 실패 Level2
    AXT_RT_HW_DPRAM_CMD_READ_ERROR_LV3                      = 4267,    // DPRAM Comamnd Read 실패 Level3                                     
    
    AXT_RT_COMPENSATION_SET_PARAM_FIRST                     = 4300,    // 보정 파라미터 중 첫번째 값이 잘못 설정되었음
    AXT_RT_COMPENSATION_NOT_INIT                            = 4301,    // 보정테이블 기능 초기화 되지않음
    AXT_RT_COMPENSATION_POS_OUTOFBOUND                      = 4302,    // 위치 값이 범위내에 존재하지 않음
    AXT_RT_COMPENSATION_BACKLASH_NOT_INIT                   = 4303,    // 백랙쉬 보정기능 초기화 되지않음
    AXT_RT_COMPENSATION_INVALID_ENTRY                       = 4304,    // 보정테이블 개수가 잘못 입력되었음.
                                                                       
    AXT_RT_SEQ_NOT_IN_SERVICE                               = 4400,    // 순차 구동 함수 실행 중 자원 할당 실패
    AXT_ERROR_SEQ_INVALID_MAP_NO                            = 4401,    // 순차 구동 함수 실행 중 맵핑 번호 이상.
    AXT_ERROR_INVALID_AXIS_NO                               = 4402,    // 함수 설정 인자중 축번호 이상.
    AXT_RT_ERROR_NOT_SEQ_NODE_BEGIN                         = 4403,    // 순차 구동 노드 입력 시작 함수를 호출하지 않음.
    AXT_RT_ERROR_NOT_SEQ_NODE_END                           = 4404,    // 순차 구동 노드 입력 종료 함수를 호출하지 않음.
    AXT_RT_ERROR_NO_NODE                                    = 4405,    // 순차 구동 노드 입력이 없음.
    AXT_RT_ERROR_SEQ_STOP_TIMEOUT                           = 4406,    // 순차 구동 함수 종료 시 TimeOut 발생
    AXT_RT_ERROR_INVALID_SEQ_MASTER_AXIS_NO                 = 4407,    // 순차 구동 Master 축이 유효하지 않음.
                                                                       
    AXT_RT_ERROR_RING_COUNTER_ENABLE                        = 4420,    // Ring Counter 기능이 사용 중
    AXT_RT_ERROR_RING_COUNTER_OUT_OF_RANGE                  = 4421,    // Ring Counter 사용 중 범위 밖 명령 위치 호출(POS_ABS_LONG_MODE or POS_ABS_SHORT_MODE 일 경우)
    AXT_RT_ERROR_SOFT_LIMIT_ENABLE                          = 4430,    // Software Limit 기능이 사용 중
    AXT_RT_ERROR_SOFT_LIMIT_NEGATIVE                        = 4431,    // 이동할 위치가 Negative Software Limit을 벗어남
    AXT_RT_ERROR_SOFT_LIMIT_POSITIVE                        = 4432,    // 이동할 위치가 Positive Software Limit을 벗어남

    AXT_RT_M3_COMMUNICATION_FAILED                          = 4500,    // ML3 통신 기준, 통신 실패
    AXT_RT_MOTION_ONE_OF_AXES_IS_NOT_M3                     = 4501,    // ML3 통신 기준, 구성된 ML3 노드 중에서 모션 노드 없음 
    AXT_RT_MOTION_BIGGER_VEL_THEN_MAX_VEL                   = 4502,    // ML3 통신 기준, 지정된 축의 설정된 최대 속도보다 큼
    AXT_RT_MOTION_SMALLER_VEL_THEN_MAX_VEL                  = 4503,    // ML3 통신 기준, 지정된 축의 설정된 최대 속도보다 작음
    AXT_RT_MOTION_ACCEL_MUST_BIGGER_THEN_ZERO               = 4504,    // ML3 통신 기준, 지정된 축의 설정된 가속도가 0보다 큼
    AXT_RT_MOTION_SMALL_ACCEL_WITH_UNIT_PULSE               = 4505,    // ML3 통신 기준, UnitPulse가 적용된 가속도가 0보다 큼
    AXT_RT_MOTION_INVALID_INPUT_ACCEL                       = 4506,    // ML3 통신 기준, 지정된 축의 가속도 입력이 잘못됨
    AXT_RT_MOTION_SMALL_DECEL_WITH_UNIT_PULSE               = 4507,    // ML3 통신 기준, UnitPulse가 적용된 감속도가 0보다 큼
    AXT_RT_MOTION_INVALID_INPUT_DECEL                       = 4508,    // ML3 통신 기준, 지정된 축의 감속도 입력이 잘못됨
    AXT_RT_MOTION_SAME_START_AND_CENTER_POS                 = 4509,    // ML3 통신 기준, 원호보간의 시작점과 중심점이 같음
    AXT_RT_MOTION_INVALID_JERK                              = 4510,    // ML3 통신 기준, 지정된 축의 저크 입력이 잘못됨
    AXT_RT_MOTION_INVALID_INPUT_VALUE                       = 4511,    // ML3 통신 기준, 지정된 축의 입력값이 잘못됨
    AXT_RT_MOTION_NOT_SUPPORT_PROFILE                       = 4512,    // ML3 통신 기준, 제공되지 않는 속도 프로파일임
    AXT_RT_MOTION_INPOS_UNUSED                              = 4513,    // ML3 통신 기준, 인포지션 사용하지 않음
    AXT_RT_MOTION_AXIS_IN_SLAVE_STATE                       = 4514,    // ML3 통신 기준, 지정된 축이 슬레이브 상태가 아님
    AXT_RT_MOTION_AXES_ARE_NOT_SAME_BOARD                   = 4515,    // ML3 통신 기준, 지정된 축들이 같은 보드 내에 있지 않음
    AXT_RT_MOTION_ERROR_IN_ALARM                            = 4516,    // ML3 통신 기준, 지정된 축이 알람 상태임
    AXT_RT_MOTION_ERROR_IN_EMGN                             = 4517,    // ML3 통신 기준, 지정된 축이 비상정지 상태임
    AXT_RT_MOTION_CAN_NOT_CHANGE_COORD_NO                   = 4518,    // ML3 통신 기준, 코디네이터 넘버 변환 불가임
    AXT_RT_MOTION_INVALID_INTERNAL_RADIOUS                  = 4519,    // ML3 통신 기준, 원호보간의 X, Y축 반지름 불일치
    AXT_RT_MOTION_CONTI_QUEUE_FULL                          = 4521,    // ML3 통신 기준, 보간의 큐가 가득 참
    AXT_RT_MOTION_SAME_START_AND_END_POSITION               = 4522,    // ML3 통신 기준, 원호보간의 시작점과 종료점이 같음
    AXT_RT_MOTION_INVALID_ANGLE                             = 4523,    // ML3 통신 기준, 원호보간의 각도가 360도 초과됨
    AXT_RT_MOTION_CONTI_QUEUE_EMPTY                         = 4524,    // ML3 통신 기준, 보간의 큐가 비어있음
    AXT_RT_MOTION_ERROR_GEAR_ENABLE                         = 4525,    // ML3 통신 기준, 지정된 축이 이미 링크 설정 상태임
    AXT_RT_MOTION_ERROR_GEAR_AXIS                           = 4526,    // ML3 통신 기준, 지정된 축이 링크축이 아님
    AXT_RT_MOTION_ERROR_NO_GANTRY_ENABLE                    = 4527,    // ML3 통신 기준, 지정된 축이 겐트리 설정 상태가 아님
    AXT_RT_MOTION_ERROR_NO_GEAR_ENABLE                      = 4528,    // ML3 통신 기준, 지정된 축이 링크 설정 상태가 아님
    AXT_RT_MOTION_ERROR_GANTRY_ENABLE_FULL                  = 4529,    // ML3 통신 기준, 겐트리 설정 가득참
    AXT_RT_MOTION_ERROR_GEAR_ENABLE_FULL                    = 4530,    // ML3 통신 기준, 링크 설정 가득참
    AXT_RT_MOTION_ERROR_NO_GANTRY_SLAVE                     = 4531,    // ML3 통신 기준, 지정된 축이 겐트리 슬레이브 설정상태가 아님
    AXT_RT_MOTION_ERROR_NO_GEAR_SLAVE                       = 4532,    // ML3 통신 기준, 지정된 축이 링크 슬레이브 설정상태가 아님
    AXT_RT_MOTION_ERROR_MASTER_SLAVE_SAME                   = 4533,    // 마스터 축과 슬레이브 축이 동일함
    AXT_RT_MOTION_NOT_SUPPORT_HOMESIGNAL                    = 4534,    // ML3 통신 기준, 지정된 축의 홈신호는 지원되지 않음
    AXT_RT_MOTION_ERROR_NOT_SYNC_CONNECT                    = 4535,    // ML3 통신 기준, 지정된 축이 싱크 연결 상태가 아님
    AXT_RT_MOTION_OVERFLOW_POSITION                         = 4536,    // ML3 통신 기준, 지정된 축에 대한 구동 위치값이 오버플로우임
    AXT_RT_MOTION_ERROR_INVALID_CONTIMAPAXIS                = 4537,    // ML3 통신 기준, 보간작업을 위한 지정된 좌표계 축맵핑이 없음
    AXT_RT_MOTION_ERROR_INVALID_CONTIMAPSIZE                = 4538,    // ML3 통신 기준, 보간작업을 위한 지정된 좌표계 축맵핑 축사이즈가 잘못됨
    AXT_RT_MOTION_ERROR_IN_SERVO_OFF                        = 4539,    // ML3 통신 기준, 지정된 축이 서보 OFF되어 있음
    AXT_RT_MOTION_ERROR_POSITIVE_LIMIT                      = 4540,    // ML3 통신 기준, 지정된 축이 (+)리밋 ON되어 있음
    AXT_RT_MOTION_ERROR_NEGATIVE_LIMIT                      = 4541,    // ML3 통신 기준, 지정된 축이 (-)리밋 ON되어 있음
    AXT_RT_MOTION_ERROR_OVERFLOW_SWPROFILE_NUM              = 4542,    // ML3 통신 기준, 지정된 축들에 대한 지원 프로파일 개수가 오버플로우됨
    AXT_RT_PROTECTED_DURING_INMOTION                        = 4543,    // in_motion 되어 있는 상태에서 사용 못 함
	
    AXT_ERROR_SYNC_INVALID_AXIS_NO                          = 4600,    // Sync 축맵핑 시 유효한 축이 아닐 때
    AXT_ERROR_SYNC_INVALID_MAP_NO                           = 4601,    // Sync 맵핑 시 유효한 맵핑 번호가 아닐 때
    AXT_ERROR_SYNC_DUPLICATED_TIME                          = 4602,    // Time table이 중복되었을 때

    AXT_RT_DATA_FLASH_NOT_EXIST                             = 5000,    // 플래시 메모리가 존재하지 않음
    AXT_RT_DATA_FLASH_BUSY                                  = 5001,    // 플래시 메모리가 사용 중

    AXT_RT_QUEUE_CMD_ERROR                                  = 5010,
    AXT_RT_QUEUE_CMD_WAIT_ERROR                             = 5011,
    AXT_RT_QUEUE_CMD_WAIT_TIMEOUT                           = 5012,

    AXT_RT_QUEUE_RSP_ERROR                                  = 5015,
    AXT_RT_QUEUE_RSP_WAIT_ERROR                             = 5016,
    AXT_RT_QUEUE_RSP_WAIT_TIMEOUT                           = 5017,
	
    AXT_RT_MOTION_STILL_CONTI_MOTION                        = 5018,        // 연속보간 구동 중에 WriteClear나 SetAxisMap 등의 함수를 호출하였음.
	
    AXT_RT_MOTION_INVALD_SET                                = 6000,    
    AXT_RT_MOTION_INVALD_RESET                              = 6001,    
    AXT_RT_MOTION_INVALD_ENABLE                             = 6002,
	
    AXT_RT_LICENSE_INVALID                                  = 6500,        // 유효하지않은 License

    AXT_RT_MONITOR_IN_OPERATION                             = 6600,        // 현재 Monitor 기능이 동작중에 있음
    AXT_RT_MONITOR_NOT_OPERATION                            = 6601,        // 현재 Monitor 기능이 동작중이지 않음
    AXT_RT_MONITOR_EMPTY_QUEUE                              = 6602,        // Monitor data queue가 비어있음
    AXT_RT_MONITOR_INVALID_TRIGGER_OPTION                   = 6603,        // 트리거 설정이 유효하지 않음
    AXT_RT_MONITOR_EMPTY_ITEM                               = 6604,        // Item이 비어 있음

    AXT_RT_MACRO_INVALID_MACRO_NO                           = 6700,
    AXT_RT_MACRO_INVALID_NODE_NO                            = 6701,
    AXT_RT_MACRO_INVALID_STOP_MODE                          = 6702,
    AXT_RT_MACRO_MEMORY_MISMATCH                            = 6703,
    AXT_RT_MACRO_CONTROL_LOCKED                             = 6704,
    AXT_RT_MACRO_INVALID_STATUS                             = 6705,
    AXT_RT_MACRO_INVALID_ARGUMENT                           = 6706,

    AXT_RT_MACRO_NOT_NODE_BEGIN                             = 6710,
    AXT_RT_MACRO_NOT_NODE_END                               = 6711,
    AXT_RT_MACRO_ALREADY_BEGIN                              = 6712,
    AXT_RT_MACRO_NODE_EMPTY                                 = 6713,
    AXT_RT_MACRO_IN_OPERATION                               = 6714,
    AXT_RT_MACRO_NOT_OPERATION                              = 6715,
    AXT_RT_MACRO_NOT_SUPPORT_FUNCTION                       = 6716,
    AXT_RT_MACRO_NODE_FULL                                  = 6717,
    AXT_RT_MACRO_NODE_CHECK_ERROR                           = 6720,
    AXT_RT_MACRO_NOT_CHECKED                                = 6721,
    AXT_RT_MACRO_NOT_PAUSED                                 = 6722,         // Macro가 Pause 상태가 아닐때	  

	
    AXT_MK_RT_INVALID_AXIS                                  = 7100,
    AXT_MK_RT_INVALID_AXIS_SIZE                             = 7101,
    AXT_MK_RT_INVALID_COORD                                 = 7102,
    AXT_MK_RT_INVALID_COORD_SIZE                            = 7103,
    AXT_MK_RT_INVALID_AXIS_MAP                              = 7104,
    AXT_MK_RT_INVALID_AXIS_MAP_SIZE                         = 7105,
    AXT_MK_RT_INVALID_VEL                                   = 7106,
    AXT_MK_RT_INVALID_END_VEL                               = 7107,
    AXT_MK_RT_INVALID_ACCEL                                 = 7108,
    AXT_MK_RT_INVALID_DECEL                                 = 7109,
    AXT_MK_RT_INVALID_ABS_REL                               = 7110,
    AXT_MK_RT_INVALID_PROFILE                               = 7111,
    AXT_MK_RT_INVALID_STOP_DECEL                            = 7112,
    AXT_MK_RT_INVALID_STOP_TIME                             = 7113,
    AXT_MK_RT_INVALID_ACCEL_JERK_RATE                       = 7114,
    AXT_MK_RT_INVALID_DECEL_JERK_RATE                       = 7115,
    AXT_MK_RT_INVALID_ACCEL_UNIT                            = 7116,
    AXT_MK_RT_INVALID_DISTANCE                              = 7117,
    AXT_MK_RT_INVALID_ANGLE                                 = 7118,
    AXT_MK_RT_INVALID_BIT                                   = 7119,
    AXT_MK_RT_INVALID_PORT                                  = 7120,
    AXT_MK_RT_INVALID_SPLINE_INDEX                          = 7121,
    AXT_MK_RT_INVALID_THREAD                                = 7122,
    AXT_MK_RT_INVALID_TIMER                                 = 7123,
    AXT_MK_RT_INVALID_SEGMENT_COUNT                         = 7124,
    AXT_MK_RT_INVALID_SEGMENT_NO                            = 7125,
    AXT_MK_RT_INVALID_NODE_NO                               = 7126,
    AXT_MK_RT_INVALID_HWQ_COUNT                             = 7127,
    AXT_MK_RT_INVALID_NODE_SIZE                             = 7128,
    AXT_MK_RT_INVALID_STOP_NODE_SIZE                        = 7129,
    AXT_MK_RT_INVALID_SPLINE_SIZE                           = 7130,
    AXT_MK_RT_INVALID_LINE_LINE_FILLET                      = 7131,
    AXT_MK_RT_INVALID_LINE_ARC_FILLET                       = 7132,
    AXT_MK_RT_INVALID_ARC_LINE_FILLET                       = 7133,
    AXT_MK_RT_INVALID_ARC_ARC_FILLET                        = 7134,
    AXT_MK_RT_INVALID_RESET_FILLET                          = 7135,
    AXT_MK_RT_INVALID_TASK                                  = 7136,
    AXT_MK_RT_INVALID_ROUND_INDEX                           = 7137,
    AXT_MK_RT_INVALID_LOG_DATA                              = 7138,
    AXT_MK_RT_INVALID_LOG10_DATA                            = 7139,
    AXT_MK_RT_INVALID_PORT_NO                               = 7140,
    AXT_MK_RT_INVALID_BAUD_RATE                             = 7141,
    AXT_MK_RT_INVALID_STOP_BIT                              = 7142,
    AXT_MK_RT_INVALID_PARITY                                = 7143,
    AXT_MK_RT_INVALID_EDGE                                  = 7144,
    AXT_MK_RT_INVALID_STOP_MODE                             = 7145,
    AXT_MK_RT_INVALID_TRIGGER_TIME                          = 7146,
    AXT_MK_RT_INVALID_TRIGGER_LEVEL                         = 7147,
    AXT_MK_RT_INVALID_TRIGGER_SELECT                        = 7148,
    AXT_MK_RT_INVALID_TRIGGER_INTERRUPT                     = 7149,
    AXT_MK_RT_INVALID_TRIGGER_METHOD                        = 7150,
    AXT_MK_RT_INVALID_TRIGGER_POSITION                      = 7151,
    AXT_MK_RT_INVALID_TRIGGER_INDEX                         = 7152,
    AXT_MK_RT_INVALID_ECAM_DATA                             = 7153,
    AXT_MK_RT_INVALID_ECAM_POSITION                         = 7154,
    AXT_MK_RT_INVALID_EGEAR_SIZE                            = 7155,
    AXT_MK_RT_INVALID_INDEX                                 = 7156,
    AXT_MK_RT_INVALID_MOTION_MODE                           = 7157,
    AXT_MK_RT_INVALID_SIGNAL                                = 7158,
    AXT_MK_RT_INVALID_STOP_DISTANCE                         = 7159,
    AXT_MK_RT_INVALID_DIRECTION                             = 7160,
    AXT_MK_RT_INVALID_ZERO_VELOCITY                         = 7161,
    AXT_MK_RT_INVALID_COORDINATE_INMOTION                   = 7162,
    AXT_MK_RT_INVALID_COORDINATE_MOTIONDONE                 = 7163,
    AXT_MK_RT_INVALID_BLENDING_MODE                         = 7164,
    AXT_MK_RT_INVALID_BLENDING_VALUE                        = 7165,
    AXT_MK_RT_INVALID_BLENDING_RATIO                        = 7166,
    AXT_MK_RT_INVALID_EGEAR_RATIO                           = 7167,
    AXT_MK_RT_INVALID_SLAVE_AXIS                            = 7168,
    AXT_MK_RT_INVALID_OPERATION_MODE                        = 7169,

    AXT_MK_RT_INVALID_CONTIQ_DISABLE                        = 7170,
    AXT_MK_RT_INVALID_CONTIQ_MODE                           = 7171,
    AXT_MK_RT_INVALID_CONTIQ_ANGLE                          = 7172,
    AXT_MK_RT_INVALID_CONTIQ_VELRATE                        = 7173,
    AXT_MK_RT_INVALID_CONTIQ_FILLET                         = 7174,

    AXT_MK_RT_CONTIQ_AUTO_VEL                               = 7175,
    AXT_MK_RT_CONTIQ_AUTO_ARC                               = 7176,
    AXT_MK_RT_CONTIQ_LINE                                   = 7177,
    AXT_MK_RT_CONTIQ_CIRCLE                                 = 7178,
    AXT_MK_RT_CONTIQ_ARC                                    = 7179,
    AXT_MK_RT_CONTIQ_SYNC_SLAVE                             = 7180,
    AXT_MK_RT_INVALID_SEGMENT_OUTPUT_SIZE                   = 7181,
    AXT_MK_RT_INVALID_SEGMENT_NUMBER                        = 7182,
    AXT_MK_RT_INVALID_TRIG_COUNT                            = 7183,
    AXT_MK_RT_INVALID_TRIG_TIME                             = 7184,
    AXT_MK_RT_NOT_CAPTURED                                  = 7185,
    AXT_MK_RT_INVALID_CAPTURE_INDEX                         = 7186,
    AXT_MK_RT_INVALID_LEVEL                                 = 7187,
    AXT_MK_RT_INVALID_SEGMENT_OUTPUT_MODE                   = 7188,
    AXT_MK_RT_INVALID_SEGMENT_OUTPUT_VALUE                  = 7189,
    AXT_MK_RT_INVALID_SEGMENT_OUTPUT_RATIO                  = 7190,

    AXT_MK_RT_INVALID_SPLINE_POINT_SIZE                     = 7191,
    AXT_MK_RT_INVALID_INMOTION                              = 7192,

    AXT_MK_RT_ENABLE_CONTIQ                                 = 7200,
    AXT_MK_RT_DISABLE_CONTIQ                                = 7201,
    AXT_MK_RT_ENABLE_CONTIQ_SYNC                            = 7202,
    AXT_MK_RT_DISABLE_CONTIQ_SYNC                           = 7203,
    AXT_MK_RT_ENABLE_CONTI                                  = 7204,
    AXT_MK_RT_DISABLE_CONTI                                 = 7205,
    AXT_MK_RT_ENABLE_EGEAR                                  = 7206,
    AXT_MK_RT_DISABLE_EGEAR                                 = 7207,
    AXT_MK_RT_ENABLE_TASK                                   = 7208,
    AXT_MK_RT_DISABLE_TASK                                  = 7209,
    AXT_MK_RT_DISABLE_PORT_NO                               = 7210,

    AXT_MK_RT_ALREADY_OPEN                                  = 7300,
    AXT_MK_RT_ALREADY_CLOSE                                 = 7301,

    AXT_MK_RT_ERROR_HOME                                    = 7400,
    AXT_MK_RT_ERROR_MOTION                                  = 7401,
    AXT_MK_RT_ERROR_INSTOPPING                              = 7402,
    AXT_MK_RT_ERROR_TIME_OUT                                = 7403,
    AXT_MK_RT_ERROR_BUFFER_FULL                             = 7404,
    AXT_MK_RT_ERROR_DATA_CREATE                             = 7405,
    AXT_MK_RT_ERROR_CALCULATION                             = 7406,

    AXT_MK_RT_ERROR_QUEUE_FULL                              = 7500,
    AXT_MK_RT_ERROR_QUEUE_NULL                              = 7501,

    AXT_MK_RT_ERROR_ECAM_TABLE                              = 7502,

    AXT_MK_RT_ERROR_SPLINE_POSITION                         = 7503,

    AXT_MK_RT_INVALID_CIRCULAR_POINT                        = 7510,
    AXT_MK_RT_INVALID_POINT                                 = 7511,
    AXT_MK_RT_INVALID_QUEUE_SIZE                            = 7512,
    AXT_MK_RT_INVALID_POSITION                              = 7513,
    AXT_MK_RT_INVALID_ROTATION                              = 7514,

    AXT_MK_RT_INVALID_TABLE                                 = 7600,
    AXT_MK_RT_INVALID_TABLE_NO                              = 7601,
    AXT_MK_RT_INVALID_TABLE_DATA                            = 7602,
    AXT_MK_RT_INVALID_POSITION_SIZE                         = 7603,

    AXT_MK_RT_INVALID_TABLE_ENABLED                         = 7604,
    AXT_MK_RT_INVALID_TABLE_NOT_ENABLED                     = 7605,
    AXT_MK_RT_INVALID_TABLE_NONE                            = 7606,
    AXT_MK_RT_INVALID_GET_TABLE                             = 7607,
    AXT_MK_RT_INVALID_ENABLE_TABLE                          = 7608,
    AXT_MK_RT_INVALID_DISABLE_TABLE                         = 7609,

    AXT_MK_RT_INVALID_SET                                   = 7610,
    AXT_MK_RT_INVALID_RESET                                 = 7611,
    AXT_MK_RT_INVALID_ENABLE                                = 7612,

    AXT_MK_RT_NOT_SUPPORT                                   = 7900,
    AXT_MK_RT_ERROR                                         = 7901,
    AXT_MK_RT_INVLID_FUNCTION_TYPE                          = 7902,

    AXT_MK_RT_INVALID_ROBOT_SIZE                            = 7700,
    AXT_MK_RT_INVALID_ROBOT_AXIS_SIZE                       = 7701,
    AXT_MK_RT_INVALID_ROBOT_COORD_SIZE                      = 7702,
    AXT_MK_RT_INVALID_ROBOT_NO                              = 7703,
    AXT_MK_RT_INVALID_ROBOT_COORD                           = 7704,
    AXT_MK_RT_INVALID_ROBOT_LIMIT                           = 7705,
    AXT_MK_RT_INVALID_ROBOT_POS_LIMIT                       = 7706,
    AXT_MK_RT_INVALID_ROBOT_NEG_LIMIT                       = 7707,
    AXT_MK_RT_INVALID_ROBOT_VEL_LIMIT                       = 7708,
    AXT_MK_RT_INVALID_ROBOT_ACCEL_LIMIT                     = 7709,
    AXT_MK_RT_INVALID_ROBOT_DECEL_LIMIT                     = 7710,
    AXT_MK_RT_ERROR_ROBOT_CALCULATION                       = 7711,
    AXT_MK_RT_INVALID_FRAME                                 = 7712,
    AXT_MK_RT_INVALID_FRAME_NO                              = 7713,
    AXT_MK_RT_INVALID_FRAME_TYPE                            = 7714,
    AXT_MK_RT_INVALID_OBJECT_NO                             = 7715,
    AXT_MK_RT_INVALID_ROBOT_SYNC                            = 7716,
    AXT_MK_RT_INVALID_ROBOT_SYNC_MOTION                     = 7717,
    AXT_MK_RT_INVALID_ROBOT_SYNC_ENABLE                     = 7718,
    AXT_MK_RT_INVALID_ROBOT_SYNC_DISABLE                    = 7719,
    AXT_MK_RT_INVALID_ROBOT_SYNC_MOTION_MODE                = 7720,
    AXT_MK_RT_INVALID_ROBOT_SYNC_WORK_COORD                 = 7721,
    AXT_MK_RT_INVALID_CAPTURE_POS_NO                        = 7722,
    
    AXT_MK_RT_INVALID_ROBOT_CAPTURE_POS                     = 7730,
    AXT_MK_RT_INVALID_ROBOT_AXIS                            = 7731,
    AXT_MK_RT_INVALID_WORK_NEGATIVE_LIMIT                   = 7732,
    AXT_MK_RT_INVALID_WORK_POSITIVE_LIMIT                   = 7733,

    AXT_MK_RT_INVALID_TOOL_NO                               = 7740,

    AXT_MK_RT_INVALID_FREQUENCY_SIZE                        = 7800,
    AXT_MK_RT_INVALID_IMPULSE_COUNT                         = 7801,
    AXT_MK_RT_INVALID_AMPLITUDE                             = 7802,

    AXT_MK_RT_INVALID_INPUT_SHAPER__NONE                    = 7803,
    AXT_MK_RT_INVALID_INPUT_SHAPER_ENABLED                  = 7804,

    AXT_MK_RT_INVALID_ARRAY_SIZE                            = 7805
} AXT_FUNC_RESULT;
#endif

#ifndef __AFX_H__
#define HWND                                HANDLE
#endif

#ifndef FALSE
#define FALSE                                0
#define TRUE                                 1
#endif

#ifndef WM_USER
#define WM_USER                              0x0400
#endif

#ifndef WM_AXL_INTERRUPT
#define WM_AXL_INTERRUPT                    (WM_USER + 1001)
#endif

#ifndef __AXT_LOG_LEVEL__
#define __AXT_LOG_LEVEL__
typedef enum _AXT_LOG_LEVEL
{
    LEVEL_NONE,
    LEVEL_ERROR,
    LEVEL_RUNSTOP,
    LEVEL_FUNCTION
} AXT_LOG_LEVEL;
#endif

#ifndef __AXT_EXISTENCE__
#define __AXT_EXISTENCE__
typedef enum _AXT_EXISTENCE
{
    STATUS_NOTEXIST,
    STATUS_EXIST
} AXT_EXISTENCE;
#endif
 
#ifndef __AXT_USE__
#define __AXT_USE__
typedef enum _AXT_USE
{
    DISABLE,
    ENABLE
} AXT_USE;
#endif

//////////////////////////////////////////////////////////////////////////
#ifndef __AXT_AIO_TRIGGER_MODE__
#define __AXT_AIO_TRIGGER_MODE__
typedef enum _AXT_AIO_TRIGGER_MODE
{
    DISABLE_MODE                                            = 0,
    NORMAL_MODE                                             = 1,
    TIMER_MODE, 
    EXTERNAL_MODE
} AXT_AIO_TRIGGER_MODE;
#endif

#ifndef __AXT_AIO_FULL_MODE__
#define __AXT_AIO_FULL_MODE__
typedef enum _AXT_AIO_FULL_MODE
{
    NEW_DATA_KEEP,
    CURR_DATA_KEEP
} AXT_AIO_FULL_MODE;
#endif

#ifndef __AXT_AIO_EVENT_MASK__
#define __AXT_AIO_EVENT_MASK__
typedef enum _AXT_AIO_EVENT_MASK             //AIO 쪽 하드웨어 인터럽트가 추가 되면서 명칭의 혼란을 우려해서 수정함. 
{
    DATA_EMPTY                                              = 0x01,
    DATA_MANY                                               = 0x02,
    DATA_SMALL                                              = 0x04,
    DATA_FULL                                               = 0x08
} AXT_AIO_EVENT_MASK;
#endif

#ifndef __AXT_AIO_INTERRUPT_MASK__
#define __AXT_AIO_INTERRUPT_MASK__
typedef enum _AXT_AIO_INTERRUPT_MASK         //AIO 쪽 하드웨어 인터럽트가 추가되면서 기존의 내용을 변경함. 
{
    ADC_DONE                                                = 0x00,
    SCAN_END                                                = 0x01,
    FIFO_HALF_FULL                                          = 0x02,
    NO_SIGNAL                                               = 0x03
} AXT_AIO_INTERRUPT_MASK;
#endif
#ifndef __AXT_AIO_QUEUE_EVENT__
#define __AXT_AIO_QUEUE_EVENT__              //AIO쪽 이벤트 /콜백 /윈도우 메시지의 파라메터 값에 대한 정의 추가 
typedef enum  _AXT_AIO_EVENT_MODE  
{
    AIO_EVENT_DATA_RESET                                    = 0x00, 
    AIO_EVENT_DATA_UPPER, 
    AIO_EVENT_DATA_LOWER, 
    AIO_EVENT_DATA_FULL, 
    AIO_EVENT_DATA_EMPTY 
} AXT_AIO_EVENT_MODE;
#endif

#ifndef AXT_AIO_FIFO_STATUS_DEF
#define AXT_AIO_FIFO_STATUS_DEF
typedef enum _AXT_AIO_FIFO_STATUS
{
    FIFO_DATA_EXIST                                         = 0,
    FIFO_DATA_EMPTY                                         = 1,
    FIFO_DATA_HALF                                          = 2,
    FIFO_DATA_FULL                                          = 6
} AXT_AIO_FIFO_STATUS;
#endif

#ifndef AXT_AIO_EXTERNAL_STATUS_DEF
#define AXT_AIO_EXTERNAL_STATUS_DEF
typedef enum _AXT_AIO_EXTERNAL_STATUS
{
    EXTERNAL_DATA_DONE                                      = 0,
    EXTERNAL_DATA_FINE                                      = 1,
    EXTERNAL_DATA_HALF                                      = 2,
    EXTERNAL_DATA_FULL                                      = 3,
    EXTERNAL_COMPLETE                                       = 4
} AXT_AIO_EXTERNAL_STATUS;
#endif

//////////////////////////////////////////////////////////////////////////
#ifndef __AXT_DIO_EDGE__
#define __AXT_DIO_EDGE__
typedef enum _AXT_DIO_EDGE
{
    DOWN_EDGE,
    UP_EDGE
} AXT_DIO_EDGE;
#endif

#ifndef __AXT_DIO_STATE__
#define __AXT_DIO_STATE__
typedef enum _AXT_DIO_STATE
{
    OFF_STATE,
    ON_STATE
} AXT_DIO_STATE;
#endif

#ifndef __AXT_DIO_PORT_TYPE__
#define __AXT_DIO_PORT_TYPE__
typedef enum _AXT_DIO_PORT_TYPE
{
	DIO_PORT_OUTPUT, 
	DIO_PORT_INPORT	
} AXT_DIO_PORT_TYPE;
#endif

//////////////////////////////////////////////////////////////////////////
#ifndef AXT_MOTION_STOPMODE_DEF
#define AXT_MOTION_STOPMODE_DEF
typedef enum _AXT_MOTION_STOPMODE
{
    EMERGENCY_STOP,
    SLOWDOWN_STOP
} AXT_MOTION_STOPMODE;
#endif

#ifndef AXT_MOTION_EDGE_DEF
#define AXT_MOTION_EDGE_DEF
typedef enum _AXT_MOTION_EDGE
{
    SIGNAL_DOWN_EDGE,
    SIGNAL_UP_EDGE,
    SIGNAL_LOW_LEVEL,
    SIGNAL_HIGH_LEVEL
} AXT_MOTION_EDGE;
#endif

#ifndef AXT_MOTION_SELECTION_DEF
#define AXT_MOTION_SELECTION_DEF
typedef enum _AXT_MOTION_SELECTION
{
    COMMAND,
    ACTUAL
} AXT_MOTION_SELECTION;
#endif

#ifndef AXT_MOTION_TRIGGER_MODE_DEF
#define AXT_MOTION_TRIGGER_MODE_DEF
typedef enum _AXT_MOTION_TRIGGER_MODE
{
    PERIOD_MODE,
    ABS_POS_MODE
} AXT_MOTION_TRIGGER_MODE;
#endif

#ifndef AXT_MOTION_LEVEL_MODE_DEF
#define AXT_MOTION_LEVEL_MODE_DEF
typedef enum _AXT_MOTION_LEVEL_MODE
{
    LOW,
    HIGH,
    UNUSED,
    USED
} AXT_MOTION_LEVEL_MODE;
#endif

#ifndef AXT_MOTION_ABSREL_MODE_DEF
#define AXT_MOTION_ABSREL_MODE_DEF
typedef enum _AXT_MOTION_ABSREL_MODE
{
    POS_ABS_MODE,
    POS_REL_MODE,
	POS_ABS_LONG_MODE,
	POS_ABS_SHORT_MODE
} AXT_MOTION_ABSREL_MODE;
#endif

#ifndef AXT_MOTION_ENCODER_TYPE_DEF
#define AXT_MOTION_ENCODER_TYPE_DEF
typedef enum _AXT_MOTION_ENCODER_TYPE
{
    ENCODER_TYPE_INCREMENTAL,
    ENCODER_TYPE_ABSOLUTE,
    ENCODER_TYPE_NONE
} AXT_MOTION_ENCODER_TYPE;
#endif

#ifndef AXT_MOTION_PROFILE_MODE_DEF
#define AXT_MOTION_PROFILE_MODE_DEF
typedef enum _AXT_MOTION_PROFILE_MODE
{
    SYM_TRAPEZOIDE_MODE,
    ASYM_TRAPEZOIDE_MODE,
    QUASI_S_CURVE_MODE,
    SYM_S_CURVE_MODE,
    ASYM_S_CURVE_MODE,
    SYM_TRAP_M3_SW_MODE,     //ML-3 Only, Support Velocity profile
    ASYM_TRAP_M3_SW_MODE,    //ML-3 Only, Support Velocity profile
    SYM_S_M3_SW_MODE,        //ML-3 Only, Support Velocity profile
    ASYM_S_M3_SW_MODE        //ML-3 Only, Support Velocity profile
} AXT_MOTION_PROFILE_MODE;
#endif

#ifndef AXT_MOTION_SIGNAL_LEVEL_DEF
#define AXT_MOTION_SIGNAL_LEVEL_DEF
typedef enum _AXT_MOTION_SIGNAL_LEVEL
{
    INACTIVE,
    ACTIVE
} AXT_MOTION_SIGNAL_LEVEL;
#endif

#ifndef AXT_MOTION_HOME_RESULT_DEF
#define AXT_MOTION_HOME_RESULT_DEF
typedef enum AXT_MOTION_HOME_RESULT
{
	HOME_RESERVED                                           = 0x00,    // ML3
	HOME_SUCCESS                                            = 0x01,    // 원점 검색 완료
	HOME_SEARCHING                                          = 0x02,    // 원점 검색 중
	HOME_ERR_GNT_RANGE                                      = 0x10,    // 갠트리 원점 검색 기준, 두 축 사이의 설정이상 오차 발생
	HOME_ERR_USER_BREAK                                     = 0x11,    // 원점 검색 사용자 중지시
	HOME_ERR_VELOCITY                                       = 0x12,    // 원점 검색 속도 이상 에러 발생
	HOME_ERR_AMP_FAULT                                      = 0x13,    // 서보팩 알람 발생 에러
	HOME_ERR_NEG_LIMIT                                      = 0x14,    // (-)방향 구동중 (+)리미트 센서 감지 에러
	HOME_ERR_POS_LIMIT                                      = 0x15,    // (+)방향 구동중 (-)리미트 센서 감지 에러
	HOME_ERR_NOT_DETECT                                     = 0x16,    // 지정한 신호 검출하지 못 할 경우 에러
	HOME_ERR_SETTING                                        = 0x17,    // 사용자 설정 파라미터가 구동시 제약 조건 발생시
	HOME_ERR_SERVO_OFF                                      = 0x18,    // 서보 Off일경우
	HOME_ERR_INTERLOCK                                      = 0x19,    // Interlock 상태 발생
	HOME_ERR_TIMEOUT                                        = 0x20,    // 지정된 시간 초과 발생으로 오류 발생 
	HOME_ERR_POS_CLEAR										= 0x21,    // 0점설정오류
	HOME_ERR_FUNCALL                                        = 0x30,    // 함수 호출 실패
	HOME_ERR_HOME_METHOD                                    = 0x31,    // 지원하지않는 원점검색 방법임.
	HOME_ERR_COUPLING                                       = 0x40,    // Gantry Master to Slave Over Distance protection
	HOME_ERR_ESTOP											= 0x41,	   // EStop 센서 감지 에러
	HOME_ERR_UNKNOWN                                        = 0xFF     // 미지정 에러
} AXT_MOTION_HOME_RESULT;
#endif                                                       
                                                             
#ifndef AXT_MOTION_UNIV_INPUT_DEF                            
#define AXT_MOTION_UNIV_INPUT_DEF                            
typedef enum _AXT_MOTION_UNIV_INPUT                          
{                                                            
    UIO_INP0,                                                
    UIO_INP1,                                                
    UIO_INP2,                                                
    UIO_INP3,                                                
    UIO_INP4,
    UIO_INP5
} AXT_MOTION_UNIV_INPUT;
#endif

#ifndef AXT_MOTION_UNIV_OUTPUT_DEF
#define AXT_MOTION_UNIV_OUTPUT_DEF
typedef enum _AXT_MOTION_UNIV_OUTPUT
{
    UIO_OUT0,
    UIO_OUT1,
    UIO_OUT2,
    UIO_OUT3,
    UIO_OUT4,
    UIO_OUT5
} AXT_MOTION_UNIV_OUTPUT;
#endif


#ifndef AXT_MOTION_DETECT_DOWN_START_POINT_DEF
#define AXT_MOTION_DETECT_DOWN_START_POINT_DEF
typedef enum _AXT_MOTION_DETECT_DOWN_START_POINT
{
    AutoDetect,
    RestPulse
} AXT_MOTION_DETECT_DOWN_START_POINT;
#endif

#ifndef AXT_MOTION_PULSE_OUTPUT_DEF
#define AXT_MOTION_PULSE_OUTPUT_DEF
/* Pulse Output Method                    */
typedef enum _AXT_MOTION__PULSE_OUTPUT 
{
    OneHighLowHigh                                          = 0x0,    // 1펄스 방식, PULSE(Active High), 정방향(DIR=Low)  / 역방향(DIR=High)
    OneHighHighLow                                          = 0x1,    // 1펄스 방식, PULSE(Active High), 정방향(DIR=High) / 역방향(DIR=Low)
    OneLowLowHigh                                           = 0x2,    // 1펄스 방식, PULSE(Active Low),  정방향(DIR=Low)  / 역방향(DIR=High)
    OneLowHighLow                                           = 0x3,    // 1펄스 방식, PULSE(Active Low),  정방향(DIR=High) / 역방향(DIR=Low)
    TwoCcwCwHigh                                            = 0x4,    // 2펄스 방식, PULSE(CCW:역방향),  DIR(CW:정방향),  Active High     
    TwoCcwCwLow                                             = 0x5,    // 2펄스 방식, PULSE(CCW:역방향),  DIR(CW:정방향),  Active Low     
    TwoCwCcwHigh                                            = 0x6,    // 2펄스 방식, PULSE(CW:정방향),   DIR(CCW:역방향), Active High
    TwoCwCcwLow                                             = 0x7,    // 2펄스 방식, PULSE(CW:정방향),   DIR(CCW:역방향), Active Low
    TwoPhase                                                = 0x8,    // 2상(90' 위상차),  PULSE lead DIR(CW: 정방향), PULSE lag DIR(CCW:역방향)
    TwoPhaseReverse                                         = 0x9     // 2상(90' 위상차),  PULSE lead DIR(CCW: 정방향), PULSE lag DIR(CW:역방향)
} AXT_MOTION_PULSE_OUTPUT;                                   
#endif                                                       
                                                             
#ifndef AXT_MOTION_EXTERNAL_COUNTER_INPUT_DEF                
#define AXT_MOTION_EXTERNAL_COUNTER_INPUT_DEF
/* Mode2 Data   External Counter Input */
typedef enum _AXT_MOTION__EXTERNAL_COUNTER_INPUT
{
    ObverseUpDownMode                                       = 0x0,    // 정방향 Up/Down
    ObverseSqr1Mode                                         = 0x1,    // 정방향 1체배
    ObverseSqr2Mode                                         = 0x2,    // 정방향 2체배
    ObverseSqr4Mode                                         = 0x3,    // 정방향 4체배
    ReverseUpDownMode                                       = 0x4,    // 역방향 Up/Down
    ReverseSqr1Mode                                         = 0x5,    // 역방향 1체배
    ReverseSqr2Mode                                         = 0x6,    // 역방향 2체배
    ReverseSqr4Mode                                         = 0x7     // 역방향 4체배
} AXT_MOTION_EXTERNAL_COUNTER_INPUT;                        
#endif                                                      
                                                            
#ifndef AXT_MOTION_ACC_UNIT_DEF                             
#define AXT_MOTION_ACC_UNIT_DEF                             
typedef enum _AXT_MOTION_ACC_UNIT                           
{                                                           
    UNIT_SEC2                                               = 0x0,    // unit/sec2
    SEC                                                     = 0x1     // sec
} AXT_MOTION_ACC_UNIT;
#endif

#ifndef AXT_MOTION_MOVE_DIR_DEF
#define AXT_MOTION_MOVE_DIR_DEF
typedef enum _AXT_MOTION_MOVE_DIR
{
    DIR_CCW                                                 = 0x0,    // 반시계방향, 역방향(조그 구동 기준)
    DIR_CW                                                  = 0x1     // 시계방향, 정방향(조그 구동 기준)
} AXT_MOTION_MOVE_DIR;
#endif

#ifndef AXT_MOTION_RADIUS_DISTANCE_DEF
#define AXT_MOTION_RADIUS_DISTANCE_DEF
typedef enum _AXT_MOTION_RADIUS_DISTANCE
{
    SHORT_DISTANCE                                          = 0x0,    // 짧은 거리의 원호 이동 
    LONG_DISTANCE                                           = 0x1     // 긴 거리의 원호 이동 
} AXT_MOTION_RADIUS_DISTANCE;
#endif

#ifndef AXT_MOTION_POS_TYPE_DEF
#define AXT_MOTION_POS_TYPE_DEF
typedef enum _AXT_MOTION_POS_TYPE_DEF
{
    POSITION_LIMIT                                          = 0x0,    // 전체 영역사용
    POSITION_BOUND                                          = 0x1     // Pos 지정 사용
} AXT_MOTION_POS_TYPE;
#endif

#ifndef AXT_MOTION_INTERPOLATION_AXIS_DEF
#define AXT_MOTION_INTERPOLATION_AXIS_DEF
typedef enum _AXT_MOTION_INTERPOLATION_AXIS
{
    INTERPOLATION_AXIS2                                     = 0x2,    // 2축을 보간으로 사용할 때
    INTERPOLATION_AXIS3                                     = 0x3,    // 3축을 보간으로 사용할 때
    INTERPOLATION_AXIS4                                     = 0x4     // 4축을 보간으로 사용할 때
} AXT_MOTION_INTERPOLATION_AXIS;
#endif

#ifndef AXT_MOTION_CONTISTART_NODE_DEF
#define AXT_MOTION_CONTISTART_NODE_DEF
typedef enum _AXT_MOTION_CONTISTART_NODE
{
    CONTI_NODE_VELOCITY                                     = 0x0,    // 속도 지정 보간 모드
    CONTI_NODE_MANUAL                                       = 0x1,    // 노드 가감속 보간 모드
    CONTI_NODE_AUTO                                         = 0x2     // 자동 가감속 보간 모드
} AXT_MOTION_CONTISTART_NODE;
#endif

#ifndef AXT_MOTION_LINE_MOVE_VEL_MODE_DEF
#define AXT_MOTION_LINE_MOVE_VEL_MODE_DEF
typedef enum _AXT_MOTION_LINE_MOVE_VEL_MODE
{
	LINE_MOVE_VEL_MODE_VECTOR                               = 0x0,    // 속도 벡터
	LINE_MOVE_VEL_MODE_MASTER                               = 0x1,    // 속도 지정 축 기준
	LINE_MOVE_VEL_MODE_LONG_DISTANCE                        = 0x2     // 속도 최대 이동 거리축 기준
} AXT_MOTION_LINE_MOVE_VEL_MODE;
#endif

#ifndef AXT_MOTION_HOME_DETECT_SIGNAL_DEF
#define AXT_MOTION_HOME_DETECT_SIGNAL_DEF
typedef enum _AXT_MOTION_HOME_DETECT_SIGNAL
{
    PosEndLimit                                             = 0x0,    // +Elm(End limit) +방향 리미트 센서 신호
    NegEndLimit                                             = 0x1,    // -Elm(End limit) -방향 리미트 센서 신호
    PosSloLimit                                             = 0x2,    // +Slm(Slow Down limit) 신호 - 사용하지 않음
    NegSloLimit                                             = 0x3,    // -Slm(Slow Down limit) 신호 - 사용하지 않음
    HomeSensor                                              = 0x4,    // IN0(ORG)  원점 센서 신호
    EncodZPhase                                             = 0x5,    // IN1(Z상)  Encoder Z상 신호
    UniInput02                                              = 0x6,    // IN2(범용) 범용 입력 2번 신호
    UniInput03                                              = 0x7,    // IN3(범용) 범용 입력 3번 신호
    UniInput04                                              = 0x8,    // IN4(범용) 범용 입력 4번 신호
    UniInput05                                              = 0x9,    // IN5(범용) 범용 입력 5번 신호
	HomeDetectSignalTorqueLimit                             = 0x10,   // Motor Torque Limit 신호를 이용 
	HomingMethod                                            = 0x64,   // Homing Method Base
	HomingMethod_01                                         = 0x65,   // Homing Method Start(90 ~ 137) 
	//...
	HomingMethod_37                                         = 0x89,   // Homing Method Start(90 ~ 137)  
	Manufacture										        = 0xFF	  // 서보 전용의 특주 홈서치 동작 수행.
} AXT_MOTION_HOME_DETECT_SIGNAL;
#endif

#ifndef AXT_MOTION_HOME_INTERLOCK_DEF
#define AXT_MOTION_HOME_INTERLOCK_DEF
typedef enum _AXT_MOTION_HOME_INTERLOCK_MODE
{
    HOME_INTERLOCK_UNUSED                                   = 0x0,    // Home Interlock 사용하지 않음
    HOME_INTERLOCK_SENSOR_CHECK                             = 0x1,    // Limit 센서가 감지되었을 때 원점센서가 같이 감지되지않았을 때
    HOME_INTERLOCK_DISTANCE                                 = 0x2,    // 원점검색 진행 방향에 설치된 Limit에서 원점센서까지의 거리가 클 때  
} AXT_MOTION_HOME_INTERLOCK_MODE;
#endif


#ifndef AXT_MOTION_INPUT_FILTER_SIGNAL_DEF
#define AXT_MOTION_INPUT_FILTER_SIGNAL_DEF
typedef enum _AXT_MOTION_INPUT_FILTER_SIGNAL
{
    END_LIMIT                                               = 0x10,    // End limit +/-방향 리미트 센서 신호
    INP_ALARM                                               = 0x11,    // Inposition/Alarm 신호
    UIN_00_01                                               = 0x12,    // Home/Z-Phase 신호
    UIN_02_04                                               = 0x13,    // UIN 2, 3, 4 신호       
} AXT_MOTION_INPUT_FILTER_SIGNAL;
#endif

#ifndef AXT_MOTION_MPG_INPUT_METHOD_DEF                      
#define AXT_MOTION_MPG_INPUT_METHOD_DEF                      
typedef enum _AXT_MOTION_MPG_INPUT_METHOD                    
{                                                            
    MPG_DIFF_ONE_PHASE                                      = 0x0,    // MPG 입력 방식 One Phase
    MPG_DIFF_TWO_PHASE_1X                                   = 0x1,    // MPG 입력 방식 TwoPhase1
    MPG_DIFF_TWO_PHASE_2X                                   = 0x2,    // MPG 입력 방식 TwoPhase2
    MPG_DIFF_TWO_PHASE_4X                                   = 0x3,    // MPG 입력 방식 TwoPhase4
    MPG_LEVEL_ONE_PHASE                                     = 0x4,    // MPG 입력 방식 Level One Phase
    MPG_LEVEL_TWO_PHASE_1X                                  = 0x5,    // MPG 입력 방식 Level Two Phase1
    MPG_LEVEL_TWO_PHASE_2X                                  = 0x6,    // MPG 입력 방식 Level Two Phase2
    MPG_LEVEL_TWO_PHASE_4X                                  = 0x7,    // MPG 입력 방식 Level Two Phase4
} AXT_MOTION_MPG_INPUT_METHOD;                               
#endif                                                       
                                                             
#ifndef AXT_MOTION_SENSOR_INPUT_METHOD_DEF                   
#define AXT_MOTION_SENSOR_INPUT_METHOD_DEF                   
typedef enum _AXT_MOTION_SENSOR_INPUT_METHOD                 
{
    SENSOR_METHOD1                                          = 0x0,    // 일반 구동
    SENSOR_METHOD2                                          = 0x1,    // 센서 신호 검출 전은 저속 구동. 신호 검출 후 일반 구동
    SENSOR_METHOD3                                          = 0x2     // 저속 구동
} AXT_MOTION_SENSOR_INPUT_METHOD;
#endif

#ifndef AXT_MOTION_HOME_CRC_SELECT_DEF
#define AXT_MOTION_HOME_CRC_SELECT_DEF
typedef enum _AXT_MOTION_HOME_CRC_SELECT
{
    CRC_SELECT1                                             = 0x0,    // 위치클리어 사용않함, 잔여펄스 클리어 사용 안함
    CRC_SELECT2                                             = 0x1,    // 위치클리어 사용함, 잔여펄스 클리어 사용 안함
    CRC_SELECT3                                             = 0x2,    // 위치클리어 사용안함, 잔여펄스 클리어 사용함
    CRC_SELECT4                                             = 0x3     // 위치클리어 사용함, 잔여펄스 클리어 사용함
} AXT_MOTION_HOME_CRC_SELECT;
#endif

#ifndef AXT_MOTION_IPDETECT_DESTINATION_SIGNAL_DEF
#define AXT_MOTION_IPDETECT_DESTINATION_SIGNAL_DEF
/* Detect Destination Signal            */
typedef enum _AXT_MOTION_IPDETECT_DESTINATION_SIGNAL
{
    PElmNegativeEdge                                        = 0x0,    // +Elm(End limit) 하강 edge
    NElmNegativeEdge                                        = 0x1,    // -Elm(End limit) 하강 edge
    PSlmNegativeEdge                                        = 0x2,    // +Slm(Slowdown limit) 하강 edge
    NSlmNegativeEdge                                        = 0x3,    // -Slm(Slowdown limit) 하강 edge
    In0DownEdge                                             = 0x4,    // IN0(ORG) 하강 edge
    In1DownEdge                                             = 0x5,    // IN1(Z상) 하강 edge
    In2DownEdge                                             = 0x6,    // IN2(범용) 하강 edge
    In3DownEdge                                             = 0x7,    // IN3(범용) 하강 edge
    PElmPositiveEdge                                        = 0x8,    // +Elm(End limit) 상승 edge
    NElmPositiveEdge                                        = 0x9,    // -Elm(End limit) 상승 edge
    PSlmPositiveEdge                                        = 0xa,    // +Slm(Slowdown limit) 상승 edge
    NSlmPositiveEdge                                        = 0xb,    // -Slm(Slowdown limit) 상승 edge
    In0UpEdge                                               = 0xc,    // IN0(ORG) 상승 edge
    In1UpEdge                                               = 0xd,    // IN1(Z상) 상승 edge
    In2UpEdge                                               = 0xe,    // IN2(범용) 상승 edge
    In3UpEdge                                               = 0xf     // IN3(범용) 상승 edge
} AXT_MOTION_IPDETECT_DESTINATION_SIGNAL;
#endif

#ifndef AXT_MOTION_IPEND_STATUS_DEF
#define AXT_MOTION_IPEND_STATUS_DEF
/* IP End status : 0x0000이면 정상종료    */
typedef    enum _AXT_MOTION_IPEND_STATUS
{
    IPEND_STATUS_SLM                                        = 0x0001,    // Bit 0, limit 감속정지 신호 입력에 의한 종료
    IPEND_STATUS_ELM                                        = 0x0002,    // Bit 1, limit 급정지 신호 입력에 의한 종료
    IPEND_STATUS_SSTOP_SIGNAL                               = 0x0004,    // Bit 2, 감속 정지 신호 입력에 의한 종료
    IPEND_STATUS_ESTOP_SIGNAL                               = 0x0008,    // Bit 3, 급정지 신호 입력에 의한 종료
    IPEND_STATUS_SSTOP_COMMAND                              = 0x0010,    // Bit 4, 감속 정지 명령에 의한 종료
    IPEND_STATUS_ESTOP_COMMAND                              = 0x0020,    // Bit 5, 급정지 정지 명령에 의한 종료
    IPEND_STATUS_ALARM_SIGNAL                               = 0x0040,    // Bit 6, Alarm 신호 입력에 희한 종료
    IPEND_STATUS_DATA_ERROR                                 = 0x0080,    // Bit 7, 데이터 설정 에러에 의한 종료
    IPEND_STATUS_DEVIATION_ERROR                            = 0x0100,    // Bit 8, 탈조 에러에 의한 종료
    IPEND_STATUS_ORIGIN_DETECT                              = 0x0200,    // Bit 9, 원점 검출에 의한 종료
    IPEND_STATUS_SIGNAL_DETECT                              = 0x0400,    // Bit 10, 신호 검출에 의한 종료(Signal search-1/2 drive 종료)
    IPEND_STATUS_PRESET_PULSE_DRIVE                         = 0x0800,    // Bit 11, Preset pulse drive 종료
    IPEND_STATUS_SENSOR_PULSE_DRIVE                         = 0x1000,    // Bit 12, Sensor pulse drive 종료
    IPEND_STATUS_LIMIT                                      = 0x2000,    // Bit 13, Limit 완전정지에 의한 종료
    IPEND_STATUS_SOFTLIMIT                                  = 0x4000,    // Bit 14, Soft limit에 의한 종료
    IPEND_STATUS_INTERPOLATION_DRIVE                        = 0x8000     // Bit 15, 보간 드라이브에 의한 종료.
} AXT_MOTION_IPEND_STATUS;
#endif

#ifndef AXT_MOTION_IPDRIVE_STATUS_DEF
#define AXT_MOTION_IPDRIVE_STATUS_DEF
/* IP Drive status                        */
typedef    enum _AXT_MOTION_IPDRIVE_STATUS
{
    IPDRIVE_STATUS_BUSY                                     = 0x00001,    // Bit 0, BUSY(드라이브 구동 중)
    IPDRIVE_STATUS_DOWN                                     = 0x00002,    // Bit 1, DOWN(감속 중)
    IPDRIVE_STATUS_CONST                                    = 0x00004,    // Bit 2, CONST(등속 중)
    IPDRIVE_STATUS_UP                                       = 0x00008,    // Bit 3, UP(가속 중)
    IPDRIVE_STATUS_ICL                                      = 0x00010,    // Bit 4, ICL(내부 위치 카운터 < 내부 위치 카운터 비교값)
    IPDRIVE_STATUS_ICG                                      = 0x00020,    // Bit 5, ICG(내부 위치 카운터 > 내부 위치 카운터 비교값)
    IPDRIVE_STATUS_ECL                                      = 0x00040,    // Bit 6, ECL(외부 위치 카운터 < 외부 위치 카운터 비교값)
    IPDRIVE_STATUS_ECG                                      = 0x00080,    // Bit 7, ECG(외부 위치 카운터 > 외부 위치 카운터 비교값)
    IPDRIVE_STATUS_DRIVE_DIRECTION                          = 0x00100,    // Bit 8, 드라이브 방향 신호(0=CW/1=CCW)
    IPDRIVE_STATUS_COMMAND_BUSY                             = 0x00200,    // Bit 9, 명령어 수행중
    IPDRIVE_STATUS_PRESET_DRIVING                           = 0x00400,    // Bit 10, Preset pulse drive 중
    IPDRIVE_STATUS_CONTINUOUS_DRIVING                       = 0x00800,    // Bit 11, Continuouse speed drive 중
    IPDRIVE_STATUS_SIGNAL_SEARCH_DRIVING                    = 0x01000,    // Bit 12, Signal search-1/2 drive 중
    IPDRIVE_STATUS_ORG_SEARCH_DRIVING                       = 0x02000,    // Bit 13, 원점 검출 drive 중
    IPDRIVE_STATUS_MPG_DRIVING                              = 0x04000,    // Bit 14, MPG drive 중
    IPDRIVE_STATUS_SENSOR_DRIVING                           = 0x08000,    // Bit 15, Sensor positioning drive 중
    IPDRIVE_STATUS_L_C_INTERPOLATION                        = 0x10000,    // Bit 16, 직선/원호 보간 중
    IPDRIVE_STATUS_PATTERN_INTERPOLATION                    = 0x20000,    // Bit 17, 비트 패턴 보간 중
    IPDRIVE_STATUS_INTERRUPT_BANK1                          = 0x40000,    // Bit 18, 인터럽트 bank1에서 발생
    IPDRIVE_STATUS_INTERRUPT_BANK2                          = 0x80000     // Bit 19, 인터럽트 bank2에서 발생
} AXT_MOTION_IPDRIVE_STATUS;
#endif

#ifndef AXT_MOTION_IPINTERRUPT_BANK1_DEF
#define AXT_MOTION_IPINTERRUPT_BANK1_DEF
/* IP Interrupt MASK setting            */
typedef    enum _AXT_MOTION_IPINTERRUPT_BANK1
{
    IPINTBANK1_DONTUSE                                      = 0x00000000,    // INTERRUT DISABLED.
    IPINTBANK1_DRIVE_END                                    = 0x00000001,    // Bit 0, Drive end(default value : 1).
    IPINTBANK1_ICG                                          = 0x00000002,    // Bit 1, INCNT is greater than INCNTCMP.
    IPINTBANK1_ICE                                          = 0x00000004,    // Bit 2, INCNT is equal with INCNTCMP.
    IPINTBANK1_ICL                                          = 0x00000008,    // Bit 3, INCNT is less than INCNTCMP.
    IPINTBANK1_ECG                                          = 0x00000010,    // Bit 4, EXCNT is greater than EXCNTCMP.
    IPINTBANK1_ECE                                          = 0x00000020,    // Bit 5, EXCNT is equal with EXCNTCMP.
    IPINTBANK1_ECL                                          = 0x00000040,    // Bit 6, EXCNT is less than EXCNTCMP.
    IPINTBANK1_SCRQEMPTY                                    = 0x00000080,    // Bit 7, Script control queue is empty.
    IPINTBANK1_CAPRQEMPTY                                   = 0x00000100,    // Bit 8, Caption result data queue is empty.
    IPINTBANK1_SCRREG1EXE                                   = 0x00000200,    // Bit 9, Script control register-1 command is executed.
    IPINTBANK1_SCRREG2EXE                                   = 0x00000400,    // Bit 10, Script control register-2 command is executed.
    IPINTBANK1_SCRREG3EXE                                   = 0x00000800,    // Bit 11, Script control register-3 command is executed.
    IPINTBANK1_CAPREG1EXE                                   = 0x00001000,    // Bit 12, Caption control register-1 command is executed.
    IPINTBANK1_CAPREG2EXE                                   = 0x00002000,    // Bit 13, Caption control register-2 command is executed.
    IPINTBANK1_CAPREG3EXE                                   = 0x00004000,    // Bit 14, Caption control register-3 command is executed.
    IPINTBANK1_INTGGENCMD                                   = 0x00008000,    // Bit 15, Interrupt generation command is executed(0xFF)
    IPINTBANK1_DOWN                                         = 0x00010000,    // Bit 16, At starting point for deceleration drive.
    IPINTBANK1_CONT                                         = 0x00020000,    // Bit 17, At starting point for constant speed drive.
    IPINTBANK1_UP                                           = 0x00040000,    // Bit 18, At starting point for acceleration drive.
    IPINTBANK1_SIGNALDETECTED                               = 0x00080000,    // Bit 19, Signal assigned in MODE1 is detected.
    IPINTBANK1_SP23E                                        = 0x00100000,    // Bit 20, Current speed is equal with rate change point RCP23.
    IPINTBANK1_SP12E                                        = 0x00200000,    // Bit 21, Current speed is equal with rate change point RCP12.
    IPINTBANK1_SPE                                          = 0x00400000,    // Bit 22, Current speed is equal with speed comparison data(SPDCMP).
    IPINTBANK1_INCEICM                                      = 0x00800000,    // Bit 23, INTCNT(1'st counter) is equal with ICM(1'st count minus limit data)
    IPINTBANK1_SCRQEXE                                      = 0x01000000,    // Bit 24, Script queue command is executed When SCRCONQ's 30 bit is '1'.
    IPINTBANK1_CAPQEXE                                      = 0x02000000,    // Bit 25, Caption queue command is executed When CAPCONQ's 30 bit is '1'.
    IPINTBANK1_SLM                                          = 0x04000000,    // Bit 26, NSLM/PSLM input signal is activated.
    IPINTBANK1_ELM                                          = 0x08000000,    // Bit 27, NELM/PELM input signal is activated.
    IPINTBANK1_USERDEFINE1                                  = 0x10000000,    // Bit 28, Selectable interrupt source 0(refer "0xFE" command).
    IPINTBANK1_USERDEFINE2                                  = 0x20000000,    // Bit 29, Selectable interrupt source 1(refer "0xFE" command).
    IPINTBANK1_USERDEFINE3                                  = 0x40000000,    // Bit 30, Selectable interrupt source 2(refer "0xFE" command).
    IPINTBANK1_USERDEFINE4                                  = 0x80000000     // Bit 31, Selectable interrupt source 3(refer "0xFE" command).
} AXT_MOTION_IPINTERRUPT_BANK1;
#endif

#ifndef AXT_MOTION_IPINTERRUPT_BANK2_DEF
#define AXT_MOTION_IPINTERRUPT_BANK2_DEF
typedef    enum _AXT_MOTION_IPINTERRUPT_BANK2
{
    IPINTBANK2_DONTUSE                                      = 0x00000000,    // INTERRUT DISABLED.
    IPINTBANK2_L_C_INP_Q_EMPTY                              = 0x00000001,    // Bit 0, Linear/Circular interpolation parameter queue is empty.
    IPINTBANK2_P_INP_Q_EMPTY                                = 0x00000002,    // Bit 1, Bit pattern interpolation queue is empty.
    IPINTBANK2_ALARM_ERROR                                  = 0x00000004,    // Bit 2, Alarm input signal is activated.
    IPINTBANK2_INPOSITION                                   = 0x00000008,    // Bit 3, Inposition input signal is activated.
    IPINTBANK2_MARK_SIGNAL_HIGH                             = 0x00000010,    // Bit 4, Mark input signal is activated.
    IPINTBANK2_SSTOP_SIGNAL                                 = 0x00000020,    // Bit 5, SSTOP input signal is activated.
    IPINTBANK2_ESTOP_SIGNAL                                 = 0x00000040,    // Bit 6, ESTOP input signal is activated.
    IPINTBANK2_SYNC_ACTIVATED                               = 0x00000080,    // Bit 7, SYNC input signal is activated.
    IPINTBANK2_TRIGGER_ENABLE                               = 0x00000100,    // Bit 8, Trigger output is activated.
    IPINTBANK2_EXCNTCLR                                     = 0x00000200,    // Bit 9, External(2'nd) counter is cleard by EXCNTCLR setting.
    IPINTBANK2_FSTCOMPARE_RESULT_BIT0                       = 0x00000400,    // Bit 10, ALU1's compare result bit 0 is activated.
    IPINTBANK2_FSTCOMPARE_RESULT_BIT1                       = 0x00000800,    // Bit 11, ALU1's compare result bit 1 is activated.
    IPINTBANK2_FSTCOMPARE_RESULT_BIT2                       = 0x00001000,    // Bit 12, ALU1's compare result bit 2 is activated.
    IPINTBANK2_FSTCOMPARE_RESULT_BIT3                       = 0x00002000,    // Bit 13, ALU1's compare result bit 3 is activated.
    IPINTBANK2_FSTCOMPARE_RESULT_BIT4                       = 0x00004000,    // Bit 14, ALU1's compare result bit 4 is activated.
    IPINTBANK2_SNDCOMPARE_RESULT_BIT0                       = 0x00008000,    // Bit 15, ALU2's compare result bit 0 is activated.
    IPINTBANK2_SNDCOMPARE_RESULT_BIT1                       = 0x00010000,    // Bit 16, ALU2's compare result bit 1 is activated.
    IPINTBANK2_SNDCOMPARE_RESULT_BIT2                       = 0x00020000,    // Bit 17, ALU2's compare result bit 2 is activated.
    IPINTBANK2_SNDCOMPARE_RESULT_BIT3                       = 0x00040000,    // Bit 18, ALU2's compare result bit 3 is activated.
    IPINTBANK2_SNDCOMPARE_RESULT_BIT4                       = 0x00080000,    // Bit 19, ALU2's compare result bit 4 is activated.
    IPINTBANK2_L_C_INP_Q_LESS_4                             = 0x00100000,    // Bit 20, Linear/Circular interpolation parameter queue is less than 4.
    IPINTBANK2_P_INP_Q_LESS_4                               = 0x00200000,    // Bit 21, Pattern interpolation parameter queue is less than 4.
    IPINTBANK2_XSYNC_ACTIVATED                              = 0x00400000,    // Bit 22, X axis sync input signal is activated.
    IPINTBANK2_YSYNC_ACTIVATED                              = 0x00800000,    // Bit 23, Y axis sync input siangl is activated.
    IPINTBANK2_P_INP_END_BY_END_PATTERN                     = 0x01000000     // Bit 24, Bit pattern interpolation is terminated by end pattern.
//    IPINTBANK2_                                           = 0x02000000,    // Bit 25, Don't care.
//    IPINTBANK2_                                           = 0x04000000,    // Bit 26, Don't care.
//    IPINTBANK2_                                           = 0x08000000,    // Bit 27, Don't care.
//    IPINTBANK2_                                           = 0x10000000,    // Bit 28, Don't care.
//    IPINTBANK2_                                           = 0x20000000,    // Bit 29, Don't care.
//    IPINTBANK2_                                           = 0x40000000,    // Bit 30, Don't care.
//    IPINTBANK2_                                           = 0x80000000     // Bit 31, Don't care.
} AXT_MOTION_IPINTERRUPT_BANK2;
#endif


#ifndef AXT_MOTION_IPMECHANICAL_SIGNAL_DEF
#define AXT_MOTION_IPMECHANICAL_SIGNAL_DEF
/* IP Drive status                        */
typedef    enum _AXT_MOTION_IPMECHANICAL_SIGNAL
{

    IPMECHANICAL_PELM_LEVEL                                 = 0x0001,    // Bit 0, +Limit 급정지 신호가 액티브 됨
    IPMECHANICAL_NELM_LEVEL                                 = 0x0002,    // Bit 1, -Limit 급정지 신호 액티브 됨
    IPMECHANICAL_PSLM_LEVEL                                 = 0x0004,    // Bit 2, +limit 감속정지 신호 액티브 됨
    IPMECHANICAL_NSLM_LEVEL                                 = 0x0008,    // Bit 3, -limit 감속정지 신호 액티브 됨
    IPMECHANICAL_ALARM_LEVEL                                = 0x0010,    // Bit 4, Alarm 신호 액티브 됨
    IPMECHANICAL_INP_LEVEL                                  = 0x0020,    // Bit 5, Inposition 신호 액티브 됨
    IPMECHANICAL_ENC_DOWN_LEVEL                             = 0x0040,    // Bit 6, 엔코더 DOWN(B상) 신호 입력 Level
    IPMECHANICAL_ENC_UP_LEVEL                               = 0x0080,    // Bit 7, 엔코더 UP(A상) 신호 입력 Level
    IPMECHANICAL_EXMP_LEVEL                                 = 0x0100,    // Bit 8, EXMP 신호 입력 Level
    IPMECHANICAL_EXPP_LEVEL                                 = 0x0200,    // Bit 9, EXPP 신호 입력 Level
    IPMECHANICAL_MARK_LEVEL                                 = 0x0400,    // Bit 10, MARK# 신호 액티브 됨
    IPMECHANICAL_SSTOP_LEVEL                                = 0x0800,    // Bit 11, SSTOP 신호 액티브 됨
    IPMECHANICAL_ESTOP_LEVEL                                = 0x1000,    // Bit 12, ESTOP 신호 액티브 됨
    IPMECHANICAL_SYNC_LEVEL                                 = 0x2000,    // Bit 13, SYNC 신호 입력 Level
    IPMECHANICAL_MODE8_16_LEVEL                             = 0x4000     // Bit 14, MODE8_16 신호 입력 Level
                                                            
} AXT_MOTION_IPMECHANICAL_SIGNAL;
#endif

#ifndef AXT_MOTION_QIDETECT_DESTINATION_SIGNAL_DEF
#define AXT_MOTION_QIDETECT_DESTINATION_SIGNAL_DEF
/* Detect Destination Signal            */
typedef enum _AXT_MOTION_QIDETECT_DESTINATION_SIGNAL
{
    Signal_PosEndLimit                                      = 0x0,    // +Elm(End limit) +방향 리미트 센서 신호
    Signal_NegEndLimit                                      = 0x1,    // -Elm(End limit) -방향 리미트 센서 신호
    Signal_PosSloLimit                                      = 0x2,    // +Slm(Slow Down limit) 신호 - 사용하지 않음
    Signal_NegSloLimit                                      = 0x3,    // -Slm(Slow Down limit) 신호 - 사용하지 않음
    Signal_HomeSensor                                       = 0x4,    // IN0(ORG)  원점 센서 신호
    Signal_EncodZPhase                                      = 0x5,    // IN1(Z상)  Encoder Z상 신호
    Signal_UniInput02                                       = 0x6,    // IN2(범용) 범용 입력 2번 신호
    Signal_UniInput03                                       = 0x7     // IN3(범용) 범용 입력 3번 신호
} AXT_MOTION_QIDETECT_DESTINATION_SIGNAL;
#endif

#ifndef AXT_MOTION_QIMECHANICAL_SIGNAL_DEF
#define AXT_MOTION_QIMECHANICAL_SIGNAL_DEF
/* QI Drive status                        */
typedef    enum _AXT_MOTION_QIMECHANICAL_SIGNAL
{
    QIMECHANICAL_PELM_LEVEL                                 = 0x00001,    // Bit 0, +Limit 급정지 신호 현재 상태
    QIMECHANICAL_NELM_LEVEL                                 = 0x00002,    // Bit 1, -Limit 급정지 신호 현재 상태
    QIMECHANICAL_PSLM_LEVEL                                 = 0x00004,    // Bit 2, +limit 감속정지 현재 상태.
    QIMECHANICAL_NSLM_LEVEL                                 = 0x00008,    // Bit 3, -limit 감속정지 현재 상태
    QIMECHANICAL_ALARM_LEVEL                                = 0x00010,    // Bit 4, Alarm 신호 현재 상태
    QIMECHANICAL_INP_LEVEL                                  = 0x00020,    // Bit 5, Inposition 신호 현재 상태
    QIMECHANICAL_ESTOP_LEVEL                                = 0x00040,    // Bit 6, 비상 정지 신호(ESTOP) 현재 상태.
    QIMECHANICAL_ORG_LEVEL                                  = 0x00080,    // Bit 7, 원점 신호 헌재 상태
    QIMECHANICAL_ZPHASE_LEVEL                               = 0x00100,    // Bit 8, Z 상 입력 신호 현재 상태
    QIMECHANICAL_ECUP_LEVEL                                 = 0x00200,    // Bit 9, ECUP 터미널 신호 상태.
    QIMECHANICAL_ECDN_LEVEL                                 = 0x00400,    // Bit 10, ECDN 터미널 신호 상태.
    QIMECHANICAL_EXPP_LEVEL                                 = 0x00800,    // Bit 11, EXPP 터미널 신호 상태
    QIMECHANICAL_EXMP_LEVEL                                 = 0x01000,    // Bit 12, EXMP 터미널 신호 상태
    QIMECHANICAL_SQSTR1_LEVEL                               = 0x02000,    // Bit 13, SQSTR1 터미널 신호 상태
    QIMECHANICAL_SQSTR2_LEVEL                               = 0x04000,    // Bit 14, SQSTR2 터미널 신호 상태
    QIMECHANICAL_SQSTP1_LEVEL                               = 0x08000,    // Bit 15, SQSTP1 터미널 신호 상태
    QIMECHANICAL_SQSTP2_LEVEL                               = 0x10000,    // Bit 16, SQSTP2 터미널 신호 상태
    QIMECHANICAL_MODE_LEVEL                                 = 0x20000     // Bit 17, MODE 터미널 신호 상태.
} AXT_MOTION_QIMECHANICAL_SIGNAL;
#endif

#ifndef AXT_MOTION_QIEND_STATUS_DEF
#define AXT_MOTION_QIEND_STATUS_DEF
/* QI 종료 status : 0x1FFF로 bit-and 하여 결과가 0x000이면 정상종료    */
typedef    enum _AXT_MOTION_QIEND_STATUS
{
    QIEND_STATUS_0                                          = 0x00000001,    // Bit 0, 정방향 리미트 신호(PELM)에 의한 종료
    QIEND_STATUS_1                                          = 0x00000002,    // Bit 1, 역방향 리미트 신호(NELM)에 의한 종료
    QIEND_STATUS_2                                          = 0x00000004,    // Bit 2, 정방향 부가 리미트 신호(PSLM)에 의한 구동 종료
    QIEND_STATUS_3                                          = 0x00000008,    // Bit 3, 역방향 부가 리미트 신호(NSLM)에 의한 구동 종료
    QIEND_STATUS_4                                          = 0x00000010,    // Bit 4, 정방향 소프트 리미트 급정지 기능에 의한 구동 종료
    QIEND_STATUS_5                                          = 0x00000020,    // Bit 5, 역방향 소프트 리미트 급정지 기능에 의한 구동 종료
    QIEND_STATUS_6                                          = 0x00000040,    // Bit 6, 정방향 소프트 리미트 감속정지 기능에 의한 구동 종료
    QIEND_STATUS_7                                          = 0x00000080,    // Bit 7, 역방향 소프트 리미트 감속정지 기능에 의한 구동 종료
    QIEND_STATUS_8                                          = 0x00000100,    // Bit 8, 서보 알람 기능에 의한 구동 종료.
    QIEND_STATUS_9                                          = 0x00000200,    // Bit 9, 비상 정지 신호 입력에 의한 구동 종료.
    QIEND_STATUS_10                                         = 0x00000400,    // Bit 10, 급 정지 명령에 의한 구동 종료.
    QIEND_STATUS_11                                         = 0x00000800,    // Bit 11, 감속 정지 명령에 의한 구동 종료.
    QIEND_STATUS_12                                         = 0x00001000,    // Bit 12, 전축 급정지 명령에 의한 구동 종료
    QIEND_STATUS_13                                         = 0x00002000,    // Bit 13, 동기 정지 기능 #1(SQSTP1)에 의한 구동 종료.
    QIEND_STATUS_14                                         = 0x00004000,    // Bit 14, 동기 정지 기능 #2(SQSTP2)에 의한 구동 종료.
    QIEND_STATUS_15                                         = 0x00008000,    // Bit 15, 인코더 입력(ECUP,ECDN) 오류 발생
    QIEND_STATUS_16                                         = 0x00010000,    // Bit 16, MPG 입력(EXPP,EXMP) 오류 발생
    QIEND_STATUS_17                                         = 0x00020000,    // Bit 17, 원점 검색 성공 종료.
    QIEND_STATUS_18                                         = 0x00040000,    // Bit 18, 신호 검색 성공 종료.
    QIEND_STATUS_19                                         = 0x00080000,    // Bit 19, 보간 데이터 이상으로 구동 종료.
    QIEND_STATUS_20                                         = 0x00100000,    // Bit 20, 비정상 구동 정지발생.
    QIEND_STATUS_21                                         = 0x00200000,    // Bit 21, MPG 기능 블록 펄스 버퍼 오버플로우 발생
    QIEND_STATUS_22                                         = 0x00400000,    // Bit 22, DON'CARE
    QIEND_STATUS_23                                         = 0x00800000,    // Bit 23, DON'CARE
    QIEND_STATUS_24                                         = 0x01000000,    // Bit 24, DON'CARE
    QIEND_STATUS_25                                         = 0x02000000,    // Bit 25, DON'CARE
    QIEND_STATUS_26                                         = 0x04000000,    // Bit 26, DON'CARE
    QIEND_STATUS_27                                         = 0x08000000,    // Bit 27, DON'CARE
    QIEND_STATUS_28                                         = 0x10000000,    // Bit 28, 현재/마지막 구동 드라이브 방향
    QIEND_STATUS_29                                         = 0x20000000,    // Bit 29, 잔여 펄스 제거 신호 출력 중.
    QIEND_STATUS_30                                         = 0x40000000,    // Bit 30, 비정상 구동 정지 원인 상태
    QIEND_STATUS_31                                         = 0x80000000     // Bit 31, 보간 드라이브 데이타 오류 상태.

} AXT_MOTION_QIEND_STATUS;
#endif

#ifndef AXT_MOTION_QIDRIVE_STATUS_DEF
#define AXT_MOTION_QIDRIVE_STATUS_DEF
/* QI Drive status                        */
typedef    enum _AXT_MOTION_QIDRIVE_STATUS
{
    QIDRIVE_STATUS_0                                        = 0x0000001,    // Bit 0, BUSY(드라이브 구동 중)
    QIDRIVE_STATUS_1                                        = 0x0000002,    // Bit 1, DOWN(감속 중)
    QIDRIVE_STATUS_2                                        = 0x0000004,    // Bit 2, CONST(등속 중)
    QIDRIVE_STATUS_3                                        = 0x0000008,    // Bit 3, UP(가속 중)
    QIDRIVE_STATUS_4                                        = 0x0000010,    // Bit 4, 연속 드라이브 구동 중
    QIDRIVE_STATUS_5                                        = 0x0000020,    // Bit 5, 지정 거리 드라이브 구동 중
    QIDRIVE_STATUS_6                                        = 0x0000040,    // Bit 6, MPG 드라이브 구동 중
    QIDRIVE_STATUS_7                                        = 0x0000080,    // Bit 7, 원점검색 드라이브 구동중
    QIDRIVE_STATUS_8                                        = 0x0000100,    // Bit 8, 신호 검색 드라이브 구동 중
    QIDRIVE_STATUS_9                                        = 0x0000200,    // Bit 9, 보간 드라이브 구동 중
    QIDRIVE_STATUS_10                                       = 0x0000400,    // Bit 10, Slave 드라이브 구동중
    QIDRIVE_STATUS_11                                       = 0x0000800,    // Bit 11, 현재 구동 드라이브 방향(보간 드라이브에서는 표시 정보 다름)
    QIDRIVE_STATUS_12                                       = 0x0001000,    // Bit 12, 펄스 출력후 서보위치 완료 신호 대기중.
    QIDRIVE_STATUS_13                                       = 0x0002000,    // Bit 13, 직선 보간 드라이브 구동중.
    QIDRIVE_STATUS_14                                       = 0x0004000,    // Bit 14, 원호 보간 드라이브 구동중.
    QIDRIVE_STATUS_15                                       = 0x0008000,    // Bit 15, 펄스 출력 중.
    QIDRIVE_STATUS_16                                       = 0x0010000,    // Bit 16, 구동 예약 데이터 개수(처음)(0-7)
    QIDRIVE_STATUS_17                                       = 0x0020000,    // Bit 17, 구동 예약 데이터 개수(중간)(0-7)
    QIDRIVE_STATUS_18                                       = 0x0040000,    // Bit 18, 구동 예약 데이터 갯수(끝)(0-7)
    QIDRIVE_STATUS_19                                       = 0x0080000,    // Bit 19, 구동 예약 Queue 비어 있음.
    QIDRIVE_STATUS_20                                       = 0x0100000,    // Bit 20, 구동 예약 Queue 가득 ?
    QIDRIVE_STATUS_21                                       = 0x0200000,    // Bit 21, 현재 구동 드라이브의 속도 모드(처음)
    QIDRIVE_STATUS_22                                       = 0x0400000,    // Bit 22, 현재 구동 드라이브의 속도 모드(끝)
    QIDRIVE_STATUS_23                                       = 0x0800000,    // Bit 23, MPG 버퍼 #1 Full
    QIDRIVE_STATUS_24                                       = 0x1000000,    // Bit 24, MPG 버퍼 #2 Full
    QIDRIVE_STATUS_25                                       = 0x2000000,    // Bit 25, MPG 버퍼 #3 Full
    QIDRIVE_STATUS_26                                       = 0x4000000     // Bit 26, MPG 버퍼 데이터 OverFlow
} AXT_MOTION_QIDRIVE_STATUS;
#endif

#ifndef AXT_MOTION_QIINTERRUPT_BANK1_DEF
#define AXT_MOTION_QIINTERRUPT_BANK1_DEF
/* QI Interrupt MASK setting            */
typedef    enum _AXT_MOTION_QIINTERRUPT_BANK1
{
    QIINTBANK1_DISABLE                                      = 0x00000000,    // INTERRUT DISABLED.
    QIINTBANK1_0                                            = 0x00000001,    // Bit 0,  인터럽트 발생 사용 설정된 구동 종료시.
    QIINTBANK1_1                                            = 0x00000002,    // Bit 1,  구동 종료시.
    QIINTBANK1_2                                            = 0x00000004,    // Bit 2,  구동 시작시.(주의:라이브러리 내부 설정으로 AXM 모션 구동시 두번 발생할수 있음) 
    QIINTBANK1_3                                            = 0x00000008,    // Bit 3,  카운터 #1 < 비교기 #1 이벤트 발생
    QIINTBANK1_4                                            = 0x00000010,    // Bit 4,  카운터 #1 = 비교기 #1 이벤트 발생
    QIINTBANK1_5                                            = 0x00000020,    // Bit 5,  카운터 #1 > 비교기 #1 이벤트 발생
    QIINTBANK1_6                                            = 0x00000040,    // Bit 6,  카운터 #2 < 비교기 #2 이벤트 발생
    QIINTBANK1_7                                            = 0x00000080,    // Bit 7,  카운터 #2 = 비교기 #2 이벤트 발생
    QIINTBANK1_8                                            = 0x00000100,    // Bit 8,  카운터 #2 > 비교기 #2 이벤트 발생
    QIINTBANK1_9                                            = 0x00000200,    // Bit 9,  카운터 #3 < 비교기 #3 이벤트 발생
    QIINTBANK1_10                                           = 0x00000400,    // Bit 10, 카운터 #3 = 비교기 #3 이벤트 발생
    QIINTBANK1_11                                           = 0x00000800,    // Bit 11, 카운터 #3 > 비교기 #3 이벤트 발생
    QIINTBANK1_12                                           = 0x00001000,    // Bit 12, 카운터 #4 < 비교기 #4 이벤트 발생
    QIINTBANK1_13                                           = 0x00002000,    // Bit 13, 카운터 #4 = 비교기 #4 이벤트 발생
    QIINTBANK1_14                                           = 0x00004000,    // Bit 14, 카운터 #4 < 비교기 #4 이벤트 발생
    QIINTBANK1_15                                           = 0x00008000,    // Bit 15, 카운터 #5 < 비교기 #5 이벤트 발생
    QIINTBANK1_16                                           = 0x00010000,    // Bit 16, 카운터 #5 = 비교기 #5 이벤트 발생
    QIINTBANK1_17                                           = 0x00020000,    // Bit 17, 카운터 #5 > 비교기 #5 이벤트 발생
    QIINTBANK1_18                                           = 0x00040000,    // Bit 18, 타이머 #1 이벤트 발생.
    QIINTBANK1_19                                           = 0x00080000,    // Bit 19, 타이머 #2 이벤트 발생.
    QIINTBANK1_20                                           = 0x00100000,    // Bit 20, 구동 예약 설정 Queue 비워짐.
    QIINTBANK1_21                                           = 0x00200000,    // Bit 21, 구동 예약 설정 Queue 가득?
    QIINTBANK1_22                                           = 0x00400000,    // Bit 22, 트리거 발생거리 주기/절대위치 Queue 비워짐.
    QIINTBANK1_23                                           = 0x00800000,    // Bit 23, 트리거 발생거리 주기/절대위치 Queue 가득?
    QIINTBANK1_24                                           = 0x01000000,    // Bit 24, 트리거 신호 발생 이벤트
    QIINTBANK1_25                                           = 0x02000000,    // Bit 25, 스크립트 #1 명령어 예약 설정 Queue 비워짐.
    QIINTBANK1_26                                           = 0x04000000,    // Bit 26, 스크립트 #2 명령어 예약 설정 Queue 비워짐.
    QIINTBANK1_27                                           = 0x08000000,    // Bit 27, 스크립트 #3 명령어 예약 설정 레지스터 실행되어 초기화 됨.
    QIINTBANK1_28                                           = 0x10000000,    // Bit 28, 스크립트 #4 명령어 예약 설정 레지스터 실행되어 초기화 됨.
    QIINTBANK1_29                                           = 0x20000000,    // Bit 29, 서보 알람신호 인가됨.
    QIINTBANK1_30                                           = 0x40000000,    // Bit 30, |CNT1| - |CNT2| >= |CNT4| 이벤트 발생.
    QIINTBANK1_31                                           = 0x80000000     // Bit 31, 인터럽트 발생 명령어|INTGEN| 실행.
} AXT_MOTION_QIINTERRUPT_BANK1;
#endif

#ifndef AXT_MOTION_QIINTERRUPT_BANK2_DEF
#define AXT_MOTION_QIINTERRUPT_BANK2_DEF
typedef    enum _AXT_MOTION_QIINTERRUPT_BANK2
{
    QIINTBANK2_DISABLE                                     = 0x00000000,    // INTERRUT DISABLED.
    QIINTBANK2_0                                           = 0x00000001,    // Bit 0,  스크립트 #1 읽기 명령 결과 Queue 가 가득?.
    QIINTBANK2_1                                           = 0x00000002,    // Bit 1,  스크립트 #2 읽기 명령 결과 Queue 가 가득?.
    QIINTBANK2_2                                           = 0x00000004,    // Bit 2,  스크립트 #3 읽기 명령 결과 레지스터가 새로운 데이터로 갱신됨.
    QIINTBANK2_3                                           = 0x00000008,    // Bit 3,  스크립트 #4 읽기 명령 결과 레지스터가 새로운 데이터로 갱신됨.
    QIINTBANK2_4                                           = 0x00000010,    // Bit 4,  스크립트 #1 의 예약 명령어 중 실행 시 인터럽트 발생으로 설정된 명령어 실행됨.
    QIINTBANK2_5                                           = 0x00000020,    // Bit 5,  스크립트 #2 의 예약 명령어 중 실행 시 인터럽트 발생으로 설정된 명령어 실행됨.
    QIINTBANK2_6                                           = 0x00000040,    // Bit 6,  스크립트 #3 의 예약 명령어 실행 시 인터럽트 발생으로 설정된 명령어 실행됨.
    QIINTBANK2_7                                           = 0x00000080,    // Bit 7,  스크립트 #4 의 예약 명령어 실행 시 인터럽트 발생으로 설정된 명령어 실행됨.
    QIINTBANK2_8                                           = 0x00000100,    // Bit 8,  구동중 다음 구동명령이 실행될 때
    QIINTBANK2_9                                           = 0x00000200,    // Bit 9,  서보 위치 결정 완료(Inposition)기능을 사용한 구동,종료 조건 발생.
    QIINTBANK2_10                                          = 0x00000400,    // Bit 10, 이벤트 카운터로 동작 시 사용할 이벤트 선택 #1 조건 발생.
    QIINTBANK2_11                                          = 0x00000800,    // Bit 11, 이벤트 카운터로 동작 시 사용할 이벤트 선택 #2 조건 발생.
    QIINTBANK2_12                                          = 0x00001000,    // Bit 12, SQSTR1 신호 인가 됨.
    QIINTBANK2_13                                          = 0x00002000,    // Bit 13, SQSTR2 신호 인가 됨.
    QIINTBANK2_14                                          = 0x00004000,    // Bit 14, UIO0 터미널 신호가 '1'로 변함.
    QIINTBANK2_15                                          = 0x00008000,    // Bit 15, UIO1 터미널 신호가 '1'로 변함.
    QIINTBANK2_16                                          = 0x00010000,    // Bit 16, UIO2 터미널 신호가 '1'로 변함.
    QIINTBANK2_17                                          = 0x00020000,    // Bit 17, UIO3 터미널 신호가 '1'로 변함.
    QIINTBANK2_18                                          = 0x00040000,    // Bit 18, UIO4 터미널 신호가 '1'로 변함.
    QIINTBANK2_19                                          = 0x00080000,    // Bit 19, UIO5 터미널 신호가 '1'로 변함.
    QIINTBANK2_20                                          = 0x00100000,    // Bit 20, UIO6 터미널 신호가 '1'로 변함.
    QIINTBANK2_21                                          = 0x00200000,    // Bit 21, UIO7 터미널 신호가 '1'로 변함.
    QIINTBANK2_22                                          = 0x00400000,    // Bit 22, UIO8 터미널 신호가 '1'로 변함.
    QIINTBANK2_23                                          = 0x00800000,    // Bit 23, UIO9 터미널 신호가 '1'로 변함.
    QIINTBANK2_24                                          = 0x01000000,    // Bit 24, UIO10 터미널 신호가 '1'로 변함.
    QIINTBANK2_25                                          = 0x02000000,    // Bit 25, UIO11 터미널 신호가 '1'로 변함.
    QIINTBANK2_26                                          = 0x04000000,    // Bit 26, 오류 정지 조건(LMT, ESTOP, STOP, ESTOP, CMD, ALARM) 발생.
    QIINTBANK2_27                                          = 0x08000000,    // Bit 27, 보간 중 데이터 설정 오류 발생.
    QIINTBANK2_28                                          = 0x10000000,    // Bit 28, Don't Care
    QIINTBANK2_29                                          = 0x20000000,    // Bit 29, 리미트 신호(PELM, NELM)신호가 입력 됨.
    QIINTBANK2_30                                          = 0x40000000,    // Bit 30, 부가 리미트 신호(PSLM, NSLM)신호가 입력 됨.
    QIINTBANK2_31                                          = 0x80000000     // Bit 31, 비상 정지 신호(ESTOP)신호가 입력됨.
} AXT_MOTION_QIINTERRUPT_BANK2;
#endif

#ifndef AXT_NETWORK_STATUS_DEF
#define AXT_NETWORK_STATUS_DEF
typedef    enum _AXT_NETWORK_STATUS
{
    NET_STATUS_DISCONNECTED                                = 1,
    NET_STATUS_LOCK_MISMATCH                               = 5,
    NET_STATUS_CONNECTED                                   = 6
} AXT_NETWORK_STATUS;
#endif

#ifndef AXT_MOTION_INFO_DEF
#define AXT_MOTION_INFO_DEF
typedef struct _MOTION_INFO
{
    double   dCmdPos;      // Command 위치[0x01]
    double   dActPos;      // Encoder 위치[0x02]
    DWORD    dwMechSig;    // Mechanical Signal[0x04]    
    DWORD    dwDrvStat;    // Driver Status[0x08]
    DWORD    dwInput;      // Universal Signal Input[0x10]
    DWORD    dwOutput;     // Universal Signal Output[0x10]
    DWORD    dwMask;       // 읽기 설정 Mask Ex) 0x1F, 모든정보 읽기    
} MOTION_INFO, *PMOTION_INFO;
#endif

#ifndef MAX_BOARD_COUNT
#define MAX_BOARD_COUNT		20
#endif

typedef struct _SIIIHBoardInfo
{
	BOOL bIsSIIIHBoard;// SIIIH 보드인 경우 TRUE, 아니면 FALSE
	long lNodeCount; // 해당 보드에 연결된 Node 개수
} SIIIHBoardInfo;

typedef struct _SCAN_RESULT
{
	long lTotalBoardCount; //SIIIH 포함 장착된 전체 보드 개수
	SIIIHBoardInfo BoardInfo[MAX_BOARD_COUNT]; 
} SCAN_RESULT;

#ifndef AXT_MOTION_OVERRIDE_MODE_DEF
#define AXT_MOTION_OVERRIDE_MODE_DEF
typedef enum _AXT_MOTION_OVERRIDE_MODE
{
    OVERRIDE_POS_START                                     = 0,
    OVERRIDE_POS_END                                       = 1,
    OVERRIDE_POS_AUTO                                      = 2
} AXT_MOTION_OVERRIDE_MODE;
#endif

#ifndef AXT_MOTION_PROFILE_PRIORITY_DEF
#define AXT_MOTION_PROFILE_PRIORITY_DEF
typedef enum _AXT_MOTION_PROFILE_PRIORITY
{
    PRIORITY_VELOCITY                                      = 0,
    PRIORITY_ACCELTIME                                     = 1
} AXT_MOTION_PROFILE_PRIORITY;
#endif

#ifndef AXT_MOTION_FUNC_RETURN_MODE_DEF
#define AXT_MOTION_FUNC_RETURN_MODE_DEF
typedef enum _AXT_MOTION_FUNC_RETURN_MODE
{
    FUNC_RETURN_IMMEDIATE                                  = 0,
    FUNC_RETURN_BLOCKING                                   = 1,
    FUNC_RETURN_NON_BLOCKING                               = 2
} AXT_MOTION_FUNC_RETURN_MODE;
#endif


#ifndef AXT_MOTION_BACKLASH_DIR_DEF
#define AXT_MOTION_BACKLASH_DIR_DEF
typedef enum _AXT_MOTION_BACKLASH_DIR
{
    BACKLASH_DIR_P                                         = 0,
    BACKLASH_DIR_N                                         = 1,
    BACKLASH_DIR_U                                         = 2
} AXT_MOTION_BACKLASH_DIR;
#endif

#ifndef MAX_SERVO_ALARM_HISTORY
#define MAX_SERVO_ALARM_HISTORY   15
#endif

//Mechatrolink III
enum{
COM_CONNECT,
COM_DISCONNECT,
COM_OFFLINE
};

enum MONITOR_SELECT_INFORMATION
{
    _MONI_APOS_ = 0,                // feedback position  : current position of the motor
    _MONI_CPOS_,                    // command position   : command position after acceleration/deceleration filter
    _MONI_PERR_,                    // position error     : Position error of the control loop
    _MONI_LPOS1_,                   // latched position 1 : motor position 1 latched by the latch signal 
    _MONI_LPOS2_,                   // latched position 2 : motor position 2 latched by the latch signal
    _MONI_FSPD_,                    // feedback speed     : current speed of the motor 
    _MONI_CSPD_,                    // reference speed    : command speed of the motor
    _MONI_TRQ_,                     // torque(force) reference : command torque(force) of the motor
    _MONI_ALARM_,                   // detailed information on the current alarm : current alarm/warning (2-byte data, higher 2 bytes fixed as 0x0000)
    _MONI_MPOS_,                    // command position(optional) : the details of the monitor data are specified in the product specifications. example : internal command position of the control loop
    _MONI_RES1_,                    // reserved
    _MONI_RES2_,                    // reserved    
    _MONI_CMN1_,                    // common monitor 1 : selects the monitor data specified at common parameter 89.(for the contents of the monitor data, refer to common parameter 89)
    _MONI_CMN2_,                    // common monitor 2 : selects the monitor data specified at common parameter 8A.(for the contents of the monitor data, refer to common parameter 8A)
    _MONI_OMN1_,                    // optional monitor 1 : selects the monitor data specified by parameter.(the contents of the monitor data depend on the product specifications)
    _MONI_OMN2_                     // optional monitor 2 : selects the monitor data specified by parameter.(the contents of the monitor data depend on the product specifications)
};

#ifndef __AXT_CN2CH_DEFINE_H__
#define __AXT_CN2CH_DEFINE_H__

/* Main clock */
#define F_50M_CLK                 50000000L                /* 50.000 MHz */

/* Read/Write port */
typedef enum _CNTPORT_DATA_WRITE
{
    CnCommand                                              = 0x10,
    CnData1                                                = 0x12,
    CnData2                                                = 0x14,
    CnData3                                                = 0x16,
    CnData4                                                = 0x18,
    CnData12                                               = 0x44,
    CnData34                                               = 0x46
}CNTPORT_DATA;

typedef enum _CNTRAM_DATA
{
    CnRamAddr1                                             = 0x28,
    CnRamAddr2                                             = 0x2A,
    CnRamAddr3                                             = 0x2C,
    CnRamAddrx1                                            = 0x48,
    CnRamAddr23                                            = 0x4A
}CNTRAM_DATA;

typedef enum _PHASE_SEL
{
    CnAbPhase                                              = 0x0,
    CnZPhase                                               = 0x1
} PHASE_SEL;

typedef enum _COUNTER_INPUT
{
    CnUpDownMode                                           = 0x0,    // Up/Down
    CnSqr1Mode                                             = 0x1,    // 1체배
    CnSqr2Mode                                             = 0x2,    // 2체배
    CnSqr4Mode                                             = 0x3     // 4체배
} COUNTER_INPUT;

/* CNT COMMAND LIST */
typedef enum _CNTCOMMAND
{
    // CH-1 Group Register
    CnCh1CounterRead                                       = 0x10,    // CH1 COUNTER READ, 24BIT
    CnCh1CounterWrite                                      = 0x90,    // CH1 COUNTER WRITE
    CnCh1CounterModeRead                                   = 0x11,    // CH1 COUNTER MODE READ, 8BIT
    CnCh1CounterModeWrite                                  = 0x91,    // CH1 COUNTER MODE WRITE
    CnCh1TriggerRegionLowerDataRead                        = 0x12,    // CH1 TRIGGER REGION LOWER DATA READ, 24BIT
    CnCh1TriggerRegionLowerDataWrite                       = 0x92,    // CH1 TRIGGER REGION LOWER DATA WRITE
    CnCh1TriggerRegionUpperDataRead                        = 0x13,    // CH1 TRIGGER REGION UPPER DATA READ, 24BIT
    CnCh1TriggerRegionUpperDataWrite                       = 0x93,    // CH1 TRIGGER REGION UPPER DATA WRITE
    CnCh1TriggerPeriodRead                                 = 0x14,    // CH1 TRIGGER PERIOD READ, 24BIT, RESERVED
    CnCh1TriggerPeriodWrite                                = 0x94,    // CH1 TRIGGER PERIOD WRITE
    CnCh1TriggerPulseWidthRead                             = 0x15,    // CH1 TRIGGER PULSE WIDTH READ
    CnCh1TriggerPulseWidthWrite                            = 0x95,    // CH1 RIGGER PULSE WIDTH WRITE
    CnCh1TriggerModeRead                                   = 0x16,    // CH1 TRIGGER MODE READ
    CnCh1TriggerModeWrite                                  = 0x96,    // CH1 RIGGER MODE WRITE
    CnCh1TriggerStatusRead                                 = 0x17,    // CH1 TRIGGER STATUS READ
    CnCh1NoOperation_97                                    = 0x97, 
    CnCh1TriggerEnable                                     = 0x98, 
    CnCh1TriggerDisable                                    = 0x99, 
    CnCh1TimeTriggerFrequencyRead                          = 0x1A, 
    CnCh1TimeTriggerFrequencyWrite                         = 0x9A, 
    CnCh1ComparatorValueRead                               = 0x1B, 
    CnCh1ComparatorValueWrite                              = 0x9B, 
    CnCh1CompareatorConditionRead                          = 0x1D, 
    CnCh1CompareatorConditionWrite                         = 0x9D, 
                                                                    
    // CH-2 Group Register                                          
    CnCh2CounterRead                                       = 0x20,    // CH2 COUNTER READ, 24BIT
    CnCh2CounterWrite                                      = 0xA1,    // CH2 COUNTER WRITE
    CnCh2CounterModeRead                                   = 0x21,    // CH2 COUNTER MODE READ, 8BIT
    CnCh2CounterModeWrite                                  = 0xA1,    // CH2 COUNTER MODE WRITE
    CnCh2TriggerRegionLowerDataRead                        = 0x22,    // CH2 TRIGGER REGION LOWER DATA READ, 24BIT
    CnCh2TriggerRegionLowerDataWrite                       = 0xA2,    // CH2 TRIGGER REGION LOWER DATA WRITE
    CnCh2TriggerRegionUpperDataRead                        = 0x23,    // CH2 TRIGGER REGION UPPER DATA READ, 24BIT
    CnCh2TriggerRegionUpperDataWrite                       = 0xA3,    // CH2 TRIGGER REGION UPPER DATA WRITE
    CnCh2TriggerPeriodRead                                 = 0x24,    // CH2 TRIGGER PERIOD READ, 24BIT, RESERVED
    CnCh2TriggerPeriodWrite                                = 0xA4,    // CH2 TRIGGER PERIOD WRITE
    CnCh2TriggerPulseWidthRead                             = 0x25,    // CH2 TRIGGER PULSE WIDTH READ, 24BIT
    CnCh2TriggerPulseWidthWrite                            = 0xA5,    // CH2 RIGGER PULSE WIDTH WRITE
    CnCh2TriggerModeRead                                   = 0x26,    // CH2 TRIGGER MODE READ
    CnCh2TriggerModeWrite                                  = 0xA6,    // CH2 RIGGER MODE WRITE
    CnCh2TriggerStatusRead                                 = 0x27,    // CH2 TRIGGER STATUS READ
    CnCh2NoOperation_A7                                    = 0xA7, 
    CnCh2TriggerEnable                                     = 0xA8, 
    CnCh2TriggerDisable                                    = 0xA9, 
    CnCh2TimeTriggerFrequencyRead                          = 0x2A, 
    CnCh2TimeTriggerFrequencyWrite                         = 0xAA, 
    CnCh2ComparatorValueRead                               = 0x2B, 
    CnCh2ComparatorValueWrite                              = 0xAB, 
    CnCh2CompareatorConditionRead                          = 0x2D, 
    CnCh2CompareatorConditionWrite                         = 0xAD, 
                                                                     
    // Ram Access Group Register                                     
    CnRamDataWithRamAddress                                = 0x30,    // READ RAM DATA WITH RAM ADDR PORT ADDRESS
    CnRamDataWrite                                         = 0xB0,    // RAM DATA WRITE
    CnRamDataRead                                          = 0x31     // RAM DATA READ, 32BIT
                                                                    
}CNTCOMMAND;                                                        
#endif

#ifndef AXT_MOTION_JOINT_MODE_DEF
#define AXT_MOTION_JOINT_MODE_DEF
typedef enum _AXT_MOTION_JOINT_MODE
{
	AxisMode         = 0x0,
	JointMode        = 0x1,
	ToolMode         = 0x2
}AXT_MOTION_JOINT_MODE;
#endif

#ifndef AXT_MOTION_COMPENSATION_USE_REVERSE_DEF
#define AXT_MOTION_COMPENSATION_USE_REVERSE_DEF
typedef enum _AXT_MOTION_COMPENSATION_USE_REVERSE
{
	COMPENSATION_REVERSE_UNUSE = 0,
	COMPENSATION_REVERSE_USE   = 1
} AXT_MOTION_COMPENSATION_USE_REVERSE;
#endif

#ifndef AXT_MOTION_COMPENSATION_INTERPOLATION_METHOD_DEF
#define AXT_MOTION_COMPENSATION_INTERPOLATION_METHOD_DEF
typedef enum _AXT_MOTION_COMPENSATION_INTERPOLATION_METHOD
{
	INTERPOLATION_LINEAR       = 0,
	INTERPOLATION_CUBIC_SPLINE = 1
} AXT_MOTION_COMPENSATION_INTERPOLATION_METHOD;
#endif

#ifndef AXT_MONITOR_SIGNAL_TYPE_DEF
#define AXT_MONITOR_SIGNAL_TYPE_DEF
typedef enum
{
	// Motion 
	eMonitorSignalType_CmdPos = 0,
	eMonitorSignalType_ActPos,
	eMonitorSignalType_CmdVel,
	eMonitorSignalType_ActVel,    
	eMonitorSignalType_CmdAccDec,
	eMonitorSignalType_ActAccDec,
	eMonitorSignalType_PosErr,
	eMonitorSignalType_InMotion,
	eMonitorSignalType_ActTorque,
	eMonitorSignalType_PositionDemand,
	eMonitorSignalType_VelocityDemand,
	eMonitorSignalType_TorqueDemand,
	eMonitorSignalType_MotionAiValue,
	eMonitorSignalType_MotionDiValue,
	eMonitorSignalType_MotionDoValue,
	eMonitorSignalType_Event_ContiEndNode,

	// Digital I/O
	eMonitorSignalType_DiValue,
	eMonitorSignalType_DoValue,

	// Analog I/O
	eMonitorSignalType_AiValue,
	eMonitorSignalType_AoValue,
	MAX_eMonitorSignalType
} MonitorSignalType;
#endif //AXT_MONITOR_SIGNAL_TYPE_DEF

#ifndef AXT_MONITOR_OPERATOR_TYPE_DEF
#define AXT_MONITOR_OPERATOR_TYPE_DEF
typedef enum
{
	eMonitorOperationType_Grater = 0,
	eMonitorOperationType_Smaller,
	eMonitorOperationType_RisingEdge,
	eMonitorOperationType_FallingEdge,
	MAX_eMonitorOperationType
} MonitorOperationType;
#endif //AXT_MONITOR_OPERATOR_TYPE_DEF               

#ifndef AXT_MONITOR_START_OPTION_DEF
#define AXT_MONITOR_START_OPTION_DEF
typedef enum
{
	eMonitorStartOption_Immediately = 0,
	eMonitorStartOption_Trigger,
	MAX_eMonitorStartOption
} MonitorStartOption;
#endif //AXT_MONITOR_START_OPTION_DEF      

#ifndef AXT_MONITOR_OVERFLOW_OPTION_DEF
#define AXT_MONITOR_OVERFLOW_OPTION_DEF
typedef enum
{
	eMonitorOverflowOption_IgnoreQueueFull = 0,
	eMonitorOverflowOption_WaitQueueFull,
	MAX_eMonitorOverflowOption
} MonitorOverflowOption;
#endif //AXT_MONITOR_OVERFLOW_OPTION_DEF   

#ifndef    __AXL_HPC_DEFINE_
#define    __AXL_HPC_DEFINE_

/* Read/Write port                            */
typedef enum _HPCPORT_DATA_WRITE
{    
    HpcReset          = 0x06,            // Software reset.
    HpcCommand        = 0x10,
    HpcData12         = 0x12,            // MSB of data port(31 ~ 16 bit)
    HpcData34         = 0x14,            // LSB of data port(15 ~ 0 bit)
    HpcCmStatus       = 0x1C,
}HPCPORT_DATA_WRITE;

typedef enum _HPCPORT_CH_STAUTS
{
    HpcCh1Mech        = 0x20,
    HpcCh1Status      = 0x22,
    HpcCh2Mech        = 0x30,
    HpcCh2Status      = 0x32,
    HpcCh3Mech        = 0x40,
    HpcCh3Status      = 0x42,
    HpcCh4Mech        = 0x50,
    HpcCh4Status      = 0x52
}HPCPORT_CH_STAUTS;

typedef enum _HPCPORT_ETC
{
    HpcDiIntFlag      = 0x60,
    HpcDiIntRiseMask  = 0x62,
    HpcDiIntFallMask  = 0x64,
    HpcCompIntFlag    = 0x66,
    HpcCompIntMask    = 0x68,
    HpcDinData        = 0x6A,
    HpcDoutData       = 0x6C
}HPCPORT_ETC;


typedef enum _HPCRAM_DATA
{
    HpcRamAddr1        = 0x70,            // MSB of Ram address(31  ~ 16 bit)
    HpcRamAddr2        = 0x72            // LSB of Ram address(15  ~ 0 bit)
}HPCRAM_DATA;


/*
--1) count_mode
--  0 bit : reverse.(0: normal, 1:reverse)
--  1 bit : count source selection.(0 : A/B 2 phase signal, 1 :  Z phase signal)
--  2~3 bit : count mode
--      "00" up/Down mode 
--      "01" 2 phase, 1times
--      "10" 2 phase, 2times
--      "11" 2 phase, 4times
*/

/*
-- Description : Trigger pulse generation block for SIO-CN2CH
-- trig_mode
-- 0 bit : trigger signal active level.
-- 1 bit : reserved.
-- 2 bit : trigger signal output method, valid when trig_mode(1) = '0', (0 : pulse out, 1 : level out)
-- 3 bit : reserved.
-- 5~4 bit : Trigger method
--    "00" : absolute mode[with ram data].
--    "01" : timer mode.
--    "10" : absolute mode[with fifo].
--    "11" : periodic mode.
-- 6 bit : direction valid check enable/disable when periodic mode.
-- 7 bit : direction valid when periodic mode.
*/

/* CNT COMMAND LIST                        */
typedef enum _HPCCOMMAND
{
    // CH-1 Group Register
    HpcCh1CounterRead                   = 0x10,                // CH1 COUNTER READ, 32BIT
    HpcCh1CounterWrite                  = 0x90,                // CH1 COUNTER WRITE, 32BIT
    HpcCh1CounterModeRead               = 0x11,                // CH1 COUNTER MODE READ, 4BIT
    HpcCh1CounterModeWrite              = 0x91,                // CH1 COUNTER MODE WRITE, 4BIT
    HpcCh1TriggerRegionLowerDataRead    = 0x12,                // CH1 TRIGGER REGION LOWER DATA READ, 31BIT
    HpcCh1TriggerRegionLowerDataWrite   = 0x92,                // CH1 TRIGGER REGION LOWER DATA WRITE
    HpcCh1TriggerRegionUpperDataRead    = 0x13,                // CH1 TRIGGER REGION UPPER DATA READ, 31BIT
    HpcCh1TriggerRegionUpperDataWrite   = 0x93,                // CH1 TRIGGER REGION UPPER DATA WRITE
    HpcCh1TriggerPeriodRead             = 0x14,                // CH1 TRIGGER PERIOD READ, 31BIT
    HpcCh1TriggerPeriodWrite            = 0x94,                // CH1 TRIGGER PERIOD WRITE
    HpcCh1TriggerPulseWidthRead         = 0x15,                // CH1 TRIGGER PULSE WIDTH READ, 31BIT
    HpcCh1TriggerPulseWidthWrite        = 0x95,                // CH1 RIGGER PULSE WIDTH WRITE
    HpcCh1TriggerModeRead               = 0x16,                // CH1 TRIGGER MODE READ, 8BIT
    HpcCh1TriggerModeWrite              = 0x96,                // CH1 RIGGER MODE WRITE
    HpcCh1TriggerStatusRead             = 0x17,                // CH1 TRIGGER STATUS READ, 8BIT
    HpcCh1NoOperation_97                = 0x97,                // Reserved.
    HpcCh1NoOperation_18                = 0x17,                // Reserved.
    HpcCh1TriggerEnable                 = 0x98,                // CH1 TRIGGER ENABLE.
    HpcCh1NoOperation_19                = 0x19,                // Reserved.
    HpcCh1TriggerDisable                = 0x99,                // CH1 TRIGGER DISABLE.
    HpcCh1TimeTriggerFrequencyRead      = 0x1A,                // CH1 TRIGGER FREQUNCE INFO. WRITE, 28BIT
    HpcCh1TimeTriggerFrequencyWrite     = 0x9A,                // CH1 TRIGGER FREQUNCE INFO. READ
    HpcCh1Comparator1ValueRead          = 0x1B,                // CH1 COMPAREATOR1 READ, 31BIT
    HpcCh1Comparator1ValueWrite         = 0x9B,                // CH1 COMPAREATOR1 WRITE, 31BIT
    HpcCh1Comparator2ValueRead          = 0x1C,                // CH1 COMPAREATOR2 READ, 31BIT
    HpcCh1Comparator2ValueWrite         = 0x9C,                // CH1 COMPAREATOR2 WRITE, 31BIT
    HpcCh1CompareatorConditionRead      = 0x1D,                // CH1 COMPAREATOR CONDITION READ, 4BIT
    HpcCh1CompareatorConditionWrite     = 0x9D,                // CH1 COMPAREATOR CONDITION WRITE, 4BIT
    HpcCh1AbsTriggerTopPositionRead     = 0x1E,                // CH1 ABS TRIGGER POSITION READ, 31BIT
    HpcCh1AbsTriggerPositionWrite       = 0x9E,                // CH1 ABS TRIGGER POSITION WRITE, 31BIT
    HpcCh1AbsTriggerFifoStatusRead      = 0x1F,                // CH1 ABS TRIGGER POSITION FIFO STATUS READ, 16BIT
    HpcCh1AbsTriggerPositionClear       = 0x9F,                // CH1 ABS TRIGGER POSITION FIFO CLEAR

    // CH-2 Group Register
    HpcCh2CounterRead                   = 0x20,                // CH2 COUNTER READ, 32BIT
    HpcCh2CounterWrite                  = 0xA0,                // CH2 COUNTER WRITE, 32BIT
    HpcCh2CounterModeRead               = 0x21,                // CH2 COUNTER MODE READ, 4BIT
    HpcCh2CounterModeWrite              = 0xA1,                // CH2 COUNTER MODE WRITE, 4BIT
    HpcCh2TriggerRegionLowerDataRead    = 0x22,                // CH2 TRIGGER REGION LOWER DATA READ, 31BIT
    HpcCh2TriggerRegionLowerDataWrite   = 0xA2,                // CH2 TRIGGER REGION LOWER DATA WRITE
    HpcCh2TriggerRegionUpperDataRead    = 0x23,                // CH2 TRIGGER REGION UPPER DATA READ, 31BIT
    HpcCh2TriggerRegionUpperDataWrite   = 0xA3,                // CH2 TRIGGER REGION UPPER DATA WRITE
    HpcCh2TriggerPeriodRead             = 0x24,                // CH2 TRIGGER PERIOD READ, 31BIT
    HpcCh2TriggerPeriodWrite            = 0xA4,                // CH2 TRIGGER PERIOD WRITE
    HpcCh2TriggerPulseWidthRead         = 0x25,                // CH2 TRIGGER PULSE WIDTH READ, 31BIT
    HpcCh2TriggerPulseWidthWrite        = 0xA5,                // CH2 RIGGER PULSE WIDTH WRITE
    HpcCh2TriggerModeRead               = 0x26,                // CH2 TRIGGER MODE READ, 8BIT
    HpcCh2TriggerModeWrite              = 0xA6,                // CH2 RIGGER MODE WRITE
    HpcCh2TriggerStatusRead             = 0x27,                // CH2 TRIGGER STATUS READ, 8BIT
    HpcCh2NoOperation_97                = 0xA7,                // Reserved.
    HpcCh2NoOperation_18                = 0x27,                // Reserved.
    HpcCh2TriggerEnable                 = 0xA8,                // CH2 TRIGGER ENABLE.
    HpcCh2NoOperation_19                = 0x29,                // Reserved.
    HpcCh2TriggerDisable                = 0xA9,                // CH2 TRIGGER DISABLE.
    HpcCh2TimeTriggerFrequencyRead      = 0x2A,                // CH2 TRIGGER FREQUNCE INFO. WRITE, 28BIT
    HpcCh2TimeTriggerFrequencyWrite     = 0xAA,                // CH2 TRIGGER FREQUNCE INFO. READ
    HpcCh2Comparator1ValueRead          = 0x2B,                // CH2 COMPAREATOR1 READ, 31BIT
    HpcCh2Comparator1ValueWrite         = 0xAB,                // CH2 COMPAREATOR1 WRITE, 31BIT
    HpcCh2Comparator2ValueRead          = 0x2C,                // CH2 COMPAREATOR2 READ, 31BIT
    HpcCh2Comparator2ValueWrite         = 0xAC,                // CH2 COMPAREATOR2 WRITE, 31BIT
    HpcCh2CompareatorConditionRead      = 0x2D,                // CH2 COMPAREATOR CONDITION READ, 4BIT
    HpcCh2CompareatorConditionWrite     = 0xAD,                // CH2 COMPAREATOR CONDITION WRITE, 4BIT
    HpcCh2AbsTriggerTopPositionRead     = 0x2E,                // CH2 ABS TRIGGER POSITION READ, 31BIT
    HpcCh2AbsTriggerPositionWrite       = 0xAE,                // CH2 ABS TRIGGER POSITION WRITE, 31BIT
    HpcCh2AbsTriggerFifoStatusRead      = 0x2F,                // CH2 ABS TRIGGER POSITION FIFO STATUS READ, 16BIT
    HpcCh2AbsTriggerPositionClear       = 0xAF,                // CH2 ABS TRIGGER POSITION FIFO CLEAR

    // CH-3 Group Register
    HpcCh3CounterRead                   = 0x30,                // CH3 COUNTER READ, 32BIT
    HpcCh3CounterWrite                  = 0xB0,                // CH3 COUNTER WRITE, 32BIT
    HpcCh3CounterModeRead               = 0x31,                // CH3 COUNTER MODE READ, 4BIT
    HpcCh3CounterModeWrite              = 0xB1,                // CH3 COUNTER MODE WRITE, 4BIT
    HpcCh3TriggerRegionLowerDataRead    = 0x32,                // CH3 TRIGGER REGION LOWER DATA READ, 31BIT
    HpcCh3TriggerRegionLowerDataWrite   = 0xB2,                // CH3 TRIGGER REGION LOWER DATA WRITE
    HpcCh3TriggerRegionUpperDataRead    = 0x33,                // CH3 TRIGGER REGION UPPER DATA READ, 31BIT
    HpcCh3TriggerRegionUpperDataWrite   = 0xB3,                // CH3 TRIGGER REGION UPPER DATA WRITE
    HpcCh3TriggerPeriodRead             = 0x34,                // CH3 TRIGGER PERIOD READ, 31BIT
    HpcCh3TriggerPeriodWrite            = 0xB4,                // CH3 TRIGGER PERIOD WRITE
    HpcCh3TriggerPulseWidthRead         = 0x35,                // CH3 TRIGGER PULSE WIDTH READ, 31BIT
    HpcCh3TriggerPulseWidthWrite        = 0xB5,                // CH3 RIGGER PULSE WIDTH WRITE
    HpcCh3TriggerModeRead               = 0x36,                // CH3 TRIGGER MODE READ, 8BIT
    HpcCh3TriggerModeWrite              = 0xB6,                // CH3 RIGGER MODE WRITE
    HpcCh3TriggerStatusRead             = 0x37,                // CH3 TRIGGER STATUS READ, 8BIT
    HpcCh3NoOperation_97                = 0xB7,                // Reserved.
    HpcCh3NoOperation_18                = 0x37,                // Reserved.
    HpcCh3TriggerEnable                 = 0xB8,                // CH3 TRIGGER ENABLE.
    HpcCh3NoOperation_19                = 0x39,                // Reserved.
    HpcCh3TriggerDisable                = 0xB9,                // CH3 TRIGGER DISABLE.
    HpcCh3TimeTriggerFrequencyRead      = 0x3A,                // CH3 TRIGGER FREQUNCE INFO. WRITE, 28BIT
    HpcCh3TimeTriggerFrequencyWrite     = 0xBA,                // CH3 TRIGGER FREQUNCE INFO. READ
    HpcCh3Comparator1ValueRead          = 0x3B,                // CH3 COMPAREATOR1 READ, 31BIT
    HpcCh3Comparator1ValueWrite         = 0xBB,                // CH3 COMPAREATOR1 WRITE, 31BIT
    HpcCh3Comparator2ValueRead          = 0x3C,                // CH3 COMPAREATOR2 READ, 31BIT
    HpcCh3Comparator2ValueWrite         = 0xBC,                // CH3 COMPAREATOR2 WRITE, 31BIT
    HpcCh3CompareatorConditionRead      = 0x3D,                // CH3 COMPAREATOR CONDITION READ, 4BIT
    HpcCh3CompareatorConditionWrite     = 0xBD,                // CH3 COMPAREATOR CONDITION WRITE, 4BIT
    HpcCh3AbsTriggerTopPositionRead     = 0x3E,                // CH3 ABS TRIGGER POSITION READ, 31BIT
    HpcCh3AbsTriggerPositionWrite       = 0xBE,                // CH3 ABS TRIGGER POSITION WRITE, 31BIT
    HpcCh3AbsTriggerFifoStatusRead      = 0x3F,                // CH3 ABS TRIGGER POSITION FIFO STATUS READ, 16BIT
    HpcCh3AbsTriggerPositionClear       = 0xBF,                // CH3 ABS TRIGGER POSITION FIFO CLEAR

    // CH-4 Group Register
    HpcCh4CounterRead                   = 0x40,                // CH4 COUNTER READ, 32BIT
    HpcCh4CounterWrite                  = 0xC0,                // CH4 COUNTER WRITE, 32BIT
    HpcCh4CounterModeRead               = 0x41,                // CH4 COUNTER MODE READ, 4BIT
    HpcCh4CounterModeWrite              = 0xC1,                // CH4 COUNTER MODE WRITE, 4BIT
    HpcCh4TriggerRegionLowerDataRead    = 0x42,                // CH4 TRIGGER REGION LOWER DATA READ, 31BIT
    HpcCh4TriggerRegionLowerDataWrite   = 0xC2,                // CH4 TRIGGER REGION LOWER DATA WRITE
    HpcCh4TriggerRegionUpperDataRead    = 0x43,                // CH4 TRIGGER REGION UPPER DATA READ, 31BIT
    HpcCh4TriggerRegionUpperDataWrite   = 0xC3,                // CH4 TRIGGER REGION UPPER DATA WRITE
    HpcCh4TriggerPeriodRead             = 0x44,                // CH4 TRIGGER PERIOD READ, 31BIT
    HpcCh4TriggerPeriodWrite            = 0xC4,                // CH4 TRIGGER PERIOD WRITE
    HpcCh4TriggerPulseWidthRead         = 0x45,                // CH4 TRIGGER PULSE WIDTH READ, 31BIT
    HpcCh4TriggerPulseWidthWrite        = 0xC5,                // CH4 RIGGER PULSE WIDTH WRITE
    HpcCh4TriggerModeRead               = 0x46,                // CH4 TRIGGER MODE READ, 8BIT
    HpcCh4TriggerModeWrite              = 0xC6,                // CH4 RIGGER MODE WRITE
    HpcCh4TriggerStatusRead             = 0x47,                // CH4 TRIGGER STATUS READ, 8BIT
    HpcCh4NoOperation_97                = 0xC7,                // Reserved.
    HpcCh4NoOperation_18                = 0x47,                // Reserved.
    HpcCh4TriggerEnable                 = 0xC8,                // CH4 TRIGGER ENABLE.
    HpcCh4NoOperation_19                = 0x49,                // Reserved.
    HpcCh4TriggerDisable                = 0xC9,                // CH4 TRIGGER DISABLE.
    HpcCh4TimeTriggerFrequencyRead      = 0x4A,                // CH4 TRIGGER FREQUNCE INFO. WRITE, 28BIT
    HpcCh4TimeTriggerFrequencyWrite     = 0xCA,                // CH4 TRIGGER FREQUNCE INFO. READ
    HpcCh4Comparator1ValueRead          = 0x4B,                // CH4 COMPAREATOR1 READ, 31BIT
    HpcCh4Comparator1ValueWrite         = 0xCB,                // CH4 COMPAREATOR1 WRITE, 31BIT
    HpcCh4Comparator2ValueRead          = 0x4C,                // CH4 COMPAREATOR2 READ, 31BIT
    HpcCh4Comparator2ValueWrite         = 0xCC,                // CH4 COMPAREATOR2 WRITE, 31BIT
    HpcCh4CompareatorConditionRead      = 0x4D,                // CH4 COMPAREATOR CONDITION READ, 4BIT
    HpcCh4CompareatorConditionWrite     = 0xCD,                // CH4 COMPAREATOR CONDITION WRITE, 4BIT
    HpcCh4AbsTriggerTopPositionRead     = 0x4E,                // CH4 ABS TRIGGER POSITION READ, 31BIT
    HpcCh4AbsTriggerPositionWrite       = 0xCE,                // CH4 ABS TRIGGER POSITION WRITE, 31BIT
    HpcCh4AbsTriggerFifoStatusRead      = 0x4F,                // CH4 ABS TRIGGER POSITION FIFO STATUS READ, 16BIT
    HpcCh4AbsTriggerPositionClear       = 0xCF,                // CH4 ABS TRIGGER POSITION FIFO CLEAR

    // Ram Access Group Register
    HpcRamDataWithRamAddress            = 0x51,                // READ RAM DATA WITH RAM ADDR PORT ADDRESS
    HpcRamDataWrite                     = 0xD0,                // RAM DATA WRITE
    HpcRamDataRead                      = 0x50,                // RAM DATA READ, 32BIT

    // Debugging Registers    
    HpcCh1TrigPosIndexRead              = 0x60,                // CH1 Current RAM trigger index position on 32Bit data, 8BIT
    HpcCh1TrigBackwardDataRead          = 0x61,                // CH1 Current RAM trigger backward position data, 32BIT
    HpcCh1TrigCurrentDataRead           = 0x62,                // CH1 Current RAM trigger current position data, 32BIT
    HpcCh1TrigForwardDataRead           = 0x63,                // CH1 Current RAM trigger next position data, 32BIT
    HpcCh1TrigRamAddressRead            = 0x64,                // CH1 Current RAM trigger address, 20BIT

    HpcCh2TrigPosIndexRead              = 0x65,                // CH2 Current RAM trigger index position on 32Bit data, 8BIT
    HpcCh2TrigBackwardDataRead          = 0x66,                // CH2 Current RAM trigger backward position data, 32BIT
    HpcCh2TrigCurrentDataRead           = 0x67,                // CH2 Current RAM trigger current position data, 32BIT
    HpcCh2TrigForwardDataRead           = 0x68,                // CH2 Current RAM trigger next position data, 32BIT
    HpcCh2TrigRamAddressRead            = 0x69,                // CH2 Current RAM trigger address, 20BIT

    HpcCh3TrigPosIndexRead              = 0x70,                // CH3 Current RAM trigger index position on 32Bit data, 8BIT
    HpcCh3TrigBackwardDataRead          = 0x71,                // CH3 Current RAM trigger backward position data, 32BIT
    HpcCh3TrigCurrentDataRead           = 0x72,                // CH3 Current RAM trigger current position data, 32BIT
    HpcCh3TrigForwardDataRead           = 0x73,                // CH3 Current RAM trigger next position data, 32BIT
    HpcCh3TrigRamAddressRead            = 0x74,                // CH3 Current RAM trigger address, 20BIT

    HpcCh4TrigPosIndexRead              = 0x75,                // CH4 Current RAM trigger index position on 32Bit data, 8BIT
    HpcCh4TrigBackwardDataRead          = 0x76,                // CH4 Current RAM trigger backward position data, 32BIT
    HpcCh4TrigCurrentDataRead           = 0x77,                // CH4 Current RAM trigger current position data, 32BIT
    HpcCh4TrigForwardDataRead           = 0x78,                // CH4 Current RAM trigger next position data, 32BIT
    HpcCh4TrigRamAddressRead            = 0x79,                // CH4 Current RAM trigger address, 20BIT

    HpcCh1TestEnable                    = 0x81,                // CH1 test enable(Manufacturer only)
    HpcCh2TestEnable                    = 0x82,                // CH2 test enable(Manufacturer only)
    HpcCh3TestEnable                    = 0x83,                // CH3 test enable(Manufacturer only)
    HpcCh4TestEnable                    = 0x84,                // CH4 test enable(Manufacturer only)

    HpcTestFrequency                    = 0x8C,                // Test counter output frequency(32bit)
    HpcTestCountStart                   = 0x8D,                // Start test counter output with position(32bit signed).
    HpcTestCountEnd                     = 0x8E,                // End counter output.

    HpcCh1TrigVectorTopDataOfFifo       = 0x54,                // CH1 UnitVector X positin of FIFO top.
    HpcCh1TrigVectorFifoStatus          = 0x55,                // CH1 UnitVector X FIFO Status.
    HpcCh2TrigVectorTopDataOfFifo       = 0x56,                // CH2 UnitVector Y positin of FIFO top.
    HpcCh2TrigVectorFifoStatus          = 0x57,                // CH2 UnitVector Y FIFO Status.
    HpcCh1TrigVectorFifoPush            = 0xD2,                // CH1 UnitVector X position, fifo data push.
    HpcCh2TrigVectorFifoPush            = 0xD3,                // CH2 UnitVector Y position, fifo data push.
	HpcCh1MaskingTime					= 0xF8,
	HpcCh2MaskingTime					= 0xF9
} HPCCOMMAND;
#endif
                                                                    
#ifndef	__AXL_HPC_30_DEFINE_
#define	__AXL_HPC_30_DEFINE_

/* Read/Write port							*/
typedef enum _HPC30PORT_DATA_WRITE
{	
	Hpc30Reset		    = 0x06,			// Software reset.
	Hpc30Command		= 0x10,
	Hpc30Data12		    = 0x12,			// MSB of data port(31 ~ 16 bit)
	Hpc30Data34		    = 0x14,			// LSB of data port(15 ~ 0 bit)
	Hpc30CmStatus     = 0x1C,
}HPC30PORT_DATA_WRITE;

typedef enum _HPC30PORT_CH_STAUTS
{
	Hpc30Ch1Mech		= 0x20,
	Hpc30Ch1Status	    = 0x22,
	Hpc30Ch2Mech		= 0x30,
	Hpc30Ch2Status	    = 0x32,
	Hpc30Ch3Mech		= 0x40,
	Hpc30Ch3Status	    = 0x42,
	Hpc30Ch4Mech		= 0x50,
	Hpc30Ch4Status	    = 0x52
}HPC30PORT_CH_STAUTS;

typedef enum _HPC30PORT_ETC
{
	Hpc30DiIntFlag	    = 0x60,
	Hpc30DiIntRiseMask  = 0x62,
	Hpc30DiIntFallMask  = 0x64,
	Hpc30CompIntFlag	= 0x66,
	Hpc30CompIntMask	= 0x68,
	Hpc30DinData		= 0x6A,
	Hpc30DoutData		= 0x6C
}HPC30PORT_ETC;


typedef enum _HPC30RAM_DATA
{
	Hpc30RamAddr1		= 0x70,			// MSB of Ram address(31  ~ 16 bit)
	Hpc30RamAddr2		= 0x72			// LSB of Ram address(15  ~ 0 bit)
}HPC30RAM_DATA;

/* CNT COMMAND LIST						*/
typedef enum _HPC30COMMAND
{
	Hpc30Ch1CounterRead					    = 0x00,				// CH1 COUNTER READ, 32BIT
	Hpc30Ch1CounterWrite					= 0x80,				// CH1 COUNTER WRITE, 32BIT
	Hpc30Ch1CounterModeRead				    = 0x01,				// CH1 COUNTER MODE READ, 4BIT
	Hpc30Ch1CounterModeWrite				= 0x81,				// CH1 COUNTER MODE WRITE, 4BIT
	Hpc30Table1FifoDataXRead			    = 0x02,				// TB1 FIFO DATA X READ, 32BIT    
	Hpc30Table1FifoDataClear			    = 0x82,				// TB1 FIFO DATA CLEAR, 1BIT
	Hpc30Table1FifoDataYRead			    = 0x03,				// TB1 FIFO DATA Y READ, 32BIT   
    Hpc30Table1HoldTimeWrite       		    = 0x83,				// TB1 HOLD TIME WRITE, 32BIT
	Hpc30Table1FifoStatusRead			    = 0x04,				// TB1 FIFO STATUS XY READ, 32BIT    
	Hpc30Table1OutputCountRead			    = 0x05,				// TB1 OUTPUT COUNT READ, 32BIT
	Hpc30Table1OutputCountClear			    = 0x85,				// TB1 OUTPUT COUNT CLEAR, 1BIT
    Hpc30Table1FifoDatauXRead			    = 0x06,				// TB1 FIFO DATA uX READ, 32BIT    
   	Hpc30Table1FifoDatauXWrite	            = 0x86,				// TB1 FIFO DATA uX WRITE, 32BIT	
    Hpc30Table1FifoDatauYRead			    = 0x07,				// TB1 FIFO DATA uY READ, 32BIT    
    Hpc30Table1FifoDatauYWrite	            = 0x87,				// TB1 FIFO DATA uY WRITE, 32BIT
    Hpc30Table1FifoStatusuXuYRead		    = 0x08,				// TB1 FIFO STATUS uXuY READ, 32BIT    
    Hpc30Table1FifoDataExeXRead		    	= 0x09,				// TB1 FIFO DATA EXE X READ, 32BIT    
    Hpc30Table1FifoDataExeYRead			    = 0x0A,				// TB1 FIFO DATA EXE Y READ, 32BIT    
    Hpc30Table1FifoStatusExeXYRead		    = 0x0B,				// TB1 FIFO STATUS EXE XY READ, 32BIT        
    Hpc30Table1FifoPopExeXY       		    = 0x0C,				// TB1 FIFO POP EXE XY READ, 32BIT        
    Hpc30Table1HoldTimeRead       		    = 0x0D,				// TB1 HOLD TIME READ, 32BIT
	Hpc30Table1ActiveLevelRead			    = 0x10,				// TB1 ACITVE LEVEL READ, 1BIT
	Hpc30Table1ActiveLevelWrite			    = 0x90,				// TB1 ACITVE LEVEL WRITE, 1BIT
	Hpc30Table1PulseWidthRead			    = 0x11,				// TB1 PULSE WDITH READ, 32BIT
	Hpc30Table1PulseWidthWrite			    = 0x91,				// TB1 PULSE WDITH WRITE, 32BIT
	Hpc30Table1EncoderInputChannelRead	    = 0x12,				// TB1 ENCODER INPUT CHANNEL READ, 4BIT
	Hpc30Table1EncoderInputChannelWrite	    = 0x92,				// TB1 ENCODER INPUT CHANNEL WRITE, 4BIT
	Hpc30Table1ErrorRangeRead	            = 0x13,				// TB1 ERROR RANGE READ, 32BIT
	Hpc30Table1ErrorRangeWrite	            = 0x93,				// TB1 ERROR RANGE WRITE, 32BIT
	Hpc30Table1OutPortRead	                = 0x14,				// TB1 OUTPUT CHANNEL SETTING READ, 4BIT
	Hpc30Table1OutPortWrite	                = 0x94,				// TB1 OUTPUT CHANNEL SETTING WRITE, 4BIT
	Hpc30Table1PatternTimeRead	            = 0x15,				// TB1 PATTERN TRIGGER TIME READ, 32BIT
	Hpc30Table1PatternTimeWrite	            = 0x95,				// TB1 PATTERN TRIGGER TIME WRITE, 32BIT
	Hpc30Table1PatternCountRead	            = 0x16,				// TB1 PATTERN TRIGGER COUNT READ, 32BIT
	Hpc30Table1PatternCountWrite	        = 0x96,				// TB1 PATTERN TRIGGER COUNT WRITE, 32BIT
	Hpc30Table1TriggerModeRead	            = 0x17,				// TB1 TRIGGER MODE READ, 2BIT
	Hpc30Table1TriggerModeWrite	            = 0x97,				// TB1 TRIGGER MODE WRITE, 2BIT
	Hpc30Table1CaptureTimeRead	            = 0x18,				// TB1 CAPTURE TIME READ, 32BIT
	Hpc30Table1CaptureTimeWrite	            = 0x98,				// TB1 CAPTURE TIME WRITE, 32BIT
	Hpc30Table1OneShot	                    = 0x99,				// TB1 ONE SHOT , 1BIT
	Hpc30Table1GetEnable	                = 0x1A,				// TB1 GET ENABLE, 1BIT    
	Hpc30Table1Enable	                    = 0x9A,				// TB1 ENABLE , 1BIT
	Hpc30Table1Disable	                    = 0x9B,				// TB1 DISABLE , 1BIT
	Hpc30Table1FifoDataXWrite	            = 0x9C,				// TB1 FIFO DATA X WRITE, 32BIT
	Hpc30Table1FifoDataYWrite	            = 0x9D,				// TB1 FIFO DATA Y WRITE, 32BIT
    Hpc30Table1FifoRepeatTimeRead           = 0x1E,				// TB1 FIFO REPEAT TIME READ, 32BIT
	Hpc30Table1FifoRepeatTimeWrite	        = 0x9E,				// TB1 FIFO REPEAT TIME WRITE, 32BIT
    Hpc30Table1FifoRepeatCountRead 	        = 0x1F,				// TB1 FIFO REPEAT COUNT READ, 32BIT
	Hpc30Table1FifoRepeatCountWrite	        = 0x9F,				// TB1 FIFO REPEAT COUNT WRITE, 32BIT

	Hpc30Ch2CounterRead					    = 0x20,				// CH2 COUNTER READ, 32BIT
	Hpc30Ch2CounterWrite					= 0xA0,				// CH2 COUNTER WRITE, 32BIT
	Hpc30Ch2CounterModeRead				    = 0x21,				// CH2 COUNTER MODE READ, 4BIT
	Hpc30Ch2CounterModeWrite				= 0xA1,				// CH2 COUNTER MODE WRITE, 4BIT
	Hpc30Table2FifoDataXRead			    = 0x22,				// TB2 FIFO DATA X READ, 32BIT    
	Hpc30Table2FifoDataClear			    = 0xA2,				// TB2 FIFO DATA CLEAR, 1BIT
	Hpc30Table2FifoDataYRead			    = 0x23,				// TB2 FIFO DATA Y READ, 32BIT
    Hpc30Table2HoldTimeWrite       		    = 0xA3,				// TB2 HOLD TIME WRITE, 32BIT    
	Hpc30Table2FifoStatusRead			    = 0x24,				// TB2 FIFO STATUS XY READ, 32BIT    
	Hpc30Table2OutputCountRead			    = 0x25,				// TB2 OUTPUT COUNT READ, 32BIT
	Hpc30Table2OutputCountClear			    = 0xA5,				// TB2 OUTPUT COUNT CLEAR, 1BIT
    Hpc30Table2FifoDatauXRead			    = 0x26,				// TB2 FIFO DATA uX READ, 32BIT    
    Hpc30Table2FifoDatauXWrite	            = 0xA6,				// TB2 FIFO DATA uX WRITE, 32BIT	
    Hpc30Table2FifoDatauYRead			    = 0x27,				// TB2 FIFO DATA uY READ, 32BIT    
    Hpc30Table2FifoDatauYWrite	            = 0xA7,				// TB2 FIFO DATA uY WRITE, 32BIT
    Hpc30Table2FifoStatusuXuYRead		    = 0x28,				// TB2 FIFO STATUS uXuY READ, 32BIT    
    Hpc30Table2FifoDataExeXRead		    	= 0x29,				// TB2 FIFO DATA EXE X READ, 32BIT    
    Hpc30Table2FifoDataExeYRead			    = 0x2A,				// TB2 FIFO DATA EXE Y READ, 32BIT    
    Hpc30Table2FifoStatusExeXYRead		    = 0x2B,				// TB2 FIFO STATUS EXE XY READ, 32BIT            
    Hpc30Table2FifoPopExeXY       		    = 0x2C,				// TB2 FIFO POP EXE XY READ, 32BIT        
    Hpc30Table2HoldTimeRead       		    = 0x2D,				// TB2 HOLD TIME READ, 32BIT
	Hpc30Table2ActiveLevelRead			    = 0x30,				// TB2 ACITVE LEVEL READ, 1BIT
	Hpc30Table2ActiveLevelWrite			    = 0xB0,				// TB2 ACITVE LEVEL WRITE, 1BIT
	Hpc30Table2PulseWidthRead			    = 0x31,				// TB2 PULSE WDITH READ, 32BIT
	Hpc30Table2PulseWidthWrite			    = 0xB1,				// TB2 PULSE WDITH WRITE, 32BIT
	Hpc30Table2EncoderInputChannelRead	    = 0x32,				// TB2 ENCODER INPUT CHANNEL READ, 4BIT
	Hpc30Table2EncoderInputChannelWrite	    = 0xB2,				// TB2 ENCODER INPUT CHANNEL WRITE, 4BIT
	Hpc30Table2ErrorRangeRead	            = 0x33,				// TB2 ERROR RANGE READ, 32BIT
	Hpc30Table2ErrorRangeWrite	            = 0xB3,				// TB2 ERROR RANGE WRITE, 32BIT
	Hpc30Table2OutPortRead	                = 0x34,				// TB2 OUTPUT CHANNEL SETTING READ, 4BIT
	Hpc30Table2OutPortWrite	                = 0xB4,				// TB2 OUTPUT CHANNEL SETTING WRITE, 4BIT
	Hpc30Table2PatternTimeRead	            = 0x35,				// TB2 PATTERN TRIGGER TIME READ, 32BIT
	Hpc30Table2PatternTimeWrite	            = 0xB5,				// TB2 PATTERN TRIGGER TIME WRITE, 32BIT
	Hpc30Table2PatternCountRead	            = 0x36,				// TB2 PATTERN TRIGGER COUNT READ, 32BIT
	Hpc30Table2PatternCountWrite	        = 0xB6,				// TB2 PATTERN TRIGGER COUNT WRITE, 32BIT
	Hpc30Table2TriggerModeRead	            = 0x37,				// TB2 TRIGGER MODE READ, 2BIT
	Hpc30Table2TriggerModeWrite	            = 0xB7,				// TB2 TRIGGER MODE WRITE, 2BIT
   	Hpc30Table2CaptureTimeRead	            = 0x38,				// TB2 CAPTURE TIME READ, 32BIT
	Hpc30Table2CaptureTimeWrite	            = 0xB8,				// TB2 CAPTURE TIME WRITE, 32BIT
	Hpc30Table2GetEnable	                = 0x3A,				// TB2 GET ENABLE, 1BIT
	Hpc30Table2OneShot	                    = 0xB9,				// TB2 ONE SHOT , 1BIT
	Hpc30Table2Enable	                    = 0xBA,				// TB2 ENABLE , 1BIT
	Hpc30Table2Disable	                    = 0xBB,				// TB2 DISABLE , 1BIT
	Hpc30Table2FifoDataXWrite	            = 0xBC,				// TB2 FIFO DATA X WRITE, 32BIT
	Hpc30Table2FifoDataYWrite	            = 0xBD,				// TB2 FIFO DATA Y WRITE, 32BIT
    Hpc30Table2FifoRepeatTimeRead           = 0x3E,				// TB2 FIFO REPEAT TIME READ, 32BIT
	Hpc30Table2FifoRepeatTimeWrite	        = 0xBE,				// TB2 FIFO REPEAT TIME WRITE, 32BIT
    Hpc30Table2FifoRepeatCountRead 	        = 0x3F,				// TB2 FIFO REPEAT COUNT READ, 32BIT
	Hpc30Table2FifoRepeatCountWrite	        = 0xBF,				// TB2 FIFO REPEAT COUNT WRITE, 32BIT

	Hpc30Ch3CounterRead					    = 0x40,				// CH3 COUNTER READ, 32BIT
	Hpc30Ch3CounterWrite					= 0xC0,				// CH3 COUNTER WRITE, 32BIT
	Hpc30Ch3CounterModeRead				    = 0x41,				// CH3 COUNTER MODE READ, 4BIT
	Hpc30Ch3CounterModeWrite				= 0xC1,				// CH3 COUNTER MODE WRITE, 4BIT
	Hpc30Table3FifoDataXRead			    = 0x42,				// TB3 FIFO DATA X READ, 32BIT    
	Hpc30Table3FifoDataClear			    = 0xC2,				// TB3 FIFO DATA CLEAR, 1BIT
	Hpc30Table3FifoDataYRead			    = 0x43,				// TB3 FIFO DATA Y READ, 32BIT    
    Hpc30Table3HoldTimeWrite       		    = 0xC3,				// TB3 HOLD TIME WRITE, 32BIT    
	Hpc30Table3FifoStatusRead			    = 0x44,				// TB3 FIFO STATUS XY READ, 32BIT    
	Hpc30Table3OutputCountRead			    = 0x45,				// TB3 OUTPUT COUNT READ, 32BIT
    Hpc30Table3OutputCountClear			    = 0xC5,				// TB3 OUTPUT COUNT CLEAR, 1BIT
    Hpc30Table3FifoDatauXRead			    = 0x46,				// TB3 FIFO DATA uX READ, 32BIT    
    Hpc30Table3FifoDatauXWrite	            = 0xC6,				// TB3 FIFO DATA uX WRITE, 32BIT	
    Hpc30Table3FifoDatauYRead			    = 0x47,				// TB3 FIFO DATA uY READ, 32BIT    
    Hpc30Table3FifoDatauYWrite	            = 0xC7,				// TB3 FIFO DATA uY WRITE, 32BIT
    Hpc30Table3FifoStatusuXuYRead		    = 0x48,				// TB3 FIFO STATUS uXuY READ, 32BIT    
    Hpc30Table3FifoDataExeXRead		    	= 0x49,				// TB3 FIFO DATA EXE X READ, 32BIT    
    Hpc30Table3FifoDataExeYRead			    = 0x4A,				// TB3 FIFO DATA EXE Y READ, 32BIT    
    Hpc30Table3FifoStatusExeXYRead		    = 0x4B,				// TB3 FIFO STATUS EXE XY READ, 32BIT            	
    Hpc30Table3FifoPopExeXY       		    = 0x4C,				// TB3 FIFO POP EXE XY READ, 32BIT        
    Hpc30Table3HoldTimeRead       		    = 0x4D,				// TB3 HOLD TIME READ, 32BIT
	Hpc30Table3ActiveLevelRead			    = 0x50,				// TB3 ACITVE LEVEL READ, 1BIT
	Hpc30Table3ActiveLevelWrite			    = 0xD0,				// TB3 ACITVE LEVEL WRITE, 1BIT
	Hpc30Table3PulseWidthRead			    = 0x51,				// TB3 PULSE WDITH READ, 32BIT
	Hpc30Table3PulseWidthWrite			    = 0xD1,				// TB3 PULSE WDITH WRITE, 32BIT
	Hpc30Table3EncoderInputChannelRead	    = 0x52,				// TB3 ENCODER INPUT CHANNEL READ, 4BIT
	Hpc30Table3EncoderInputChannelWrite	    = 0xD2,				// TB3 ENCODER INPUT CHANNEL WRITE, 4BIT
	Hpc30Table3ErrorRangeRead	            = 0x53,				// TB3 ERROR RANGE READ, 32BIT
	Hpc30Table3ErrorRangeWrite	            = 0xD3,				// TB3 ERROR RANGE WRITE, 32BIT
	Hpc30Table3OutPortRead	                = 0x54,				// TB3 OUTPUT CHANNEL SETTING READ, 4BIT
	Hpc30Table3OutPortWrite	                = 0xD4,				// TB3 OUTPUT CHANNEL SETTING WRITE, 4BIT
	Hpc30Table3PatternTimeRead	            = 0x55,				// TB3 PATTERN TRIGGER TIME READ, 32BIT
	Hpc30Table3PatternTimeWrite	            = 0xD5,				// TB3 PATTERN TRIGGER TIME WRITE, 32BIT
	Hpc30Table3PatternCountRead	            = 0x56,				// TB3 PATTERN TRIGGER COUNT READ, 32BIT
	Hpc30Table3PatternCountWrite	        = 0xD6,				// TB3 PATTERN TRIGGER COUNT WRITE, 32BIT
	Hpc30Table3TriggerModeRead	            = 0x57,				// TB3 TRIGGER MODE READ, 2BIT
	Hpc30Table3TriggerModeWrite	            = 0xD7,				// TB3 TRIGGER MODE WRITE, 2BIT
  	Hpc30Table3CaptureTimeRead	            = 0x58,				// TB3 CAPTURE TIME READ, 32BIT
	Hpc30Table3CaptureTimeWrite	            = 0xD8,				// TB3 CAPTURE TIME WRITE, 32BIT
	Hpc30Table3OneShot	                    = 0xD9,				// TB3 ONE SHOT , 1BIT
	Hpc30Table3GetEnable	                = 0x5A,				// TB3 GET ENABLE, 1BIT
	Hpc30Table3Enable	                    = 0xDA,				// TB3 ENABLE , 1BIT
	Hpc30Table3Disable	                    = 0xDB,				// TB3 DISABLE , 1BIT
	Hpc30Table3FifoDataXWrite	            = 0xDC,				// TB3 FIFO DATA X WRITE, 32BIT
	Hpc30Table3FifoDataYWrite	            = 0xDD,				// TB3 FIFO DATA Y WRITE, 32BIT
    Hpc30Table3FifoRepeatTimeRead           = 0x5E,				// TB3 FIFO REPEAT TIME READ, 32BIT
	Hpc30Table3FifoRepeatTimeWrite	        = 0xDE,				// TB3 FIFO REPEAT TIME WRITE, 32BIT
    Hpc30Table3FifoRepeatCountRead 	        = 0x5F,				// TB3 FIFO REPEAT COUNT READ, 32BIT
	Hpc30Table3FifoRepeatCountWrite	        = 0xDF,				// TB3 FIFO REPEAT COUNT WRITE, 32BIT

	Hpc30Ch4CounterRead					    = 0x60,				// CH4 COUNTER READ, 32BIT
	Hpc30Ch4CounterWrite					= 0xE0,				// CH4 COUNTER WRITE, 32BIT
	Hpc30Ch4CounterModeRead				    = 0x61,				// CH4 COUNTER MODE READ, 4BIT
	Hpc30Ch4CounterModeWrite				= 0xE1,				// CH4 COUNTER MODE WRITE, 4BIT
	Hpc30Table4FifoDataXRead			    = 0x62,				// TB4 FIFO DATA X READ, 32BIT    
	Hpc30Table4FifoDataClear			    = 0xE2,				// TB4 FIFO DATA CLEAR, 1BIT
	Hpc30Table4FifoDataYRead			    = 0x63,				// TB4 FIFO DATA Y READ, 32BIT    
    Hpc30Table4HoldTimeWrite       		    = 0xE3,				// TB4 HOLD TIME WRITE, 32BIT
	Hpc30Table4FifoStatusRead			    = 0x64,				// TB4 FIFO STATUS XY READ, 32BIT    
	Hpc30Table4OutputCountRead			    = 0x65,				// TB4 OUTPUT COUNT READ, 32BIT
	Hpc30Table4OutputCountClear			    = 0xE5,				// TB4 OUTPUT COUNT CLEAR, 1BIT
    Hpc30Table4FifoDatauXRead			    = 0x66,				// TB4 FIFO DATA uX READ, 32BIT    
    Hpc30Table4FifoDatauXWrite	            = 0xE6,				// TB4 FIFO DATA uX WRITE, 32BIT	
    Hpc30Table4FifoDatauYRead			    = 0x67,				// TB4 FIFO DATA uY READ, 32BIT    
    Hpc30Table4FifoDatauYWrite	            = 0xE7,				// TB4 FIFO DATA uY WRITE, 32BIT
    Hpc30Table4FifoStatusuXuYRead		    = 0x68,				// TB4 FIFO STATUS uXuY READ, 32BIT    
    Hpc30Table4FifoDataExeXRead		    	= 0x69,				// TB4 FIFO DATA EXE X READ, 32BIT    
    Hpc30Table4FifoDataExeYRead			    = 0x6A,				// TB4 FIFO DATA EXE Y READ, 32BIT    
    Hpc30Table4FifoStatusExeXYRead		    = 0x6B,				// TB4 FIFO STATUS EXE XY READ, 32BIT      
    Hpc30Table4FifoPopExeXY       		    = 0x6C,				// TB4 FIFO POP EXE XY READ, 32BIT        
    Hpc30Table4HoldTimeRead       		    = 0x6D,				// TB4 HOLD TIME READ, 32BIT
	Hpc30Table4ActiveLevelRead			    = 0x70,				// TB4 ACITVE LEVEL READ, 1BIT
	Hpc30Table4ActiveLevelWrite			    = 0xF0,				// TB4 ACITVE LEVEL WRITE, 1BIT
	Hpc30Table4PulseWidthRead			    = 0x71,				// TB4 PULSE WDITH READ, 32BIT
	Hpc30Table4PulseWidthWrite			    = 0xF1,				// TB4 PULSE WDITH WRITE, 32BIT
	Hpc30Table4EncoderInputChannelRead	    = 0x72,				// TB4 ENCODER INPUT CHANNEL READ, 4BIT
	Hpc30Table4EncoderInputChannelWrite	    = 0xF2,				// TB4 ENCODER INPUT CHANNEL WRITE, 4BIT
	Hpc30Table4ErrorRangeRead	            = 0x73,				// TB4 ERROR RANGE READ, 32BIT
	Hpc30Table4ErrorRangeWrite	            = 0xF3,				// TB4 ERROR RANGE WRITE, 32BIT
	Hpc30Table4OutPortRead	                = 0x74,				// TB4 OUTPUT CHANNEL SETTING READ, 4BIT
	Hpc30Table4OutPortWrite	                = 0xF4,				// TB4 OUTPUT CHANNEL SETTING WRITE, 4BIT
	Hpc30Table4PatternTimeRead	            = 0x75,				// TB4 PATTERN TRIGGER TIME READ, 32BIT
	Hpc30Table4PatternTimeWrite	            = 0xF5,				// TB4 PATTERN TRIGGER TIME WRITE, 32BIT
	Hpc30Table4PatternCountRead	            = 0x76,				// TB4 PATTERN TRIGGER COUNT READ, 32BIT
	Hpc30Table4PatternCountWrite	        = 0xF6,				// TB4 PATTERN TRIGGER COUNT WRITE, 32BIT
	Hpc30Table4TriggerModeRead	            = 0x77,				// TB4 TRIGGER MODE READ, 2BIT
	Hpc30Table4TriggerModeWrite	            = 0xF7,				// TB4 TRIGGER MODE WRITE, 2BIT
	Hpc30Table4CaptureTimeRead	            = 0x78,				// TB4 CAPTURE TIME READ, 32BIT
	Hpc30Table4CaptureTimeWrite	            = 0xF8,				// TB4 CAPTURE TIME WRITE, 32BIT    
	Hpc30Table4OneShot	                    = 0xF9,				// TB4 ONE SHOT , 1BIT
	Hpc30Table4GetEnable	                = 0x7A,				// TB4 GET ENABLE, 1BIT
	Hpc30Table4Enable	                    = 0xFA,				// TB4 ENABLE , 1BIT
	Hpc30Table4Disable	                    = 0xFB,				// TB4 DISABLE , 1BIT
	Hpc30Table4FifoDataXWrite	            = 0xFC,				// TB4 FIFO DATA X WRITE, 32BIT
	Hpc30Table4FifoDataYWrite	            = 0xFD,				// TB4 FIFO DATA Y WRITE, 32BIT
    Hpc30Table4FifoRepeatTimeRead           = 0x7E,				// TB4 FIFO REPEAT TIME READ, 32BIT
	Hpc30Table4FifoRepeatTimeWrite	        = 0xFE,				// TB4 FIFO REPEAT TIME WRITE, 32BIT
    Hpc30Table4FifoRepeatCountRead 	        = 0x7F,				// TB4 FIFO REPEAT COUNT READ, 32BIT
	Hpc30Table4FifoRepeatCountWrite	        = 0xFF,				// TB4 FIFO REPEAT COUNT WRITE, 32BIT
} HPC30COMMAND;



/* Read/Write port							*/
typedef enum _LCM10PORT_DATA_WRITE
{	
	Lcm10Reset		    = 0x06,			// Software reset.
	Lcm10Command		= 0x10,
	Lcm10Data12		    = 0x12,			// MSB of data port(31 ~ 16 bit)
	Lcm10Data34		    = 0x14,			// LSB of data port(15 ~ 0 bit)
	Lcm10CmStatus     	= 0x1C,
}LCM10PORT_DATA_WRITE;

typedef enum _LCM10PORT_CH_STAUTS
{
	Lcm10Ch1Mech		= 0x20,
	Lcm10Ch1Status	    = 0x22,
	Lcm10Ch2Mech		= 0x30,
	Lcm10Ch2Status	    = 0x32,
	Lcm10Ch3Mech		= 0x40,
	Lcm10Ch3Status	    = 0x42,
	Lcm10Ch4Mech		= 0x50,
	Lcm10Ch4Status	    = 0x52
}LCM10PORT_CH_STAUTS;

typedef enum _LCM10PORT_ETC
{
	Lcm10DiIntFlag	    = 0x60,
	Lcm10DiIntRiseMask  = 0x62,
	Lcm10DiIntFallMask  = 0x64,
	Lcm10CompIntFlag	= 0x66,
	Lcm10CompIntMask	= 0x68,
	Lcm10DinData		= 0x6A,
	Lcm10DoutData		= 0x6C
}LCM10PORT_ETC;

/* CNT COMMAND LIST						*/
typedef enum _LCM10COMMAND
{
	Lcm10Ch1CounterRead						= 0x00,				// CH1 COUNTER READ, 32BIT
	Lcm10Ch1CounterClearWrite 				= 0xC5, 			// CH1 COUNTER CLEAR, 1BIT
	Lcm10Ch1CounterModeWrite				= 0x81,				// CH1 COUNTER MODE WRITE, 8bit
	Lcm10Ch1CounterModeRead					= 0x01, 			// CH1 COUNTER MODE READ, 8bit
	Lcm10Ch1FrequencyAddressWrite			= 0x82,				// CH1 FREQUENCY ADDRESS WRITE, 32bit
	Lcm10Ch1DutaRatioAddressWrite			= 0x83,				// CH1 DUTY RATIO ADDRESS WRITE, 32bit
	Lcm10Ch1FrequencyWrite					= 0x84,				// CH1 FREQUENCY DATA WRITE, 32bit
	Lcm10Ch1FrequencyRead					= 0x08,				// CH1 FREQUENCY DATA READ, 32bit		
	Lcm10Ch1DutyRatioWrite					= 0x85,				// CH1 DUTY RATIO WRUTE, 32bit		
	Lcm10Ch1DutyRatioRead					= 0x09,				// CH1 DUTY RATIO READ, 32bit
	Lcm10Ch1ManualFrequencyWrite			= 0x86, 			// CH1 MANUAL FREQUENCY WRITE, 32bit
	Lcm10Ch1ManualFrequencyRead 			= 0x0C, 			// CH1 MANUAL FREQUENCY READ, 32bit
	Lcm10Ch1ManualDutyRatioWrite			= 0x87,				// CH1 MANUAL DUTY RATIO WRITE, 32bit
	Lcm10Ch1ManualDutyRatioRead				= 0x0D,				// CH1 MANUAL DUTY RATIO READ, 32bit

	Lcm10Ch1PulseWidthWrite					= 0xC7,				// CH1 PULSE WIDTH WRITE(Auto), 32bit 
	Lcm10Ch1PulseWidthRead					= 0x4B,				// CH1 PULSE WIDTH READ(Auto), 32bit

	Lcm10Ch1ManualPulseWidthWrite			= 0xC8,				// CH1 MANUAL PULSE WIDTH WRITE, 32bit
	Lcm10Ch1ManualPulseWidthRead			= 0x4C,				// CH1 MANUAL PULSE WIDTH READ, 32bit

	Lcm10Ch1PWMOutModeWrite					= 0x88,				// CH1 PWM OUT MODE WRITE, 8bit
	Lcm10Ch1PWMOutModeRead 					= 0x06, 			// TO CHANGE !!!!!!!!!!!!!!!!!!! //CH1 PWM OUT MODE WRITE, 8bit
	Lcm10Ch1PWMRamDataClearWrite			= 0x89,				// CH! PWM RAMDATA CLEAR WRITE, 1bit
	Lcm10Ch1PWMEnableWrite					= 0x8A,				// CH1 PWM ENABLE WRITE, 1bit
	Lcm10Ch1PWMEnableRead					= 0x07, 			// CH1 PWM ENABLE READ, 1bit
	Lcm10Ch1MaxVelWrite						= 0x8C,				// CH1 MAX VEL WRITE 
	Lcm10Ch1MaxVelRead						= 0x04,				// CH1 MAX VEL READ 
	Lcm10Ch1MinVelWrite 					= 0xC6, 			// CH1 MIN VEL WRITE 
	Lcm10Ch1MinVelRead						= 0x4A, 			// CH1 MIN VEL READ 
	Lcm10Ch1PWMPulseControlModeWrite		= 0xC9, 			// CH1 PULSE CONTROL MODE WRITE (1bit)
	Lcm10Ch1PWMPulseControlModeRead			= 0x4D, 			// CH1 PULSE CONTROL MODE READ (1bit)
	Lcm10Ch1PWMVelIntervalWrite				= 0x8D,				// CH1 VEL INTERVAL WRITE (32bit)	
	Lcm10Ch1PWMVelIntervalRead	 			= 0x49, 			// CH1 VEL INTERVAL READ (32bit)


	Lcm10Ch1RamWriteEnableWrite				= 0xC2,				// CH1 RAM WRTIE ENABLE WRITE, 1bit
	Lcm10Ch1RamWriteEnableRead 				= 0x44, 			// CH1 RAM WRTIE ENABLE WRITE, 1bit
	Lcm10Ch1RamWriteDoneRead				= 0x0E,				// CH1 RAM WRITE DONW READ, 4bit
	Lcm10Ch1ActualVelocityRead				= 0x45,				// CH1 ACTUAL VELOCITY READ, 32bit
	Lcm10RamAddressDataCheckWrite			= 0xC0,				// CH1 RAM ADDRESS FOR DATA CHECK WRITE, 32bit 
	Lcm10RamDataCheckWrite					= 0xC1,				// CH1 RAM DATA CHECK ENABLE WRITE, 1bit

	Lm10Ch1OperationModeWrite				= 0xC3, 			// CH1 OPERATIONMODE WRITE, 4bit  (default : 5000 table mode)
	Lm10Ch1OperationModeRead				= 0x40, 			// CH1 OPERATIONMODE READ, 4bit  (default : 5000 table mode)
	
} LCM10COMMAND;

#endif

#ifndef AXT_COUNTER_HPC4_TRRIGER_MODE_DEF
#define AXT_COUNTER_HPC4_TRRIGER_MODE_DEF
typedef enum _AXT_COUNTER_HPC4_TRRIGER_MODE
{
	HPC4_RANGE_TRIGGER                                      = 0,        // Error distance check
	HPC4_VECTOR_TRIGGER                                     = 1,        // Error distance + Vector direction check
	HPC4_PATTERN_TRIGGER                                    = 3         // Pattern Trigger
} AXT_COUNTER_HPC4_TRRIGER_MODE;
#endif

typedef void (__stdcall *AXT_EVENT_PROC)(long lActiveNo, DWORD uFlag);
typedef void (__stdcall *AXT_INTERRUPT_PROC)(long lActiveNo, DWORD  uFlag);
                                                                    
#endif                                                                //__AXT_AXHS_H__                                                                                     
