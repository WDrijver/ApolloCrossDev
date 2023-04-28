--- ../../unistd_getopt.c	2006-11-13 09:51:53.000000000 +0000
+++ unistd_getopt.c	2015-01-04 23:23:14.100000466 +0000
@@ -57,7 +57,7 @@ char *	optarg;
 int
 getopt(int argc, char * const argv[], const char *opts)
 {
-	static int sp = 1;
+	static int spp = 1;
 	int result = EOF;
 	char *cp;
 	int c;
@@ -87,7 +87,7 @@ getopt(int argc, char * const argv[], co
 
 	SHOWVALUE(optind);
 
-	if(sp == 1)
+	if(spp == 1)
 	{
 		if(optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0')
 		{
@@ -101,18 +101,18 @@ getopt(int argc, char * const argv[], co
 		}
 	}
 
-	optopt = c = argv[optind][sp];
+	optopt = c = argv[optind][spp];
 
 	if(c == ':' || (cp = strchr(opts, c)) == NULL)
 	{
 		if(opterr != 0)
 			fprintf(stderr, "%s%s%c\n", argv[0], ": illegal option -- ", c);
 
-		if(argv[optind][++sp] == '\0')
+		if(argv[optind][++spp] == '\0')
 		{
 			optind++;
 
-			sp = 1;
+			spp = 1;
 		}
 
 		result = '?';
@@ -121,16 +121,16 @@ getopt(int argc, char * const argv[], co
 
 	if(*++cp == ':')
 	{
-		if(argv[optind][sp+1] != '\0')
+		if(argv[optind][spp+1] != '\0')
 		{
-			optarg = &argv[optind++][sp+1];
+			optarg = &argv[optind++][spp+1];
 		}
 		else if (++optind >= argc)
 		{
 			if(opterr != 0)
 				fprintf(stderr, "%s%s%c\n", argv[0], ": option requires an argument -- ", c);
 
-			sp = 1;
+			spp = 1;
 
 			result = '?';
 			goto out;
@@ -140,13 +140,13 @@ getopt(int argc, char * const argv[], co
 			optarg = argv[optind++];
 		}
 
-		sp = 1;
+		spp = 1;
 	}
 	else
 	{
-		if(argv[optind][++sp] == '\0')
+		if(argv[optind][++spp] == '\0')
 		{
-			sp = 1;
+			spp = 1;
 
 			optind++;
 		}
