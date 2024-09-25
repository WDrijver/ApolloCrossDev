// Apollo Dump Graphics Routines

#include "PrecompiledInclude.h"
#include "ApolloDump.h"


ApolloDump::ApolloDump()
{}

void ApolloDump::ApolloDumpSBPictureSixteen()
{
    int NumberOfSourceFiles = 2 + 18 + 13 + 15;
    
    std::string SourceFileNames[NumberOfSourceFiles] =
    {
        "Data/Levels/Custom1/Nottingham.map",
        "Data/Levels/Custom1/Nottingham.min",

        "Data/Levels/Day/Croisement01.map",
        "Data/Levels/Day/croisement01.min",
        "Data/Levels/Day/Croisement02.map",
        "Data/Levels/Day/croisement02.min",
        "Data/Levels/Day/Croisement03.map",
        "Data/Levels/Day/croisement03.min",
        "Data/Levels/Day/Derby.map",
        "Data/Levels/Day/Derby.min",
        "Data/Levels/Day/Leicester.map",
        "Data/Levels/Day/Leicester.min",
        "Data/Levels/Day/Lincoln.map",
        "Data/Levels/Day/Lincoln.min",
        "Data/Levels/Day/Nottingham.map",
        "Data/Levels/Day/Nottingham.min",
        "Data/Levels/Day/Sherwood.map",
        "Data/Levels/Day/sherwood.min",
        "Data/Levels/Day/york.map",
        "Data/Levels/Day/York.min",

        "Data/Levels/Fog/croisement01.min",
        "Data/Levels/Fog/croisement02.min",
        "Data/Levels/Fog/croisement03.min",
        "Data/Levels/Fog/Derby.map",
        "Data/Levels/Fog/Derby.min",
        "Data/Levels/Fog/Leicester.min",
        "Data/Levels/Fog/Lincoln.map",
        "Data/Levels/Fog/Lincoln.min",
        "Data/Levels/Fog/Nottingham.map",
        "Data/Levels/Fog/Nottingham.min",
        "Data/Levels/Fog/sherwood.min",
        "Data/Levels/Fog/york.map",
        "Data/Levels/Fog/York.min",

        "Data/Levels/Night/croisement01.min",
        "Data/Levels/Night/croisement02.min",
        "Data/Levels/Night/croisement03.min",
        "Data/Levels/Night/Derby.map",
        "Data/Levels/Night/Derby.min",
        "Data/Levels/Night/Leicester.map",
        "Data/Levels/Night/Leicester.min",
        "Data/Levels/Night/Lincoln.map",
        "Data/Levels/Night/Lincoln.min",
        "Data/Levels/Night/Nottingham.map",
        "Data/Levels/Night/Nottingham.min",
        "Data/Levels/Night/sherwood.map",
        "Data/Levels/Night/sherwood.min",
        "Data/Levels/Night/york.map",
        "Data/Levels/Night/York.min",
    };
    
    FILE    *DestinationFile;

    SBPictureSixteen        SourcePicture;
    SBPictureSixteenHeader  SourcePictureHeader;

    for (int i=0 ; i< NumberOfSourceFiles; i++)
    {
        DebugPutStr("ApolloDump::ApolloDumpSBPictureSixteen: Converting to Apollo RAW format: ");
        DebugPutStr(SourceFileNames[i].c_str());
        DebugPutStr("\n");

        SourcePicture.LoadFromFile(SourceFileNames[i]);

        std::string DestinationFileName  = SourceFileNames[i].append(".raw");

        DestinationFile =  fopen(DestinationFileName.c_str(), "w");

        UWORD width = ApolloSwapWord(SourcePicture.GetWidth());
        UWORD height = ApolloSwapWord(SourcePicture.GetHeight());
        ULONG packaging = ApolloSwapLong(SBPICTURESIXTEEN_APOLLO_RAW);
        ULONG size = ApolloSwapLong( SourcePicture.GetWidth() * SourcePicture.GetHeight() * 2);

        fwrite(&width, sizeof(UWORD), 1, DestinationFile);
        fwrite(&height, sizeof(UWORD), 1, DestinationFile);
        fwrite(&packaging, sizeof(ULONG), 1, DestinationFile);
        fwrite(&size, sizeof(ULONG), 1, DestinationFile);

        fwrite(SourcePicture.GetDataPointer(), SourcePicture.GetSize(),1,DestinationFile);
        
        fclose(DestinationFile);
    }
}

