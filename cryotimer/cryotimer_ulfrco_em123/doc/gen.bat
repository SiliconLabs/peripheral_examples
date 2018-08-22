
set projpath=C:\Users\comorgan\Documents\cryo_ports\cryotimer\cryotimer_ulfrco_em123\

"C:\Program Files\JetBrains\PyCharm Community Edition 2017.1.3\bin\pycharm64.exe" buildxml.py %projpath%doc\

"C:\Program Files\JetBrains\PyCharm Community Edition 2017.1.3\bin\pycharm64.exe" "C:\Users\comorgan\Documents\Github\s026_utils\bin\genproject.py" "%projpath%doc\project_new.xml"

"C:\Program Files\JetBrains\PyCharm Community Edition 2017.1.3\bin\pycharm64.exe" fixEFR.py %projpath%SimplicityStudio\
cmd /k