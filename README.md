# dirt_sim_enhancer
Dirt Rally simulation enhancer by means of vibrators RPM based.

This PoC was developed during the COVID lockdown. They idea was:
1) Connecting to Dirt Rally telemetry server with python.
2) Extract the RPM signal.
3) Encode and send the signal to Arduino using serial communications.
4) Use the signal in Arduino to drive 2 vibrators attached to a PlaySeat.

The project integrates 4 parts:
1) Development of a Python Script for binding to a socket for the telemetry of the game.
2) Development of an Arduino code for driving the vibrators.
3) Electronics and Wiring
4) 3D design and printing of the system enclosure and vibrators supports.
