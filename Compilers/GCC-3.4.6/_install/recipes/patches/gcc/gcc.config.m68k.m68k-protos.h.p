--- gcc-3.4.6/gcc/config/m68k/m68k-protos.h	2013-05-19 20:09:27.000000000 +0200
+++ gcc/config/m68k/m68k-protos.h	2013-05-19 20:23:32.000000000 +0200
@@ -68,3 +68,11 @@
 extern void override_options (void);
 extern void init_68881_table (void);
 extern int m68k_hard_regno_rename_ok(unsigned int, unsigned int);
+
+#ifdef RTX_CODE
+#ifdef TREE_CODE
+extern void m68k_init_cumulative_args (CUMULATIVE_ARGS *, tree);
+extern void m68k_function_arg_advance (CUMULATIVE_ARGS *);
+extern struct rtx_def *m68k_function_arg (CUMULATIVE_ARGS *, enum machine_mode, tree);
+#endif
+#endif
