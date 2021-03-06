/*
 * utility functions to handle the java class
 * org.tigris.subversion.lib.Schedule
 *
 * ====================================================================
 * Copyright (c) 2000-2002 CollabNet.  All rights reserved.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at http://subversion.tigris.org/license-1.html.
 * If newer versions of this license are posted there, you may use a
 * newer version instead, at your option.
 *
 * This software consists of voluntary contributions made by many
 * individuals.  For exact contribution history, see the revision
 * history and logs, available at http://subversion.tigris.org/.
 * ====================================================================
 */

#ifndef SVN_JNI_SCHEDULE_H
#define SVN_JNI_SCHEDULE_H

/* includes */
#include <jni.h>
#include <svn_wc.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* functions */

/**
 * create a new org.tigris.subversion.lib.Schedule instance 
 *
 * @param JNIEnv JNI Environment
 * @param hasException
 * @param schedule integer representation of the appropriate constants
 */
jobject 
schedule__create(JNIEnv *env, jboolean *hasException, jint schedule);

/**
 * create a new org.tigris.subversion.lib.Schedule instance
 * and use the corresponding svn_wc_schedule_t as parameter
 */
jobject
schedule__create_from_svn_wc_schedule_t(JNIEnv *env, jboolean *hasException,
                                        enum svn_wc_schedule_t schedule);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
/* 
 * local variables:
 * eval: (load-file "../../../../tools/dev/svn-dev.el")
 * end: 
 */
