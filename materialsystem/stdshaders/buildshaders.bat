@echo off

py "%cd%\..\..\devtools\shadercompile\buildshaders.py" "%cd%\game_shader_dx9_20b.txt" -20b
py "%cd%\..\..\devtools\shadercompile\buildshaders.py" "%cd%\game_shader_dx9_30.txt" -30
py "%cd%\..\..\devtools\shadercompile\buildshaders.py" "%cd%\vance_dx9_20b.txt" -20b
py "%cd%\..\..\devtools\shadercompile\buildshaders.py" "%cd%\vance_dx9_30.txt" -30

pause
