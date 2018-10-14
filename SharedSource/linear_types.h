//
//  linear_types.h
//  LinearVoice
//
//  Created by Carl-Johan Waldeck on 19/12/14.
//  Copyright (c) 2014 Roger Martinson. All rights reserved.
//

#ifndef LinearVoice_linear_types_h
#define LinearVoice_linear_types_h

#ifdef LINVO_ON_SHARC
typedef short int16_t;
typedef unsigned short uint16_t;
typedef char int8_t;
typedef unsigned char uint8_t;
#else
#include <stdint.h>
#endif //LINVO_ON_SHARC

#endif
