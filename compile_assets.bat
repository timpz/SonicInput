@echo off

echo Starting multiple calls to ...

set datadir=_source\data
set outputdir=_source\formatted_data

if exist _source\formatted_data (
    rd _source\formatted_data /s /q
)

_tools\DataFormatter.exe _tools\DataFormatter.exe -i %datadir%\timpz_logo.bmp -d %outputdir% -c 1

_tools\DataFormatter.exe _tools\DataFormatter.exe -i %datadir%\A_Button.bmp -d %outputdir% -c 1
_tools\DataFormatter.exe _tools\DataFormatter.exe -i %datadir%\B_Button.bmp -d %outputdir% -c 1
_tools\DataFormatter.exe _tools\DataFormatter.exe -i %datadir%\C_Button.bmp -d %outputdir% -c 1

_tools\DataFormatter.exe _tools\DataFormatter.exe -i %datadir%\A_Press.bmp -d %outputdir% -c 1
_tools\DataFormatter.exe _tools\DataFormatter.exe -i %datadir%\B_Press.bmp -d %outputdir% -c 1
_tools\DataFormatter.exe _tools\DataFormatter.exe -i %datadir%\C_Press.bmp -d %outputdir% -c 1
_tools\DataFormatter.exe _tools\DataFormatter.exe -i %datadir%\S_Press.bmp -d %outputdir% -c 1

_tools\DataFormatter.exe _tools\DataFormatter.exe -i %datadir%\Dpad.bmp -v Dpad_Image -d %outputdir% -c 1
_tools\DataFormatter.exe _tools\DataFormatter.exe -i %datadir%\DpadInner.bmp -v DpadInner_Image -d %outputdir% -c 1

_tools\DataFormatter.exe _tools\DataFormatter.exe -i %datadir%\Direction_1.bmp -d %outputdir% -c 1
_tools\DataFormatter.exe _tools\DataFormatter.exe -i %datadir%\Direction_2.bmp -d %outputdir% -c 1
_tools\DataFormatter.exe _tools\DataFormatter.exe -i %datadir%\Direction_3.bmp -d %outputdir% -c 1
_tools\DataFormatter.exe _tools\DataFormatter.exe -i %datadir%\Direction_4.bmp -d %outputdir% -c 1
_tools\DataFormatter.exe _tools\DataFormatter.exe -i %datadir%\Direction_5.bmp -d %outputdir% -c 1
_tools\DataFormatter.exe _tools\DataFormatter.exe -i %datadir%\Direction_6.bmp -d %outputdir% -c 1
_tools\DataFormatter.exe _tools\DataFormatter.exe -i %datadir%\Direction_7.bmp -d %outputdir% -c 1
_tools\DataFormatter.exe _tools\DataFormatter.exe -i %datadir%\Direction_8.bmp -d %outputdir% -c 1

echo Formatting Done!