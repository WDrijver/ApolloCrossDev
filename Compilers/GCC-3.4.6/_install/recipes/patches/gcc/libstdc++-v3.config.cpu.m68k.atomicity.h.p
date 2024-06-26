--- gcc-3.4.6/libstdc++-v3/config/cpu/m68k/atomicity.h	2004-03-18 18:36:28.000000000 +0100
+++ libstdc++-v3/config/cpu/m68k/atomicity.h	2013-05-19 20:23:32.000000000 +0200
@@ -31,7 +31,21 @@
 
 namespace __gnu_cxx
 {
-#if ( defined(__mc68020__) || defined(__mc68030__) \
+#if defined(__amigaos__)
+
+  _Atomic_word
+  __attribute__ ((__unused__))
+  __exchange_and_add (volatile _Atomic_word *__mem, int __val)
+  {
+    _Atomic_word __result;
+
+    __result = *__mem;
+    *__mem = __result + __val;
+
+    return __result;
+  }
+
+#elif ( defined(__mc68020__) || defined(__mc68030__) \
       || defined(__mc68040__) || defined(__mc68060__) ) \
     && !defined(__mcpu32__)
   // These variants support compare-and-swap.
