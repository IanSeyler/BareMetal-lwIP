# BareMetal lwIP

A port of lwIP for use on the BareMetal kernel.

`include` contains the newLib headers. `lib` contains a pre-built newlib library and the crt0.o for linking. `lwip-22.2.1.zip` is from [here](https://download-mirror.savannah.gnu.org/releases/lwip/).

## Instructions

Run `./build-app.sh`, install in a BareMetal disk image, and run it.

The app will request an IP address via DHCP and display the values it received.

## Todo

- Fix `sys_check_timeouts();`

// EOF
