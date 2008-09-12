/**
 * @copyright
 * ====================================================================
 * Copyright (c) 2008 CollabNet.  All rights reserved.
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
 * @endcopyright
 *
 * @file svn_wc_db.h
 * @brief The Subversion Working Copy Library - Metadata/Base-Text Support
 *
 * Requires:
 *            - A working copy
 *
 * Provides:
 *            - Ability to manipulate working copy's administrative files.
 *
 * Used By:
 *            - The main working copy library
 */

#ifndef SVN_WC_DB_H
#define SVN_WC_DB_H

#include "svn_types.h"
#include "svn_error.h"
#include "svn_config.h"
#include "svn_io.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Context data structure for interacting with the administrative data. */
typedef struct svn_wc__db_t svn_wc__db_t;


/* Enum indicating what kind of versioned object we're talking about.
 *
 * ### KFF: That is, my understanding is that this is *not* an enum
 * ### indicating what kind of storage the DB is using, even though
 * ### one might think that from its name.  Rather, the "svn_wc__db_"
 * ### is a generic prefix, and this "_kind_t" type indicates the kind
 * ### of something that's being stored in the DB.
 *
 * ### KFF: Does this overlap too much with what svn_node_kind_t does?
 * ### Also, does it make sense to encode 'absent' across these types,
 * ### rather than just having a separate 'absent' flag?  In general,
 * ### the interfaces in here give a lot of prominence to absence; I'm
 * ### wondering why we're treating it so specially.
 */
typedef enum svn_wc__db_kind_t {
    svn_wc__db_kind_dir,
    svn_wc__db_kind_file,
    svn_wc__db_kind_symlink,

    svn_wc__db_kind_absent_dir,
    svn_wc__db_kind_absent_file,
    svn_wc__db_kind_absent_symlink
} svn_wc__db_kind_t;


typedef enum svn_wc__db_status_t {
    svn_wc__db_status_normal,
    svn_wc__db_status_added,  /* ### no history. text_mod set to TRUE */
    svn_wc__db_status_moved,  /* ### has history */
    svn_wc__db_status_copied,  /* ### has history */
    svn_wc__db_status_deleted  /* ### text_mod, prop_mod will be FALSE */
} svn_wc__db_status_t;


/* ### note conventions of "result_pool" for the pool where return results
   ### are allocated, and "scratch_pool" for the pool that is used for
   ### intermediate allocations (and which can be safely cleared upon
   ### return from the function).
*/

/* ### NOTE: I have not provided docstrings for most of this file at this
   ### point in time. The shape and extent of this API is still in massive
   ### flux. I'm iterating in public, but do not want to doc until it feels
   ### like it is "Right".
*/

/* ### where/how to handle: text_time, prop_time, locks, working_size
 */

/**
 * Open the administrative database for the working copy identified by the
 * (absolute) @a path. The (opaque) handle for interacting with the database
 * will be returned in @a *db. Note that the database MAY NOT be specific
 * to this working copy. The path is merely used to locate the database, but
 * the administrative database could be global, or it could be per-WC.
 *
 * ### KFF: Would be good to state, here or in an introductory comment
 * ### at the top of this file, whether subsequent 'path' parameters
 * ### are absolute, or relative to the root at which @a *db was
 * ### opened, or perhaps that both are acceptable.
 * ###
 * ### Also, suppose @a path is some subdirectory deep inside a
 * ### working copy.  Is it okay to pass it, or do we need to pass the
 * ### root of that working copy?  Perhaps there needs to be an output
 * ### parameter 'const char **wc_root_path', so a person can tell if
 * ### they opened the root or some subdir?
 *
 * The configuration options are provided by @a config, and must live at
 * least as long as the database.
 *
 * Intermediate allocations will be performed in @a scratch_pool, and the
 * resulting context will be allocated in @a result_pool.
 */
svn_error_t *
svn_wc__db_open(svn_wc__db_t **db,
                const char *path,
                svn_config_t *config,
                apr_pool_t *result_pool,
                apr_pool_t *scratch_pool);


/**
 * @defgroup svn_wc__db_base  BASE tree management
 * @{
 */

/* ### base_add_* can also replace. should be okay? */

/* ### props optional. children must be known before calling (but may be
 * ### NULL or a zero-length array to indicate "no children").
 *
 * ### KFF: By the way, I like the convention of using "scratch_pool"
 * ### to indicate "pool in which temporary work may be done, but no
 * ### results allocated (so you can feel free to clear or destroy it
 * ### after this call)".  I presume that's what it means?
 */
svn_error_t *
svn_wc__db_base_add_directory(svn_wc__db_t *db,
                              const char *path,
                              svn_revnum_t revision,
                              apr_hash_t *props,
                              const apr_array_header_t *children,
                              const char *repos_url,
                              const char *repos_uuid,
                              apr_pool_t *scratch_pool);


/* ### contents, props, checksum are optional */
svn_error_t *
svn_wc__db_base_add_file(svn_wc__db_t *db,
                         const char *path,
                         svn_revnum_t revision,
                         apr_hash_t *props,
                         svn_stream_t *contents,
                         svn_checksum_t *checksum,
                         apr_pool_t *scratch_pool);


svn_error_t *
svn_wc__db_base_set_props(svn_wc__db_t *db,
                          const char *path,
                          apr_hash_t *props,
                          apr_pool_t *scratch_pool);


/* ### lib pulls contents into storage. resulting content's checksum is
   ### given by @a checksum. (checksum optional; if omitted, it will be
   ### computed during stream-read) */
svn_error_t *
svn_wc__db_base_set_contents(svn_wc__db_t *db,
                             const char *path,
                             svn_stream_t *contents,
                             svn_checksum_t *checksum,
                             apr_pool_t *scratch_pool);


/* ### caller pushes contents into storage. the resulting (pushed) content
   ### will have @a checksum recorded. (checksum optional; if omitted, it
   ### will be computed to stream-write) */
svn_error_t *
svn_wc__db_base_get_writable_contents(svn_stream_t **contents,
                                      svn_wc__db_t *db,
                                      const char *path,
                                      svn_checksum_t *checksum,
                                      apr_pool_t *result_pool,
                                      apr_pool_t *scratch_pool);


/* ### what data to keep for a symlink? props optional.
 *
 * ### KFF: This is an interesting question, because currently
 * ### symlinks are versioned as regular files with the svn:special
 * ### property; then the file's text contents indicate that it is a
 * ### symlink and where that symlink points.  That's for portability:
 * ### you can check 'em out onto a platform that doesn't support
 * ### symlinks, and even modify the link and check it back in.  It's
 * ### a great solution; but then the question for wc-ng is:
 * ###
 * ### Suppose you check out a symlink on platform X and platform Y. 
 * ### X supports symlinks; Y does not.  Should the wc-ng storage for
 * ### those two be the same?  I mean, on platform Y, the file is just
 * ### going to look and behave like a regular file.  It would be sort
 * ### of odd for the wc-ng storage for that file to be of a different
 * ### type from all the other files.  (On the other hand, maybe it's
 * ### weird today that the wc-1 storage for a working symlink is to
 * ### be like a regular file (i.e., regular text-base and whatnot).
 * ###
 * ### I'm still feeling my way around this problem; just pointing out
 * ### the issues.
 */
svn_error_t *
svn_wc__db_base_add_symlink(svn_wc__db_t *db,
                            const char *path,
                            svn_revnum_t revision,
                            apr_hash_t *props,
                            const char *target,
                            apr_pool_t *scratch_pool);


/* ### keep the revision?
 *
 * ### KFF: What are the possible reasons for absence?  
 *
 *   - excluded (as in 'svn_depth_exclude')
 *   - ...?  I know there's more, but I can't think of it now.
 *
 * ### I think it would help to list out the causes of absence;
 * ### that'll help us think about questions like whether we need
 * ### 'revision' or not.
 */
svn_error_t *
svn_wc__db_base_add_absent_node(svn_wc__db_t *db,
                                const char *path,
                                svn_revnum_t revision,
                                svn_wc__db_kind_t kind,
                                apr_pool_t *scratch_pool);


svn_error_t *
svn_wc__db_base_delete(svn_wc__db_t *db,
                       const char *path,
                       apr_pool_t *scratch_pool);


/* ### revision is for dst_path ("after moving src_path to dst_path,
 * ### mark the new nodes (node?) as being at that revision")
 *
 * ### KFF: Hrm?  Do you mean src_path?
 */
svn_error_t *
svn_wc__db_base_move(svn_wc__db_t *db,
                     const char *src_path,
                     const char *dst_path,
                     svn_revnum_t revision,
                     apr_pool_t *scratch_pool);


/* ### NULL may be given for OUT params */
svn_error_t *
svn_wc__db_base_get_info(svn_wc__db_kind_t *kind,
                         svn_revnum_t *revision,
                         const char **url,
                         const char **repos_url,
                         const char **repos_uuid,
                         svn_wc__db_t *db,
                         const char *path,
                         apr_pool_t *result_pool,
                         apr_pool_t *scratch_pool);


svn_error_t *
svn_wc__db_base_get_prop(const svn_string_t **propval,
                         svn_wc__db_t *db,
                         const char *path,
                         const char *propname,
                         apr_pool_t *result_pool,
                         apr_pool_t *scratch_pool);


/* ### KFF: hash mapping 'const char *' prop names to svn_string_t vals?
 */
svn_error_t *
svn_wc__db_base_get_props(apr_hash_t **props,
                          svn_wc__db_t *db,
                          const char *path,
                          apr_pool_t *result_pool,
                          apr_pool_t *scratch_pool);


/* ### return some basic info for each child? e.g. kind
 *
 * ### KFF: perhaps you want an array of 'svn_dirent_t's?  Oh, but
 * ### they don't store the names... Well, but maybe you want
 * ### *children to be a hash anyway, not an array, so you can get the
 * ### name->child mapping and have children able to be looked up in
 * ### constant time.
 */
svn_error_t *
svn_wc__db_base_get_children(const apr_array_header_t **children,
                             svn_wc__db_t *db,
                             const char *path,
                             apr_pool_t *result_pool,
                             apr_pool_t *scratch_pool);


/* ### NULL allowed for OUT params */
svn_error_t *
svn_wc__db_base_get_contents(svn_stream_t **contents,
                             svn_checksum_t **checksum,
                             svn_wc__db_t *db,
                             const char *path,
                             apr_pool_t *result_pool,
                             apr_pool_t *scratch_pool);

/* ### KFF: Hm, yeah, see earlier about symlink questions. */
svn_error_t *
svn_wc__db_base_get_symlink_target(const char **target,
                                   svn_wc__db_t *db,
                                   const char *path,
                                   apr_pool_t *result_pool,
                                   apr_pool_t *scratch_pool);


/* ### how to handle depth? empty != absent. thus, record depth on each
   ### directory? empty, files, immediates, infinity. recording depth
   ### doesn't seem to be part of BASE, but instructions on how to maintain
   ### the BASE/WORKING/ACTUAL trees. are there other instructional items?
*/

/* ### anything else needed for maintaining the BASE tree? */


/** @} */

/**
 * @defgroup svn_wc__db_op  Operations on WORKING tree
 * @{
 */

/* ### svn cp WCPATH WCPATH ... can copy mixed base/working around */
svn_error_t *
svn_wc__db_op_copy(svn_wc__db_t *db,
                   const char *src_path,
                   const char *dst_path,
                   apr_pool_t *scratch_pool);


/* ### svn cp URL WCPATH ... copies pure repos into wc. only this "root"
   ### metadata is present. caller needs to "set" all information recursively.
   ### and caller definitely has to populate ACTUAL. */
/* ### mark node as absent? adding children or props: auto-convert away
   ### from absent? */
svn_error_t *
svn_wc__db_op_copy_url(svn_wc__db_t *db,
                       const char *path,
                       const char *copyfrom_url,
                       svn_revnum_t copyfrom_revision,
                       apr_pool_t *scratch_pool);


/* ### props may be NULL. children must be known before calling.
 *
 * ### KFF: Okay, if children can be NULL here, then it should be
 * ### able to be NULL up in svn_wc__db_base_add_directory().
 */
svn_error_t *
svn_wc__db_op_add_directory(svn_wc__db_t *db,
                            const char *path,
                            apr_hash_t *props,
                            const apr_array_header_t *children,
                            apr_pool_t *scratch_pool);


/* ### props may be NULL */
svn_error_t *
svn_wc__db_op_add_file(svn_wc__db_t *db,
                       const char *path,
                       apr_hash_t *props,
                       apr_pool_t *scratch_pool);


/* ### props may be NULL */
svn_error_t *
svn_wc__db_op_add_symlink(svn_wc__db_t *db,
                          const char *path,
                          apr_hash_t *props,
                          const char *target,
                          apr_pool_t *scratch_pool);


svn_error_t *
svn_wc__db_op_add_absent_node(svn_wc__db_t *db,
                              const char *path,
                              svn_wc__db_kind_t kind,
                              apr_pool_t *scratch_pool);


svn_error_t *
svn_wc__db_op_set_prop(svn_wc__db_t *db,
                       const char *path,
                       const char *propname,
                       const svn_string_t *propval,
                       apr_pool_t *scratch_pool);


svn_error_t *
svn_wc__db_op_set_props(svn_wc__db_t *db,
                        const char *path,
                        apr_hash_t *props,
                        apr_pool_t *scratch_pool);


/* ### KFF: This handles files, dirs, symlinks, anything else? */
svn_error_t *
svn_wc__db_op_delete(svn_wc__db_t *db,
                     const char *path,
                     apr_pool_t *scratch_pool);


/* ### KFF: Would like to know behavior when dst_path already exists
 * ### and is a) a dir or b) a non-dir. */
svn_error_t *
svn_wc__db_op_move(svn_wc__db_t *db,
                   const char *src_path,
                   const char *dst_path,
                   apr_pool_t *scratch_pool);


/* ### mark PATH as (possibly) modified. "svn edit" ... right API here? */
svn_error_t *
svn_wc__db_op_modified(svn_wc__db_t *db,
                       const char *path,
                       apr_pool_t *scratch_pool);


/* ### use NULL to remove from changelist ("add to the <null> changelist") */
svn_error_t *
svn_wc__db_op_add_to_changelist(svn_wc__db_t *db,
                                const char *path,
                                const char *changelist,
                                apr_pool_t *scratch_pool);


/* ### caller maintains ACTUAL. we're just recording state. */
svn_error_t *
svn_wc__db_op_mark_conflict(svn_wc__db_t *db,
                            const char *path,
                            apr_pool_t *scratch_pool);


/* ### caller maintains ACTUAL. we're just recording state. */
svn_error_t *
svn_wc__db_op_mark_resolved(svn_wc__db_t *db,
                            const char *path,
                            apr_pool_t *scratch_pool);


svn_error_t *
svn_wc__db_op_revert(svn_wc__db_t *db,
                     const char *path,
                     /* ### depth? */
                     apr_pool_t *scratch_pool);


/* ### status */


/** @} */

/**
 * @defgroup svn_wc__db_read  Read operations on the BASE/WORKING tree
 * @{
 */

/* ### NULL may be given for OUT params.

   ### if the node has not been committed (after adding):
   ###   url, repos_* will be NULL
   ###   revision will be SVN_INVALID_REVNUM
   ###   status will be svn_wc__db_status_added
   ###   text_mod will be TRUE
   ###   prop_mod will be TRUE if any props have been set
   ###   base_shadowed will be FALSE

   ### put all these OUT params into a structure? but this interface allows
   ### us to query for one or all pieces of information (harder with a struct)

   ### original_url will be NULL if this node is not copied/moved
   ### original_rev will be SVN_INVALID_REVNUM if this node is not copied/moved

   ### KFF: The position of 'db' in the parameter list is sort of
   ### floating around (e.g., compare this func with the next one).
   ### Would be nice to keep it consistent.  For example, it always
   ### comes first, or always comes first after any result params, or
   ### whatever.
*/
svn_error_t *
svn_wc__db_read_info(svn_wc__db_kind_t *kind,
                     svn_revnum_t *revision,
                     const char **url,
                     const char **repos_url,
                     const char **repos_uuid,
                     const char **changelist,
                     svn_wc__db_status_t *status,
                     svn_boolean_t *text_mod,  /* ### possibly modified */
                     svn_boolean_t *props_mod,
                     svn_boolean_t *base_shadowed,  /* ### WORKING shadows a
                                                       ### deleted BASE? */
                     const char **original_url,
                     svn_revnum_t *original_rev,
                     svn_wc__db_t *db,
                     const char *path,
                     apr_pool_t *result_pool,
                     apr_pool_t *scratch_pool);


svn_error_t *
svn_wc__db_read_prop(const svn_string_t **propval,
                     svn_wc__db_t *db,
                     const char *path,
                     const char *propname,
                     apr_pool_t *result_pool,
                     apr_pool_t *scratch_pool);


svn_error_t *
svn_wc__db_read_props(apr_hash_t **props,
                      svn_wc__db_t *db,
                      const char *path,
                      apr_pool_t *result_pool,
                      apr_pool_t *scratch_pool);


/* ### return some basic info for each child? e.g. kind.
 * ### maybe the data in _read_get_info should be a structure, and this
 * ### can return a struct for each one.
 * ### however: _read_get_info can say "not interested", which isn't the
 * ###   case with a struct. thus, a struct requires fetching and/or
 * ###   computing all info.
 * 
 * ### KFF: see earlier comment on svn_wc__db_base_get_children().
 */
svn_error_t *
svn_wc__db_read_children(const apr_array_header_t **children,
                         svn_wc__db_t *db,
                         const char *path,
                         apr_pool_t *result_pool,
                         apr_pool_t *scratch_pool);


svn_error_t *
svn_wc__db_read_symlink_target(const char **target,
                               svn_wc__db_t *db,
                               const char *path,
                               apr_pool_t *result_pool,
                               apr_pool_t *scratch_pool);


/* ### changelists. return an array, or an iterator interface? how big
   ### are these things? are we okay with an in-memory array? examine other
   ### changelist usage -- we may already assume the list fits in memory.
  */

/* ### bulk stuff like revision_status. */


/** @} */


/**
 * @defgroup svn_wc__db_global  Operations that alter BASE and WORKING trees
 * @{
 */

svn_error_t *
svn_wc__db_global_relocate(svn_wc__db_t *db,
                           const char *from_url,
                           const char *to_url,
                           svn_depth_t depth,
                           apr_pool_t *scratch_pool);


/* ### post-commit handling.
 * ### maybe multiple phases?
 * ### 1) mark a changelist as being-committed
 * ### 2) collect ACTUAL content, store for future use as TEXTBASE
 * ### 3) caller performs commit
 * ### 4) post-commit, integrate changelist into BASE
 */


/** @} */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SVN_WC_DB_H */
