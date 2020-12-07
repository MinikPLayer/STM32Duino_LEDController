@echo off
echo Commit name: 
set /p %commit=
git add * :/
git commit -m "%commit%"
git push origin master