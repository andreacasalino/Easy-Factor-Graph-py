#pragma once

#include <EasyFactorGraph/model/ConditionalRandomField.h>
#include <EasyFactorGraph/model/Graph.h>
#include <EasyFactorGraph/model/RandomField.h>

#include <EasyFactorGraph/python/factor.h>

namespace EFG::python {
// strct::PropagationContext

strct::PropagationContext
make_prop_context(std::size_t max_iterations_loopy_propagation);

// strct::PropagationResult

namespace detail {
template <typename T> class ComponentAware {
protected:
  ComponentAware() = default;

  template <typename U> void init_(U &down_casted) { ref = &down_casted; }

protected:
  T *ref = nullptr;
};

template <typename... Ts> class ComponentsAware : ComponentAware<Ts>... {
protected:
  template <typename U> void init(U &down_casted) {
    (this->ComponentAware<Ts>::init_(down_casted), ...);
  }

  template <typename T> T &getAs() { return *this->ComponentAware<T>::ref; }
  template <typename T> const T &getAs() const {
    return *this->ComponentAware<T>::ref;
  }
};
} // namespace detail

class EvidenceBase : public detail::ComponentsAware<strct::EvidenceSetter,
                                                    strct::EvidenceRemover> {
public:
  void setEvidence(const Variable &variable, std::size_t value);
  void setEvidenceByName(const std::string &variable, std::size_t value);

  void removeEvidence(const Variable &variable);
  void removeEvidenceByName(const std::string &variable);
  void removeEvidences(const std::vector<Variable> &variables);
  void removeEvidencesByNames(const std::vector<std::string> &variables);
  void removeAllEvidences() {
    getAs<strct::EvidenceRemover>().removeAllEvidences();
  }
};

struct SamplesGenerationContext
    : public strct::GibbsSampler::SamplesGenerationContext {

  SamplesGenerationContext(std::size_t samples_number) {
    this->samples_number = samples_number;
  }

  void setDeltaIterations(std::size_t val) { delta_iterations = val; }
  void setSeed(std::size_t val) { seed = val; }
  void setTransient(std::size_t val) { transient = val; }
};

class ModelBase
    : public detail::ComponentsAware<strct::BeliefAware, strct::FactorsAware,
                                     strct::GibbsSampler, strct::QueryManager> {
public:
  strct::PropagationContext getPropagationContext() const {
    return getAs<strct::BeliefAware>().getPropagationContext();
  }
  void setPropagationContext(strct::PropagationContext ctxt) {
    getAs<strct::BeliefAware>().setPropagationContext(ctxt);
  }
  bool hasPropagationResult() const {
    return getAs<strct::BeliefAware>().hasPropagationResult();
  }
  strct::PropagationResult getLastPropagationResult() const {
    return getAs<strct::BeliefAware>().getLastPropagationResult();
  }

  std::pair<std::vector<Factor>, std::vector<FactorExponential>>
  getAllFactors() const;

  std::vector<Variable> getAllVariables() const;
  std::vector<Variable> getHiddenVariables() const;
  std::vector<Variable> getObservedVariables() const;
  std::unordered_map<std::string, std::size_t> getEvidences() const;
  Variable findVariable(const std::string &name) const;

  std::vector<std::vector<std::size_t>>
  makeSamples(const SamplesGenerationContext &context, std::size_t threads);

  std::vector<float> getMarginalDistribution(const Variable &var,
                                             std::size_t threads);
  std::vector<float> getMarginalDistributionByName(const std::string &var,
                                                   std::size_t threads);
  Factor getJointMarginalDistribution(const Group &subgroup,
                                      std::size_t threads);
  Factor
  getJointMarginalDistributionByName(const std::vector<std::string> &subgroup,
                                     std::size_t threads);
  std::size_t getMAP(const Variable &var, std::size_t threads);
  std::size_t getMAPByName(const std::string &var, std::size_t threads);
  std::vector<size_t> getHiddenSetMAP(std::size_t threads) {
    return getAs<strct::QueryManager>().getHiddenSetMAP(threads);
  }
};

class FactorsConstInserter
    : public detail::ComponentsAware<strct::FactorsConstInserter> {
public:
  void addConstFactor(const Factor &factor);
  void addConstFactorExp(const FactorExponential &factor);

  void copyConstFactor(const Factor &factor);
  void copyConstFactorExp(const FactorExponential &factor);
};

// class TrainSet;

// class TrainSet::Iterator;

class FactorsTunableGetter
    : public detail::ComponentsAware<train::FactorsTunableGetter> {
public:
  std::vector<FactorExponential> getTunableFactors() const;
  std::vector<std::vector<FactorExponential>> getTunableClusters() const;
  std::vector<float> getWeights() const {
    return getAs<train::FactorsTunableGetter>().getWeights();
  }
  void setWeights(const std::vector<float> &weights) {
    getAs<train::FactorsTunableGetter>().setWeights(weights);
  }
  std::vector<float>
  getWeightsGradient(const train::TrainSet::Iterator &train_set_combinations,
                     std::size_t threads) {
    return getAs<train::FactorsTunableGetter>().getWeightsGradient(
        train_set_combinations, threads);
  }

  void setOnes() { train::set_ones(getAs<train::FactorsTunableGetter>()); }
};

class FactorsTunableInserter
    : public detail::ComponentsAware<train::FactorsTunableInserter> {
public:
  void addTunableFactor(const FactorExponential &factor);
  void addTunableFactorSharingWeight(
      const FactorExponential &factor,
      const std::vector<Variable> &group_sharing_weight);
  void copyTunableFactor(const FactorExponential &factor);
  void copyTunableFactorSharingWeight(
      const FactorExponential &factor,
      const std::vector<Variable> &group_sharing_weight);
};

/////////////////////////////////////////////////////////////////////////////////////////

namespace detail {
template <typename ModelT, typename... Components>
class Model : public Components... {
public:
  template <typename... Args>
  Model(Args &&...args) : model_{std::forward<Args>(args)...} {
    (this->Components::init(model_), ...);
  }

  const auto &getModel() const { return model_; }
  auto &getModelMut() { return model_; }

  using ModelT_value = ModelT;

protected:
  ModelT model_;
};
} // namespace detail

using Graph =
    detail::Model<model::Graph, EvidenceBase, FactorsConstInserter, ModelBase>;

using RandomField =
    detail::Model<model::RandomField, EvidenceBase, FactorsConstInserter,
                  ModelBase, FactorsTunableGetter, FactorsTunableInserter>;

class ConditionalRandomField
    : public detail::Model<model::ConditionalRandomField, ModelBase,
                           FactorsTunableGetter> {
public:
  ConditionalRandomField(const RandomField &source, bool copy);

  void setEvidences(const std::vector<std::size_t> &values) {
    model_.setEvidences(values);
  }

  std::vector<std::vector<std::size_t>>
  makeTrainSet(const SamplesGenerationContext &context, float range_percentage,
               std::size_t threads) {
    return model_.makeTrainSet(context, range_percentage, threads);
  }
};
} // namespace EFG::python
