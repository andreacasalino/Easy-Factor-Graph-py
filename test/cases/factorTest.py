from efg import Variable, Group, Factor, FactorExponential
from math import exp

import unittest

class FactorTests(unittest.TestCase):
    A = Variable(3, 'A')
    B = Variable(4, 'B')
    C = Variable(3, 'C')
    D = Variable(3, 'D')

    @staticmethod
    def makeCombKey(comb):
        return '-'.join([str(c) for c in comb])

    @staticmethod
    def extractFunction(factor):
        res = {}
        def emplace(comb, img):
            res[FactorTests.makeCombKey(comb)] = img
        factor.forEachNonNullCombination(emplace)
        return res
    
    @staticmethod
    def extractVars(factor):
        return [v.name() for v in factor.vars()]

    def test_simplyCorrelating(self):
        factor = Factor.makeSimplyCorrelated(Group.make([FactorTests.A, FactorTests.C, FactorTests.D]))
        fnct = FactorTests.extractFunction(factor)
        fnct_expected = {
            "0-0-0":1,
            "1-1-1":1,
            "2-2-2":1   
        }
        self.assertDictEqual(fnct, fnct_expected)
        self.assertEqual(['A', 'C', 'D'], FactorTests.extractVars(factor))

    def test_simplyAntiCorrelating(self):
        factor = Factor.makeSimplyAntiCorrelated(Group.make([FactorTests.A, FactorTests.C, FactorTests.D]))
        fnct = FactorTests.extractFunction(factor)
        fnct_expected = {}
        for a in range(0,3):
            for b in range(0,3):
                for c in range(0,3):
                    comb = [a,b,c]
                    if comb == [0,0,0] or comb == [1,1,1] or comb == [2,2,2]:
                        continue
                    fnct_expected[FactorTests.makeCombKey(comb)] = 1
        self.assertDictEqual(fnct, fnct_expected)
        self.assertEqual(['A', 'C', 'D'], FactorTests.extractVars(factor))

    def test_merge(self):
        factor_AC = Factor.makeSimplyCorrelated(Group.make([FactorTests.A, FactorTests.C]))
        factor_CD = Factor.makeSimplyCorrelated(Group.make([FactorTests.C, FactorTests.D]))
        factor = Factor.mergeFactors([factor_AC, factor_CD])
        fnct = FactorTests.extractFunction(factor)
        fnct_expected = {
            "0-0-0":1,
            "1-1-1":1,
            "2-2-2":1   
        }
        self.assertDictEqual(fnct, fnct_expected)
        vars = FactorTests.extractVars(factor)
        vars.sort()
        self.assertEqual(['A', 'C', 'D'], vars)

    def test_cloneWithPermutedGroup(self):
        factor = Factor.makeSimplyCorrelated(Group.make([FactorTests.A, FactorTests.C, FactorTests.D]))
        factor2 = factor.cloneWithPermutedGroup(Group.make([FactorTests.D, FactorTests.C, FactorTests.A]))
        self.assertEqual(['D','C','A'], FactorTests.extractVars(factor2))
        fnct = FactorTests.extractFunction(factor2)
        fnct_expected = {
            "0-0-0":1,
            "1-1-1":1,
            "2-2-2":1   
        }
        self.assertDictEqual(fnct, fnct_expected)

    def test_set_clear(self):
        factor = Factor(Group.make([FactorTests.A, FactorTests.C, FactorTests.D]))
        self.assertTrue(len(FactorTests.extractFunction(factor)) == 0)
        factor.set([0,1,1], 2)
        factor.set([0,2,1], 1.5)
        factor.set([1,0,1], 1)
        fnct = FactorTests.extractFunction(factor)
        fnct_expected = {
            "0-1-1":2,
            "0-2-1":1.5,
            "1-0-1":1   
        }
        self.assertDictEqual(fnct, fnct_expected)
        factor.clear()
        self.assertTrue(len(FactorTests.extractFunction(factor)) == 0)

    def test_replaceVariables(self):
        factor = Factor(Group.make([FactorTests.A, FactorTests.C, FactorTests.D]))
        factor.replaceVariables([Variable(3, 'A2'), Variable(3, 'C2'), Variable(3, 'D2')])
        self.assertEqual(['A2', 'C2', 'D2'], FactorTests.extractVars(factor))

    def test_makeFinder(self):
        factor = Factor.makeSimplyCorrelated(Group.make([FactorTests.A, FactorTests.C, FactorTests.D]))
        factor.set([0,1,1], 2)
        factor.set([0,2,1], 1.5)
        finder = factor.makeFinder([FactorTests.A, FactorTests.B, FactorTests.C, FactorTests.D])
        self.assertEqual(2, finder.findTransformed([0,0,1,1]))
        self.assertEqual(1.5, finder.findTransformed([0,0,2,1]))

    def test_getProbabilities(self):
        factor = Factor(Group.make([Variable(2, 'A'), Variable(3, 'B')]))
        factor.set([0,2], 2)
        factor.set([1,1], 1.5)
        prob = factor.getProbabilities()
        prob_expected = [0 for _ in range(0,6)]
        prob_expected[2] = float(2) / float(3.5)
        prob_expected[4] = float(1.5) / float(3.5)
        self.assertEqual(len(prob), len(prob_expected))
        for i in range(0, len(prob)):
            self.assertAlmostEqual(prob[i], prob_expected[i])

    #TODO test forEach methods

class FactorExponentialTests(unittest.TestCase):
    def test_getProbabilities(self):
        w = 1.3
        factor = FactorExponential(Factor.makeSimplyCorrelated(Group.make([Variable(2, 'A'), Variable(2, 'B')])), w) 
        prob = factor.getProbabilities()
        Z = 2 * (1 + exp(w))
        prob_expected = [exp(w) / Z, 1 / Z, 1 / Z,  exp(w) / Z]
        self.assertEqual(len(prob), len(prob_expected))
        for i in range(0, len(prob)):
            self.assertAlmostEqual(prob[i], prob_expected[i])

# if __name__ == '__main__':
#     unittest.main()
