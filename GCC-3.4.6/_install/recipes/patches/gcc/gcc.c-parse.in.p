--- c-parse.in	2004-10-15 00:12:53.000000000 +0100
+++ gcc/c-parse.in	2015-01-01 13:56:51.039003490 +0000
@@ -29,7 +29,7 @@ Software Foundation, 59 Temple Place - S
    written by AT&T, but I have never seen it.  */
 
 @@ifc
-%expect 10 /* shift/reduce conflicts, and no reduce/reduce conflicts.  */
+%expect 11 /* shift/reduce conflicts, and no reduce/reduce conflicts.  */
 @@end_ifc
 
 %{
@@ -1730,7 +1730,7 @@ enum_head:
 
 structsp_attr:
 	  struct_head identifier '{'
-		{ $$ = start_struct (RECORD_TYPE, $2);
+		{ $<ttype>$ = start_struct (RECORD_TYPE, $2);
 		  /* Start scope of tag before parsing components.  */
 		}
 	  component_decl_list '}' maybe_attribute
@@ -1741,7 +1741,7 @@ structsp_attr:
 				      nreverse ($3), chainon ($1, $5));
 		}
 	| union_head identifier '{'
-		{ $$ = start_struct (UNION_TYPE, $2); }
+		{ $<ttype>$ = start_struct (UNION_TYPE, $2); }
 	  component_decl_list '}' maybe_attribute
 		{ $$ = finish_struct ($<ttype>4, nreverse ($5),
 				      chainon ($1, $7)); }
@@ -1750,12 +1750,12 @@ structsp_attr:
 				      nreverse ($3), chainon ($1, $5));
 		}
 	| enum_head identifier '{'
-		{ $$ = start_enum ($2); }
+		{ $<ttype>$ = start_enum ($2); }
 	  enumlist maybecomma_warn '}' maybe_attribute
 		{ $$ = finish_enum ($<ttype>4, nreverse ($5),
 				    chainon ($1, $8)); }
 	| enum_head '{'
-		{ $$ = start_enum (NULL_TREE); }
+		{ $<ttype>$ = start_enum (NULL_TREE); }
 	  enumlist maybecomma_warn '}' maybe_attribute
 		{ $$ = finish_enum ($<ttype>3, nreverse ($4),
 				    chainon ($1, $7)); }
@@ -1927,20 +1927,25 @@ absdcl:   /* an absolute declarator */
 	| absdcl1
 	;
 
+/* begin-GG-local: explicit register specification for parameters */
 absdcl_maybe_attribute:   /* absdcl maybe_attribute, but not just attributes */
-	/* empty */
-		{ $$ = build_tree_list (build_tree_list (current_declspecs,
-							 NULL_TREE),
-					all_prefix_attributes); }
-	| absdcl1
-		{ $$ = build_tree_list (build_tree_list (current_declspecs,
-							 $1),
-					all_prefix_attributes); }
-	| absdcl1_noea attributes
-		{ $$ = build_tree_list (build_tree_list (current_declspecs,
-							 $1),
-					chainon ($2, all_prefix_attributes)); }
+	maybeasm
+		{ $$ = build_tree_list (
+			 build_tree_list (build_tree_list (current_declspecs, NULL_TREE),
+					all_prefix_attributes),
+			 $1); }
+	| absdcl1 maybeasm
+		{ $$ = build_tree_list (
+			 build_tree_list (build_tree_list (current_declspecs, $1),
+					  all_prefix_attributes),
+			 $2); }
+	| absdcl1_noea maybeasm attributes
+		{ $$ = build_tree_list (
+			 build_tree_list (build_tree_list (current_declspecs, $1),
+					  chainon ($3, all_prefix_attributes)),
+			 $2); }
 	;
+/* end-GG-local */
 
 absdcl1:  /* a nonempty absolute declarator */
 	  absdcl1_ea
@@ -2596,33 +2601,37 @@ parmlist_2:  /* empty */
 		}
 	;
 
+/* begin-GG-local: explicit register specification for parameters */
 parms:
 	firstparm
-		{ push_parm_decl ($1); }
+		{ push_parm_decl (TREE_PURPOSE($1), TREE_VALUE($1)); }
 	| parms ',' parm
-		{ push_parm_decl ($3); }
+		{ push_parm_decl (TREE_PURPOSE($3), TREE_VALUE($3)); }
 	;
 
 /* A single parameter declaration or parameter type name,
    as found in a parmlist.  */
 parm:
-	  declspecs_ts setspecs parm_declarator maybe_attribute
-		{ $$ = build_tree_list (build_tree_list (current_declspecs,
-							 $3),
-					chainon ($4, all_prefix_attributes));
+	  declspecs_ts setspecs parm_declarator maybeasm maybe_attribute
+		{ $$ = build_tree_list (
+			 build_tree_list (build_tree_list (current_declspecs, $3),
+					  chainon ($5, all_prefix_attributes)),
+			 $4);
 		  POP_DECLSPEC_STACK; }
-	| declspecs_ts setspecs notype_declarator maybe_attribute
-		{ $$ = build_tree_list (build_tree_list (current_declspecs,
-							 $3),
-					chainon ($4, all_prefix_attributes));
+	| declspecs_ts setspecs notype_declarator maybeasm maybe_attribute
+		{ $$ = build_tree_list (
+			 build_tree_list (build_tree_list (current_declspecs, $3),
+					  chainon ($5, all_prefix_attributes)),
+			 $4);
 		  POP_DECLSPEC_STACK; }
 	| declspecs_ts setspecs absdcl_maybe_attribute
 		{ $$ = $3;
 		  POP_DECLSPEC_STACK; }
-	| declspecs_nots setspecs notype_declarator maybe_attribute
-		{ $$ = build_tree_list (build_tree_list (current_declspecs,
-							 $3),
-					chainon ($4, all_prefix_attributes));
+	| declspecs_nots setspecs notype_declarator maybeasm maybe_attribute
+		{ $$ = build_tree_list (
+			 build_tree_list (build_tree_list (current_declspecs, $3),
+					  chainon ($5, all_prefix_attributes)),
+			 $4);
 		  POP_DECLSPEC_STACK; }
 
 	| declspecs_nots setspecs absdcl_maybe_attribute
@@ -2633,29 +2642,33 @@ parm:
 /* The first parm, which must suck attributes from off the top of the parser
    stack.  */
 firstparm:
-	  declspecs_ts_nosa setspecs_fp parm_declarator maybe_attribute
-		{ $$ = build_tree_list (build_tree_list (current_declspecs,
-							 $3),
-					chainon ($4, all_prefix_attributes));
+	  declspecs_ts_nosa setspecs_fp parm_declarator maybeasm maybe_attribute
+		{ $$ = build_tree_list (
+			 build_tree_list (build_tree_list (current_declspecs, $3),
+					  chainon ($5, all_prefix_attributes)),
+			 $4);
 		  POP_DECLSPEC_STACK; }
-	| declspecs_ts_nosa setspecs_fp notype_declarator maybe_attribute
-		{ $$ = build_tree_list (build_tree_list (current_declspecs,
-							 $3),
-					chainon ($4, all_prefix_attributes));
+	| declspecs_ts_nosa setspecs_fp notype_declarator maybeasm maybe_attribute
+		{ $$ = build_tree_list (
+			 build_tree_list (build_tree_list (current_declspecs, $3),
+					  chainon ($5, all_prefix_attributes)),
+			 $4);
 		  POP_DECLSPEC_STACK; }
 	| declspecs_ts_nosa setspecs_fp absdcl_maybe_attribute
 		{ $$ = $3;
 		  POP_DECLSPEC_STACK; }
-	| declspecs_nots_nosa setspecs_fp notype_declarator maybe_attribute
-		{ $$ = build_tree_list (build_tree_list (current_declspecs,
-							 $3),
-					chainon ($4, all_prefix_attributes));
+	| declspecs_nots_nosa setspecs_fp notype_declarator maybeasm maybe_attribute
+		{ $$ = build_tree_list (
+			 build_tree_list (build_tree_list (current_declspecs, $3),
+					  chainon ($5, all_prefix_attributes)),
+			 $4);
 		  POP_DECLSPEC_STACK; }
 
 	| declspecs_nots_nosa setspecs_fp absdcl_maybe_attribute
 		{ $$ = $3;
 		  POP_DECLSPEC_STACK; }
 	;
+/* end-GG-local */
 
 setspecs_fp:
 	  setspecs
@@ -3055,28 +3068,32 @@ mydecl:
 		{ pedwarn ("empty declaration"); }
 	;
 
+/* begin-GG-local: explicit register specification for parameters */
 myparms:
 	myparm
-		{ push_parm_decl ($1); }
+		{ push_parm_decl (TREE_PURPOSE($1), TREE_VALUE($1)); }
 	| myparms ',' myparm
-		{ push_parm_decl ($3); }
+		{ push_parm_decl (TREE_PURPOSE($3), TREE_VALUE($3)); }
 	;
 
 /* A single parameter declaration or parameter type name,
    as found in a parmlist. DOES NOT ALLOW AN INITIALIZER OR ASMSPEC */
 
 myparm:
-	  parm_declarator maybe_attribute
-		{ $$ = build_tree_list (build_tree_list (current_declspecs,
-							 $1),
-					chainon ($2, all_prefix_attributes)); }
-	| notype_declarator maybe_attribute
-		{ $$ = build_tree_list (build_tree_list (current_declspecs,
-							 $1),
-					chainon ($2, all_prefix_attributes)); }
+	  parm_declarator maybeasm maybe_attribute
+		{ $$ = build_tree_list (
+			 build_tree_list (build_tree_list (current_declspecs, $1),
+					  chainon ($3, all_prefix_attributes)),
+			 $2); }
+	| notype_declarator maybeasm maybe_attribute
+		{ $$ = build_tree_list (
+			 build_tree_list (build_tree_list (current_declspecs, $1),
+					  chainon ($2, all_prefix_attributes)),
+			 $2); }
 	| absdcl_maybe_attribute
 		{ $$ = $1; }
 	;
+/* end-GG-local */
 
 optparmlist:
 	  /* empty */
