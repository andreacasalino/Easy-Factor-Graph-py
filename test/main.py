import argparse, os, sys, shutil, re
import unittest

class Path:
    def __init__(self, path):
        preamble, self.extension = os.path.splitext(path)
        self.name = os.path.basename(preamble)
        self.parent = os.path.dirname(preamble)

    def filename(self):
        return '{}{}'.format(self.name, self.extension)

def copySo(source):
    print('===> copying *.so from {}'.format(source))
    shutil.copyfile(source, Path(source).filename())

def scanTests(folder):
    fltr = filter(lambda f: re.match(r"(.*)?Test.py", f), os.listdir(folder))
    return [f for f in fltr]

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-c", "--copy", default=None, help='copy *.so library before execution')
    parser.add_argument("-t", "--test", default=None, help='run tests found in specified file. If the file is a folder, runs all tests in that folder')
    args = parser.parse_args()

    if not args.copy == None:
        copySo(args.copy)

    if args.test == None:
        path = os.path.dirname(__file__)
        tests = scanTests(path)
    elif os.path.isdir(args.test):
        tests = scanTests(args.test)
    else:
        tests = [args.test]
    print('===> running tests from:\n{}'.format('\n'.join(tests)))

    for path in tests:
        info = Path(path)
        sys.path.append(info.parent)
        print('=====> {}  {}'.format(info.parent, info.name))
        __import__(info.name)
    
    return
    unittest.main()

if __name__ == '__main__':
    main()