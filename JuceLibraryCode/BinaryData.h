/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   settings3_5_png;
    const int            settings3_5_pngSize = 867;

    extern const char*   settings_png;
    const int            settings_pngSize = 1311;

    extern const char*   settings2_png;
    const int            settings2_pngSize = 1565;

    extern const char*   settings3_png;
    const int            settings3_pngSize = 1458;

    extern const char*   navarrowdownsolid2_png;
    const int            navarrowdownsolid2_pngSize = 277;

    extern const char*   navarrowupsolid2_png;
    const int            navarrowupsolid2_pngSize = 272;

    extern const char*   pausesolid2_png;
    const int            pausesolid2_pngSize = 473;

    extern const char*   playsolid2_png;
    const int            playsolid2_pngSize = 479;

    extern const char*   skipnextsolid2_png;
    const int            skipnextsolid2_pngSize = 598;

    extern const char*   skipprevsolid2_png;
    const int            skipprevsolid2_pngSize = 562;

    extern const char*   trashsolid2_png;
    const int            trashsolid2_pngSize = 784;

    extern const char*   pausecircle_ico;
    const int            pausecircle_icoSize = 52946;

    extern const char*   playcircle_ico;
    const int            playcircle_icoSize = 52946;

    extern const char*   skipbackcircle_ico;
    const int            skipbackcircle_icoSize = 52946;

    extern const char*   skipforwardcircle_ico;
    const int            skipforwardcircle_icoSize = 52946;

    extern const char*   pausesolid_png;
    const int            pausesolid_pngSize = 354;

    extern const char*   playsolid_png;
    const int            playsolid_pngSize = 497;

    extern const char*   playlistplus_png;
    const int            playlistplus_pngSize = 330;

    extern const char*   repeatonce_png;
    const int            repeatonce_pngSize = 738;

    extern const char*   repeat_png;
    const int            repeat_pngSize = 730;

    extern const char*   skipnextsolid_png;
    const int            skipnextsolid_pngSize = 566;

    extern const char*   skipprevsolid_png;
    const int            skipprevsolid_pngSize = 556;

    extern const char*   trashsolid_png;
    const int            trashsolid_pngSize = 708;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 23;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
