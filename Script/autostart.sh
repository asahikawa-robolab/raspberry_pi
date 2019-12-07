WaitTime="0.1s"

lxterminal  --geometry=28x26 --working-directory=/home/pi/program_files/2019_ControlAutoRobot/Main -e ./main.out -j4
sleep $WaitTime
xdotool windowmove `xdotool getwindowfocus` 0 35
xdotool windowsize `xdotool getwindowfocus` 340 420