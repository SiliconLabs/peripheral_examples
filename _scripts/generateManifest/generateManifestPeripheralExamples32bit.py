'''
Created on Oct 12, 2017

@author: ritanna
'''

'''
This script is used to generate the manifestPeripheralExamples.xml file required for Studio V4 for the 32-bit boards. 
Assumptions:
1. All sls proj files contain readme file
2. Description of project is second paragraph of readme (as per template)

Before running:
Update EXAMPLES_DIR and TEMPLATES_DIR to be the path to your example directory and template (output) directory, respectively
    note: TEMPLATES_DIR will be appended to EXAMPLES_DIR, so specify as path starting from EXAMPLES_DIR
'''

## EXAMPLE SLSPROJ TEMPLATE
'''
  <descriptors label="BRD4100A EFR32BG1P acmp" description="This project demonstrates...">
    <properties key="core.boardCompatibility" value="brd4100a"/>
    <properties key="core.partCompatibility" value="mcu.arm.efr32.bg1.*"/>
    <properties key="template.category" value="acmp"/>
    <properties key="template.initiallyOpenedResource" value="src/readme_BG.txt"/>
    <properties key="template.projectFilePaths" value="../app/mcu_peripheral_examples/acmp/acmp/iar/acmp.eww,../app/mcu_peripheral_examples/acmp/acmp/SimplicityStudio/BRD4100A_EFR32BG1P_acmp.slsproj"/>
  </descriptors>
'''

import time, os
import re
from lxml import etree as et
import string

##UPDATE
EXAMPLES_DIR = 'C:\\Users\\ritanna\\Work\\Projects\\mcu_peripheral_examples'
TEMPLATES_DIR = '_template'

SDK_LOC =  '../app/mcu_peripheral_examples'

#iterate thru slsprojfile xml tree and find readme file
def findReadMe(root):
    #see if in src folder
    for child in root.findall('folder'):
        if child.attrib['name'] == 'src':
            for subchild in child:
                if 'readme' in subchild.attrib['name'].lower():
                    return subchild.attrib['name']

    #see if in project dir
    for child in root.findall('file'):
        if 'readme' in subchild.attrib['name'].lower():
            return subchild.attrib['name']

    print("WARNING: Could not find readme for proj: ", root.attrib['name'])
    return None

#takes path to readme and returns proper description (second paragraph)
def getDescriptionFromReadme(readme, getComment=False):
    with open(readme, "r") as fileString:
        data=fileString.read()

    paragraphs = re.split(r'[\n][\n]', data)
    description = paragraphs[1].replace("\n", " ")
    return description

#grabs description from readme
def getDescription(path, slsprojRoot):
    description = ''
    readme = findReadMe(slsprojRoot)
    readmePath = os.path.join(path, readme)
    if readme != None:
        description = getDescriptionFromReadme(readmePath)

    if not description:
        print("WARNING: Could not find description for project located here: {}".format(path))

    return description

#grabs part abbrev (ex. pg1) from part # and concats w mcu.arm.efm/r32
def getPartCompatibility(part):
    #find part number
    partCompat = re.findall('[mpbfg][g][1][12]{0,1}|[glwhtz]{0,1}[g]', part)

    #efr or efm?
    priorCompat = "mcu.arm."
    if(string.find(part.lower(), "efm32") is not -1):
        priorCompat = priorCompat + "efm32."
    if(string.find(part.lower(), "efr32") is not -1):
        priorCompat = priorCompat + "efr32."

    return priorCompat + partCompat[0] + ".*"

#add properties to template node of ET
def addProperties(element, key, value):
    et.SubElement(element, "properties", key=key, value=value)

#add template to root of ET
def addTemplate(root, label, name=None, description='', properties={}):
    template = et.SubElement(root, "descriptors")
    template.set('label', label)
    if name:
        template.set('name', name)
    template.set('description', description)

    for key, value in sorted(properties.items()):
        addProperties(template, key, value)

#yield all sls proj files
def findAllSlsprojFiles(path):
    for subdir, dirs, files in os.walk(path):
        for file in files:
            if file.endswith(".slsproj") and os.path.isfile(os.path.join(subdir, file)):
                slsprojFile = os.path.join(subdir, file)
                yield slsprojFile

#backup method of finding description for a proj
def getSlsprojDescription(slsprojRoot):
    label = slsprojRoot.attrib['name'].replace('_', ' ')
    if 'boardCompatibility' in slsprojRoot.attrib:
        description = "Create the {name} project for the {kit}.".format(name=label, kit=slsprojRoot.attrib[
            'boardCompatibility'].upper())
    else:
        description = "Create the {name} project.".format(name=label)
    return description

#return specific attrib of node in ET
def getElementAttrValue(parentEl, tagName, attrKeyToFind, attrValToFind, attrKeyToReturn):
    returnValue = ""
    for ele in parentEl.getiterator(tagName):
        if ele.attrib[attrKeyToFind] == attrValToFind:
            returnValue = ele.attrib[attrKeyToReturn]
            break
    return returnValue


def normalizePath(path):
    if ':' in path:
        path = path.rsplit(':', 1)[1]
    return path


def addSlsprojTemplate(templatesPath, templatesRoot, slsprojRoot, description, projFiles, category):
    label = slsprojRoot.attrib['name'].replace('_', ' ')
    if not description: description = getSlsprojDescription(slsprojRoot)

    properties = {}
    properties['template.category'] = category

    hidden = getElementAttrValue(slsprojRoot, 'property', 'key', 'core.hidden', 'value')
    if hidden:
        properties['core.hidden'] = hidden

    slsprojFilePath = os.path.relpath(normalizePath(slsprojRoot.base), normalizePath(templatesPath))
    projFiles.append(slsprojFilePath)

    projectFilePaths = ','.join(projFiles)
    projectFilePaths = projectFilePaths.replace('\\', '/')  ## Must use unix paths

    properties['template.projectFilePaths'] = re.sub('\.\.', SDK_LOC, projectFilePaths)
    if 'boardCompatibility' in slsprojRoot.attrib:
        properties['core.boardCompatibility'] = slsprojRoot.attrib['boardCompatibility']
    properties['core.partCompatibility'] = getPartCompatibility(slsprojRoot.attrib['partCompatibility'])
    properties['template.initiallyOpenedResource'] = "src/" + findReadMe(slsprojRoot)

    addTemplate(templatesRoot, label, description=description, properties=properties)


OTHER_PROJECT_ENDINGS = ['.eww']


def findOtherProjectFiles(templatesPath, path):
    projects = []
    path = os.path.abspath(os.path.join(path, '../'))
    for subdir, dirs, files in os.walk(path):
        for file in files:
            for ending in OTHER_PROJECT_ENDINGS:
                if file.endswith(ending) and os.path.isfile(os.path.join(subdir, file)):
                    projPath = os.path.relpath(normalizePath(os.path.join(subdir, file)), normalizePath(templatesPath))
                    projects.append(projPath)

    return projects


def findAndAddSlsprojFiles(templatesRoot, templatesPath, examplesPath):
    for slsprojFile in findAllSlsprojFiles(examplesPath):

        projFiles = findOtherProjectFiles(templatesPath, os.path.dirname(slsprojFile))

        try:
            slsprojTree = et.parse(slsprojFile)
        except:
            print("ERROR: {}".format(slsprojFile))
            slsprojTree = et.parse(slsprojFile)

        slsprojRoot = slsprojTree.getroot()

        slsprojContentPath = os.path.abspath(
            os.path.join(os.path.dirname(slsprojFile), slsprojRoot.attrib['contentRoot']))
        #         print(slsprojFile)
        #         print(os.path.dirname(slsprojFile))
        #         print(slsprojRoot.attrib['contentRoot'])
        #         print("path: {}".format(slsprojContentPath))
        #         print("*******")

        description = getDescription(slsprojContentPath, slsprojRoot)

        category = os.path.normpath(examplesPath)
        category = os.path.basename(category)
        addSlsprojTemplate(templatesPath, templatesRoot, slsprojRoot, description, projFiles, category)


def findSlsprojFilesAndGenerateTemplate(examplesPaths, templatesPath):
    templatesFilePath = os.path.join(templatesPath, 'manifestPeripheralExamples.xml')

    et.register_namespace("model", "http://www.silabs.com/ss/Studio.ecore")
    root = et.Element("{http://www.silabs.com/ss/Studio.ecore}MDescriptors")
    for path in examplesPaths:
        findAndAddSlsprojFiles(root, templatesPath, path)

    tree = et.ElementTree(root)
    templatesFile = open(templatesFilePath, 'w')
    templatesFile.write('<?xml version="1.0" encoding="UTF-8"?>\n')
    templatesFile.write(et.tostring(tree, pretty_print=True, encoding='unicode'))
    templatesFile.close()


def updateTemplateXml(examples, template):
    templatesPath = os.path.join(examples, template)
    examplePaths = []
    dirs = os.listdir(examples)
    for dir in dirs:
        if os.path.isdir(os.path.join(examples, dir)):
          if dir != 'Scripts' and dir != '_template' and not dir.startswith('.'):
             examplePaths.append(os.path.join(examples, dir))
    findSlsprojFilesAndGenerateTemplate(examplePaths, templatesPath)


if __name__ == '__main__':
    start = time.time()
    print("Discovering .slsproj files and updating templates.xml.")
    updateTemplateXml(EXAMPLES_DIR, TEMPLATES_DIR)
    elapsed = time.time() - start
    print("DONE! ({0}:{1:.3f})".format(int(elapsed / 60), elapsed % 60))
