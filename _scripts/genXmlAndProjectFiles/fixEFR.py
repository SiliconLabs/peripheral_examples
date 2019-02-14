import os
import re
import sys


def sorted_nicely(l):
    """ Sort the given iterable in the way that humans expect."""
    convert = lambda text: int(text) if text.isdigit() else text
    alphanum_key = lambda key: [convert(c) for c in re.split('([0-9]+)', key)]
    return sorted(l, key=alphanum_key)



path = str(sys.argv[1])
projs = sorted_nicely(os.listdir(path))
kits = [
    "  <includePath uri=\"$(sdkInstallationPath:default())/hardware/kit/EFR32BG12_BRD4103A/config\"/>",
    "  <includePath uri=\"$(sdkInstallationPath:default())/hardware/kit/EFR32BG1_BRD4100A/config\"/>",
    "  <includePath uri=\"$(sdkInstallationPath:default())/hardware/kit/EFR32FG12_BRD4253A/config\"/>",
    "  <includePath uri=\"$(sdkInstallationPath:default())/hardware/kit/EFR32FG1_BRD4250A/config\"/>",
    "  <includePath uri=\"$(sdkInstallationPath:default())/hardware/kit/EFR32MG12_BRD4161A/config\"/>",
    "  <includePath uri=\"$(sdkInstallationPath:default())/hardware/kit/EFR32MG1_BRD4151A/config\"/>"
]

parts = [
    "  <macroDefinition name=\" EFR32BG12P332F1024GL125\" value=\"1\" />",
    "  <macroDefinition name=\" EFR32BG1P232F256GM48\" value=\"1\" />",
    "  <macroDefinition name=\" EFR32FG12P433F1024GL125\" value=\"1\" />",
    "  <macroDefinition name=\" EFR32FG1P133F256GM48\" value=\"1\" />",
    "  <macroDefinition name=\" EFR32MG12P432F1024GL125\" value=\"1\" />",
    "  <macroDefinition name=\" EFR32MG1P232F256GM48\" value=\"1\" />"
]


for proj in projs:
    if "BRD4103A_" in proj:
        kit = kits[0]
        part = parts[0]
    elif "BRD4100A_" in proj:
        kit = kits[1]
        part = parts[1]
    elif "BRD4253A_" in proj:
        kit = kits[2]
        part = parts[2]
    elif "BRD4250A_" in proj:
        kit = kits[3]
        part = parts[3]
    elif "BRD4161A_" in proj:
        kit = kits[4]
        part = parts[4]
    elif "BRD4151A_" in proj:
        kit = kits[5]
        part = parts[5]
    else:
        continue

    f = open(path + proj, "r")
    contents = f.readlines()
    f.close()

    contents.insert(6, kit)
    contents.insert(6, part)

    f = open(path + proj, "w")
    contents = "".join(contents)
    f.write(contents)
    f.close()
