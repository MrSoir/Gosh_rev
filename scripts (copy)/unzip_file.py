import zipfile
import sys
import os
import argparse
        
def extractZipFile(zipFilePath, unzipTarPath):
    zipper = zipfile.ZipFile(zipFilePath, 'r', zipfile.ZIP_DEFLATED)
    zipper.extractall(unzipTarPath)
    zipper.close()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='zip files')
    parser.add_argument('-z', '--zip',    help='abs path of zip file', required=True)
    parser.add_argument('-t', '--target', help='target path: folder  that will contain the unzipped files', required=False)
    
    args = parser.parse_args()
    
    zipFilePath   = args.zip;
    unzipTarPath  = args.target
    
    if unzipTarPath is None:
        unzipDir = os.path.dirname(os.path.abspath(zipFilePath))
        unzipFileNameWithoutExt = os.path.splitext(zipFilePath)[0]
        unzipTarPath = os.path.join(unzipDir, unzipFileNameWithoutExt)
        
    print('zipFilePath: ', zipFilePath)
    print('unzipTarPath: ', unzipTarPath)
    
    extractZipFile(zipFilePath, unzipTarPath)



