@echo off
if exist cmake-build-debug\obs_plugin_test.dll (
    xcopy /Y cmake-build-debug\obs_plugin_test.dll d:\Streaming\Tools\OBS-Studio-30.1.2\obs-plugins\64bit\
) else (
    echo DLL not found!
)

if exist cmake-build-debug\obs_plugin_test.pdb (
    xcopy /Y cmake-build-debug\obs_plugin_test.pdb d:\Streaming\Tools\OBS-Studio-30.1.2\obs-plugins\64bit\
) else (
    echo PDB not found!
)
