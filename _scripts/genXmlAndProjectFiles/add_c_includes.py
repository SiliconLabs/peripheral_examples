import os
import re
import sys

try:
    path = str(sys.argv[1])
except Exception:
    path = input("Please enter the path to the project folder (end with \\)")


line = "    <source>##em-path-emlib##\src\em_system.c</source>\n"
alt_line = "    <source>##em-path-emlib##\src\em_emu.c</source>\n"
add_line = "    <source>##em-path-emlib##\src\em_usart.c</source>\n"

pathx = path +"doc\\project.xml"

with open(pathx,'r') as file:
    filedata = file.read()
em = filedata.split('</project>\n')
for i in range(len(em)-1):
    em[i] += '</project>\n'
for j in range(0,len(em)-1):
    m = re.search("^\s*\<source\>\#\#em\-path\-emlib\#\#\\src\\\s*em\_usart\.c\<\/source\>\\n",em[j])
    if m != None:
        print("found")
        continue
    else:
        if em[j].count(line) ==1:
            em[j] = em[j].replace(line,line + add_line)
        elif em[j].count(alt_line) == 1:
            em[j] = em[j].replace(alt_line,alt_line + add_line)
        else:
            print(j)
            raise RuntimeError("Unable to find a place to paste changes in proj no.printed above, write aborted ")
replacement =""
for k in range(len(em)):
    replacement += em[k]
with open(pathx,'w') as file:
    file.write(replacement)