from efg import Variable, Group, Factor, FactorExponential, RandomField
from math import exp

import unittest

def make_dist(vals):
    c = float(0)
    for v in vals:
        c += v
    return [v / c for v in vals ]

class Factory:
    def __init__(self):
        self.vars = {}
    def getVar_(self, n):
        if not n in self.vars:
            self.vars[n] = Variable(2, n)
        return self.vars[n]
    def makeGroup_(self, a, b):
        return Group.make([self.getVar_(a), self.getVar_(b)])
    def makeFactor(self, a, b):
        return Factor.makeSimplyCorrelated(self.makeGroup_(a,b))
    def makeFactorExponential(self, a, b, w):
        return FactorExponential(self.makeFactor(a,b), w)

def make_chain(size, w_H = None, w_EH = None):
    if w_H == None:
        w_H = 1
    if w_EH == None:
        w_EH = 1
    if size < 1:
        raise Exception('Size should be at least 1')
    factory = Factory()
    model = RandomField()
    prev = 'H0'
    model.addConstFactorExp(factory.makeFactorExponential(prev, 'E0', w_EH))
    for k in range(1, size):
        current = 'H{}'.format(k)
        model.addConstFactorExp(factory.makeFactorExponential(current, 'E{}'.format(k), w_EH))
        model.addConstFactorExp(factory.makeFactorExponential(prev, current, w_H))
        prev = current
    return model

class VariablesTest(unittest.TestCase):
    @staticmethod
    def varNames(vars):
        retVal = [var.name() for var in vars]
        retVal.sort()
        return retVal

    def test_getAllVariables(self):
        self.assertEqual(VariablesTest.varNames(make_chain(3).getAllVariables()), ['E0', 'E1', 'E2', 'H0', 'H1', 'H2'])

    def test_findVariable(self):
        model = make_chain(3)
        for k in range(0, 3):          
            name = 'H{}'.format(k)
            self.assertEqual(model.findVariable(name).name(), name)

    def test_setEvidence(self):
        model = make_chain(3)
        self.assertEqual(VariablesTest.varNames(model.getHiddenVariables()), ['E0', 'E1', 'E2', 'H0', 'H1', 'H2'])
        self.assertEqual(model.getObservedVariables(), [])
        for k in range(0, 3):            
            model.setEvidenceByName('E{}'.format(k), 0)
        self.assertEqual(VariablesTest.varNames(model.getHiddenVariables()), ['H0', 'H1', 'H2'])
        self.assertEqual(VariablesTest.varNames(model.getObservedVariables()), ['E0', 'E1', 'E2'])

    def test_removeEvidence(self):
        model = make_chain(3)
        for k in range(0, 3):            
            model.setEvidenceByName('E{}'.format(k), 0)
        model.removeEvidenceByName('E0')
        self.assertEqual(VariablesTest.varNames(model.getHiddenVariables()), ['E0', 'H0', 'H1', 'H2'])
        self.assertEqual(VariablesTest.varNames(model.getObservedVariables()), ['E1', 'E2'])
        model.removeAllEvidences()
        self.assertEqual(VariablesTest.varNames(model.getHiddenVariables()), ['E0', 'E1', 'E2', 'H0', 'H1', 'H2'])
        self.assertEqual(model.getObservedVariables(), [])

    def test_getEvidences(self):
        model = make_chain(3)
        model.setEvidenceByName('E0', 0)
        model.setEvidenceByName('E1', 1)
        self.assertDictEqual(model.getEvidences(), {'E0':0, 'E1':1})

class WithMoreAssertions(unittest.TestCase):
    def assertAlmostEqualColls(self, a, b):
        if not len(a) == len(b):
            return False
        for k in range(0, len(a)):
            self.assertAlmostEqual(a[k], b[k])

class QueryTest(WithMoreAssertions):
    alfa = 1.3
    beta = 1.5
    a_exp = exp(alfa)
    b_exp = exp(beta)

    @staticmethod
    def make_model():
        gen = Factory()
        res = RandomField()
        res.addConstFactorExp(gen.makeFactorExponential('A', 'B', QueryTest.alfa)) 
        res.addConstFactorExp(gen.makeFactorExponential('B', 'C', QueryTest.beta))
        return res

    def test_getMarginalDistribution(self):
        model = QueryTest.make_model()
        model.setEvidenceByName('C', 0)
        self.assertAlmostEqualColls(model.getMarginalDistributionByName('A'), make_dist([1 + QueryTest.a_exp * QueryTest.b_exp , QueryTest.a_exp + QueryTest.b_exp]))

    def test_getJointMarginalDistribution(self):
        model = QueryTest.make_model()
        model.setEvidenceByName('C', 0)
        jointed = model.getJointMarginalDistributionByName(['A', 'B'])
        self.assertAlmostEqualColls(jointed.getProbabilities(), make_dist([QueryTest.a_exp * QueryTest.b_exp, 1, QueryTest.b_exp, QueryTest.a_exp]))

    def test_MAP(self):
        model = make_chain(4, 1, 2)
        model.setEvidenceByName('E0', 1)
        model.setEvidenceByName('E1', 0)
        model.setEvidenceByName('E2', 1)
        model.setEvidenceByName('E3', 0)
        self.assertEqual(model.getMAPByName('H0'), 1)
        self.assertEqual(model.getMAPByName('H1'), 0)
        self.assertEqual(model.getMAPByName('H2'), 1)
        self.assertEqual(model.getMAPByName('H3'), 0)

class FactorsManipulationTest(WithMoreAssertions):
    a = 1.2
    b = 0.7
    c = 1.5

    @staticmethod
    def make_model():
        gen = Factory()
        res = RandomField()
        res.addConstFactor(gen.makeFactor('C0', 'M')) 
        res.addTunableFactor(gen.makeFactorExponential('V0', 'M', FactorsManipulationTest.a)) 
        res.addTunableFactor(gen.makeFactorExponential('V1', 'M', FactorsManipulationTest.b)) 
        res.addTunableFactor(gen.makeFactorExponential('V1', 'E0', FactorsManipulationTest.c))
        res.addTunableFactorSharingWeight(gen.makeFactorExponential('V1', 'E1', FactorsManipulationTest.c), [gen.getVar_('E0'), gen.getVar_('V1')]) 
        return res

    @staticmethod
    def extractGroup(factor):
        return '-'.join(VariablesTest.varNames(factor.vars()))

    @staticmethod
    def extractGroups(factors):
        return [FactorsManipulationTest.extractGroup(factor) for factor in factors]

    def test_getAllFactors(self):
        factors, factorsExp = FactorsManipulationTest.make_model().getAllFactors()
        self.assertEqual(len(factors), 1)
        self.assertEqual(len(factorsExp), 4)
        self.assertEqual(FactorsManipulationTest.extractGroup(factors[0]), 'C0-M')
        self.assertCountEqual(FactorsManipulationTest.extractGroups(factorsExp), ['M-V0', 'M-V1', 'E0-V1', 'E1-V1'])

    def test_getTunableClusters(self):
        clusters = FactorsManipulationTest.make_model().getTunableClusters()
        groups = [FactorsManipulationTest.extractGroups(cluster) for cluster in clusters]
        self.assertEqual(groups, [['M-V0'], ['M-V1'], ['E0-V1', 'E1-V1']])

    def test_getWeights(self):
        model = FactorsManipulationTest.make_model()
        self.assertAlmostEqualColls(model.getWeights(), [FactorsManipulationTest.a, FactorsManipulationTest.b, FactorsManipulationTest.c])
        model.setWeights([2.2, 1.7, 2.5])
        self.assertAlmostEqualColls(model.getWeights(), [2.2, 1.7, 2.5])
        model.setOnes()
        self.assertAlmostEqualColls(model.getWeights(), [1,1,1])

# TODO getWeightsGradient

# if __name__ == '__main__':
#     unittest.main()
