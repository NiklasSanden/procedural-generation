from __future__ import print_function
import os.path
import shutil

def main():
    removePath("bin")
    removePath("bin-int")
    removePath(".vs")
    removePath("ProceduralGeneration.sln")
    removePath("ProceduralGeneration/ProceduralGeneration.vcxproj")
    removePath("ProceduralGeneration/ProceduralGeneration.vcxproj.filters")
    removePath("ProceduralGeneration/ProceduralGeneration.vcxproj.user")

def removePath(filePath):
    if (os.path.exists(filePath)):
        try:
            shutil.rmtree(filePath)
        except NotADirectoryError:
            os.remove(filePath)

if __name__ == '__main__':
    main()