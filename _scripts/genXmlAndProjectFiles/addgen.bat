
set projpath=C:\Users\nilehrte\Documents\mcu_peripheral_examples\rtcc\rtcc_alarm_set\

C:\Python27\python.exe buildxml.py %projpath%

C:\Python27\python.exe add_c_includes.py %projpath%

"C:\Python27\python.exe" "C:\SiliconLabs\s026_utils\bin\genproject.py" "%projpath%doc\project.xml"

cmd /k