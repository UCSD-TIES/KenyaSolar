KenyaSolar
==========

Repository for the Kenya Solar Power team - includes Datalogger code and Remote Monitoring Application

TIES-Remote_Monitoring: Web application to remotely monitor the Datalogger; Currently, the graphing functionality works pretty well. However, there is no user interface yet, and there is a minor bug to be fixed. The user may upload a CSV (comma separated values) files and the application will parse the file and graph the numbers. The user may select which metrics go on the axes.

Datalogger: Code for the datalogging system. Currently, this supports analog current sensors (ACS712), digital temperature sensors (TMP102), a Real Time Clock module (DS1307), a Luminosity sensor, and a MicroSD card reader/writer (openLog). The microcontroller is the Arduino Mega.