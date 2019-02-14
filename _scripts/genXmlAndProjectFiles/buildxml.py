import sys
import os

# A list of all currently supported parts
parts = [
    "EFR32BG1P232F256GM48",
    "EFR32BG12P332F1024GL125",
    "EFR32FG1P133F256GM48",
    "EFR32FG12P433F1024GL125",
    "EFM32G890F128",
    "EFM32GG990F1024",
    "EFM32HG322F64",
    "EFM32LG990F256",
    "EFR32MG1P232F256GM48",
    "EFR32MG12P432F1024GL125",
    "EFM32PG1B200F256GM48",
    "EFM32PG12B500F1024GL125",
    "EFM32WG990F256",
    "EFM32ZG222F32"
]

# A list of all currently supported kits, maintained in parity with parts
# and devices
kits = [
    "EFR32BG1_BRD4100A",
    "EFR32BG12_BRD4103A",
    "EFR32FG1_BRD4250A",
    "EFR32FG12_BRD4253A",
    "EFM32_Gxxx_STK",
    "EFM32GG_STK3700",
    "SLSTK3400A_EFM32HG",
    "EFM32LG_STK3600",
    "EFR32MG1_BRD4151A",
    "EFR32MG12_BRD4161A",
    "SLSTK3401A_EFM32PG",
    "SLSTK3402A_EFM32PG12",
    "EFM32WG_STK3800",
    "EFM32ZG_STK3200"
]

# A list of all currently supported devices, maintained in parity with
# parts and kits
devices = [
    "EFR32BG1P",
    "EFR32BG12P",
    "EFR32FG1P",
    "EFR32FG12P",
    "EFM32G",
    "EFM32GG",
    "EFM32HG",
    "EFM32LG",
    "EFR32MG1P",
    "EFR32MG12P",
    "EFM32PG1B",
    "EFM32PG12B",
    "EFM32WG",
    "EFM32ZG"
]

# Helper function to extract needed em_lib files from src includes
def get_em(filepath):
    """
    :rtype: str
    """
    em_file = open(filepath, "r")
    em_contents = em_file.readlines()
    supported_em_file = open("config\\supported_emlib.txt", "r")
    supported_em_contents = supported_em_file.readlines()
    em_list = []
    for em_line in em_contents:
        em_line = em_line.replace("\"", "")
        em_line = em_line.replace("#include ", "")
        em_line = em_line.replace(".h", "")
        if  em_line in supported_em_contents:
            em_line = em_line.replace("\n", "")
            em_list.append(em_line)
    em_string = ""
    for em in em_list:
        em_string += "    <source>##em-path-emlib##\\src\\" + em + ".c</source>" + "\n"
    return em_string

# Helper function to extract needed driver files from src includes. Note that this only supports drivers defined in the
# supported_drivers.txt file
def get_drivers(filepath):
    driver_cfile = open(filepath, "r")
    driver_contents = driver_cfile.readlines()
    supported_driver_file = open("config\\supported_drivers.txt", "r")
    supported_driver_contents = supported_driver_file.readlines()
    driver_list = []
    for driver_line in driver_contents:
        driver_line = driver_line.replace("#include ", "")
        driver_line = driver_line.replace(".h", "")
        driver_line = driver_line.replace("\"", "")
        if driver_line in supported_driver_contents:
            driver_line = driver_line.replace("\n", "")
            driver_list.append(driver_line)
    driver_string = "    <group name=\"Drivers\">" + "\n"
    for d in driver_list:
        driver_string += "      <source>##em-path-drivers##\\" + d + ".c</source>" + "\n"
    driver_string += "    </group>"
    return driver_string



# get path to project from command line or user input
try:
    path = str(sys.argv[1])
except Exception:
    path = raw_input("Please enter the path to the project folder (end with \\)")

# get project name via the provided filepath
name = path.split('\\')[-2]

# allow user to edit the config.txt and customize src files used for each kit
config_file = open("config\\config.txt", "r")
config_contents = config_file.readlines()
config_file.close()

input = raw_input(
    "would you like to edit the config.txt before generating (y/n)?\nConfig is currently:\n" + "".join(config_contents) + "\n")
if input == "y":
    new_config_file = ""
    print "Enter the filename, or 'none' for each board. Typing 'none' will omit the board from project.xml"
    print "Do not use spaces or file extension (ie: main, none, or main_SERIES1 are all valid, while main.c is not)"
    for config_line in config_contents:
        s = config_line.split("=")[0] + "=" + raw_input(config_line.split("=")[0] + "=") + "\n"
        new_config_file += s
    config_file = open("config\\config.txt", "w")
    config_file.write(new_config_file)
    config_file.close()

unique_readmes = raw_input("Are you using unique readme's for each main (IE: readme_EFR32.txt)? (y/n)")

f = open("config\\god_template.xml", "r")
contents = f.readlines()
f.close()

header = contents.pop(0)
header = header.replace("%PROJECT%", name)

config_file = open("config\\config.txt", "r")
config_file_contents = config_file.readlines()
config_file.close()

footer = contents.pop(-1)
contents.insert(-1, "\n")
template = "".join(contents)
project = ""

for part, kit, device in zip(parts, kits, devices):
    for config in config_file_contents:
        if device in config:
            main = config.split("=")[-1]
            main = main.replace("\n", "")
            main = main.replace(" ", "")
            if "_" in main and "y" in unique_readmes:
                readme = "readme_"+main.split("_")[-1]
            else:
                readme = "readme"
            break
    if "none" not in main:
        emlib_src = get_em(path + "src\\" + main + ".c")
        drivergroup = get_drivers(path + "src\\" + main + ".c")
        temp = template.replace("%PROJECT%", name)
        temp = temp.replace("%DEVICE%", device)
        temp = temp.replace("%DEVICE_LC%", device.lower())
        temp = temp.replace("%KIT%", kit)
        temp = temp.replace("%PART%", part)
        temp = temp.replace("%DRIVERGROUP%", drivergroup)
        temp = temp.replace("%EM-LIB_SRC%", emlib_src)
        temp = temp.replace("%MAIN%", main)
        temp = temp.replace("%README%", readme)
        project = project + temp

project = header + project + footer
f = open(path + "doc\\" "project.xml", "w")
f.write(project)
f.close()


