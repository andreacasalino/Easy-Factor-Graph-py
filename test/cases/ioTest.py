from efg import Variable, Group, Factor, FactorExponential, RandomField

# import xml.etree.ElementTree as ET
# from efg import xml as efg_xml

import json
from efg import json as efg_json

import unittest

def make_factor(varA, varB, w=None):
    group = Group.make([varA, varB])
    res = Factor.makeSimplyCorrelated(group)
    if not w == None:
        res = FactorExponential(res, w)
    return res 

class TestBase(unittest.TestCase):
    A = Variable(2, 'A')
    B = Variable(2, 'B')
    C = Variable(2, 'C')
    model = RandomField()
    model.addConstFactorExp(make_factor(A, B, 1.3))
    model.addTunableFactor(make_factor(B, C, 1.6))
    model.addConstFactor(make_factor(A, C))

class JsonTests(TestBase):
    modelJson = json.loads(efg_json.to_string(TestBase.model))
    
    def test_conversion(self):
        self.check_variables()
        self.check_potentials()

    def check_variables(self):
        self.assertTrue('Variables' in JsonTests.modelJson)
        vars = JsonTests.modelJson['Variables']
        self.assertEqual(len(vars), 3)
        names = [var['name'] for var in vars]
        names.sort()
        self.assertEqual(names, ['A', 'B', 'C'])
        sizes = [var['Size'] for var in vars]
        self.assertEqual(sizes, ['2', '2', '2'])

    def find_potential(self, varA, varB):
        for pot in JsonTests.modelJson['Potentials']:
            if varA == pot['Variables'][0] and varB == pot['Variables'][1]:
                return pot
            if varA == pot['Variables'][1] and varB == pot['Variables'][0]:
                return pot
        msg = 'potential with variables [{}, {}] was not found'.format(varA, varB)
        raise Exception(msg)
    
    def assertContainsStr(self, subject, name, value):
        self.assertTrue(name in subject)
        self.assertEqual(subject[name], value)

    def assertContainsNumber(self, subject, name, value):
        self.assertTrue(name in subject)
        self.assertAlmostEqual(float(subject[name]), value)

    def check_potentials(self):
        self.assertTrue('Potentials' in JsonTests.modelJson)
        pots = JsonTests.modelJson['Potentials']
        self.assertEqual(len(pots), 3)

        potAB = self.find_potential('A', 'B')
        self.assertContainsStr(potAB, 'tunability', 'N')
        self.assertContainsNumber(potAB, 'weight', 1.3)

        potBC = self.find_potential('B', 'C')
        self.assertContainsStr(potBC, 'tunability', 'Y')
        self.assertContainsNumber(potBC, 'weight', 1.6)

        potAC = self.find_potential('A', 'C')
        self.assertTrue(not 'tunability' in potAC)
        self.assertTrue(not 'weight' in potAC)
        
# TODO
# class XmlTests(TestBase):
#   pass

# if __name__ == '__main__':
#     unittest.main()
