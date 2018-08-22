
set projpath=C:\Users\comorgan\Documents\successfully_ported_examples\gpio\switch_led_interrupt\doc\
copy /b project_wspace.xml+project_name.xml+ZG.xml+project_src.xml+project_name.xml+HG.xml+project_src.xml+project_name.xml+G.xml+project_src.xml+project_name.xml+GG.xml+project_src.xml+project_name.xml+LG.xml+project_src.xml+project_name.xml+WG.xml+project_src.xml+project_name.xml+PG1.xml+project_src.xml+project_name.xml+MG1.xml+project_src.xml+project_name.xml+BG1.xml+project_src.xml+project_name.xml+FG1.xml+project_src.xml+project_name.xml+PG12.xml+project_src.xml+project_name.xml+MG12.xml+project_src.xml+project_name.xml+BG12.xml+project_src.xml+project_name.xml+FG12.xml+project_src.xml+footer.xml %projpath%project.xml

"C:\Program Files\JetBrains\PyCharm Community Edition 2017.1.3\bin\pycharm64.exe" "C:\Users\comorgan\Documents\Github\s026_utils\bin\genproject.py" "%projpath%project.xml"

cmd /k