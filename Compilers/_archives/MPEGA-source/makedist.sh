mkdir distribution
mkdir distribution/mpega_library
Assign dist: distribution/mpega_library
mkdir dist:libs
mkdir dist:developer
mkdir dist:developer/include
mkdir dist:developer/demo

cp mpega_library.doc dist:
cp mpega_library.readme  dist:
cp mpega*.library dist:libs
cp mpegappc*.elf dist:libs
cp -r include dist:developer/include
cp mpega_demo.c dist:developer/demo
cp mpega_demo   dist:developer/demo
cp smakefile    dist:developer/demo
cp mpega.doc    dist:developer

