/* reps-strings.h : interpreting representations w.r.t. strings
 *
 * ====================================================================
 * Copyright (c) 2000-2001 CollabNet.  All rights reserved.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at http://subversion.tigris.org/license-1.html.
 * If newer versions of this license are posted there, you may use a
 * newer version instead, at your option.
 * ====================================================================
 */

#ifndef SVN_LIBSVN_FS_REPS_STRINGS_H
#define SVN_LIBSVN_FS_REPS_STRINGS_H

#include "db.h"
#include "svn_io.h"
#include "svn_fs.h"
#include "trail.h"
#include "reps-table.h"
#include "strings-table.h"


/* Return the string key pointed to by REP, allocated in POOL.
   ### todo:
   The behavior of this function on non-fulltext representations is
   undefined at present.  */
const char *svn_fs__string_key_from_rep (skel_t *rep, apr_pool_t *pool);


/* Set STR->data to the fulltext string for REP in FS, and STR->len to
   the string's length, as part of TRAIL.  The data is allocated in
   TRAIL->pool.  */
svn_error_t *svn_fs__string_from_rep (svn_string_t *str,
                                      svn_fs_t *fs,
                                      skel_t *rep,
                                      trail_t *trail);


/* Return non-zero if representation skel REP is mutable.  */
int svn_fs__rep_is_mutable (skel_t *rep);


/* Get a key to a mutable version of the representation pointed to by
   KEY in FS, and store it in *NEW_KEY.  If KEY is already mutable,
   *NEW_KEY is set to KEY, else *NEW_KEY is set to a new rep key
   allocated in TRAIL->pool.  */
svn_error_t *svn_fs__get_mutable_rep (const char **new_key,
                                      const char *key,
                                      svn_fs_t *fs, 
                                      trail_t *trail);

/* Copy into BUF *LEN bytes starting at OFFSET from the string
   represented by REP in FS, as part of TRAIL.
   
   The number of bytes actually copied is stored in *LEN.

   Justificatory note: REP is a skel instead of a key because our
   callers want to cache the representation once and then use it for
   every iteration of a reading loop.  If this function took a key,
   then each iteration would have an extra database hit to read the
   rep.  Comments welcome.  */
svn_error_t *svn_fs__rep_read_range (svn_fs_t *fs,
                                     skel_t *rep,
                                     char *buf,
                                     apr_size_t offset,
                                     apr_size_t *len,
                                     trail_t *trail);


/* stabilize_rep */

#endif /* SVN_LIBSVN_FS_REPS_STRINGS_H */



/* 
 * local variables:
 * eval: (load-file "../svn-dev.el")
 * end:
 */
