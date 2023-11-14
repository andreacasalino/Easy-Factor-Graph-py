#include <EasyFactorGraph/misc/Visitor.h>
#include <EasyFactorGraph/python/model.h>

namespace EFG::python {
strct::PropagationContext
make_prop_context(std::size_t max_iterations_loopy_propagation) {
  return strct::PropagationContext{max_iterations_loopy_propagation};
}

void EvidenceBase::setEvidence(const Variable &variable, std::size_t value) {
  getAs<strct::EvidenceSetter>().setEvidence(variable.get(), value);
}

void EvidenceBase::setEvidenceByName(const std::string &variable,
                                     std::size_t value) {
  getAs<strct::EvidenceSetter>().setEvidence(variable, value);
}

void EvidenceBase::removeEvidence(const Variable &variable) {
  getAs<strct::EvidenceRemover>().removeEvidence(variable.get());
}

void EvidenceBase::removeEvidenceByName(const std::string &variable) {
  getAs<strct::EvidenceRemover>().removeEvidence(variable);
}

void EvidenceBase::removeEvidences(const std::vector<Variable> &variables) {
  categoric::VariablesSet set;
  for (const auto &var : variables) {
    set.emplace(var.get());
  }
  getAs<strct::EvidenceRemover>().removeEvidences(set);
}

void EvidenceBase::removeEvidencesByNames(
    const std::vector<std::string> &variables) {
  getAs<strct::EvidenceRemover>().removeEvidences(
      std::unordered_set<std::string>{variables.begin(), variables.end()});
}

std::pair<std::vector<Factor>, std::vector<FactorExponential>>
ModelBase::getAllFactors() const {
  const auto &factorsAll = getAs<strct::FactorsAware>().getAllFactors();
  std::pair<std::vector<Factor>, std::vector<FactorExponential>> res;
  auto &[factors, factorsExp] = res;
  for (const auto &factor : factorsAll) {
    if (auto ptr = std::dynamic_pointer_cast<factor::Factor>(factor); ptr) {
      factors.emplace_back(ptr);
    } else {
      auto ptrExp =
          std::dynamic_pointer_cast<factor::FactorExponential>(factor);
      factorsExp.emplace_back(ptrExp);
    }
  }
  return res;
}

namespace {
template <typename VarsContainer>
std::vector<Variable> make_vars(const VarsContainer &vars) {
  std::vector<Variable> res;
  res.reserve(vars.size());
  for (const auto &var : vars) {
    res.emplace_back(var);
  }
  return res;
}
} // namespace

std::vector<Variable> ModelBase::getAllVariables() const {
  return make_vars(getAs<strct::BeliefAware>().getAllVariables());
}

std::vector<Variable> ModelBase::getHiddenVariables() const {
  return make_vars(getAs<strct::BeliefAware>().getHiddenVariables());
}

std::vector<Variable> ModelBase::getObservedVariables() const {
  return make_vars(getAs<strct::BeliefAware>().getObservedVariables());
}

std::unordered_map<std::string, std::size_t> ModelBase::getEvidences() const {
  std::unordered_map<std::string, std::size_t> res;
  for (const auto &[var, val] : getAs<strct::BeliefAware>().getEvidences()) {
    res.emplace(var.get()->name(), val);
  }
  return res;
}

Variable ModelBase::findVariable(const std::string &name) const {
  return getAs<strct::BeliefAware>().findVariable(name);
}

std::vector<std::vector<std::size_t>>
ModelBase::makeSamples(const SamplesGenerationContext &context,
                       std::size_t threads) {
  return getAs<strct::GibbsSampler>().makeSamples(context, threads);
}

std::vector<float> ModelBase::getMarginalDistribution(const Variable &var,
                                                      std::size_t threads) {
  return getAs<strct::QueryManager>().getMarginalDistribution(var.get(),
                                                              threads);
}

std::vector<float>
ModelBase::getMarginalDistributionByName(const std::string &var,
                                         std::size_t threads) {
  return getAs<strct::QueryManager>().getMarginalDistribution(var, threads);
}

Factor ModelBase::getJointMarginalDistribution(const Group &subgroup,
                                               std::size_t threads) {
  auto res = getAs<strct::QueryManager>().getJointMarginalDistribution(subgroup,
                                                                       threads);
  return Factor{std::make_shared<factor::Factor>(res)};
}

Factor ModelBase::getJointMarginalDistributionByName(
    const std::vector<std::string> &subgroup, std::size_t threads) {
  auto res = getAs<strct::QueryManager>().getJointMarginalDistribution(subgroup,
                                                                       threads);
  return Factor{std::make_shared<factor::Factor>(res)};
}

std::size_t ModelBase::getMAP(const Variable &var, std::size_t threads) {
  return getAs<strct::QueryManager>().getMAP(var.get(), threads);
}

std::size_t ModelBase::getMAPByName(const std::string &var,
                                    std::size_t threads) {
  return getAs<strct::QueryManager>().getMAP(var, threads);
}

void FactorsConstInserter::addConstFactor(const Factor &factor) {
  getAs<strct::FactorsConstInserter>().addConstFactor(factor.get());
}

void FactorsConstInserter::addConstFactorExp(const FactorExponential &factor) {
  getAs<strct::FactorsConstInserter>().addConstFactor(factor.get());
}

void FactorsConstInserter::copyConstFactor(const Factor &factor) {
  getAs<strct::FactorsConstInserter>().copyConstFactor(*factor.get());
}

void FactorsConstInserter::copyConstFactorExp(const FactorExponential &factor) {
  getAs<strct::FactorsConstInserter>().copyConstFactor(*factor.get());
}

namespace {
template <typename FactorT, typename Collection>
std::vector<FactorT> convert_collection(const Collection &subject) {
  std::vector<FactorT> res;
  for (const auto &ptr : subject) {
    res.emplace_back(ptr);
  }
  return res;
}
} // namespace

std::vector<FactorExponential> FactorsTunableGetter::getTunableFactors() const {
  return convert_collection<FactorExponential>(
      getAs<train::FactorsTunableGetter>().getTunableFactors());
}

std::vector<std::vector<FactorExponential>>
FactorsTunableGetter::getTunableClusters() const {
  std::vector<std::vector<FactorExponential>> res;
  for (const auto &cluster :
       getAs<train::FactorsTunableGetter>().getTunableClusters()) {
    VisitorConst<train::FactorExponentialPtr, train::TunableClusters>{
        [&res](const train::FactorExponentialPtr &ptr) {
          res.emplace_back().emplace_back(ptr);
        },
        [&res](const train::TunableClusters &collection) {
          res.emplace_back(convert_collection<FactorExponential>(collection));
        }}
        .visit(cluster);
  }
  return res;
}

void FactorsTunableInserter::addTunableFactor(const FactorExponential &factor) {
  getAs<train::FactorsTunableInserter>().addTunableFactor(factor.get());
}

namespace {
categoric::VariablesSet make_vars_set(const std::vector<Variable> &group) {
  categoric::VariablesSet res;
  for (const auto &var : group) {
    res.emplace(var.get());
  }
  return res;
}
} // namespace

void FactorsTunableInserter::addTunableFactorSharingWeight(
    const FactorExponential &factor,
    const std::vector<Variable> &group_sharing_weight) {
  getAs<train::FactorsTunableInserter>().addTunableFactor(
      factor.get(), make_vars_set(group_sharing_weight));
}

void FactorsTunableInserter::copyTunableFactor(
    const FactorExponential &factor) {
  getAs<train::FactorsTunableInserter>().copyTunableFactor(*factor.get());
}

void FactorsTunableInserter::copyTunableFactorSharingWeight(
    const FactorExponential &factor,
    const std::vector<Variable> &group_sharing_weight) {
  getAs<train::FactorsTunableInserter>().copyTunableFactor(
      *factor.get(), make_vars_set(group_sharing_weight));
}

ConditionalRandomField::ConditionalRandomField(const RandomField &source,
                                               bool copy)
    : detail::Model<model::ConditionalRandomField, ModelBase,
                    FactorsTunableGetter>{source.getModel(), copy} {}

} // namespace EFG::python
