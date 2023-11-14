from efg import Variable, Group, GroupRange

import unittest

class VariableTests(unittest.TestCase):
    def test_getters(self):
        var = Variable(4, 'foo')
        self.assertEqual(var.name(), 'foo')
        self.assertEqual(var.size(), 4)

#TODO negativetests

class GroupTests(unittest.TestCase):
    varA = Variable(4, 'AA')
    varB = Variable(3, 'BB')

    def test_ctor(self):
        group = Group(GroupTests.varA)
        self.assertEqual(group.size(), 4)

    def test_size(self):
        group = Group.make([GroupTests.varA, GroupTests.varB])
        self.assertEqual(group.size(), 12)

    def test_getVariables(self):
        group = Group.make([GroupTests.varA, GroupTests.varB])
        vars = group.getVariables()
        self.assertEqual(len(vars), 2)
        self.assertEqual(vars[0].name(), 'AA')
        self.assertEqual(vars[1].name(), 'BB')

    def test_replaceVariables(self):
        group = Group.make([GroupTests.varA, GroupTests.varB])
        vars2 = [Variable(4,'CC'), Variable(3,'DD')]
        group.replaceVariables(vars2)
        vars = group.getVariables()
        self.assertEqual(len(vars), 2)
        self.assertEqual(vars[0].name(), 'CC')
        self.assertEqual(vars[1].name(), 'DD')

    def test_add(self):
        group = Group.make([GroupTests.varA, GroupTests.varB])
        group.add(Variable(2, 'EE'))
        self.assertEqual(group.size(), 24)
        self.assertEqual(len(group.getVariables()), 3)

class GroupRangeTests(unittest.TestCase):
    sizes = [2,3,3]
    combs_expected = [
        [0,0,0],
        [0,0,1],
        [0,0,2],
        [0,1,0],
        [0,1,1],
        [0,1,2],
        [0,2,0],
        [0,2,1],
        [0,2,2],
        [1,0,0],
        [1,0,1],
        [1,0,2],
        [1,1,0],
        [1,1,1],
        [1,1,2],
        [1,2,0],
        [1,2,1],
        [1,2,2],
    ]

    def test_for_each(self):
        count = 0
        vars = []
        for s in GroupRangeTests.sizes:
            vars.append(Variable(s, 'var-{}'.format(count)))
            count += 1
        combs = []
        GroupRange.for_each(vars, lambda c : combs.append(c))
        self.assertEqual(GroupRangeTests.combs_expected, combs)

    def test_for_each_from_sizes(self):
        combs = []
        GroupRange.for_each_from_sizes(GroupRangeTests.sizes, lambda c : combs.append(c))
        self.assertEqual(GroupRangeTests.combs_expected, combs)

# if __name__ == '__main__':
#     unittest.main()
