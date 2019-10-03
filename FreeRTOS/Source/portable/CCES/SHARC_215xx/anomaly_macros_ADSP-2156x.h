/*****************************************************************************
    Copyright (C) 2018 Analog Devices Inc. All Rights Reserved.
*****************************************************************************/

#ifndef _ANOMALY_MACROS_ADSP_2156X_H
#define _ANOMALY_MACROS_ADSP_2156X_H

#if defined(__ADSP2156x__)
#undef WA_20000081
#define WA_20000081 \
 (defined(__ADSP2156x__) && defined(__ADSPSHARC__) && defined(__SILICON_REVISION__))
#endif

#endif /* _ANOMALY_MACROS_ADSP_2156X_H */
