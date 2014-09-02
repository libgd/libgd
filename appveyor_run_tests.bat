@ECHO OFF


echo testing bmp_im2im...
CALL build_x86\bmp_im2im.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bmp_im2im x86 " -Outcome %out% -Framework Custom -Filename bmp_im2im.exe -Duration 0
CALL build_x64\bmp_im2im.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bmp_im2im x64 " -Outcome %out% -Framework Custom -Filename bmp_im2im.exe -Duration 0

echo testing bmp_null...
CALL build_x86\bmp_null.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bmp_null x86 " -Outcome %out% -Framework Custom -Filename bmp_null.exe -Duration 0
CALL build_x64\bmp_null.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bmp_null x64 " -Outcome %out% -Framework Custom -Filename bmp_null.exe -Duration 0

echo testing bug00132...
CALL build_x86\bug00132.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00132 x86 " -Outcome %out% -Framework Custom -Filename bug00132.exe -Duration 0
CALL build_x64\bug00132.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00132 x64 " -Outcome %out% -Framework Custom -Filename bug00132.exe -Duration 0

echo testing gd_im2im...
CALL build_x86\gd_im2im.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gd_im2im x86 " -Outcome %out% -Framework Custom -Filename gd_im2im.exe -Duration 0
CALL build_x64\gd_im2im.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gd_im2im x64 " -Outcome %out% -Framework Custom -Filename gd_im2im.exe -Duration 0

echo testing gd_null...
CALL build_x86\gd_null.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gd_null x86 " -Outcome %out% -Framework Custom -Filename gd_null.exe -Duration 0
CALL build_x64\gd_null.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gd_null x64 " -Outcome %out% -Framework Custom -Filename gd_null.exe -Duration 0

echo testing gd_num_colors...
CALL build_x86\gd_num_colors.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gd_num_colors x86 " -Outcome %out% -Framework Custom -Filename gd_num_colors.exe -Duration 0
CALL build_x64\gd_num_colors.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gd_num_colors x64 " -Outcome %out% -Framework Custom -Filename gd_num_colors.exe -Duration 0

echo testing gd_versiontest...
CALL build_x86\gd_versiontest.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gd_versiontest x86 " -Outcome %out% -Framework Custom -Filename gd_versiontest.exe -Duration 0
CALL build_x64\gd_versiontest.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gd_versiontest x64 " -Outcome %out% -Framework Custom -Filename gd_versiontest.exe -Duration 0

echo testing gd2_empty_file...
CALL build_x86\gd2_empty_file.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gd2_empty_file x86 " -Outcome %out% -Framework Custom -Filename gd2_empty_file.exe -Duration 0
CALL build_x64\gd2_empty_file.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gd2_empty_file x64 " -Outcome %out% -Framework Custom -Filename gd2_empty_file.exe -Duration 0

echo testing gd2_im2im...
CALL build_x86\gd2_im2im.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gd2_im2im x86 " -Outcome %out% -Framework Custom -Filename gd2_im2im.exe -Duration 0
CALL build_x64\gd2_im2im.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gd2_im2im x64 " -Outcome %out% -Framework Custom -Filename gd2_im2im.exe -Duration 0

echo testing gd2_null...
CALL build_x86\gd2_null.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gd2_null x86 " -Outcome %out% -Framework Custom -Filename gd2_null.exe -Duration 0
CALL build_x64\gd2_null.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gd2_null x64 " -Outcome %out% -Framework Custom -Filename gd2_null.exe -Duration 0

echo testing gd2_read...
CALL build_x86\gd2_read.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gd2_read x86 " -Outcome %out% -Framework Custom -Filename gd2_read.exe -Duration 0
CALL build_x64\gd2_read.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gd2_read x64 " -Outcome %out% -Framework Custom -Filename gd2_read.exe -Duration 0

echo testing bug00079...
CALL build_x86\bug00079.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00079 x86 " -Outcome %out% -Framework Custom -Filename bug00079.exe -Duration 0
CALL build_x64\bug00079.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00079 x64 " -Outcome %out% -Framework Custom -Filename bug00079.exe -Duration 0

echo testing gdimagecolorclosest...
CALL build_x86\gdimagecolorclosest.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagecolorclosest x86 " -Outcome %out% -Framework Custom -Filename gdimagecolorclosest.exe -Duration 0
CALL build_x64\gdimagecolorclosest.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagecolorclosest x64 " -Outcome %out% -Framework Custom -Filename gdimagecolorclosest.exe -Duration 0

echo testing gdimagecolordeallocate...
CALL build_x86\gdimagecolordeallocate.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagecolordeallocate x86 " -Outcome %out% -Framework Custom -Filename gdimagecolordeallocate.exe -Duration 0
CALL build_x64\gdimagecolordeallocate.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagecolordeallocate x64 " -Outcome %out% -Framework Custom -Filename gdimagecolordeallocate.exe -Duration 0

echo testing gdimagecolorexact...
CALL build_x86\gdimagecolorexact.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagecolorexact x86 " -Outcome %out% -Framework Custom -Filename gdimagecolorexact.exe -Duration 0
CALL build_x64\gdimagecolorexact.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagecolorexact x64 " -Outcome %out% -Framework Custom -Filename gdimagecolorexact.exe -Duration 0

echo testing gdimagecolorreplace...
CALL build_x86\gdimagecolorreplace.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagecolorreplace x86 " -Outcome %out% -Framework Custom -Filename gdimagecolorreplace.exe -Duration 0
CALL build_x64\gdimagecolorreplace.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagecolorreplace x64 " -Outcome %out% -Framework Custom -Filename gdimagecolorreplace.exe -Duration 0

echo testing gdimagecolorresolve...
CALL build_x86\gdimagecolorresolve.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagecolorresolve x86 " -Outcome %out% -Framework Custom -Filename gdimagecolorresolve.exe -Duration 0
CALL build_x64\gdimagecolorresolve.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagecolorresolve x64 " -Outcome %out% -Framework Custom -Filename gdimagecolorresolve.exe -Duration 0

echo testing gdimagecolortransparent...
CALL build_x86\gdimagecolortransparent.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagecolortransparent x86 " -Outcome %out% -Framework Custom -Filename gdimagecolortransparent.exe -Duration 0
CALL build_x64\gdimagecolortransparent.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagecolortransparent x64 " -Outcome %out% -Framework Custom -Filename gdimagecolortransparent.exe -Duration 0

echo testing bug00007...
CALL build_x86\bug00007.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00007 x86 " -Outcome %out% -Framework Custom -Filename bug00007.exe -Duration 0
CALL build_x64\bug00007.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00007 x64 " -Outcome %out% -Framework Custom -Filename bug00007.exe -Duration 0

echo testing bug00081...
CALL build_x86\bug00081.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00081 x86 " -Outcome %out% -Framework Custom -Filename bug00081.exe -Duration 0
CALL build_x64\bug00081.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00081 x64 " -Outcome %out% -Framework Custom -Filename bug00081.exe -Duration 0

echo testing bug00020...
CALL build_x86\bug00020.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00020 x86 " -Outcome %out% -Framework Custom -Filename bug00020.exe -Duration 0
CALL build_x64\bug00020.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00020 x64 " -Outcome %out% -Framework Custom -Filename bug00020.exe -Duration 0

echo testing gdnametest...
CALL build_x86\gdnametest.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdnametest x86 " -Outcome %out% -Framework Custom -Filename gdnametest.exe -Duration 0
CALL build_x64\gdnametest.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdnametest x64 " -Outcome %out% -Framework Custom -Filename gdnametest.exe -Duration 0

echo testing bug00002_1...
CALL build_x86\bug00002_1.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00002_1 x86 " -Outcome %out% -Framework Custom -Filename bug00002_1.exe -Duration 0
CALL build_x64\bug00002_1.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00002_1 x64 " -Outcome %out% -Framework Custom -Filename bug00002_1.exe -Duration 0

echo testing bug00002_2...
CALL build_x86\bug00002_2.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00002_2 x86 " -Outcome %out% -Framework Custom -Filename bug00002_2.exe -Duration 0
CALL build_x64\bug00002_2.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00002_2 x64 " -Outcome %out% -Framework Custom -Filename bug00002_2.exe -Duration 0

echo testing bug00002_3...
CALL build_x86\bug00002_3.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00002_3 x86 " -Outcome %out% -Framework Custom -Filename bug00002_3.exe -Duration 0
CALL build_x64\bug00002_3.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00002_3 x64 " -Outcome %out% -Framework Custom -Filename bug00002_3.exe -Duration 0

echo testing bug00002_4...
CALL build_x86\bug00002_4.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00002_4 x86 " -Outcome %out% -Framework Custom -Filename bug00002_4.exe -Duration 0
CALL build_x64\bug00002_4.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00002_4 x64 " -Outcome %out% -Framework Custom -Filename bug00002_4.exe -Duration 0

echo testing bug00010...
CALL build_x86\bug00010.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00010 x86 " -Outcome %out% -Framework Custom -Filename bug00010.exe -Duration 0
CALL build_x64\bug00010.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00010 x64 " -Outcome %out% -Framework Custom -Filename bug00010.exe -Duration 0

echo testing bug00191...
CALL build_x86\bug00191.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00191 x86 " -Outcome %out% -Framework Custom -Filename bug00191.exe -Duration 0
CALL build_x64\bug00191.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00191 x64 " -Outcome %out% -Framework Custom -Filename bug00191.exe -Duration 0

echo testing bug00100...
CALL build_x86\bug00100.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00100 x86 " -Outcome %out% -Framework Custom -Filename bug00100.exe -Duration 0
CALL build_x64\bug00100.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00100 x64 " -Outcome %out% -Framework Custom -Filename bug00100.exe -Duration 0

echo testing gdimagefilledpolygon0...
CALL build_x86\gdimagefilledpolygon0.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagefilledpolygon0 x86 " -Outcome %out% -Framework Custom -Filename gdimagefilledpolygon0.exe -Duration 0
CALL build_x64\gdimagefilledpolygon0.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagefilledpolygon0 x64 " -Outcome %out% -Framework Custom -Filename gdimagefilledpolygon0.exe -Duration 0

echo testing gdimagefilledpolygon1...
CALL build_x86\gdimagefilledpolygon1.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagefilledpolygon1 x86 " -Outcome %out% -Framework Custom -Filename gdimagefilledpolygon1.exe -Duration 0
CALL build_x64\gdimagefilledpolygon1.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagefilledpolygon1 x64 " -Outcome %out% -Framework Custom -Filename gdimagefilledpolygon1.exe -Duration 0

echo testing gdimagefilledpolygon2...
CALL build_x86\gdimagefilledpolygon2.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagefilledpolygon2 x86 " -Outcome %out% -Framework Custom -Filename gdimagefilledpolygon2.exe -Duration 0
CALL build_x64\gdimagefilledpolygon2.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagefilledpolygon2 x64 " -Outcome %out% -Framework Custom -Filename gdimagefilledpolygon2.exe -Duration 0

echo testing gdimagefilledpolygon3...
CALL build_x86\gdimagefilledpolygon3.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagefilledpolygon3 x86 " -Outcome %out% -Framework Custom -Filename gdimagefilledpolygon3.exe -Duration 0
CALL build_x64\gdimagefilledpolygon3.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagefilledpolygon3 x64 " -Outcome %out% -Framework Custom -Filename gdimagefilledpolygon3.exe -Duration 0

echo testing bug00004...
CALL build_x86\bug00004.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00004 x86 " -Outcome %out% -Framework Custom -Filename bug00004.exe -Duration 0
CALL build_x64\bug00004.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00004 x64 " -Outcome %out% -Framework Custom -Filename bug00004.exe -Duration 0

echo testing bug00078...
CALL build_x86\bug00078.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00078 x86 " -Outcome %out% -Framework Custom -Filename bug00078.exe -Duration 0
CALL build_x64\bug00078.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00078 x64 " -Outcome %out% -Framework Custom -Filename bug00078.exe -Duration 0

echo testing bug00106_gdimagefilledrectangle...
CALL build_x86\bug00106_gdimagefilledrectangle.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00106_gdimagefilledrectangle x86 " -Outcome %out% -Framework Custom -Filename bug00106_gdimagefilledrectangle.exe -Duration 0
CALL build_x64\bug00106_gdimagefilledrectangle.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00106_gdimagefilledrectangle x64 " -Outcome %out% -Framework Custom -Filename bug00106_gdimagefilledrectangle.exe -Duration 0

echo testing bug00037...
CALL build_x86\bug00037.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00037 x86 " -Outcome %out% -Framework Custom -Filename bug00037.exe -Duration 0
CALL build_x64\bug00037.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00037 x64 " -Outcome %out% -Framework Custom -Filename bug00037.exe -Duration 0

echo testing gdCopyBlurred...
CALL build_x86\gdCopyBlurred.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdCopyBlurred x86 " -Outcome %out% -Framework Custom -Filename gdCopyBlurred.exe -Duration 0
CALL build_x64\gdCopyBlurred.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdCopyBlurred x64 " -Outcome %out% -Framework Custom -Filename gdCopyBlurred.exe -Duration 0

echo testing bug00072...
CALL build_x86\bug00072.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00072 x86 " -Outcome %out% -Framework Custom -Filename bug00072.exe -Duration 0
CALL build_x64\bug00072.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00072 x64 " -Outcome %out% -Framework Custom -Filename bug00072.exe -Duration 0

echo testing bug00077...
CALL build_x86\bug00077.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00077 x86 " -Outcome %out% -Framework Custom -Filename bug00077.exe -Duration 0
CALL build_x64\bug00077.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00077 x64 " -Outcome %out% -Framework Custom -Filename bug00077.exe -Duration 0

echo testing bug00111...
CALL build_x86\bug00111.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00111 x86 " -Outcome %out% -Framework Custom -Filename bug00111.exe -Duration 0
CALL build_x64\bug00111.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00111 x64 " -Outcome %out% -Framework Custom -Filename bug00111.exe -Duration 0

echo testing gdImageAALine_thickness...
CALL build_x86\gdImageAALine_thickness.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdImageAALine_thickness x86 " -Outcome %out% -Framework Custom -Filename gdImageAALine_thickness.exe -Duration 0
CALL build_x64\gdImageAALine_thickness.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdImageAALine_thickness x64 " -Outcome %out% -Framework Custom -Filename gdImageAALine_thickness.exe -Duration 0

echo testing gdimageline_aa...
CALL build_x86\gdimageline_aa.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimageline_aa x86 " -Outcome %out% -Framework Custom -Filename gdimageline_aa.exe -Duration 0
CALL build_x64\gdimageline_aa.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimageline_aa x64 " -Outcome %out% -Framework Custom -Filename gdimageline_aa.exe -Duration 0

echo testing gdimageline_aa_outofrange...
CALL build_x86\gdimageline_aa_outofrange.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimageline_aa_outofrange x86 " -Outcome %out% -Framework Custom -Filename gdimageline_aa_outofrange.exe -Duration 0
CALL build_x64\gdimageline_aa_outofrange.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimageline_aa_outofrange x64 " -Outcome %out% -Framework Custom -Filename gdimageline_aa_outofrange.exe -Duration 0

echo testing gdimageline_bug5...
CALL build_x86\gdimageline_bug5.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimageline_bug5 x86 " -Outcome %out% -Framework Custom -Filename gdimageline_bug5.exe -Duration 0
CALL build_x64\gdimageline_bug5.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimageline_bug5 x64 " -Outcome %out% -Framework Custom -Filename gdimageline_bug5.exe -Duration 0

echo testing gdimageopenpolygon0...
CALL build_x86\gdimageopenpolygon0.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimageopenpolygon0 x86 " -Outcome %out% -Framework Custom -Filename gdimageopenpolygon0.exe -Duration 0
CALL build_x64\gdimageopenpolygon0.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimageopenpolygon0 x64 " -Outcome %out% -Framework Custom -Filename gdimageopenpolygon0.exe -Duration 0

echo testing gdimageopenpolygon1...
CALL build_x86\gdimageopenpolygon1.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimageopenpolygon1 x86 " -Outcome %out% -Framework Custom -Filename gdimageopenpolygon1.exe -Duration 0
CALL build_x64\gdimageopenpolygon1.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimageopenpolygon1 x64 " -Outcome %out% -Framework Custom -Filename gdimageopenpolygon1.exe -Duration 0

echo testing gdimageopenpolygon2...
CALL build_x86\gdimageopenpolygon2.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimageopenpolygon2 x86 " -Outcome %out% -Framework Custom -Filename gdimageopenpolygon2.exe -Duration 0
CALL build_x64\gdimageopenpolygon2.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimageopenpolygon2 x64 " -Outcome %out% -Framework Custom -Filename gdimageopenpolygon2.exe -Duration 0

echo testing gdimageopenpolygon3...
CALL build_x86\gdimageopenpolygon3.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimageopenpolygon3 x86 " -Outcome %out% -Framework Custom -Filename gdimageopenpolygon3.exe -Duration 0
CALL build_x64\gdimageopenpolygon3.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimageopenpolygon3 x64 " -Outcome %out% -Framework Custom -Filename gdimageopenpolygon3.exe -Duration 0

echo testing gdimagepixelate...
CALL build_x86\gdimagepixelate.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagepixelate x86 " -Outcome %out% -Framework Custom -Filename gdimagepixelate.exe -Duration 0
CALL build_x64\gdimagepixelate.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagepixelate x64 " -Outcome %out% -Framework Custom -Filename gdimagepixelate.exe -Duration 0

echo testing gdimagepolygon0...
CALL build_x86\gdimagepolygon0.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagepolygon0 x86 " -Outcome %out% -Framework Custom -Filename gdimagepolygon0.exe -Duration 0
CALL build_x64\gdimagepolygon0.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagepolygon0 x64 " -Outcome %out% -Framework Custom -Filename gdimagepolygon0.exe -Duration 0

echo testing gdimagepolygon1...
CALL build_x86\gdimagepolygon1.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagepolygon1 x86 " -Outcome %out% -Framework Custom -Filename gdimagepolygon1.exe -Duration 0
CALL build_x64\gdimagepolygon1.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagepolygon1 x64 " -Outcome %out% -Framework Custom -Filename gdimagepolygon1.exe -Duration 0

echo testing gdimagepolygon2...
CALL build_x86\gdimagepolygon2.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagepolygon2 x86 " -Outcome %out% -Framework Custom -Filename gdimagepolygon2.exe -Duration 0
CALL build_x64\gdimagepolygon2.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagepolygon2 x64 " -Outcome %out% -Framework Custom -Filename gdimagepolygon2.exe -Duration 0

echo testing gdimagepolygon3...
CALL build_x86\gdimagepolygon3.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagepolygon3 x86 " -Outcome %out% -Framework Custom -Filename gdimagepolygon3.exe -Duration 0
CALL build_x64\gdimagepolygon3.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagepolygon3 x64 " -Outcome %out% -Framework Custom -Filename gdimagepolygon3.exe -Duration 0

echo testing bug00003...
CALL build_x86\bug00003.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00003 x86 " -Outcome %out% -Framework Custom -Filename bug00003.exe -Duration 0
CALL build_x64\bug00003.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00003 x64 " -Outcome %out% -Framework Custom -Filename bug00003.exe -Duration 0

echo testing bug00106_gdimagerectangle...
CALL build_x86\bug00106_gdimagerectangle.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00106_gdimagerectangle x86 " -Outcome %out% -Framework Custom -Filename bug00106_gdimagerectangle.exe -Duration 0
CALL build_x64\bug00106_gdimagerectangle.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00106_gdimagerectangle x64 " -Outcome %out% -Framework Custom -Filename bug00106_gdimagerectangle.exe -Duration 0

echo testing bug00067...
CALL build_x86\bug00067.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00067 x86 " -Outcome %out% -Framework Custom -Filename bug00067.exe -Duration 0
CALL build_x64\bug00067.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00067 x64 " -Outcome %out% -Framework Custom -Filename bug00067.exe -Duration 0

echo testing php_bug_64898...
CALL build_x86\php_bug_64898.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "php_bug_64898 x86 " -Outcome %out% -Framework Custom -Filename php_bug_64898.exe -Duration 0
CALL build_x64\php_bug_64898.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "php_bug_64898 x64 " -Outcome %out% -Framework Custom -Filename php_bug_64898.exe -Duration 0

echo testing bug00208_1...
CALL build_x86\bug00208_1.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00208_1 x86 " -Outcome %out% -Framework Custom -Filename bug00208_1.exe -Duration 0
CALL build_x64\bug00208_1.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00208_1 x64 " -Outcome %out% -Framework Custom -Filename bug00208_1.exe -Duration 0

echo testing bug00208_2...
CALL build_x86\bug00208_2.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00208_2 x86 " -Outcome %out% -Framework Custom -Filename bug00208_2.exe -Duration 0
CALL build_x64\bug00208_2.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00208_2 x64 " -Outcome %out% -Framework Custom -Filename bug00208_2.exe -Duration 0

echo testing bug00186...
CALL build_x86\bug00186.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00186 x86 " -Outcome %out% -Framework Custom -Filename bug00186.exe -Duration 0
CALL build_x64\bug00186.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00186 x64 " -Outcome %out% -Framework Custom -Filename bug00186.exe -Duration 0

echo testing gdimagestringft_bbox...
CALL build_x86\gdimagestringft_bbox.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagestringft_bbox x86 " -Outcome %out% -Framework Custom -Filename gdimagestringft_bbox.exe -Duration 0
CALL build_x64\gdimagestringft_bbox.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagestringft_bbox x64 " -Outcome %out% -Framework Custom -Filename gdimagestringft_bbox.exe -Duration 0

echo testing gdimagestringftex_returnfontpathname...
CALL build_x86\gdimagestringftex_returnfontpathname.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagestringftex_returnfontpathname x86 " -Outcome %out% -Framework Custom -Filename gdimagestringftex_returnfontpathname.exe -Duration 0
CALL build_x64\gdimagestringftex_returnfontpathname.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdimagestringftex_returnfontpathname x64 " -Outcome %out% -Framework Custom -Filename gdimagestringftex_returnfontpathname.exe -Duration 0

echo testing gdModesAndPalettes...
CALL build_x86\gdModesAndPalettes.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdModesAndPalettes x86 " -Outcome %out% -Framework Custom -Filename gdModesAndPalettes.exe -Duration 0
CALL build_x64\gdModesAndPalettes.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdModesAndPalettes x64 " -Outcome %out% -Framework Custom -Filename gdModesAndPalettes.exe -Duration 0

echo testing gdTrivialResize...
CALL build_x86\gdTrivialResize.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdTrivialResize x86 " -Outcome %out% -Framework Custom -Filename gdTrivialResize.exe -Duration 0
CALL build_x64\gdTrivialResize.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdTrivialResize x64 " -Outcome %out% -Framework Custom -Filename gdTrivialResize.exe -Duration 0

echo testing gdnewfilectx_null...
CALL build_x86\gdnewfilectx_null.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdnewfilectx_null x86 " -Outcome %out% -Framework Custom -Filename gdnewfilectx_null.exe -Duration 0
CALL build_x64\gdnewfilectx_null.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gdnewfilectx_null x64 " -Outcome %out% -Framework Custom -Filename gdnewfilectx_null.exe -Duration 0

echo testing bug00032...
CALL build_x86\bug00032.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00032 x86 " -Outcome %out% -Framework Custom -Filename bug00032.exe -Duration 0
CALL build_x64\bug00032.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00032 x64 " -Outcome %out% -Framework Custom -Filename bug00032.exe -Duration 0

echo testing bug00005...
CALL build_x86\bug00005.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00005 x86 " -Outcome %out% -Framework Custom -Filename bug00005.exe -Duration 0
CALL build_x64\bug00005.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00005 x64 " -Outcome %out% -Framework Custom -Filename bug00005.exe -Duration 0

echo testing bug00005_2...
CALL build_x86\bug00005_2.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00005_2 x86 " -Outcome %out% -Framework Custom -Filename bug00005_2.exe -Duration 0
CALL build_x64\bug00005_2.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00005_2 x64 " -Outcome %out% -Framework Custom -Filename bug00005_2.exe -Duration 0

echo testing bug00006...
CALL build_x86\bug00006.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00006 x86 " -Outcome %out% -Framework Custom -Filename bug00006.exe -Duration 0
CALL build_x64\bug00006.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00006 x64 " -Outcome %out% -Framework Custom -Filename bug00006.exe -Duration 0

echo testing bug00060...
CALL build_x86\bug00060.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00060 x86 " -Outcome %out% -Framework Custom -Filename bug00060.exe -Duration 0
CALL build_x64\bug00060.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00060 x64 " -Outcome %out% -Framework Custom -Filename bug00060.exe -Duration 0

echo testing bug00066...
CALL build_x86\bug00066.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00066 x86 " -Outcome %out% -Framework Custom -Filename bug00066.exe -Duration 0
CALL build_x64\bug00066.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00066 x64 " -Outcome %out% -Framework Custom -Filename bug00066.exe -Duration 0

echo testing bug00181...
CALL build_x86\bug00181.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00181 x86 " -Outcome %out% -Framework Custom -Filename bug00181.exe -Duration 0
CALL build_x64\bug00181.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00181 x64 " -Outcome %out% -Framework Custom -Filename bug00181.exe -Duration 0

echo testing bug00227...
CALL build_x86\bug00227.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00227 x86 " -Outcome %out% -Framework Custom -Filename bug00227.exe -Duration 0
CALL build_x64\bug00227.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00227 x64 " -Outcome %out% -Framework Custom -Filename bug00227.exe -Duration 0

echo testing gif_im2im...
CALL build_x86\gif_im2im.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gif_im2im x86 " -Outcome %out% -Framework Custom -Filename gif_im2im.exe -Duration 0
CALL build_x64\gif_im2im.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gif_im2im x64 " -Outcome %out% -Framework Custom -Filename gif_im2im.exe -Duration 0

echo testing gif_null...
CALL build_x86\gif_null.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gif_null x86 " -Outcome %out% -Framework Custom -Filename gif_null.exe -Duration 0
CALL build_x64\gif_null.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "gif_null x64 " -Outcome %out% -Framework Custom -Filename gif_null.exe -Duration 0

echo testing jpeg_empty_file...
CALL build_x86\jpeg_empty_file.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "jpeg_empty_file x86 " -Outcome %out% -Framework Custom -Filename jpeg_empty_file.exe -Duration 0
CALL build_x64\jpeg_empty_file.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "jpeg_empty_file x64 " -Outcome %out% -Framework Custom -Filename jpeg_empty_file.exe -Duration 0

echo testing jpeg_im2im...
CALL build_x86\jpeg_im2im.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "jpeg_im2im x86 " -Outcome %out% -Framework Custom -Filename jpeg_im2im.exe -Duration 0
CALL build_x64\jpeg_im2im.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "jpeg_im2im x64 " -Outcome %out% -Framework Custom -Filename jpeg_im2im.exe -Duration 0

echo testing jpeg_null...
CALL build_x86\jpeg_null.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "jpeg_null x86 " -Outcome %out% -Framework Custom -Filename jpeg_null.exe -Duration 0
CALL build_x64\jpeg_null.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "jpeg_null x64 " -Outcome %out% -Framework Custom -Filename jpeg_null.exe -Duration 0

echo testing jpeg_read...
CALL build_x86\jpeg_read.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "jpeg_read x86 " -Outcome %out% -Framework Custom -Filename jpeg_read.exe -Duration 0
CALL build_x64\jpeg_read.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "jpeg_read x64 " -Outcome %out% -Framework Custom -Filename jpeg_read.exe -Duration 0

echo testing jpeg_resolution...
CALL build_x86\jpeg_resolution.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "jpeg_resolution x86 " -Outcome %out% -Framework Custom -Filename jpeg_resolution.exe -Duration 0
CALL build_x64\jpeg_resolution.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "jpeg_resolution x64 " -Outcome %out% -Framework Custom -Filename jpeg_resolution.exe -Duration 0

echo testing bug00011...
CALL build_x86\bug00011.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00011 x86 " -Outcome %out% -Framework Custom -Filename bug00011.exe -Duration 0
CALL build_x64\bug00011.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00011 x64 " -Outcome %out% -Framework Custom -Filename bug00011.exe -Duration 0

echo testing bug00033...
CALL build_x86\bug00033.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00033 x86 " -Outcome %out% -Framework Custom -Filename bug00033.exe -Duration 0
CALL build_x64\bug00033.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00033 x64 " -Outcome %out% -Framework Custom -Filename bug00033.exe -Duration 0

echo testing bug00086...
CALL build_x86\bug00086.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00086 x86 " -Outcome %out% -Framework Custom -Filename bug00086.exe -Duration 0
CALL build_x64\bug00086.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00086 x64 " -Outcome %out% -Framework Custom -Filename bug00086.exe -Duration 0

echo testing bug00088...
CALL build_x86\bug00088.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00088 x86 " -Outcome %out% -Framework Custom -Filename bug00088.exe -Duration 0
CALL build_x64\bug00088.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00088 x64 " -Outcome %out% -Framework Custom -Filename bug00088.exe -Duration 0

echo testing bug00193...
CALL build_x86\bug00193.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00193 x86 " -Outcome %out% -Framework Custom -Filename bug00193.exe -Duration 0
CALL build_x64\bug00193.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "bug00193 x64 " -Outcome %out% -Framework Custom -Filename bug00193.exe -Duration 0

echo testing png_im2im...
CALL build_x86\png_im2im.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "png_im2im x86 " -Outcome %out% -Framework Custom -Filename png_im2im.exe -Duration 0
CALL build_x64\png_im2im.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "png_im2im x64 " -Outcome %out% -Framework Custom -Filename png_im2im.exe -Duration 0

echo testing png_null...
CALL build_x86\png_null.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "png_null x86 " -Outcome %out% -Framework Custom -Filename png_null.exe -Duration 0
CALL build_x64\png_null.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "png_null x64 " -Outcome %out% -Framework Custom -Filename png_null.exe -Duration 0

echo testing png_resolution...
CALL build_x86\png_resolution.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "png_resolution x86 " -Outcome %out% -Framework Custom -Filename png_resolution.exe -Duration 0
CALL build_x64\png_resolution.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "png_resolution x64 " -Outcome %out% -Framework Custom -Filename png_resolution.exe -Duration 0

echo testing tga_null...
CALL build_x86\tga_null.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "tga_null x86 " -Outcome %out% -Framework Custom -Filename tga_null.exe -Duration 0
CALL build_x64\tga_null.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "tga_null x64 " -Outcome %out% -Framework Custom -Filename tga_null.exe -Duration 0

echo testing wbmp_im2im...
CALL build_x86\wbmp_im2im.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "wbmp_im2im x86 " -Outcome %out% -Framework Custom -Filename wbmp_im2im.exe -Duration 0
CALL build_x64\wbmp_im2im.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "wbmp_im2im x64 " -Outcome %out% -Framework Custom -Filename wbmp_im2im.exe -Duration 0

echo testing wbmp_null...
CALL build_x86\wbmp_null.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "wbmp_null x86 " -Outcome %out% -Framework Custom -Filename wbmp_null.exe -Duration 0
CALL build_x64\wbmp_null.exe && (SET out=Passed) || (SET out=Failed)
appveyor AddTest "wbmp_null x64 " -Outcome %out% -Framework Custom -Filename wbmp_null.exe -Duration 0


EXIT /B 0
