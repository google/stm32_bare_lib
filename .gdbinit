target extended-remote localhost:3333
monitor  arm semihosting enable
set remotetimeout 1000
set arm force-mode thumb
load 
