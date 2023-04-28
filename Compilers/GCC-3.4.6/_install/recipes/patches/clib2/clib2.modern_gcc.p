Index: amiga_addtof.c
===================================================================
RCS file: /cvsroot/clib2/library/amiga_addtof.c,v
retrieving revision 1.4
diff -u -r1.4 amiga_addtof.c
--- amiga_addtof.c	8 Jan 2006 12:04:22 -0000	1.4
+++ amiga_addtof.c	24 Dec 2010 00:45:23 -0000
@@ -58,8 +58,10 @@
 /****************************************************************************/
 
 STATIC LONG INTERRUPT ASM
-call_routine(REG(a1,struct Isrvstr *i))
+call_routine()
 {
+	register struct Isrvstr *i __asm("a1");
+
 	CFUNC p = (CFUNC)i->ccode;
 
 	(*p)(i->Carg);
Index: amiga_beginio.c
===================================================================
RCS file: /cvsroot/clib2/library/amiga_beginio.c,v
retrieving revision 1.5
diff -u -r1.5 amiga_beginio.c
--- amiga_beginio.c	25 Sep 2006 15:12:47 -0000	1.5
+++ amiga_beginio.c	24 Dec 2010 00:45:23 -0000
@@ -64,7 +64,7 @@
   __asm volatile ("jsr a6@(-30:W)" \
   : \
   : "r"(__BeginIO__bn), "r"(__BeginIO_ior)  \
-  : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d0", "d1", "a0", "fp0", "fp1", "cc", "memory"); \
   } \
 })
 
Index: amiga_callhooka.c
===================================================================
RCS file: /cvsroot/clib2/library/amiga_callhooka.c,v
retrieving revision 1.3
diff -u -r1.3 amiga_callhooka.c
--- amiga_callhooka.c	8 Jan 2006 12:04:22 -0000	1.3
+++ amiga_callhooka.c	24 Dec 2010 00:45:23 -0000
@@ -35,58 +35,57 @@
 
 /****************************************************************************/
 
-asm("
-
-h_Entry = 8
-
-	.text
-	.even
-
-|---------------------------------------------------------------------------
-| new hook standard
-| use struct Hook (with minnode at the top)
-|
-| *** register calling convention: ***
-|	A0 - pointer to hook itself
-|	A1 - pointer to parameter packed ('message')
-|	A2 - Hook specific address data ('object,' e.g, gadget )
-|
-| ***  C conventions: ***
-| Note that parameters are in unusual register order: a0, a2, a1.
-| This is to provide a performance boost for assembly language
-| programming (the object in a2 is most frequently untouched).
-| It is also no problem in 'register direct' C function parameters.
-|
-| calling through a hook
-|	CallHook( hook, object, msgid, p1, p2, ... );
-|	CallHookA( hook, object, msgpkt );
-|
-| using a C function:	CFunction( hook, object, message );
-|	hook.h_Entry = HookEntry;
-|	hook.h_SubEntry = CFunction;
-|
-|---------------------------------------------------------------------------
-
-| C calling hook interface for prepared message packet
-
-	.globl	_CallHookA
-
-_CallHookA:
-
-	moveml	a2/a6,sp@-
-	moveal	sp@(12),a0
-	moveal	sp@(16),a2
-	moveal	sp@(20),a1
-	pea		callhooka_return
-	movel	a0@(h_Entry),sp@-
-	rts
-
-callhooka_return:
-
-	moveml	sp@+,a2/a6
-	rts
-
-");
+asm("\n"
+"\n"
+"h_Entry = 8\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"|---------------------------------------------------------------------------\n"
+"| new hook standard\n"
+"| use struct Hook (with minnode at the top)\n"
+"|\n"
+"| *** register calling convention: ***\n"
+"|	A0 - pointer to hook itself\n"
+"|	A1 - pointer to parameter packed ('message')\n"
+"|	A2 - Hook specific address data ('object,' e.g, gadget )\n"
+"|\n"
+"| ***  C conventions: ***\n"
+"| Note that parameters are in unusual register order: a0, a2, a1.\n"
+"| This is to provide a performance boost for assembly language\n"
+"| programming (the object in a2 is most frequently untouched).\n"
+"| It is also no problem in 'register direct' C function parameters.\n"
+"|\n"
+"| calling through a hook\n"
+"|	CallHook( hook, object, msgid, p1, p2, ... );\n"
+"|	CallHookA( hook, object, msgpkt );\n"
+"|\n"
+"| using a C function:	CFunction( hook, object, message );\n"
+"|	hook.h_Entry = HookEntry;\n"
+"|	hook.h_SubEntry = CFunction;\n"
+"|\n"
+"|---------------------------------------------------------------------------\n"
+"\n"
+"| C calling hook interface for prepared message packet\n"
+"\n"
+"	.globl	_CallHookA\n"
+"\n"
+"_CallHookA:\n"
+"\n"
+"	moveml	a2/a6,sp@-\n"
+"	moveal	sp@(12),a0\n"
+"	moveal	sp@(16),a2\n"
+"	moveal	sp@(20),a1\n"
+"	pea		callhooka_return\n"
+"	movel	a0@(h_Entry),sp@-\n"
+"	rts\n"
+"\n"
+"callhooka_return:\n"
+"\n"
+"	moveml	sp@+,a2/a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: amiga_hookentry.c
===================================================================
RCS file: /cvsroot/clib2/library/amiga_hookentry.c,v
retrieving revision 1.3
diff -u -r1.3 amiga_hookentry.c
--- amiga_hookentry.c	8 Jan 2006 12:04:22 -0000	1.3
+++ amiga_hookentry.c	24 Dec 2010 00:45:23 -0000
@@ -35,26 +35,25 @@
 
 /****************************************************************************/
 
-asm("
-
-h_SubEntry = 12
-
-	.text
-	.even
-
-	.globl	_HookEntry
-
-_HookEntry:
-
-	movel	a1,sp@-
-	movel	a2,sp@-
-	movel	a0,sp@-
-	movel	a0@(h_SubEntry:W),a0
-	jsr		a0@
-	lea		sp@(12:W),sp
-	rts
-
-");
+asm("\n"
+"\n"
+"h_SubEntry = 12\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_HookEntry\n"
+"\n"
+"_HookEntry:\n"
+"\n"
+"	movel	a1,sp@-\n"
+"	movel	a2,sp@-\n"
+"	movel	a0,sp@-\n"
+"	movel	a0@(h_SubEntry:W),a0\n"
+"	jsr		a0@\n"
+"	lea		sp@(12:W),sp\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: amiga_rexxvars.c
===================================================================
RCS file: /cvsroot/clib2/library/amiga_rexxvars.c,v
retrieving revision 1.19
diff -u -r1.19 amiga_rexxvars.c
--- amiga_rexxvars.c	18 Apr 2008 10:11:59 -0000	1.19
+++ amiga_rexxvars.c	24 Dec 2010 00:45:23 -0000
@@ -311,209 +311,201 @@
 /****************************************************************************/
 
 /* struct Environment * a0,APTR block a1,LONG d0 */
-asm("
-
-	.text
-	.even
-
-	.globl	__FreeSpace
-
-__FreeSpace:
-
-	moveal	sp@(4),a0
-	moveal	sp@(8),a1
-	movel	sp@(12),d0
-
-	movel	a6,sp@-
-	moveal	"A4(_RexxSysBase)",a6
-	jsr		a6@(-120)
-	moveal	sp@+,a6
-
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	__FreeSpace\n"
+"\n"
+"__FreeSpace:\n"
+"\n"
+"	moveal	sp@(4),a0\n"
+"	moveal	sp@(8),a1\n"
+"	movel	sp@(12),d0\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	moveal	"A4(_RexxSysBase)",a6\n"
+"	jsr		a6@(-120)\n"
+"	moveal	sp@+,a6\n"
+"\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
 /* struct Environment * a0,LONG d0 : APTR d0 */
-asm("
-
-	.text
-	.even
-
-	.globl	__GetSpace
-
-__GetSpace:
-
-	moveal	sp@(4),a0
-	movel	sp@(8),d0
-
-	movel	a6,sp@-
-	moveal	"A4(_RexxSysBase)",a6
-	jsr		a6@(-114)
-	moveal	sp@+,a6
-
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	__GetSpace\n"
+"\n"
+"__GetSpace:\n"
+"\n"
+"	moveal	sp@(4),a0\n"
+"	movel	sp@(8),d0\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	moveal	"A4(_RexxSysBase)",a6\n"
+"	jsr		a6@(-114)\n"
+"	moveal	sp@+,a6\n"
+"\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
 /* STRPTR a0 : LONG d0, LONG d1 */
-asm("
-
-	.text
-	.even
-
-	.globl	__IsSymbol
-
-__IsSymbol:
-
-	moveal	sp@(4),a0
-
-	movel	a6,sp@-
-	moveal	"A4(_RexxSysBase)",a6
-	jsr		a6@(-102)
-	moveal	sp@+,a6
-
-	moveal	sp@(8),a1
-	movel	d1,a1@
-
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	__IsSymbol\n"
+"\n"
+"__IsSymbol:\n"
+"\n"
+"	moveal	sp@(4),a0\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	moveal	"A4(_RexxSysBase)",a6\n"
+"	jsr		a6@(-102)\n"
+"	moveal	sp@+,a6\n"
+"\n"
+"	moveal	sp@(8),a1\n"
+"	movel	d1,a1@\n"
+"\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
 /* struct RexxTask * a0 : struct Environment * a0 */
-asm("
-
-	.text
-	.even
-
-	.globl	__CurrentEnv
-
-__CurrentEnv:
-
-	moveal	sp@(4),a0
-
-	movel	a6,sp@-
-	moveal	"A4(_RexxSysBase)",a6
-	jsr		a6@(-108)
-	moveal	sp@+,a6
-
-	moveal	sp@(8),a1
-	movel	a0,a1@
-
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	__CurrentEnv\n"
+"\n"
+"__CurrentEnv:\n"
+"\n"
+"	moveal	sp@(4),a0\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	moveal	"A4(_RexxSysBase)",a6\n"
+"	jsr		a6@(-108)\n"
+"	moveal	sp@+,a6\n"
+"\n"
+"	moveal	sp@(8),a1\n"
+"	movel	a0,a1@\n"
+"\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
 /* struct Environment * a0,struct NexxStr * a1,struct NexxStr * d0,struct Node * d1 : struct NexxStr * a0, LONG d1 */
-asm("
-
-	.text
-	.even
-
-	.globl	__FetchValue
-
-__FetchValue:
-
-	moveal	sp@(4),a0
-	moveal	sp@(8),a1
-	movel	sp@(12),d0
-	movel	sp@(16),d1
-
-	movel	a6,sp@-
-	moveal	"A4(_RexxSysBase)",a6
-	jsr		a6@(-72)
-	moveal	sp@+,a6
-
-	moveal	sp@(20),a1
-	movel	a0,a1@
-	moveal	sp@(24),a1
-	movel	d1,a1@
-
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	__FetchValue\n"
+"\n"
+"__FetchValue:\n"
+"\n"
+"	moveal	sp@(4),a0\n"
+"	moveal	sp@(8),a1\n"
+"	movel	sp@(12),d0\n"
+"	movel	sp@(16),d1\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	moveal	"A4(_RexxSysBase)",a6\n"
+"	jsr		a6@(-72)\n"
+"	moveal	sp@+,a6\n"
+"\n"
+"	moveal	sp@(20),a1\n"
+"	movel	a0,a1@\n"
+"	moveal	sp@(24),a1\n"
+"	movel	d1,a1@\n"
+"\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
 /* struct Environment a0, struct NexxStr *a1, struct NexxStr * d0 : struct Node * d0 */
-asm("
-
-	.text
-	.even
-
-	.globl	__EnterSymbol
-
-__EnterSymbol:
-
-	moveal	sp@(4),a0
-	moveal	sp@(8),a1
-	movel	sp@(12),d0
-
-	movel	a6,sp@-
-	moveal	"A4(_RexxSysBase)",a6
-	jsr		a6@(-66)
-	moveal	sp@+,a6
-
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	__EnterSymbol\n"
+"\n"
+"__EnterSymbol:\n"
+"\n"
+"	moveal	sp@(4),a0\n"
+"	moveal	sp@(8),a1\n"
+"	movel	sp@(12),d0\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	moveal	"A4(_RexxSysBase)",a6\n"
+"	jsr		a6@(-66)\n"
+"	moveal	sp@+,a6\n"
+"\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
 /* struct Environment *a0, struct NexxStr *a1, struct Node * d0 */
-asm("
-
-	.text
-	.even
-
-	.globl	__SetValue
-
-__SetValue:
-
-	moveal	sp@(4),a0
-	moveal	sp@(8),a1
-	movel	sp@(12),d0
-
-	movel	a6,sp@-
-	moveal	"A4(_RexxSysBase)",a6
-	jsr		a6@(-84)
-	moveal	sp@+,a6
-
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	__SetValue\n"
+"\n"
+"__SetValue:\n"
+"\n"
+"	moveal	sp@(4),a0\n"
+"	moveal	sp@(8),a1\n"
+"	movel	sp@(12),d0\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	moveal	"A4(_RexxSysBase)",a6\n"
+"	jsr		a6@(-84)\n"
+"	moveal	sp@+,a6\n"
+"\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
 /* STRPTR a0,STRPTR a1,LONG d0 : ULONG d0 */
-asm("
-
-	.text
-	.even
-
-	.globl	__StrcpyN
-
-__StrcpyN:
-
-	moveal	sp@(4),a0
-	moveal	sp@(8),a1
-	movel	sp@(12),d0
-
-	movel	a6,sp@-
-	moveal	"A4(_RexxSysBase)",a6
-	jsr		a6@(-270)
-	moveal	sp@+,a6
-
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	__StrcpyN\n"
+"\n"
+"__StrcpyN:\n"
+"\n"
+"	moveal	sp@(4),a0\n"
+"	moveal	sp@(8),a1\n"
+"	movel	sp@(12),d0\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	moveal	"A4(_RexxSysBase)",a6\n"
+"	jsr		a6@(-270)\n"
+"	moveal	sp@+,a6\n"
+"\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: debug_headers.h
===================================================================
RCS file: /cvsroot/clib2/library/debug_headers.h,v
retrieving revision 1.4
diff -u -r1.4 debug_headers.h
--- debug_headers.h	8 Jan 2006 12:04:22 -0000	1.4
+++ debug_headers.h	24 Dec 2010 00:45:23 -0000
@@ -96,7 +96,7 @@
   __asm volatile ("jsr a6@(-516:W)" \
   : \
   : "r"(__RawPutChar__bn), "r"(__RawPutChar_c) \
-  : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
   } \
 })
 
Index: debug_kputfmt.c
===================================================================
RCS file: /cvsroot/clib2/library/debug_kputfmt.c,v
retrieving revision 1.4
diff -u -r1.4 debug_kputfmt.c
--- debug_kputfmt.c	8 Jan 2006 12:04:22 -0000	1.4
+++ debug_kputfmt.c	24 Dec 2010 00:45:23 -0000
@@ -151,8 +151,10 @@
 /****************************************************************************/
 
 STATIC VOID ASM
-raw_put_char(REG(d0,UBYTE c))
+raw_put_char()
 {
+	register UBYTE c __asm("d0");
+
 	kputc(c); 
 }
 
Index: math_acos.c
===================================================================
RCS file: /cvsroot/clib2/library/math_acos.c,v
retrieving revision 1.7
diff -u -r1.7 math_acos.c
--- math_acos.c	8 Jan 2006 12:04:23 -0000	1.7
+++ math_acos.c	24 Dec 2010 00:45:23 -0000
@@ -72,24 +72,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubTransBase
-	.globl	___acos
-
-___acos:
-
-	movel	a6,sp@-
-	movel	"A4(_MathIeeeDoubTransBase)",a6
-	moveml	sp@(8),d0/d1
-	jsr		a6@(-120:W)
-	movel	sp@+,a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubTransBase\n"
+"	.globl	___acos\n"
+"\n"
+"___acos:\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubTransBase)",a6\n"
+"	moveml	sp@(8),d0/d1\n"
+"	jsr		a6@(-120:W)\n"
+"	movel	sp@+,a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_adddf3.c
===================================================================
RCS file: /cvsroot/clib2/library/math_adddf3.c,v
retrieving revision 1.3
diff -u -r1.3 math_adddf3.c
--- math_adddf3.c	8 Jan 2006 12:04:23 -0000	1.3
+++ math_adddf3.c	24 Dec 2010 00:45:23 -0000
@@ -51,24 +51,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubBasBase
-	.globl	___adddf3
-
-___adddf3:
-
-	moveml	d2/d3/a6,sp@-
-	movel	"A4(_MathIeeeDoubBasBase)",a6
-	moveml	sp@(16),d0/d1/d2/d3
-	jsr		a6@(-66:W)
-	moveml	sp@+,d2/d3/a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubBasBase\n"
+"	.globl	___adddf3\n"
+"\n"
+"___adddf3:\n"
+"\n"
+"	moveml	d2/d3/a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubBasBase)",a6\n"
+"	moveml	sp@(16),d0/d1/d2/d3\n"
+"	jsr		a6@(-66:W)\n"
+"	moveml	sp@+,d2/d3/a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_asin.c
===================================================================
RCS file: /cvsroot/clib2/library/math_asin.c,v
retrieving revision 1.7
diff -u -r1.7 math_asin.c
--- math_asin.c	8 Jan 2006 12:04:23 -0000	1.7
+++ math_asin.c	24 Dec 2010 00:45:23 -0000
@@ -72,24 +72,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubTransBase
-	.globl	___asin
-
-___asin:
-
-	movel	a6,sp@-
-	movel	"A4(_MathIeeeDoubTransBase)",a6
-	moveml	sp@(8),d0/d1
-	jsr		a6@(-114:W)
-	movel	sp@+,a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubTransBase\n"
+"	.globl	___asin\n"
+"\n"
+"___asin:\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubTransBase)",a6\n"
+"	moveml	sp@(8),d0/d1\n"
+"	jsr		a6@(-114:W)\n"
+"	movel	sp@+,a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_atan.c
===================================================================
RCS file: /cvsroot/clib2/library/math_atan.c,v
retrieving revision 1.5
diff -u -r1.5 math_atan.c
--- math_atan.c	8 Jan 2006 12:04:23 -0000	1.5
+++ math_atan.c	24 Dec 2010 00:45:23 -0000
@@ -72,24 +72,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubTransBase
-	.globl	___atan
-
-___atan:
-
-	movel	a6,sp@-
-	movel	"A4(_MathIeeeDoubTransBase)",a6
-	moveml	sp@(8),d0/d1
-	jsr		a6@(-30:W)
-	movel	sp@+,a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubTransBase\n"
+"	.globl	___atan\n"
+"\n"
+"___atan:\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubTransBase)",a6\n"
+"	moveml	sp@(8),d0/d1\n"
+"	jsr		a6@(-30:W)\n"
+"	movel	sp@+,a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_ceil.c
===================================================================
RCS file: /cvsroot/clib2/library/math_ceil.c,v
retrieving revision 1.6
diff -u -r1.6 math_ceil.c
--- math_ceil.c	8 Jan 2006 12:04:23 -0000	1.6
+++ math_ceil.c	24 Dec 2010 00:45:23 -0000
@@ -72,24 +72,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubBasBase
-	.globl	___ceil
-
-___ceil:
-
-	movel	a6,sp@-
-	movel	"A4(_MathIeeeDoubBasBase)",a6
-	moveml	sp@(8),d0/d1
-	jsr		a6@(-96:W)
-	movel	sp@+,a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubBasBase\n"
+"	.globl	___ceil\n"
+"\n"
+"___ceil:\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubBasBase)",a6\n"
+"	moveml	sp@(8),d0/d1\n"
+"	jsr		a6@(-96:W)\n"
+"	movel	sp@+,a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_cos.c
===================================================================
RCS file: /cvsroot/clib2/library/math_cos.c,v
retrieving revision 1.6
diff -u -r1.6 math_cos.c
--- math_cos.c	8 Jan 2006 12:04:23 -0000	1.6
+++ math_cos.c	24 Dec 2010 00:45:23 -0000
@@ -72,24 +72,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubTransBase
-	.globl	___cos
-
-___cos:
-
-	movel	a6,sp@-
-	movel	"A4(_MathIeeeDoubTransBase)",a6
-	moveml	sp@(8),d0/d1
-	jsr		a6@(-42:W)
-	movel	sp@+,a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubTransBase\n"
+"	.globl	___cos\n"
+"\n"
+"___cos:\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubTransBase)",a6\n"
+"	moveml	sp@(8),d0/d1\n"
+"	jsr		a6@(-42:W)\n"
+"	movel	sp@+,a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_cosh.c
===================================================================
RCS file: /cvsroot/clib2/library/math_cosh.c,v
retrieving revision 1.5
diff -u -r1.5 math_cosh.c
--- math_cosh.c	8 Jan 2006 12:04:23 -0000	1.5
+++ math_cosh.c	24 Dec 2010 00:45:23 -0000
@@ -72,24 +72,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubTransBase
-	.globl	___cosh
-
-___cosh:
-
-	movel	a6,sp@-
-	movel	"A4(_MathIeeeDoubTransBase)",a6
-	moveml	sp@(8),d0/d1
-	jsr		a6@(-66:W)
-	movel	sp@+,a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubTransBase\n"
+"	.globl	___cosh\n"
+"\n"
+"___cosh:\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubTransBase)",a6\n"
+"	moveml	sp@(8),d0/d1\n"
+"	jsr		a6@(-66:W)\n"
+"	movel	sp@+,a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_divdf3.c
===================================================================
RCS file: /cvsroot/clib2/library/math_divdf3.c,v
retrieving revision 1.3
diff -u -r1.3 math_divdf3.c
--- math_divdf3.c	8 Jan 2006 12:04:23 -0000	1.3
+++ math_divdf3.c	24 Dec 2010 00:45:23 -0000
@@ -51,24 +51,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubBasBase
-	.globl	___divdf3
-
-___divdf3:
-
-	moveml	d2/d3/a6,sp@-
-	movel	"A4(_MathIeeeDoubBasBase)",a6
-	moveml	sp@(16),d0/d1/d2/d3
-	jsr		a6@(-84:W)
-	moveml	sp@+,d2/d3/a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubBasBase\n"
+"	.globl	___divdf3\n"
+"\n"
+"___divdf3:\n"
+"\n"
+"	moveml	d2/d3/a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubBasBase)",a6\n"
+"	moveml	sp@(16),d0/d1/d2/d3\n"
+"	jsr		a6@(-84:W)\n"
+"	moveml	sp@+,d2/d3/a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_eqdf2.c
===================================================================
RCS file: /cvsroot/clib2/library/math_eqdf2.c,v
retrieving revision 1.3
diff -u -r1.3 math_eqdf2.c
--- math_eqdf2.c	8 Jan 2006 12:04:23 -0000	1.3
+++ math_eqdf2.c	24 Dec 2010 00:45:23 -0000
@@ -51,24 +51,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubBasBase
-	.globl	___eqdf2
-
-___eqdf2:
-
-	moveml	d2/d3/a6,sp@-
-	movel	"A4(_MathIeeeDoubBasBase)",a6
-	moveml	sp@(16),d0/d1/d2/d3
-	jsr		a6@(-42:W)
-	moveml	sp@+,d2/d3/a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubBasBase\n"
+"	.globl	___eqdf2\n"
+"\n"
+"___eqdf2:\n"
+"\n"
+"	moveml	d2/d3/a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubBasBase)",a6\n"
+"	moveml	sp@(16),d0/d1/d2/d3\n"
+"	jsr		a6@(-42:W)\n"
+"	moveml	sp@+,d2/d3/a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_exp.c
===================================================================
RCS file: /cvsroot/clib2/library/math_exp.c,v
retrieving revision 1.8
diff -u -r1.8 math_exp.c
--- math_exp.c	22 Sep 2006 09:02:51 -0000	1.8
+++ math_exp.c	24 Dec 2010 00:45:24 -0000
@@ -72,24 +72,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubTransBase
-	.globl	___exp
-
-___exp:
-
-	movel	a6,sp@-
-	movel	"A4(_MathIeeeDoubTransBase)",a6
-	moveml	sp@(8),d0/d1
-	jsr		a6@(-78:W)
-	movel	sp@+,a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubTransBase\n"
+"	.globl	___exp\n"
+"\n"
+"___exp:\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubTransBase)",a6\n"
+"	moveml	sp@(8),d0/d1\n"
+"	jsr		a6@(-78:W)\n"
+"	movel	sp@+,a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_extendsfdf2.c
===================================================================
RCS file: /cvsroot/clib2/library/math_extendsfdf2.c,v
retrieving revision 1.3
diff -u -r1.3 math_extendsfdf2.c
--- math_extendsfdf2.c	8 Jan 2006 12:04:23 -0000	1.3
+++ math_extendsfdf2.c	24 Dec 2010 00:45:24 -0000
@@ -51,24 +51,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubTransBase
-	.globl	___extendsfdf2
-
-___extendsfdf2:
-
-	movel	a6,sp@-
-	movel	"A4(_MathIeeeDoubTransBase)",a6
-	movel	sp@(8),d0
-	jsr		a6@(-108:W)
-	movel	sp@+,a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubTransBase\n"
+"	.globl	___extendsfdf2\n"
+"\n"
+"___extendsfdf2:\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubTransBase)",a6\n"
+"	movel	sp@(8),d0\n"
+"	jsr		a6@(-108:W)\n"
+"	movel	sp@+,a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_fabs.c
===================================================================
RCS file: /cvsroot/clib2/library/math_fabs.c,v
retrieving revision 1.8
diff -u -r1.8 math_fabs.c
--- math_fabs.c	8 Jan 2006 12:04:23 -0000	1.8
+++ math_fabs.c	24 Dec 2010 00:45:24 -0000
@@ -63,24 +63,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubBasBase
-	.globl	___fabs
-
-___fabs:
-
-	movel	a6,sp@-
-	movel	"A4(_MathIeeeDoubBasBase)",a6
-	moveml	sp@(8),d0/d1
-	jsr		a6@(-54:W)
-	movel	sp@+,a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubBasBase\n"
+"	.globl	___fabs\n"
+"\n"
+"___fabs:\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubBasBase)",a6\n"
+"	moveml	sp@(8),d0/d1\n"
+"	jsr		a6@(-54:W)\n"
+"	movel	sp@+,a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_fabsf.c
===================================================================
RCS file: /cvsroot/clib2/library/math_fabsf.c,v
retrieving revision 1.3
diff -u -r1.3 math_fabsf.c
--- math_fabsf.c	8 Jan 2006 12:04:23 -0000	1.3
+++ math_fabsf.c	24 Dec 2010 00:45:24 -0000
@@ -63,24 +63,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeSingBasBase
-	.globl	___fabsf
-
-___fabsf:
-
-	movel	a6,sp@-
-	movel	"A4(_MathIeeeSingBasBase)",a6
-	moveml	sp@(8),d0/d1
-	jsr		a6@(-54:W)
-	movel	sp@+,a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeSingBasBase\n"
+"	.globl	___fabsf\n"
+"\n"
+"___fabsf:\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	movel	"A4(_MathIeeeSingBasBase)",a6\n"
+"	moveml	sp@(8),d0/d1\n"
+"	jsr		a6@(-54:W)\n"
+"	movel	sp@+,a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_fixdfsi.c
===================================================================
RCS file: /cvsroot/clib2/library/math_fixdfsi.c,v
retrieving revision 1.3
diff -u -r1.3 math_fixdfsi.c
--- math_fixdfsi.c	8 Jan 2006 12:04:23 -0000	1.3
+++ math_fixdfsi.c	24 Dec 2010 00:45:24 -0000
@@ -51,24 +51,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubBasBase
-	.globl	___fixdfsi
-
-___fixdfsi:
-
-	movel	a6,sp@-
-	movel	"A4(_MathIeeeDoubBasBase)",a6
-	moveml	sp@(8),d0/d1
-	jsr		a6@(-30:W)
-	movel	sp@+,a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubBasBase\n"
+"	.globl	___fixdfsi\n"
+"\n"
+"___fixdfsi:\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubBasBase)",a6\n"
+"	moveml	sp@(8),d0/d1\n"
+"	jsr		a6@(-30:W)\n"
+"	movel	sp@+,a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_floatsidf.c
===================================================================
RCS file: /cvsroot/clib2/library/math_floatsidf.c,v
retrieving revision 1.3
diff -u -r1.3 math_floatsidf.c
--- math_floatsidf.c	8 Jan 2006 12:04:23 -0000	1.3
+++ math_floatsidf.c	24 Dec 2010 00:45:24 -0000
@@ -51,24 +51,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubBasBase
-	.globl	___floatsidf
-
-___floatsidf:
-
-	movel	a6,sp@-
-	movel	"A4(_MathIeeeDoubBasBase)",a6
-	movel	sp@(8),d0
-	jsr		a6@(-36:W)
-	movel	sp@+,a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubBasBase\n"
+"	.globl	___floatsidf\n"
+"\n"
+"___floatsidf:\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubBasBase)",a6\n"
+"	movel	sp@(8),d0\n"
+"	jsr		a6@(-36:W)\n"
+"	movel	sp@+,a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_floor.c
===================================================================
RCS file: /cvsroot/clib2/library/math_floor.c,v
retrieving revision 1.5
diff -u -r1.5 math_floor.c
--- math_floor.c	8 Jan 2006 12:04:23 -0000	1.5
+++ math_floor.c	24 Dec 2010 00:45:24 -0000
@@ -72,24 +72,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubBasBase
-	.globl	___floor
-
-___floor:
-
-	movel	a6,sp@-
-	movel	"A4(_MathIeeeDoubBasBase)",a6
-	moveml	sp@(8),d0/d1
-	jsr		a6@(-90:W)
-	movel	sp@+,a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubBasBase\n"
+"	.globl	___floor\n"
+"\n"
+"___floor:\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubBasBase)",a6\n"
+"	moveml	sp@(8),d0/d1\n"
+"	jsr		a6@(-90:W)\n"
+"	movel	sp@+,a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_gedf2.c
===================================================================
RCS file: /cvsroot/clib2/library/math_gedf2.c,v
retrieving revision 1.3
diff -u -r1.3 math_gedf2.c
--- math_gedf2.c	8 Jan 2006 12:04:23 -0000	1.3
+++ math_gedf2.c	24 Dec 2010 00:45:24 -0000
@@ -51,24 +51,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubBasBase
-	.globl	___gedf2
-
-___gedf2:
-
-	moveml	d2/d3/a6,sp@-
-	movel	"A4(_MathIeeeDoubBasBase)",a6
-	moveml	sp@(16),d0/d1/d2/d3
-	jsr		a6@(-42:W)
-	moveml	sp@+,d2/d3/a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubBasBase\n"
+"	.globl	___gedf2\n"
+"\n"
+"___gedf2:\n"
+"\n"
+"	moveml	d2/d3/a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubBasBase)",a6\n"
+"	moveml	sp@(16),d0/d1/d2/d3\n"
+"	jsr		a6@(-42:W)\n"
+"	moveml	sp@+,d2/d3/a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_gtdf2.c
===================================================================
RCS file: /cvsroot/clib2/library/math_gtdf2.c,v
retrieving revision 1.3
diff -u -r1.3 math_gtdf2.c
--- math_gtdf2.c	8 Jan 2006 12:04:23 -0000	1.3
+++ math_gtdf2.c	24 Dec 2010 00:45:24 -0000
@@ -51,24 +51,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubBasBase
-	.globl	___gtdf2
-
-___gtdf2:
-
-	moveml	d2/d3/a6,sp@-
-	movel	"A4(_MathIeeeDoubBasBase)",a6
-	moveml	sp@(16),d0/d1/d2/d3
-	jsr		a6@(-42:W)
-	moveml	sp@+,d2/d3/a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubBasBase\n"
+"	.globl	___gtdf2\n"
+"\n"
+"___gtdf2:\n"
+"\n"
+"	moveml	d2/d3/a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubBasBase)",a6\n"
+"	moveml	sp@(16),d0/d1/d2/d3\n"
+"	jsr		a6@(-42:W)\n"
+"	moveml	sp@+,d2/d3/a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_ledf2.c
===================================================================
RCS file: /cvsroot/clib2/library/math_ledf2.c,v
retrieving revision 1.3
diff -u -r1.3 math_ledf2.c
--- math_ledf2.c	8 Jan 2006 12:04:23 -0000	1.3
+++ math_ledf2.c	24 Dec 2010 00:45:24 -0000
@@ -51,24 +51,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubBasBase
-	.globl	___ledf2
-
-___ledf2:
-
-	moveml	d2/d3/a6,sp@-
-	movel	"A4(_MathIeeeDoubBasBase)",a6
-	moveml	sp@(16),d0/d1/d2/d3
-	jsr		a6@(-42:W)
-	moveml	sp@+,d2/d3/a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubBasBase\n"
+"	.globl	___ledf2\n"
+"\n"
+"___ledf2:\n"
+"\n"
+"	moveml	d2/d3/a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubBasBase)",a6\n"
+"	moveml	sp@(16),d0/d1/d2/d3\n"
+"	jsr		a6@(-42:W)\n"
+"	moveml	sp@+,d2/d3/a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_log.c
===================================================================
RCS file: /cvsroot/clib2/library/math_log.c,v
retrieving revision 1.10
diff -u -r1.10 math_log.c
--- math_log.c	8 Nov 2007 11:23:53 -0000	1.10
+++ math_log.c	24 Dec 2010 00:45:24 -0000
@@ -72,24 +72,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubTransBase
-	.globl	___log
-
-___log:
-
-	movel	a6,sp@-
-	movel	"A4(_MathIeeeDoubTransBase)",a6
-	moveml	sp@(8),d0/d1
-	jsr		a6@(-84:W)
-	movel	sp@+,a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubTransBase\n"
+"	.globl	___log\n"
+"\n"
+"___log:\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubTransBase)",a6\n"
+"	moveml	sp@(8),d0/d1\n"
+"	jsr		a6@(-84:W)\n"
+"	movel	sp@+,a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_log10.c
===================================================================
RCS file: /cvsroot/clib2/library/math_log10.c,v
retrieving revision 1.9
diff -u -r1.9 math_log10.c
--- math_log10.c	8 Nov 2007 11:23:53 -0000	1.9
+++ math_log10.c	24 Dec 2010 00:45:24 -0000
@@ -72,24 +72,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubTransBase
-	.globl	___log10
-
-___log10:
-
-	movel	a6,sp@-
-	movel	"A4(_MathIeeeDoubBasBase)",a6
-	moveml	sp@(8),d0/d1
-	jsr		a6@(-126:W)
-	movel	sp@+,a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubTransBase\n"
+"	.globl	___log10\n"
+"\n"
+"___log10:\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubBasBase)",a6\n"
+"	moveml	sp@(8),d0/d1\n"
+"	jsr		a6@(-126:W)\n"
+"	movel	sp@+,a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_ltdf2.c
===================================================================
RCS file: /cvsroot/clib2/library/math_ltdf2.c,v
retrieving revision 1.3
diff -u -r1.3 math_ltdf2.c
--- math_ltdf2.c	8 Jan 2006 12:04:23 -0000	1.3
+++ math_ltdf2.c	24 Dec 2010 00:45:24 -0000
@@ -51,24 +51,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubBasBase
-	.globl	___ltdf2
-
-___ltdf2:
-
-	moveml	d2/d3/a6,sp@-
-	movel	"A4(_MathIeeeDoubBasBase)",a6
-	moveml	sp@(16),d0/d1/d2/d3
-	jsr		a6@(-42:W)
-	moveml	sp@+,d2/d3/a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubBasBase\n"
+"	.globl	___ltdf2\n"
+"\n"
+"___ltdf2:\n"
+"\n"
+"	moveml	d2/d3/a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubBasBase)",a6\n"
+"	moveml	sp@(16),d0/d1/d2/d3\n"
+"	jsr		a6@(-42:W)\n"
+"	moveml	sp@+,d2/d3/a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_muldf3.c
===================================================================
RCS file: /cvsroot/clib2/library/math_muldf3.c,v
retrieving revision 1.3
diff -u -r1.3 math_muldf3.c
--- math_muldf3.c	8 Jan 2006 12:04:23 -0000	1.3
+++ math_muldf3.c	24 Dec 2010 00:45:24 -0000
@@ -51,24 +51,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubBasBase
-	.globl	___muldf3
-
-___muldf3:
-
-	moveml	d2/d3/a6,sp@-
-	movel	"A4(_MathIeeeDoubBasBase)",a6
-	moveml	sp@(16),d0/d1/d2/d3
-	jsr		a6@(-78:W)
-	moveml	sp@+,d2/d3/a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubBasBase\n"
+"	.globl	___muldf3\n"
+"\n"
+"___muldf3:\n"
+"\n"
+"	moveml	d2/d3/a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubBasBase)",a6\n"
+"	moveml	sp@(16),d0/d1/d2/d3\n"
+"	jsr		a6@(-78:W)\n"
+"	moveml	sp@+,d2/d3/a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_nedf2.c
===================================================================
RCS file: /cvsroot/clib2/library/math_nedf2.c,v
retrieving revision 1.3
diff -u -r1.3 math_nedf2.c
--- math_nedf2.c	8 Jan 2006 12:04:24 -0000	1.3
+++ math_nedf2.c	24 Dec 2010 00:45:24 -0000
@@ -51,24 +51,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubBasBase
-	.globl	___nedf2
-
-___nedf2:
-
-	moveml	d2/d3/a6,sp@-
-	movel	"A4(_MathIeeeDoubBasBase)",a6
-	moveml	sp@(16),d0/d1/d2/d3
-	jsr		a6@(-42:W)
-	moveml	sp@+,d2/d3/a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubBasBase\n"
+"	.globl	___nedf2\n"
+"\n"
+"___nedf2:\n"
+"\n"
+"	moveml	d2/d3/a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubBasBase)",a6\n"
+"	moveml	sp@(16),d0/d1/d2/d3\n"
+"	jsr		a6@(-42:W)\n"
+"	moveml	sp@+,d2/d3/a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_negdf2.c
===================================================================
RCS file: /cvsroot/clib2/library/math_negdf2.c,v
retrieving revision 1.3
diff -u -r1.3 math_negdf2.c
--- math_negdf2.c	8 Jan 2006 12:04:24 -0000	1.3
+++ math_negdf2.c	24 Dec 2010 00:45:24 -0000
@@ -51,24 +51,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubBasBase
-	.globl	___negdf2
-
-___negdf2:
-
-	movel	a6,sp@-
-	movel	"A4(_MathIeeeDoubBasBase)",a6
-	moveml	sp@(8),d0/d1
-	jsr		a6@(-60:W)
-	movel	sp@+,a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubBasBase\n"
+"	.globl	___negdf2\n"
+"\n"
+"___negdf2:\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubBasBase)",a6\n"
+"	moveml	sp@(8),d0/d1\n"
+"	jsr		a6@(-60:W)\n"
+"	movel	sp@+,a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_pow.c
===================================================================
RCS file: /cvsroot/clib2/library/math_pow.c,v
retrieving revision 1.10
diff -u -r1.10 math_pow.c
--- math_pow.c	8 Jan 2006 12:04:24 -0000	1.10
+++ math_pow.c	24 Dec 2010 00:45:24 -0000
@@ -72,25 +72,24 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubTransBase
-	.globl	___pow
-
-___pow:
-
-	moveml	d2/d3/a6,sp@-
-	movel	"A4(_MathIeeeDoubTransBase)",a6
-	moveml	sp@(16),d0/d1					| Note that the parameters
-	moveml	sp@(24),d2/d3					| are reversed!
-	jsr		a6@(-90:W)
-	moveml	sp@+,d2/d3/a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubTransBase\n"
+"	.globl	___pow\n"
+"\n"
+"___pow:\n"
+"\n"
+"	moveml	d2/d3/a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubTransBase)",a6\n"
+"	moveml	sp@(16),d0/d1					| Note that the parameters\n"
+"	moveml	sp@(24),d2/d3					| are reversed!\n"
+"	jsr		a6@(-90:W)\n"
+"	moveml	sp@+,d2/d3/a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_sin.c
===================================================================
RCS file: /cvsroot/clib2/library/math_sin.c,v
retrieving revision 1.5
diff -u -r1.5 math_sin.c
--- math_sin.c	8 Jan 2006 12:04:24 -0000	1.5
+++ math_sin.c	24 Dec 2010 00:45:24 -0000
@@ -72,24 +72,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubTransBase
-	.globl	___sin
-
-___sin:
-
-	movel	a6,sp@-
-	movel	"A4(_MathIeeeDoubTransBase)",a6
-	moveml	sp@(8),d0/d1
-	jsr		a6@(-36:W)
-	movel	sp@+,a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubTransBase\n"
+"	.globl	___sin\n"
+"\n"
+"___sin:\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubTransBase)",a6\n"
+"	moveml	sp@(8),d0/d1\n"
+"	jsr		a6@(-36:W)\n"
+"	movel	sp@+,a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_sinh.c
===================================================================
RCS file: /cvsroot/clib2/library/math_sinh.c,v
retrieving revision 1.5
diff -u -r1.5 math_sinh.c
--- math_sinh.c	8 Jan 2006 12:04:24 -0000	1.5
+++ math_sinh.c	24 Dec 2010 00:45:24 -0000
@@ -72,24 +72,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubTransBase
-	.globl	___sinh
-
-___sinh:
-
-	movel	a6,sp@-
-	movel	"A4(_MathIeeeDoubTransBase)",a6
-	moveml	sp@(8),d0/d1
-	jsr		a6@(-60:W)
-	movel	sp@+,a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubTransBase\n"
+"	.globl	___sinh\n"
+"\n"
+"___sinh:\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubTransBase)",a6\n"
+"	moveml	sp@(8),d0/d1\n"
+"	jsr		a6@(-60:W)\n"
+"	movel	sp@+,a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_sqrt.c
===================================================================
RCS file: /cvsroot/clib2/library/math_sqrt.c,v
retrieving revision 1.9
diff -u -r1.9 math_sqrt.c
--- math_sqrt.c	22 Sep 2006 07:54:24 -0000	1.9
+++ math_sqrt.c	24 Dec 2010 00:45:24 -0000
@@ -72,24 +72,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubTransBase
-	.globl	___sqrt
-
-___sqrt:
-
-	movel	a6,sp@-
-	movel	"A4(_MathIeeeDoubTransBase)",a6
-	moveml	sp@(8),d0/d1
-	jsr		a6@(-96:W)
-	movel	sp@+,a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubTransBase\n"
+"	.globl	___sqrt\n"
+"\n"
+"___sqrt:\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubTransBase)",a6\n"
+"	moveml	sp@(8),d0/d1\n"
+"	jsr		a6@(-96:W)\n"
+"	movel	sp@+,a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_subdf3.c
===================================================================
RCS file: /cvsroot/clib2/library/math_subdf3.c,v
retrieving revision 1.3
diff -u -r1.3 math_subdf3.c
--- math_subdf3.c	8 Jan 2006 12:04:24 -0000	1.3
+++ math_subdf3.c	24 Dec 2010 00:45:24 -0000
@@ -51,24 +51,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubBasBase
-	.globl	___subdf3
-
-___subdf3:
-
-	moveml	d2/d3/a6,sp@-
-	movel	"A4(_MathIeeeDoubBasBase)",a6
-	moveml	sp@(16),d0/d1/d2/d3
-	jsr		a6@(-72:W)
-	moveml	sp@+,d2/d3/a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubBasBase\n"
+"	.globl	___subdf3\n"
+"\n"
+"___subdf3:\n"
+"\n"
+"	moveml	d2/d3/a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubBasBase)",a6\n"
+"	moveml	sp@(16),d0/d1/d2/d3\n"
+"	jsr		a6@(-72:W)\n"
+"	moveml	sp@+,d2/d3/a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_tan.c
===================================================================
RCS file: /cvsroot/clib2/library/math_tan.c,v
retrieving revision 1.5
diff -u -r1.5 math_tan.c
--- math_tan.c	8 Jan 2006 12:04:24 -0000	1.5
+++ math_tan.c	24 Dec 2010 00:45:24 -0000
@@ -72,24 +72,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubTransBase
-	.globl	___tan
-
-___tan:
-
-	movel	a6,sp@-
-	movel	"A4(_MathIeeeDoubTransBase)",a6
-	moveml	sp@(8),d0/d1
-	jsr		a6@(-48:W)
-	movel	sp@+,a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubTransBase\n"
+"	.globl	___tan\n"
+"\n"
+"___tan:\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubTransBase)",a6\n"
+"	moveml	sp@(8),d0/d1\n"
+"	jsr		a6@(-48:W)\n"
+"	movel	sp@+,a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_tanh.c
===================================================================
RCS file: /cvsroot/clib2/library/math_tanh.c,v
retrieving revision 1.5
diff -u -r1.5 math_tanh.c
--- math_tanh.c	8 Jan 2006 12:04:24 -0000	1.5
+++ math_tanh.c	24 Dec 2010 00:45:24 -0000
@@ -72,24 +72,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubTransBase
-	.globl	___tanh
-
-___tanh:
-
-	movel	a6,sp@-
-	movel	"A4(_MathIeeeDoubTransBase)",a6
-	moveml	sp@(8),d0/d1
-	jsr		a6@(-72:W)
-	movel	sp@+,a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubTransBase\n"
+"	.globl	___tanh\n"
+"\n"
+"___tanh:\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubTransBase)",a6\n"
+"	moveml	sp@(8),d0/d1\n"
+"	jsr		a6@(-72:W)\n"
+"	movel	sp@+,a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: math_truncdfsf2.c
===================================================================
RCS file: /cvsroot/clib2/library/math_truncdfsf2.c,v
retrieving revision 1.3
diff -u -r1.3 math_truncdfsf2.c
--- math_truncdfsf2.c	8 Jan 2006 12:04:24 -0000	1.3
+++ math_truncdfsf2.c	24 Dec 2010 00:45:24 -0000
@@ -51,24 +51,23 @@
 
 /****************************************************************************/
 
-asm("
-
-	.text
-	.even
-
-	.globl	_MathIeeeDoubTransBase
-	.globl	___truncdfsf2
-
-___truncdfsf2:
-
-	movel	a6,sp@-
-	movel	"A4(_MathIeeeDoubTransBase)",a6
-	moveml	sp@(8),d0/d1
-	jsr		a6@(-102:W)
-	movel	sp@+,a6
-	rts
-
-");
+asm("\n"
+"\n"
+"	.text\n"
+"	.even\n"
+"\n"
+"	.globl	_MathIeeeDoubTransBase\n"
+"	.globl	___truncdfsf2\n"
+"\n"
+"___truncdfsf2:\n"
+"\n"
+"	movel	a6,sp@-\n"
+"	movel	"A4(_MathIeeeDoubTransBase)",a6\n"
+"	moveml	sp@(8),d0/d1\n"
+"	jsr		a6@(-102:W)\n"
+"	movel	sp@+,a6\n"
+"	rts\n"
+"\n");
 
 /****************************************************************************/
 
Index: socket_headers.h
===================================================================
RCS file: /cvsroot/clib2/library/socket_headers.h,v
retrieving revision 1.14
diff -u -r1.14 socket_headers.h
--- socket_headers.h	5 Apr 2006 07:53:24 -0000	1.14
+++ socket_headers.h	24 Dec 2010 00:45:25 -0000
@@ -203,7 +203,7 @@
   __asm volatile ("jsr a6@(-30:W)" \
   : "=r"(__socket__re) \
   : "r"(__socket__bn), "r"(__socket_domain), "r"(__socket_type), "r"(__socket_protocol)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "a0", "a1", "fp0", "fp1", "cc", "memory"); \
   __socket__re; \
   }); \
   _socket__re; \
@@ -223,7 +223,7 @@
   __asm volatile ("jsr a6@(-36:W)" \
   : "=r"(__bind__re) \
   : "r"(__bind__bn), "r"(__bind_sock), "r"(__bind_name), "r"(__bind_namelen)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "a1", "fp0", "fp1", "cc", "memory"); \
   __bind__re; \
   }); \
   _bind__re; \
@@ -241,7 +241,7 @@
   __asm volatile ("jsr a6@(-42:W)" \
   : "=r"(__listen__re) \
   : "r"(__listen__bn), "r"(__listen_sock), "r"(__listen_backlog)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "a0", "a1", "fp0", "fp1", "cc", "memory"); \
   __listen__re; \
   }); \
   _listen__re; \
@@ -261,7 +261,7 @@
   __asm volatile ("jsr a6@(-48:W)" \
   : "=r"(__accept__re) \
   : "r"(__accept__bn), "r"(__accept_sock), "r"(__accept_addr), "r"(__accept_addrlen)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "fp0", "fp1", "cc", "memory"); \
   __accept__re; \
   }); \
   _accept__re; \
@@ -281,7 +281,7 @@
   __asm volatile ("jsr a6@(-54:W)" \
   : "=r"(__connect__re) \
   : "r"(__connect__bn), "r"(__connect_sock), "r"(__connect_name), "r"(__connect_namelen)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "a1", "fp0", "fp1", "cc", "memory"); \
   __connect__re; \
   }); \
   _connect__re; \
@@ -307,7 +307,7 @@
   __asm volatile ("jsr a6@(-60:W)" \
   : "=r"(__sendto__re) \
   : "r"(__sendto__bn), "r"(__sendto_sock), "r"(__sendto_buf), "r"(__sendto_len), "r"(__sendto_flags), "r"(__sendto_to), "r"(__sendto_tolen)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "fp0", "fp1", "cc", "memory"); \
   __sendto__re; \
   }); \
   _sendto__re; \
@@ -329,7 +329,7 @@
   __asm volatile ("jsr a6@(-66:W)" \
   : "=r"(__send__re) \
   : "r"(__send__bn), "r"(__send_sock), "r"(__send_buf), "r"(__send_len), "r"(__send_flags)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "a1", "fp0", "fp1", "cc", "memory"); \
   __send__re; \
   }); \
   _send__re; \
@@ -355,7 +355,7 @@
   __asm volatile ("jsr a6@(-72:W)" \
   : "=r"(__recvfrom__re) \
   : "r"(__recvfrom__bn), "r"(__recvfrom_sock), "r"(__recvfrom_buf), "r"(__recvfrom_len), "r"(__recvfrom_flags), "r"(__recvfrom_addr), "r"(__recvfrom_addrlen)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "fp0", "fp1", "cc", "memory"); \
   __recvfrom__re; \
   }); \
   _recvfrom__re; \
@@ -377,7 +377,7 @@
   __asm volatile ("jsr a6@(-78:W)" \
   : "=r"(__recv__re) \
   : "r"(__recv__bn), "r"(__recv_sock), "r"(__recv_buf), "r"(__recv_len), "r"(__recv_flags)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "a1", "fp0", "fp1", "cc", "memory"); \
   __recv__re; \
   }); \
   _recv__re; \
@@ -395,7 +395,7 @@
   __asm volatile ("jsr a6@(-84:W)" \
   : "=r"(__shutdown__re) \
   : "r"(__shutdown__bn), "r"(__shutdown_sock), "r"(__shutdown_how)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "a0", "a1", "fp0", "fp1", "cc", "memory"); \
   __shutdown__re; \
   }); \
   _shutdown__re; \
@@ -419,7 +419,7 @@
   __asm volatile ("jsr a6@(-90:W)" \
   : "=r"(__setsockopt__re) \
   : "r"(__setsockopt__bn), "r"(__setsockopt_sock), "r"(__setsockopt_level), "r"(__setsockopt_optname), "r"(__setsockopt_optval), "r"(__setsockopt_optlen)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "a1", "fp0", "fp1", "cc", "memory"); \
   __setsockopt__re; \
   }); \
   _setsockopt__re; \
@@ -443,7 +443,7 @@
   __asm volatile ("jsr a6@(-96:W)" \
   : "=r"(__getsockopt__re) \
   : "r"(__getsockopt__bn), "r"(__getsockopt_sock), "r"(__getsockopt_level), "r"(__getsockopt_optname), "r"(__getsockopt_optval), "r"(__getsockopt_optlen)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "fp0", "fp1", "cc", "memory"); \
   __getsockopt__re; \
   }); \
   _getsockopt__re; \
@@ -463,7 +463,7 @@
   __asm volatile ("jsr a6@(-102:W)" \
   : "=r"(__getsockname__re) \
   : "r"(__getsockname__bn), "r"(__getsockname_sock), "r"(__getsockname_name), "r"(__getsockname_namelen)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "fp0", "fp1", "cc", "memory"); \
   __getsockname__re; \
   }); \
   _getsockname__re; \
@@ -483,7 +483,7 @@
   __asm volatile ("jsr a6@(-108:W)" \
   : "=r"(__getpeername__re) \
   : "r"(__getpeername__bn), "r"(__getpeername_sock), "r"(__getpeername_name), "r"(__getpeername_namelen)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "fp0", "fp1", "cc", "memory"); \
   __getpeername__re; \
   }); \
   _getpeername__re; \
@@ -503,7 +503,7 @@
   __asm volatile ("jsr a6@(-114:W)" \
   : "=r"(__IoctlSocket__re) \
   : "r"(__IoctlSocket__bn), "r"(__IoctlSocket_sock), "r"(__IoctlSocket_req), "r"(__IoctlSocket_argp)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "a1", "fp0", "fp1", "cc", "memory"); \
   __IoctlSocket__re; \
   }); \
   _IoctlSocket__re; \
@@ -545,7 +545,7 @@
   __asm volatile ("jsr a6@(-126:W)" \
   : "=r"(__WaitSelect__re) \
   : "r"(__WaitSelect__bn), "r"(__WaitSelect_nfds), "r"(__WaitSelect_read_fds), "r"(__WaitSelect_write_fds), "r"(__WaitSelect_except_fds), "r"(__WaitSelect_timeout), "r"(__WaitSelect_signals)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "fp0", "fp1", "cc", "memory"); \
   __WaitSelect__re; \
   }); \
   _WaitSelect__re; \
@@ -563,7 +563,7 @@
   __asm volatile ("jsr a6@(-132:W)" \
   : \
   : "r"(__SetSocketSignals__bn), "r"(__SetSocketSignals_int_mask), "r"(__SetSocketSignals_io_mask), "r"(__SetSocketSignals_urgent_mask)  \
-  : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "a0", "a1", "fp0", "fp1", "cc", "memory"); \
   } \
 })
 
@@ -597,7 +597,7 @@
   __asm volatile ("jsr a6@(-144:W)" \
   : "=r"(__ObtainSocket__re) \
   : "r"(__ObtainSocket__bn), "r"(__ObtainSocket_id), "r"(__ObtainSocket_domain), "r"(__ObtainSocket_type), "r"(__ObtainSocket_protocol)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "a0", "a1", "fp0", "fp1", "cc", "memory"); \
   __ObtainSocket__re; \
   }); \
   _ObtainSocket__re; \
@@ -615,7 +615,7 @@
   __asm volatile ("jsr a6@(-150:W)" \
   : "=r"(__ReleaseSocket__re) \
   : "r"(__ReleaseSocket__bn), "r"(__ReleaseSocket_sock), "r"(__ReleaseSocket_id)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "a0", "a1", "fp0", "fp1", "cc", "memory"); \
   __ReleaseSocket__re; \
   }); \
   _ReleaseSocket__re; \
@@ -633,7 +633,7 @@
   __asm volatile ("jsr a6@(-156:W)" \
   : "=r"(__ReleaseCopyOfSocket__re) \
   : "r"(__ReleaseCopyOfSocket__bn), "r"(__ReleaseCopyOfSocket_sock), "r"(__ReleaseCopyOfSocket_id)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "a0", "a1", "fp0", "fp1", "cc", "memory"); \
   __ReleaseCopyOfSocket__re; \
   }); \
   _ReleaseCopyOfSocket__re; \
@@ -663,7 +663,7 @@
   __asm volatile ("jsr a6@(-168:W)" \
   : \
   : "r"(__SetErrnoPtr__bn), "r"(__SetErrnoPtr_errno_ptr), "r"(__SetErrnoPtr_size)  \
-  : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "a1", "fp0", "fp1", "cc", "memory"); \
   } \
 })
 
@@ -693,7 +693,7 @@
   __asm volatile ("jsr a6@(-180:W)" \
   : "=r"(__inet_addr__re) \
   : "r"(__inet_addr__bn), "r"(__inet_addr_cp)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "a1", "fp0", "fp1", "cc", "memory"); \
   __inet_addr__re; \
   }); \
   _inet_addr__re; \
@@ -743,7 +743,7 @@
   __asm volatile ("jsr a6@(-198:W)" \
   : "=r"(__Inet_MakeAddr__re) \
   : "r"(__Inet_MakeAddr__bn), "r"(__Inet_MakeAddr_net), "r"(__Inet_MakeAddr_host)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "a0", "a1", "fp0", "fp1", "cc", "memory"); \
   __Inet_MakeAddr__re; \
   }); \
   _Inet_MakeAddr__re; \
@@ -759,7 +759,7 @@
   __asm volatile ("jsr a6@(-204:W)" \
   : "=r"(__inet_network__re) \
   : "r"(__inet_network__bn), "r"(__inet_network_cp)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "a1", "fp0", "fp1", "cc", "memory"); \
   __inet_network__re; \
   }); \
   _inet_network__re; \
@@ -775,7 +775,7 @@
   __asm volatile ("jsr a6@(-210:W)" \
   : "=r"(__gethostbyname__re) \
   : "r"(__gethostbyname__bn), "r"(__gethostbyname_name)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "a1", "fp0", "fp1", "cc", "memory"); \
   __gethostbyname__re; \
   }); \
   _gethostbyname__re; \
@@ -795,7 +795,7 @@
   __asm volatile ("jsr a6@(-216:W)" \
   : "=r"(__gethostbyaddr__re) \
   : "r"(__gethostbyaddr__bn), "r"(__gethostbyaddr_addr), "r"(__gethostbyaddr_len), "r"(__gethostbyaddr_type)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "a1", "fp0", "fp1", "cc", "memory"); \
   __gethostbyaddr__re; \
   }); \
   _gethostbyaddr__re; \
@@ -811,7 +811,7 @@
   __asm volatile ("jsr a6@(-222:W)" \
   : "=r"(__getnetbyname__re) \
   : "r"(__getnetbyname__bn), "r"(__getnetbyname_name)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "a1", "fp0", "fp1", "cc", "memory"); \
   __getnetbyname__re; \
   }); \
   _getnetbyname__re; \
@@ -829,7 +829,7 @@
   __asm volatile ("jsr a6@(-228:W)" \
   : "=r"(__getnetbyaddr__re) \
   : "r"(__getnetbyaddr__bn), "r"(__getnetbyaddr_net), "r"(__getnetbyaddr_type)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "a0", "a1", "fp0", "fp1", "cc", "memory"); \
   __getnetbyaddr__re; \
   }); \
   _getnetbyaddr__re; \
@@ -847,7 +847,7 @@
   __asm volatile ("jsr a6@(-234:W)" \
   : "=r"(__getservbyname__re) \
   : "r"(__getservbyname__bn), "r"(__getservbyname_name), "r"(__getservbyname_proto)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "fp0", "fp1", "cc", "memory"); \
   __getservbyname__re; \
   }); \
   _getservbyname__re; \
@@ -865,7 +865,7 @@
   __asm volatile ("jsr a6@(-240:W)" \
   : "=r"(__getservbyport__re) \
   : "r"(__getservbyport__bn), "r"(__getservbyport_port), "r"(__getservbyport_proto)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "a1", "fp0", "fp1", "cc", "memory"); \
   __getservbyport__re; \
   }); \
   _getservbyport__re; \
@@ -881,7 +881,7 @@
   __asm volatile ("jsr a6@(-246:W)" \
   : "=r"(__getprotobyname__re) \
   : "r"(__getprotobyname__bn), "r"(__getprotobyname_name)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "a1", "fp0", "fp1", "cc", "memory"); \
   __getprotobyname__re; \
   }); \
   _getprotobyname__re; \
@@ -915,7 +915,7 @@
   __asm volatile ("jsr a6@(-258:W)" \
   : \
   : "r"(__vsyslog__bn), "r"(__vsyslog_pri), "r"(__vsyslog_msg), "r"(__vsyslog_args)  \
-  : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "fp0", "fp1", "cc", "memory"); \
   } \
 })
 
@@ -931,7 +931,7 @@
   __asm volatile ("jsr a6@(-264:W)" \
   : "=r"(__Dup2Socket__re) \
   : "r"(__Dup2Socket__bn), "r"(__Dup2Socket_old_socket), "r"(__Dup2Socket_new_socket)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "a0", "a1", "fp0", "fp1", "cc", "memory"); \
   __Dup2Socket__re; \
   }); \
   _Dup2Socket__re; \
@@ -951,7 +951,7 @@
   __asm volatile ("jsr a6@(-270:W)" \
   : "=r"(__sendmsg__re) \
   : "r"(__sendmsg__bn), "r"(__sendmsg_sock), "r"(__sendmsg_msg), "r"(__sendmsg_flags)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "a1", "fp0", "fp1", "cc", "memory"); \
   __sendmsg__re; \
   }); \
   _sendmsg__re; \
@@ -971,7 +971,7 @@
   __asm volatile ("jsr a6@(-276:W)" \
   : "=r"(__recvmsg__re) \
   : "r"(__recvmsg__bn), "r"(__recvmsg_sock), "r"(__recvmsg_msg), "r"(__recvmsg_flags)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "a1", "fp0", "fp1", "cc", "memory"); \
   __recvmsg__re; \
   }); \
   _recvmsg__re; \
@@ -989,7 +989,7 @@
   __asm volatile ("jsr a6@(-282:W)" \
   : "=r"(__gethostname__re) \
   : "r"(__gethostname__bn), "r"(__gethostname_name), "r"(__gethostname_namelen)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "a1", "fp0", "fp1", "cc", "memory"); \
   __gethostname__re; \
   }); \
   _gethostname__re; \
@@ -1019,7 +1019,7 @@
   __asm volatile ("jsr a6@(-294:W)" \
   : "=r"(__SocketBaseTagList__re) \
   : "r"(__SocketBaseTagList__bn), "r"(__SocketBaseTagList_tags)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "a1", "fp0", "fp1", "cc", "memory"); \
   __SocketBaseTagList__re; \
   }); \
   _SocketBaseTagList__re; \
@@ -1035,7 +1035,7 @@
   __asm volatile ("jsr a6@(-690:W)" \
   : "=r"(__ProcessIsServer__re) \
   : "r"(__ProcessIsServer__bn), "r"(__ProcessIsServer_pr) \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "a1", "fp0", "fp1", "cc", "memory"); \
   __ProcessIsServer__re; \
   }); \
   _ProcessIsServer__re; \
Index: stdlib_constructor.h
===================================================================
RCS file: /cvsroot/clib2/library/stdlib_constructor.h,v
retrieving revision 1.3
diff -u -r1.3 stdlib_constructor.h
--- stdlib_constructor.h	8 Jan 2006 12:04:25 -0000	1.3
+++ stdlib_constructor.h	24 Dec 2010 00:45:25 -0000
@@ -83,14 +83,14 @@
 #define CONSTRUCTOR(name,pri) \
 	asm(".stabs \"___INIT_LIST__\",22,0,0,___ctor_" #name); \
 	asm(".stabs \"___INIT_LIST__\",20,0,0," #pri); \
-	VOID __ctor_##name##(VOID); \
-	VOID __ctor_##name##(VOID)
+	VOID __ctor_##name(VOID); \
+	VOID __ctor_##name(VOID)
 
 #define DESTRUCTOR(name,pri) \
 	asm(".stabs \"___EXIT_LIST__\",22,0,0,___dtor_" #name); \
 	asm(".stabs \"___EXIT_LIST__\",20,0,0," #pri); \
-	VOID __dtor_##name##(VOID); \
-	VOID __dtor_##name##(VOID)
+	VOID __dtor_##name(VOID); \
+	VOID __dtor_##name(VOID)
 
 #endif /* __amigaos4__ */
 
Index: stdlib_main.c
===================================================================
RCS file: /cvsroot/clib2/library/stdlib_main.c,v
retrieving revision 1.34
diff -u -r1.34 stdlib_main.c
--- stdlib_main.c	30 Sep 2008 14:09:00 -0000	1.34
+++ stdlib_main.c	24 Dec 2010 00:45:25 -0000
@@ -272,8 +272,10 @@
 /****************************************************************************/
 
 STATIC VOID ASM
-detach_cleanup(REG(d0, LONG UNUSED unused_return_code),REG(d1, BPTR segment_list))
+detach_cleanup()
 {
+	register BPTR segment_list __asm("d1");
+
 	#if NOT defined(__amigaos4__)
 	{
 		/* The following trick is necessary only under dos.library V40 and below. */
Index: usergroup_headers.h
===================================================================
RCS file: /cvsroot/clib2/library/usergroup_headers.h,v
retrieving revision 1.8
diff -u -r1.8 usergroup_headers.h
--- usergroup_headers.h	8 Jan 2006 12:04:27 -0000	1.8
+++ usergroup_headers.h	24 Dec 2010 00:45:25 -0000
@@ -147,7 +147,7 @@
   __asm volatile ("jsr a6@(-30:W)" \
   : "=r"(__ug_SetupContextTagList__re) \
   : "r"(__ug_SetupContextTagList__bn), "r"(__ug_SetupContextTagList_name), "r"(__ug_SetupContextTagList_tags)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "fp0", "fp1", "cc", "memory"); \
   __ug_SetupContextTagList__re; \
   }); \
   _ug_SetupContextTagList__re; \
@@ -177,7 +177,7 @@
   __asm volatile ("jsr a6@(-42:W)" \
   : "=r"(__ug_StrError__re) \
   : "r"(__ug_StrError__bn), "r"(__ug_StrError_err)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "a0", "a1", "fp0", "fp1", "cc", "memory"); \
   __ug_StrError__re; \
   }); \
   _ug_StrError__re; \
@@ -223,7 +223,7 @@
   __asm volatile ("jsr a6@(-60:W)" \
   : "=r"(__setreuid__re) \
   : "r"(__setreuid__bn), "r"(__setreuid_real), "r"(__setreuid_effective)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "a0", "a1", "fp0", "fp1", "cc", "memory"); \
   __setreuid__re; \
   }); \
   _setreuid__re; \
@@ -285,7 +285,7 @@
   __asm volatile ("jsr a6@(-84:W)" \
   : "=r"(__setregid__re) \
   : "r"(__setregid__bn), "r"(__setregid_real), "r"(__setregid_effective)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "a0", "a1", "fp0", "fp1", "cc", "memory"); \
   __setregid__re; \
   }); \
   _setregid__re; \
@@ -319,7 +319,7 @@
   __asm volatile ("jsr a6@(-96:W)" \
   : "=r"(__getgroups__re) \
   : "r"(__getgroups__bn), "r"(__getgroups_gidsetlen), "r"(__getgroups_gidset)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "a0", "fp0", "fp1", "cc", "memory"); \
   __getgroups__re; \
   }); \
   _getgroups__re; \
@@ -337,7 +337,7 @@
   __asm volatile ("jsr a6@(-102:W)" \
   : "=r"(__setgroups__re) \
   : "r"(__setgroups__bn), "r"(__setgroups_gidsetlen), "r"(__setgroups_gidset)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "a0", "fp0", "fp1", "cc", "memory"); \
   __setgroups__re; \
   }); \
   _setgroups__re; \
@@ -355,7 +355,7 @@
   __asm volatile ("jsr a6@(-108:W)" \
   : "=r"(__initgroups__re) \
   : "r"(__initgroups__bn), "r"(__initgroups_name), "r"(__initgroups_basegid)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "a0", "fp0", "fp1", "cc", "memory"); \
   __initgroups__re; \
   }); \
   _initgroups__re; \
@@ -371,7 +371,7 @@
   __asm volatile ("jsr a6@(-114:W)" \
   : "=r"(__getpwnam__re) \
   : "r"(__getpwnam__bn), "r"(__getpwnam_login)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "a0", "fp0", "fp1", "cc", "memory"); \
   __getpwnam__re; \
   }); \
   _getpwnam__re; \
@@ -433,7 +433,7 @@
   __asm volatile ("jsr a6@(-144:W)" \
   : "=r"(__getgrnam__re) \
   : "r"(__getgrnam__bn), "r"(__getgrnam_name)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "a0", "fp0", "fp1", "cc", "memory"); \
   __getgrnam__re; \
   }); \
   _getgrnam__re; \
@@ -497,7 +497,7 @@
   __asm volatile ("jsr a6@(-174:W)" \
   : "=r"(__crypt__re) \
   : "r"(__crypt__bn), "r"(__crypt_key), "r"(__crypt_set)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "fp0", "fp1", "cc", "memory"); \
   __crypt__re; \
   }); \
   _crypt__re; \
@@ -517,7 +517,7 @@
   __asm volatile ("jsr a6@(-180:W)" \
   : "=r"(__ug_GetSalt__re) \
   : "r"(__ug_GetSalt__bn), "r"(__ug_GetSalt_user), "r"(__ug_GetSalt_buf), "r"(__ug_GetSalt_size)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "fp0", "fp1", "cc", "memory"); \
   __ug_GetSalt__re; \
   }); \
   _ug_GetSalt__re; \
@@ -533,7 +533,7 @@
   __asm volatile ("jsr a6@(-186:W)" \
   : "=r"(__getpass__re) \
   : "r"(__getpass__bn), "r"(__getpass_prompt)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "a0", "fp0", "fp1", "cc", "memory"); \
   __getpass__re; \
   }); \
   _getpass__re; \
@@ -621,7 +621,7 @@
   __asm volatile ("jsr a6@(-222:W)" \
   : "=r"(__setlogin__re) \
   : "r"(__setlogin__bn), "r"(__setlogin_name)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "a0", "fp0", "fp1", "cc", "memory"); \
   __setlogin__re; \
   }); \
   _setlogin__re; \
@@ -687,7 +687,7 @@
   __asm volatile ("jsr a6@(-252:W)" \
   : "=r"(__setlastlog__re) \
   : "r"(__setlastlog__bn), "r"(__setlastlog_uid), "r"(__setlastlog_name), "r"(__setlastlog_host)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "fp0", "fp1", "cc", "memory"); \
   __setlastlog__re; \
   }); \
   _setlastlog__re; \
@@ -703,7 +703,7 @@
   __asm volatile ("jsr a6@(-258:W)" \
   : "=r"(__getcredentials__re) \
   : "r"(__getcredentials__bn), "r"(__getcredentials_task)  \
-  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
+  : "d1", "a1", "fp0", "fp1", "cc", "memory"); \
   __getcredentials__re; \
   }); \
   _getcredentials__re; \
