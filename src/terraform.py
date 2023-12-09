import argparse, os, re, json

class Paths:
    SOURCES = os.path.dirname(__file__)
    DEPS_FOLDER = os.path.join(SOURCES, 'external')
    META_DATA = os.path.join(SOURCES, '..', 'MetaData.json')

class CMakeLists:
    Location = os.path.join(Paths.DEPS_FOLDER, 'CMakeLists.txt')

    def __init__(self):
        with open(CMakeLists.Location, 'w') as stream:
            pass
        with open(Paths.META_DATA, 'r') as stream:
            self.deps = json.load(stream)['deps']

    def makeCMakeCaches_(self, info):
        res = ''
        for k, v in info.items():
            res += """set({} {} CACHE BOOL "" FORCE)\n""".format(k, "ON" if v else "OFF")
        return res

    def print(self):
        for name, info in self.deps.items():
            print('===> adding external target {} , version {}'.format(name, info['version']))
            cmd = '\n'
            if 'cmake_cache' in info:
                cmd += self.makeCMakeCaches_(info['cmake_cache'])
            cmd += 'FETCH_DEP({} {} {})\n'.format(info['cmake_name'], info['version'], info['url'])
            with open(CMakeLists.Location, 'a') as stream:
                stream.write(cmd)

def terraform():
    print('===> terraforming dependencies into {}'.format(Paths.DEPS_FOLDER))
    os.makedirs(Paths.DEPS_FOLDER, exist_ok=True)
    cmake = CMakeLists()
    cmake.print()

if __name__ == '__main__':
    terraform()
