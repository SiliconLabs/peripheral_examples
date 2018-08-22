
set projpath=C:\SiliconLabs\SimplicityStudio\v4\developer\sdks\gecko_sdk_suite\v1.1\app\mcu_peripheral_examples\gpio\EM4_WakeUp\

C:\Python27\python.exe buildxml.py %projpath%

"C:\Python27\python.exe" "C:\SiliconLabs\s026_utils\bin\genproject.py" "%projpath%doc\project.xml"

cmd /k