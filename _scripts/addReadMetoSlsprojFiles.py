'''
Created on: 10/12/17

author: ritanna
'''

'''
This script is used to add the corresponding readme files to each slsproj file to aid in building
the xml file. Assumes only 1 readme file is present. Ignores files that already have readme 

Before Running:
Update EXAMPLES_DIR to specify path to example directory
'''

import os

#update
EXAMPLES_DIR = "C:\\Users\\ritanna\\Work\\Projects\\mcu_peripheral_examples"

#returns name of first readme found in parent directory
def findReadMe(slsprojfilepath):
    projDir = os.path.dirname(os.path.dirname(slsprojfilepath))
    for dirs in os.listdir(projDir):
        if 'readme' in dirs.lower():
            return dirs


def addReadMetoFile(slsprojfilepath):
    name = findReadMe(slsprojfilepath)
    newLine = '    <file name="' + name + '" uri="' + name + '" />\n'
    file = open(slsprojfilepath, "r")
    contents = file.readlines()
    file.close()

    #find place to insert line
    lineNumber = 0
    for line in contents:
        lineNumber = lineNumber+1
        if 'name="src"' in line:
            lineNumber = lineNumber+1
            break

    #insert line
    contents.insert(lineNumber, newLine)
    file = open(slsprojfilepath, "w")
    contents = "".join(contents)
    file.write(contents)
    file.close()

def containsReadMe(slsprojfilepath):
    file = open(slsprojfilepath)
    if 'readme' in file.read().lower():
        file.close()
        return True
    else:
        file.close()
        return False

def findAllSlsprojFiles(path):
    for subdir, dirs, files in os.walk(path):
        for file in files:
            if file.endswith(".slsproj") and os.path.isfile(os.path.join(subdir, file)):
                slsprojFilePath = os.path.join(subdir, file)
                yield slsprojFilePath

if __name__ == '__main__':
    for slsprojfilepath in findAllSlsprojFiles(EXAMPLES_DIR):
        if not containsReadMe(slsprojfilepath):
            addReadMetoFile(slsprojfilepath)
