# useful link: https://www.youtube.com/watch?v=tEFkHEKypLI

- do the changes and open the PR

- bump version in MetaData.json before merging the PR

- wait for the github action named `wheels.yml` to create the artifacts with:
    - `*.tar.gz`
    - `*.whl`

- create `./dist` folder

- run python `python3 updatePackage.py`
