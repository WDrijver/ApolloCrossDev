--- sys-include/inline/intuition.h	2006-05-10 18:29:23.000000000 +0100
+++ sys-include/inline/intuition.h	2016-02-29 00:06:48.824015807 +0000
@@ -448,7 +448,7 @@
 	LP3(0x27c, APTR, NewObjectA, struct IClass *, classPtr, a0, CONST_STRPTR, classID, a1, const struct TagItem *, tagList, a2, \
 	, INTUITION_BASE_NAME)
 
-#ifndef NO_INLINE_STDARG
+#if 0
 __inline APTR NewObject(struct IClass * classPtr, CONST_STRPTR classID, ULONG tagList, ...)
 {
   return NewObjectA(classPtr, classID, (const struct TagItem *) &tagList);
