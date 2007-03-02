#############################################################################
#
# $Id$
#
# Copyright (C) 1998-2006 Inria. All rights reserved.
#
# This software was developed at:
# IRISA/INRIA Rennes
# Projet Lagadic
# Campus Universitaire de Beaulieu
# 35042 Rennes Cedex
# http://www.irisa.fr/lagadic
#
# This file is part of the ViSP toolkit.
#
# This file may be distributed under the terms of the Q Public License
# as defined by Trolltech AS of Norway and appearing in the file
# LICENSE included in the packaging of this file.
#
# Licensees holding valid ViSP Professional Edition licenses may
# use this file in accordance with the ViSP Commercial License
# Agreement provided with the Software.
#
# This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
# WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#
# Contact visp@irisa.fr if any conditions of this licensing are
# not clear to you.
#
# Description:
# Try to find pthread library.
# Once run this will define:
#
# PTHREAD_FOUND
# PTHREAD_INCLUDE_DIR
# PTHREAD_LIBRARIES
#
# Authors:
# Fabien Spindler
#
#############################################################################

#IF(NOT UNIX AND NOT WIN32)
#  SET(PTHREAD_FOUND FALSE)
#ELSE(NOT UNIX AND NOT WIN32)

  FIND_PATH(PTHREAD_INCLUDE_DIR pthread.h
    /usr/include
    $ENV{PTHREAD_INCLUDE_PATH}
    )
  #MESSAGE("DBG PTHREAD_INCLUDE_DIR=${PTHREAD_INCLUDE_DIR}")

  # pthreadVSE pthreadGCE pthreadGC pthreadVC1 pthreadVC2 are comming from web
  FIND_LIBRARY(PTHREAD_LIBRARY
    NAMES pthread pthreadGC2 pthreadVSE pthreadGCE pthreadGC pthreadVC1 pthreadVC2
    PATHS
    /usr/lib64
    /usr/lib
    /usr/local/lib
    /lib
    /lib64
    $ENV{PTHREAD_LIBRARY_PATH}
    )

  #MESSAGE(STATUS "DBG PTHREAD_LIBRARY=${PTHREAD_LIBRARY}")

  ## --------------------------------

  IF(PTHREAD_LIBRARY)
    SET(PTHREAD_LIBRARIES ${PTHREAD_LIBRARY})
  ELSE(PTHREAD_LIBRARY)
    #MESSAGE(SEND_ERROR "pthread library not found.")
  ENDIF(PTHREAD_LIBRARY)

  IF(NOT PTHREAD_INCLUDE_DIR)
    #MESSAGE(SEND_ERROR "pthread include dir not found.")
  ENDIF(NOT PTHREAD_INCLUDE_DIR)

  IF(PTHREAD_LIBRARIES AND PTHREAD_INCLUDE_DIR)
    SET(PTHREAD_FOUND TRUE)
    SET(HAVE_PTHREAD 1)
  ELSE(PTHREAD_LIBRARIES AND PTHREAD_INCLUDE_DIR)
    SET(PTHREAD_FOUND FALSE)
  ENDIF(PTHREAD_LIBRARIES AND PTHREAD_INCLUDE_DIR)

  #MARK_AS_ADVANCED(
  #  PTHREAD_INCLUDE_DIR
  #  PTHREAD_LIBRARIES
  #  )
  MESSAGE(STATUS "PTHREAD_FOUND: ${PTHREAD_FOUND}")

#ENDIF(NOT UNIX AND NOT WIN32)
