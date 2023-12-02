import argparse, os, re

class Paths:
    SOURCES = os.path.dirname(__file__)
    CASES = os.path.join(SOURCES, 'cases')
    COMMANDS = os.path.join(SOURCES, 'commands')

def scanTests():
    fltr = filter(lambda f: re.match(r"(.*)?Test.py", f), os.listdir(Paths.CASES))
    return [os.path.join(Paths.CASES, f) for f in fltr]

def show():
    print('===> test sources')
    for s in scanTests():
        print(s)

class CMakeLists:
    Location = os.path.join(Paths.COMMANDS, 'CMakeLists.txt')

    def __init__(self, soPath):
        with open(CMakeLists.Location, 'w') as stream:
            pass
        self.mainPath = os.path.join(os.path.dirname(__file__), 'main.py')
        self.soPath = soPath
        self.tests = []

    def add(self, target_name, location):
        self.tests.append((
            target_name,
            location,
        ))

    def print(self):
        with open(CMakeLists.Location, 'a') as stream:
            for target, location in self.tests:
                cmakeCmd = """
add_custom_target({} ALL 
    COMMAND "{}" "{}" -c "{}" -t "{}"
    WORKING_DIRECTORY {}
    DEPENDS {}
)           
                """.format(target, '${Python3_EXECUTABLE}', self.mainPath, self.soPath, location, '${CMAKE_CURRENT_SOURCE_DIR}/..', '${WRAPPER_NAME}')
                stream.write(cmakeCmd)

def terraform(args):
    print('===> terraforming into {}'.format(Paths.COMMANDS))
    os.makedirs(Paths.COMMANDS, exist_ok=True)
    cmake = CMakeLists(args.bin)
    cmake.add('Tests', Paths.CASES)
    for c in scanTests():
        name, _ = os.path.splitext(c)
        name = os.path.basename(name)
        cmake.add(name, c)
    cmake.print()

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("-m", "--mode", default='show',help='possibilities:[show terraform]')
    parser.add_argument("-b", "--bin", default=None, help='.so location')
    args = parser.parse_args()

    if args.mode == 'show':
        show()
    elif args.mode == 'terraform':
        if args.bin == None:
            raise Exception('location of .so library was not specified')
        terraform(args)
