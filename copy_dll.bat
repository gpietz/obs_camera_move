@echo off
if exist cmake-build-debug\obs_camera_move.dll (
    xcopy /Y cmake-build-debug\obs_camera_move.dll d:\Streaming\Tools\OBS-Studio-30.1.2\obs-plugins\64bit\
) else (
    echo DLL not found!
)

if exist cmake-build-debug\obs_camera_move.pdb (
    xcopy /Y cmake-build-debug\obs_camera_move.pdb d:\Streaming\Tools\OBS-Studio-30.1.2\obs-plugins\64bit\
) else (
    echo PDB not found!
)
