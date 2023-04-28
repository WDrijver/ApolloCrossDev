*** collect2.c.old	2023-04-25 17:15:14.150891846 +0200
--- collect2.c.new	2023-04-25 16:33:11.366427373 +0200
***************
*** 144,149 ****
--- 144,155 ----
  #define SCAN_LIBRARIES
  #endif
  
+ /* begin-GG-local: dynamic libraries */
+ #ifndef DO_COLLECTING
+ #define DO_COLLECTING do_collecting
+ #endif
+ /* end-GG-local */
+ 
  #ifdef USE_COLLECT2
  int do_collecting = 1;
  #else
***************
*** 256,263 ****
  static void prefix_from_env (const char *, struct path_prefix *);
  static void prefix_from_string (const char *, struct path_prefix *);
  static void do_wait (const char *);
! static void fork_execute (const char *, char **);
! static void maybe_unlink (const char *);
  static void add_to_list (struct head *, const char *);
  static int extract_init_priority (const char *);
  static void sort_ids (struct head *);
--- 262,271 ----
  static void prefix_from_env (const char *, struct path_prefix *);
  static void prefix_from_string (const char *, struct path_prefix *);
  static void do_wait (const char *);
! /* begin-GG-local: dynamic libraries */
! void fork_execute (const char *, char **);
! void maybe_unlink (const char *);
! /* end-GG-local */
  static void add_to_list (struct head *, const char *);
  static int extract_init_priority (const char *);
  static void sort_ids (struct head *);
***************
*** 335,340 ****
--- 343,354 ----
    if (status != 0 && output_file != 0 && output_file[0])
      maybe_unlink (output_file);
  
+ /* begin-GG-local: dynamic libraries */
+ #ifdef COLLECT2_EXTRA_CLEANUP
+   COLLECT2_EXTRA_CLEANUP();
+ #endif
+ /* end-GG-local */
+ 
    exit (status);
  }
  
***************
*** 423,428 ****
--- 437,448 ----
      maybe_unlink (export_file);
  #endif
  
+ /* begin-GG-local: dynamic libraries */
+ #ifdef COLLECT2_EXTRA_CLEANUP
+   COLLECT2_EXTRA_CLEANUP();
+ #endif
+ /* end-GG-local */
+ 
    signal (signo, SIG_DFL);
    kill (getpid (), signo);
  }
***************
*** 609,619 ****
  
    /* Determine the filename to execute (special case for absolute paths).  */
  
!   if (*name == '/'
! #ifdef HAVE_DOS_BASED_FILE_SYSTEM
!       || (*name && name[1] == ':')
! #endif
!       )
      {
        if (access (name, X_OK) == 0)
  	{
--- 629,635 ----
  
    /* Determine the filename to execute (special case for absolute paths).  */
  
!   if (IS_ABSOLUTE_PATH (name))
      {
        if (access (name, X_OK) == 0)
  	{
***************
*** 881,886 ****
--- 897,907 ----
        const char *q = extract_string (&p);
        if (*q == '-' && (q[1] == 'm' || q[1] == 'f'))
  	num_c_args++;
+ /* begin-GG-local: dynamic libraries */
+ #ifdef COLLECT2_GCC_OPTIONS_HOOK
+       COLLECT2_GCC_OPTIONS_HOOK(q);
+ #endif
+ /* end-GG-local */
      }
    obstack_free (&temporary_obstack, temporary_firstobj);
  
***************
*** 1112,1117 ****
--- 1133,1143 ----
  		add_to_list (&libs, s);
  	      }
  #endif
+ /* begin-GG-local: dynamic libraries */
+ #ifdef COLLECT2_LIBNAME_HOOK
+ 	      COLLECT2_LIBNAME_HOOK(arg);
+ #endif
+ /* end-GG-local */
  	      break;
  
  #ifdef COLLECT_EXPORT_LIST
***************
*** 1146,1152 ****
  	      break;
  
  	    case 's':
! 	      if (arg[2] == '\0' && do_collecting)
  		{
  		  /* We must strip after the nm run, otherwise C++ linking
  		     will not work.  Thus we strip in the second ld run, or
--- 1172,1180 ----
  	      break;
  
  	    case 's':
! /* begin-GG-local: dynamic libraries */
! 	      if (arg[2] == '\0' && DO_COLLECTING)
! /* end-GG-local */
  		{
  		  /* We must strip after the nm run, otherwise C++ linking
  		     will not work.  Thus we strip in the second ld run, or
***************
*** 1190,1195 ****
--- 1218,1228 ----
                add_to_list (&libs, arg);
              }
  #endif
+ /* begin-GG-local: dynamic libraries */
+ #ifdef COLLECT2_LIBNAME_HOOK
+ 	  COLLECT2_LIBNAME_HOOK(arg);
+ #endif
+ /* end-GG-local */
  	}
      }
  
***************
*** 1283,1288 ****
--- 1316,1327 ----
        fprintf (stderr, "\n");
      }
  
+ /* begin-GG-local: dynamic libraries */
+ #ifdef COLLECT2_PRELINK_HOOK
+   COLLECT2_PRELINK_HOOK(ld1_argv, &strip_flag);
+ #endif
+ /* end-GG-local */
+ 
    /* Load the program, searching all libraries and attempting to provide
       undefined symbols from repository information.  */
  
***************
*** 1295,1301 ****
       constructor or destructor list, just return now.  */
    if (rflag
  #ifndef COLLECT_EXPORT_LIST
!       || ! do_collecting
  #endif
        )
      {
--- 1334,1342 ----
       constructor or destructor list, just return now.  */
    if (rflag
  #ifndef COLLECT_EXPORT_LIST
! /* begin-GG-local: dynamic libraries */
!       || ! DO_COLLECTING
! /* end-GG-local */
  #endif
        )
      {
***************
*** 1312,1317 ****
--- 1353,1360 ----
        return 0;
      }
  
+ /* begin-GG-local: dynamic libraries */
+ #ifndef COLLECT2_POSTLINK_HOOK
    /* Examine the namelist with nm and search it for static constructors
       and destructors to call.
       Write the constructor and destructor tables to a .s file and reload.  */
***************
*** 1331,1336 ****
--- 1374,1383 ----
        notice ("%d destructor(s)  found\n", destructors.number);
        notice ("%d frame table(s) found\n", frame_tables.number);
      }
+ #else /* COLLECT2_POSTLINK_HOOK */
+   COLLECT2_POSTLINK_HOOK(output_file);
+ #endif
+ /* end-GG-local */
  
    if (constructors.number == 0 && destructors.number == 0
        && frame_tables.number == 0
***************
*** 1363,1368 ****
--- 1410,1420 ----
  #endif
        maybe_unlink (c_file);
        maybe_unlink (o_file);
+ /* begin-GG-local: dynamic libraries */
+ #ifdef COLLECT2_EXTRA_CLEANUP
+       COLLECT2_EXTRA_CLEANUP();
+ #endif
+ /* end-GG-local */
        return 0;
      }
  
***************
*** 1454,1459 ****
--- 1506,1516 ----
    maybe_unlink (export_file);
  #endif
  
+ /* begin-GG-local: dynamic libraries */
+ #ifdef COLLECT2_EXTRA_CLEANUP
+   COLLECT2_EXTRA_CLEANUP();
+ #endif
+ /* end-GG-local */
    return 0;
  }
  
***************
*** 1534,1540 ****
    if (redir)
      {
        /* Open response file.  */
!       redir_handle = open (redir, O_WRONLY | O_TRUNC | O_CREAT);
  
        /* Duplicate the stdout and stderr file handles
  	 so they can be restored later.  */
--- 1591,1597 ----
    if (redir)
      {
        /* Open response file.  */
!       redir_handle = open (redir, O_WRONLY | O_TRUNC | O_CREAT, 0600);
  
        /* Duplicate the stdout and stderr file handles
  	 so they can be restored later.  */
***************
*** 1567,1573 ****
     fatal_perror (errmsg_fmt, errmsg_arg);
  }
  
! static void
  fork_execute (const char *prog, char **argv)
  {
    collect_execute (prog, argv, NULL);
--- 1624,1630 ----
     fatal_perror (errmsg_fmt, errmsg_arg);
  }
  
! void
  fork_execute (const char *prog, char **argv)
  {
    collect_execute (prog, argv, NULL);
***************
*** 1576,1582 ****
  
  /* Unlink a file unless we are debugging.  */
  
! static void
  maybe_unlink (const char *file)
  {
    if (!debug)
--- 1633,1639 ----
  
  /* Unlink a file unless we are debugging.  */
  
! void
  maybe_unlink (const char *file)
  {
    if (!debug)
