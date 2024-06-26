--- gcc-3.4.6/gcc/function.h	2013-05-19 19:56:54.000000000 +0200
+++ gcc/function.h	2013-05-19 20:23:32.000000000 +0200
@@ -643,4 +643,8 @@
 
 extern void do_warn_unused_parameter (tree);
 
+/* begin-GG-local: explicit register specification for parameters */
+extern int function_arg_regno_p (int);
+/* end-GG-local */
+
 #endif  /* GCC_FUNCTION_H */
