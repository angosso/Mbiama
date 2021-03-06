/*
 * add-cmd.c -- Subversion add/unadd commands
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

/* ==================================================================== */



/*** Includes. ***/

#include "svn_wc.h"
#include "svn_client.h"
#include "svn_string.h"
#include "svn_path.h"
#include "svn_delta.h"
#include "svn_error.h"
#include "svn_pools.h"
#include "cl.h"



/*** Code. ***/

svn_error_t *
svn_cl__add (apr_getopt_t *os,
             svn_cl__opt_state_t *opt_state,
             apr_pool_t *pool)
{
  svn_error_t *err;
  apr_array_header_t *targets;
  int i;
  svn_boolean_t recursive = opt_state->recursive;

  targets = svn_cl__args_to_target_array (os, opt_state, FALSE, pool);

  if (targets->nelts)
    {
      apr_pool_t *subpool = svn_pool_create (pool);

      for (i = 0; i < targets->nelts; i++)
        {
          svn_stringbuf_t *target = ((svn_stringbuf_t **) (targets->elts))[i];
          err = svn_client_add (target, recursive,
                                SVN_CL_NOTIFY(opt_state),
                                svn_cl__make_notify_baton (subpool),
                                subpool);
          if (err)
            {
              if (err->apr_err == SVN_ERR_ENTRY_EXISTS)
                {
                  svn_handle_warning (err, err->message);
                  svn_error_clear_all (err);
                }
              else
                return err;
            }

          svn_pool_clear (subpool);
        }

      svn_pool_destroy (subpool);
    }
  else
    {
      svn_cl__subcommand_help ("add", pool);
      return svn_error_create (SVN_ERR_CL_ARG_PARSING_ERROR, 0, 0, pool, "");
    }

  return SVN_NO_ERROR;
}



/* 
 * local variables:
 * eval: (load-file "../../../tools/dev/svn-dev.el")
 * end: 
 */
