'''
There was an update to genproject.py that resulted in weird control characters being outputted by IAR.
These are consumed by a test report generator used for Jenkins. These new characters broke the test
report generator, since it can't consume them.

This script removes those characters.
'''



import os

ALLOWED_CONTROL_CHARS = [9, 10, 13] ## Tab, newline, carriage return

if __name__ == "__main__":

    print("Cleaning output xml, because it is dumb sometimes.")
    path = os.path.dirname(os.path.realpath(__file__))
    path = os.path.abspath(os.path.join(path, '../../out/checkproject_report.xml'))
    print("Cleaning {}".format(path))
    if os.path.isfile(path):
        with open(path, 'r+') as myFile:
            text = myFile.read()
            text = ''.join(c for c in text if (ord(c) in ALLOWED_CONTROL_CHARS or ord(c) >= 32))
            myFile.seek(0)
            myFile.write(text)
            myFile.truncate()
    else:
        print("File not found!")
    print("Done cleaning!")

