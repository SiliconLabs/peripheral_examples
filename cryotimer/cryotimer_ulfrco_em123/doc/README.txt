Generates XML and project files for ALL boards for mcu_peripheral_examples

Steps to use:
1. Ensure the desired project.xml is the default provided version, which should generate ONLY for the PG12 board. If this is not the case, delete the extraneous parts of project.xml
2. Modify gen.bat 'projpath' variable to the desired destination for project files, and other locations if necessary.
3. run gen.bat

*3*. When you are ready to submit a pull request, overwrite project.xml with the contents of project_new.xml . Do not do this before you are completely done porting examples. 
This feature is so that while working and editing code, you can easily generate projects without overwriting the original project.xml which is necessary for the script to run correctly (see step 1)

|Optional| If you would like to only generate project xml for specific files, open buildxml.py and comment out the parts you do not wish to generate (ensure you do this for parts, kits, and devices)
