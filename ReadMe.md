![binaries_compilation](https://github.com/andreacasalino/Easy-Factor-Graph-py/actions/workflows/runTests.yml/badge.svg)

[![Downloads](https://static.pepy.tech/badge/efg)](https://pepy.tech/project/efg)

Python bindings of [**Easy Factor Graph**](https://github.com/andreacasalino/Easy-Factor-Graph).
[**Easy Factor Graph**](https://github.com/andreacasalino/Easy-Factor-Graph) is a general purpose library handling undirected graphical models.
**Markov Random Fields** as well as **Conditional Random Fields** fall under this cathegory.
Refer to [*this documentation*](https://github.com/andreacasalino/Easy-Factor-Graph/blob/master/doc/EFG.pdf) for all the details.

Isn't great that you can have the access to the whole power of **Easy Factor Graph** inside python?? Leave a star in the [original repo](https://github.com/andreacasalino/Easy-Factor-Graph) please ;).


## USAGE

### FACTORS CONSTRUCTION

```python 
from efg import Variable

# define a couple of variables, with the same size
A = Variable(3, "A") # size is 3
B = Variable(3, "B") # size is 3
```

Such variables can be referred by the factors correlating them. In order to build a simple correlating factor this is what you would do:
```python 
from efg import Factor, Group

# build a simply correlating factor connecting the 2 variables
factor_AB = Factor.makeSimplyCorrelated(Group.make([A, B]))
```

And this is what you would do to generate an exponential simple correlating factor:
```python 
from efg import FactorExponential

# build an exponential factor using as base `factor_AB`: values of the
# images are assumed as exp(weight * images_factor_AB)
factor_AB_exponential = FactorExponential(factor_AB, 1.5) # 1.5 this will be the value assumed for the weight
```

You can also define custom factors, specifying the shape function that maps the values in their domain with their images. For example:
```python 
factor_BC = Factor.makeSimplyCorrelated(Group.make([B, Variable(3, 'C')]))
# set some individual images of factor_BC
# set for <0,1> -> 2
factor_BC.set([0,1], 2)
# set for <2,0> -> 1.3f
factor_BC.set([2,0], 1.3)
```

### MODELS CONSTRUCTION

Factor graphs can be built incrementally, passing one by one the factors that compose them. Without loss of generality suppose to start from an empty **random field**:
```python
from efg import RandomField

# start building an empty random field
model = RandomField()
```

then, you can build some factors and enrich the model with them:
```python
# define some variables, which will be later connected
A = Variable(4, "varA")
B = Variable(4, "varB")
C = Variable(4, "varC")

# without loss of generality, add to the model some simply correlating factors
factor_AB = Factor.makeSimplyCorrelated(Group.make([A, B]))
model.addConstFactor(factor_AB) # the same factor_AB is added to the model
model.copyConstFactor(Factor.makeSimplyCorrelated(Group.make([A, C]))) # a deep-copy of the factor is added into the model
```

The previously added factor are kept constant in the model. In order to enrich the model with a tunable factor you can call a different method:
```python
# build some additional tunable exponential factors that will be too added
factor_exp_BC = FactorExponential(Factor.makeSimplyCorrelated(Group.make([B, C])), 1.0)
model.addTunableFactor(factor_exp_BC)

D = Variable(4, "varD")
factor_exp_CD = FactorExponential(Factor.makeSimplyCorrelated(Group.make([C, D])), 1.5)
model.addTunableFactor(factor_exp_CD)
```

You can also add a tunable factor, that must share its weigth with an already inserted factor of the model:
```python
# insert another tunable factor, this time specifying that it needs to
# share the weight with already inserted exponential factor that connects B and C
model.addTunableFactorSharingWeight(FactorExponential(Factor.makeSimplyCorrelated(Group.make([C, D])), 
                                                      1.5 # actually this value is irrelevant, as the weight of factor_exp_BC will be assumed from now on
                                                     ),
                                    [B, C] # this additional input is to specify that this exponential factor needs to share the weight with the one connecting B and C
                                    )
```

You can also import the entire graph defined in an xml file (check [these samples](https://github.com/andreacasalino/Easy-Factor-Graph/tree/master/samples) for the expected format):
```python
from efg import xml as efg_xml

model = RandomField()
efg_xml.from_file(model, 'some/path/model.xml')
```

Similarly, you can also import the structure defined in a json
```python
import json
from efg import json as efg_json

model_json = json...

model = RandomField()
modelJson = efg_json.from_string(model, json.dumps(model_json))
```

### QUERY THE MODEL

A generated model can be queried in many ways. However, any query that you can do, is conditioned to the latest set of evidences.

Setting the evidences can be easily done by calling:
```python
# set some evidences
model.setEvidenceByName("variable_1", 0) # setting variable_1 = 0
model.setEvidenceByName("variable_2", 2) # setting variable_2 = 2
```

You can get the conditioned marginal distribution of a variable by calling:
```python
# get the marginal conditioned distribution of an hidden variable
conditioned_marginals = model.getMarginalDistributionByName("var_A")
```

Or you might be interested in the maximum a posteriori estimation of the entire evidence set:
```python
# get maxiomum a posteriori estimation of the entire hidden set
MAP_hidden_set = model.getHiddenSetMAP()
```

As already mentioned, results are subjected to the latest evidences set (which can be also empty). Of course, you can update the evidences and get the updated marginals:
```python
# set some new evidences
model.removeAllEvidences()
model.setEvidenceByName("evid_1", 1)

# compute new conditioned marginals: the should be different as the
# evidences were changed
conditioned_marginals = model.getMarginalDistributionByName("var_A")
```

### GIBBS SAMPLING

```python
from efg import SamplesGenerationContext

# some definitions to control the samples generation process
ctxt = SamplesGenerationContext()
ctxt.setDeltaIterations(1000) # samples number
ctxt.setSeed(0)               # seed used by random engines
ctxt.setTransient(500)        # number of iterations to discard at the beginning (burn out)

# get samples from the model using Gibbs sampler
samples = # this will be a list of lists [[0,2,1], [0,1,1], etc... ]
    model.makeSamples(info,
                        4 # threads to use
    )
```
