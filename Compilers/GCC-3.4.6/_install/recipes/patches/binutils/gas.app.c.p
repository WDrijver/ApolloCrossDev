Index: gas/app.c
===================================================================
--- gas/app.c	(revision 240)
+++ gas/app.c	(working copy)
@@ -1279,6 +1279,8 @@
 			case 1: *to++ = *from++;
 			}
 		    }
+		  if (to >= toend)
+		    goto tofull;
 		  ch = GET ();
 		}
 	    }
