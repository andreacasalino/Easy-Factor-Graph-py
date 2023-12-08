- A) wait for the ci action named `wheels.yml` to create the artifacts with:
    - `*.tar.gz`
    - `*.whl`

- B) create `./dist` folder and copy the above content taken from the ci

- C) then
    - run python `python3 updatePackage.py` to trivially bump the current version and then upload to pypi
    - run python `python3 updatePackage.py -v THE_VERSION_YOU_WANT` to use the sepcified version and then upload to pypi

useful link: https://www.youtube.com/watch?v=tEFkHEKypLI
