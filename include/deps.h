/* 
    This file defines and includes all necessary dependencies.
 !  NOTE: This is only for non release/early develpment! 
    The "!" and "?" in comments are for the "Better Comments" extension, "aaron-bond.better-comments".
    TODO: add JSON headers, add als SDL2 headers to files(otherwise it doesn't work)
*/
//?############################################################
/*
  *  Standart C Library, used by:
*/
#include <sys/stat.h>       // main.c
#include <unistd.h>         // main.c
#include <math.h>           // map.c
#include <stdio.h>          // map.c, texture.c, log_utils.c
#include <stdlib.h>         // map.c, texture.c
#include <string.h>         // map.c, texture.c, log_utils.c
#include <time.h>           // log_utils.c
#include <stddef.h>         // log_utils.h

//?###########################################################
/*
  *  SDL2 Headers, used by:
*/
#include <SDL2/SDL.h>       // texture.c, raycaster.h, texture.h
#include <SDL2/SDL_image.h> // texture.c

//?###########################################################
/*                            
  *  Project Headers, used by:
*/                            
#include "raycaster.h"      // main.c, raycaster.c
#include "texture.h"        // main.c, raycaster.c, texture.c, raycaster.h
#include "log_utils.h"      // main.c, map.c, texture.c, log_utils.c
#include "map.h"            // raycaster.c, map.c, raycaster.h
#include "json.h"           // map.c

//?###########################################################
/*
  *  OPENMP Header, used by:
  !  main.c
  !  raycaster.c
*/
#ifdef _OPENMP
#include <omp.h> 
#endif
//?######################END#OF#FILE##########################