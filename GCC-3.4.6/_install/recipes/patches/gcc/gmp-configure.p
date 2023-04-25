--- gmp/configure.orig	2012-08-17 18:03:57.000000000 +0100
+++ gmp/configure	2012-08-17 18:04:17.000000000 +0100
@@ -30006,8 +30006,6 @@
 echo "define(<M4WRAP_SPURIOUS>,<$gmp_cv_m4_m4wrap_spurious>)" >> $gmp_tmpconfigm4
 
 
-else
-  M4=m4-not-needed
 fi
 
 # Only do the GMP_ASM checks if there's a .S or .asm wanting them.
