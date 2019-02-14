'''
Searches the repo for any project.xml files and generates all projects from them.

Usage:

python.exe genprojects.py <path to genproject.py>

genproject.py is found in this repo: https://stash.silabs.com/projects/mcu32_repos/repos/s026_utils/browse in the bin folder

This repo is found in super under: tool/emtool
'''



import sys, os, subprocess, shutil

## Project folders to delete / clean before generating new project files
projectDirs = ['iar', 'arm', 'armgcc', 'codesourcery', 'rowley', 'atollic', 'SimplicityStudio']


GENPROJ_PATH = ''
import argparse

def RepoPathArg(v):
    if not os.path.exists(v):
        print('ERROR: Path {} does not exist'.format(v))
        raise argparse.ArgumentError(v, 'Path {} does not exist'.format(v))
    return os.path.abspath(v)

def get_parser_args():
    parser = argparse.ArgumentParser(description="Parse alias files and update DDML")
    parser.add_argument('genProjectPath',
                        type=RepoPathArg,
                        help="Path to the genproject.py script",
                        default="C:\\Git\\MCU32\\super\\tool\\emtool\\bin\\genproject.py")

    # # parse args and return
    args = parser.parse_args()
    return args

def runCmd(commandline):
    if os.name == 'nt':
        build = subprocess.Popen(commandline)
        ret = build.wait()
    else:
        ret = os.system(commandline)
    return ret

def cleanProject(path):
    for p in projectDirs:
        projectDir = os.path.join(path, p)
        if os.path.isdir(projectDir):
            shutil.rmtree(projectDir, ignore_errors=True)


def genProjects():
    cmd = sys.executable + " {}".format(GENPROJ_PATH)
    i = 0

    ## Examples path is one directory below the script directory
    examplesPath = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../'))
    for dirpath, dirnames, filenames in os.walk(examplesPath):
        for file in filenames:
            if file != 'project.xml':
                continue

            basePath = os.path.join(dirpath, '../')
            cleanProject(basePath)
            projectXmlPath = os.path.join(dirpath, file)
            print("Generating project files for " + dirpath)
            sys.stdout.flush()
            retVal = runCmd(cmd + " {}".format(projectXmlPath))
            if (retVal != 0):
                print("Error: Failed to generate project files for " + dirpath)
                sys.stdout.flush()
                sys.exit(retVal)



if __name__ == "__main__":

    if sys.version_info[0] > 2:
        raise RuntimeError("This script must be ran in Python 2.xx (because of genproject.py derpiness)")

    _ARGS = get_parser_args()
    GENPROJ_PATH = _ARGS.genProjectPath

    ## Generate Projects
    print("********************************")
    print("Generating Projects")
    print("********************************")
    genProjects()