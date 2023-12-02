#include <EasyFactorGraph/python/categoric.h>
#include <EasyFactorGraph/python/factor.h>
#include <EasyFactorGraph/python/model.h>

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
constexpr auto byref = py::return_value_policy::reference_internal;

PYBIND11_MODULE(efg, m) {
  m.attr("__version__") = "1.0.0";
  m.doc() = "TODO";

  //////////////// categoric ////////////////

  py::class_<EFG::python::Variable>(m, "Variable")
      .def(py::init<std::size_t, const std::string &>())
      .def("size", &EFG::python::Variable::size)
      .def("name", &EFG::python::Variable::name);

  py::class_<EFG::python::Group>(m, "Group")
      .def(py::init<const EFG::python::Variable &>())
      .def_static("make", &EFG::python::Group::make)
      .def("replaceVariables", &EFG::python::Group::replaceVariables)
      .def("add", &EFG::python::Group::add)
      .def("getVariables", &EFG::python::Group::getVariables)
      .def("size", &EFG::python::Group::size);

  py::class_<EFG::python::GroupRange>(m, "GroupRange")
      .def_static("for_each", &EFG::python::GroupRange::for_each)
      .def_static("for_each_from_sizes",
                  &EFG::python::GroupRange::for_each_from_sizes);

  //////////////// factor ////////////////

  py::class_<EFG::python::ImageFinder>(m, "ImageFinder")
      .def("findTransformed", &EFG::python::ImageFinder::findTransformed)
      .def("findImage", &EFG::python::ImageFinder::findImage);

  py::class_<EFG::python::ImmutablesAggregator>(m, "ImmutablesAggregator")
      .def(py::init<>())
      .def("addFactor", &EFG::python::ImmutablesAggregator::addFactor)
      .def("addFactorExp", &EFG::python::ImmutablesAggregator::addFactorExp);

  py::class_<EFG::python::Factor>(m, "Factor")
      .def(py::init<const EFG::python::Group &>())
      .def("vars", &EFG::python::Factor::vars)
      .def_static("cloneTrasformedFactor",
                  &EFG::python::Factor::cloneTrasformedFactor)
      .def_static("cloneTrasformedFactorExp",
                  &EFG::python::Factor::cloneTrasformedFactorExp)
      .def_static("makeSimplyCorrelated",
                  &EFG::python::Factor::makeSimplyCorrelated)
      .def_static("makeSimplyAntiCorrelated",
                  &EFG::python::Factor::makeSimplyAntiCorrelated)
      .def_static("merge", &EFG::python::Factor::merge)
      .def_static("mergeFactors", &EFG::python::Factor::mergeFactors)
      .def_static("mergeFactorsExp", &EFG::python::Factor::mergeFactorsExp)
      .def("cloneWithPermutedGroup",
           &EFG::python::Factor::cloneWithPermutedGroup)
      .def("makeFinder", &EFG::python::Factor::makeFinder)
      .def("getProbabilities", &EFG::python::Factor::getProbabilities)
      .def("set", &EFG::python::Factor::set)
      .def("clear", &EFG::python::Factor::clear)
      .def("replaceVariables", &EFG::python::Factor::replaceVariables)
      .def("forEachCombination", &EFG::python::Factor::forEachCombination)
      .def("forEachNonNullCombination",
           &EFG::python::Factor::forEachNonNullCombination)
      .def("forEachCombination2", &EFG::python::Factor::forEachCombination2)
      .def("forEachNonNullCombination2",
           &EFG::python::Factor::forEachNonNullCombination2);

  py::class_<EFG::python::FactorExponential>(m, "FactorExponential")
      .def(py::init<const EFG::python::Factor &, float>())
      .def("vars", &EFG::python::Factor::vars)
      .def("setWeight", &EFG::python::FactorExponential::setWeight)
      .def("getWeight", &EFG::python::FactorExponential::getWeight)
      .def("replaceVariables",
           &EFG::python::FactorExponential::replaceVariables)
      .def("makeFinder", &EFG::python::FactorExponential::makeFinder)
      .def("getProbabilities",
           &EFG::python::FactorExponential::getProbabilities)
      .def("forEachCombination", &EFG::python::Factor::forEachCombination)
      .def("forEachNonNullCombination",
           &EFG::python::Factor::forEachNonNullCombination)
      .def("forEachCombination2", &EFG::python::Factor::forEachCombination2)
      .def("forEachNonNullCombination2",
           &EFG::python::Factor::forEachNonNullCombination2);

  //////////////// model ////////////////

  py::class_<EFG::strct::PropagationContext>(m, "PropagationContext");

  m.def("make_prop_context", &EFG::python::make_prop_context);

  py::class_<EFG::strct::PropagationResult>(m, "PropagationResult");

  py::class_<EFG::python::SamplesGenerationContext>(m,
                                                    "SamplesGenerationContext")
      .def(py::init<std::size_t>())
      .def("setDeltaIterations",
           &EFG::python::SamplesGenerationContext::setDeltaIterations)
      .def("setSeed", &EFG::python::SamplesGenerationContext::setSeed)
      .def("setTransient",
           &EFG::python::SamplesGenerationContext::setTransient);

  py::class_<EFG::train::TrainSet>(m, "TrainSet");
  py::class_<EFG::train::TrainSet::Iterator>(m, "TrainSetIterator");

#define Bind_EvidenceBase(ModelT)                                              \
  .def("setEvidence", &ModelT::setEvidence)                                    \
      .def("setEvidenceByName", &ModelT::setEvidenceByName)                    \
      .def("removeEvidence", &ModelT::removeEvidence)                          \
      .def("removeEvidenceByName", &ModelT::removeEvidenceByName)              \
      .def("removeEvidences", &ModelT::removeEvidences)                        \
      .def("removeEvidencesByNames", &ModelT::removeEvidencesByNames)          \
      .def("removeAllEvidences", &ModelT::removeAllEvidences)

#define Bind_ModelBase(ModelT)                                                 \
  .def("getPropagationContext", &ModelT::getPropagationContext)                \
      .def("setPropagationContext", &ModelT::setPropagationContext)            \
      .def("hasPropagationResult", &ModelT::hasPropagationResult)              \
      .def("getLastPropagationResult", &ModelT::getLastPropagationResult)      \
      .def("getAllFactors", &ModelT::getAllFactors)                            \
      .def("getAllVariables", &ModelT::getAllVariables)                        \
      .def("getHiddenVariables", &ModelT::getHiddenVariables)                  \
      .def("getObservedVariables", &ModelT::getObservedVariables)              \
      .def("getEvidences", &ModelT::getEvidences)                              \
      .def("findVariable", &ModelT::findVariable)                              \
      .def("makeSamples", &ModelT::makeSamples, py::arg("context"),            \
           py::arg("threads") = 1)                                             \
      .def("getMarginalDistribution", &ModelT::getMarginalDistribution,        \
           py::arg("var"), py::arg("threads") = 1)                             \
      .def("getMarginalDistributionByName",                                    \
           &ModelT::getMarginalDistributionByName, py::arg("var"),             \
           py::arg("threads") = 1)                                             \
      .def("getJointMarginalDistribution",                                     \
           &ModelT::getJointMarginalDistribution, py::arg("subgroup"),         \
           py::arg("threads") = 1)                                             \
      .def("getJointMarginalDistributionByName",                               \
           &ModelT::getJointMarginalDistributionByName, py::arg("subgroup"),   \
           py::arg("threads") = 1)                                             \
      .def("getMAP", &ModelT::getMAP, py::arg("var"), py::arg("threads") = 1)  \
      .def("getMAPByName", &ModelT::getMAPByName, py::arg("var"),              \
           py::arg("threads") = 1)                                             \
      .def("getHiddenSetMAP", &ModelT::getHiddenSetMAP,                        \
           py::arg("threads") = 1)

#define Bind_FactorsConstInserter(ModelT)                                      \
  .def("addConstFactor", &ModelT::addConstFactor)                              \
      .def("addConstFactorExp", &ModelT::addConstFactorExp)                    \
      .def("copyConstFactor", &ModelT::copyConstFactor)                        \
      .def("copyConstFactorExp", &ModelT::copyConstFactorExp)

  py::class_<EFG::python::Graph>(m, "Graph")
      .def(py::init<>()) Bind_EvidenceBase(EFG::python::Graph)
          Bind_FactorsConstInserter(EFG::python::Graph)
              Bind_ModelBase(EFG::python::Graph);

#define Bind_FactorsTunableGetter(ModelT)                                      \
  .def("getTunableFactors", &ModelT::getTunableFactors)                        \
      .def("getTunableClusters", &ModelT::getTunableClusters)                  \
      .def("getWeights", &ModelT::getWeights)                                  \
      .def("setWeights", &ModelT::setWeights)                                  \
      .def("getWeightsGradient", &ModelT::getWeightsGradient,                  \
           py::arg("train_set_combinations"), py::arg("threads") = 1)          \
      .def("setOnes", &ModelT::setOnes)

#define Bind_FactorsTunableInserter(ModelT)                                    \
  .def("addTunableFactor", &ModelT::addTunableFactor)                          \
      .def("addTunableFactorSharingWeight",                                    \
           &ModelT::addTunableFactorSharingWeight)                             \
      .def("copyTunableFactor", &ModelT::copyTunableFactor)                    \
      .def("copyTunableFactorSharingWeight",                                   \
           &ModelT::copyTunableFactorSharingWeight)

  py::class_<EFG::python::RandomField>(m, "RandomField")
      .def(py::init<>()) Bind_EvidenceBase(EFG::python::RandomField)
          Bind_FactorsConstInserter(EFG::python::Graph)
              Bind_ModelBase(EFG::python::RandomField)
                  Bind_FactorsTunableGetter(EFG::python::RandomField)
                      Bind_FactorsTunableInserter(EFG::python::RandomField);

  py::class_<EFG::python::ConditionalRandomField>(m, "ConditionalRandomField")
      .def(py::init<const EFG::python::RandomField &, bool>())
      .def("setEvidences", &EFG::python::ConditionalRandomField::setEvidences)
      .def("makeTrainSet", &EFG::python::ConditionalRandomField::makeTrainSet,
           py::arg("context"), py::arg("range_percentage") = 1.f,
           py::arg("threads") = 1)
          Bind_ModelBase(EFG::python::ConditionalRandomField)
              Bind_FactorsTunableGetter(EFG::python::ConditionalRandomField);
}
