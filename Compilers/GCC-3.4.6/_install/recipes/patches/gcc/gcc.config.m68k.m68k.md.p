--- m68k.md.orig	2005-07-26 21:32:25.000000000 +0100
+++ gcc/config/m68k/m68k.md	2015-01-01 14:46:17.666994805 +0000
@@ -3679,7 +3679,7 @@
       target = operand_subword_force (operands[0], 0, SFmode);
       result = expand_binop (SImode, xor_optab,
 			     operand_subword_force (operands[1], 0, SFmode),
-			     GEN_INT (0x80000000), target, 0, OPTAB_WIDEN);
+			     GEN_INT (-2147483647 - 1), target, 0, OPTAB_WIDEN);
       if (result == 0)
 	abort ();
 
@@ -3723,7 +3723,7 @@
       target = operand_subword (operands[0], 0, 1, DFmode);
       result = expand_binop (SImode, xor_optab,
 			     operand_subword_force (operands[1], 0, DFmode),
-			     GEN_INT (0x80000000), target, 0, OPTAB_WIDEN);
+			     GEN_INT (-2147483647 - 1), target, 0, OPTAB_WIDEN);
       if (result == 0)
 	abort ();
 
@@ -6418,7 +6418,7 @@
 	 (match_operand:SI 1 "general_operand" "g"))]
   ;; Operand 1 not really used on the m68000.
 
-  "! flag_pic"
+  "(! flag_pic || flag_pic >= 3)"
 {
 #if MOTOROLA && !defined (USE_GAS)
   return "jsr %0";
@@ -6433,7 +6433,7 @@
 	 (match_operand:SI 1 "general_operand" "g"))]
   ;; Operand 1 not really used on the m68000.
 
-  "flag_pic"
+  "(flag_pic && flag_pic < 3)"
 {
   m68k_output_pic_call(operands[0]);
   return "";
@@ -6460,7 +6460,7 @@
 	(call (match_operand:QI 1 "memory_operand" "o")
 	      (match_operand:SI 2 "general_operand" "g")))]
   ;; Operand 2 not really used on the m68000.
-  "! flag_pic"
+  "(! flag_pic || flag_pic >= 3)"
 {
 #if MOTOROLA && !defined (USE_GAS)
   return "jsr %1";
@@ -6475,7 +6475,7 @@
 	(call (match_operand:QI 1 "memory_operand" "o")
 	      (match_operand:SI 2 "general_operand" "g")))]
   ;; Operand 2 not really used on the m68000.
-  "flag_pic"
+  "(flag_pic && flag_pic < 3)"
 {
   m68k_output_pic_call(operands[1]);
   return "";
@@ -7170,7 +7170,7 @@
       target = operand_subword (operands[0], 0, 1, XFmode);
       result = expand_binop (SImode, xor_optab,
 			     operand_subword_force (operands[1], 0, XFmode),
-			     GEN_INT (0x80000000), target, 0, OPTAB_WIDEN);
+			     GEN_INT (-2147483647 - 1), target, 0, OPTAB_WIDEN);
       if (result == 0)
 	abort ();
 
@@ -7334,3 +7334,16 @@
   default: abort();
   }
 })
+
+; This is only needed for some subtargets.
+(define_expand "allocate_stack"
+  [(set (match_operand:SI 0 "register_operand" "=r")
+	(minus:SI (reg:SI 15) (match_operand:SI 1 "general_operand" "")))
+   (set (reg:SI 15) (minus:SI (reg:SI 15) (match_dup 1)))]
+  "TARGET_ALTERNATE_ALLOCATE_STACK"
+  "
+{
+#ifdef ALTERNATE_ALLOCATE_STACK
+  ALTERNATE_ALLOCATE_STACK(operands);
+#endif
+}")
