#pragma once

#include <EasyFactorGraph/factor/Factor.h>
#include <EasyFactorGraph/factor/FactorExponential.h>

#include <EasyFactorGraph/python/categoric.h>

#include <functional>
#include <variant>

namespace EFG::python {
class Factor;
class FactorExponential;

class ImageFinder : public Wrapper<factor::ImageFinder> {
public:
  ImageFinder(factor::ImageFinder &&finder);

  float findTransformed(const std::vector<std::size_t> &comb) const {
    return wrapped_->findTransformed(comb);
  }

  float findImage(const std::vector<std::size_t> &comb) const {
    return wrapped_->findImage(comb);
  }
};

class ImmutablesAggregator {
public:
  ImmutablesAggregator() = default;

  template <typename FactorT> void add(const FactorT &factor) {
    if constexpr (std::is_same_v<FactorT, Factor>) {
      addFactor(factor);
    } else if constexpr (std::is_same_v<FactorT, FactorExponential>) {
      addFactorExp(factor);
    } else {
      throw Error{"Invalid factor"};
    }
  }

  void addFactor(const Factor &factor);
  void addFactorExp(const FactorExponential &factor);

  std::vector<const factor::Immutable *> access() const;

private:
  std::vector<std::shared_ptr<const factor::Immutable>> elements;
};

class FactorBase {
public:
  void forEachCombination(
      std::function<void(const std::vector<std::size_t> &, float)> pred) const {
    immutable_->function().forEachCombination<true>(pred);
  }

  // this one don't transform
  void forEachCombination2(
      std::function<void(const std::vector<std::size_t> &, float)> pred) const {
    immutable_->function().forEachCombination<false>(pred);
  }

  void forEachNonNullCombination(
      std::function<void(const std::vector<std::size_t> &, float)> pred) const {
    immutable_->function().forEachNonNullCombination<true>(pred);
  }

  // this one don't transform
  void forEachNonNullCombination2(
      std::function<void(const std::vector<std::size_t> &, float)> pred) const {
    immutable_->function().forEachNonNullCombination<false>(pred);
  }

  ImageFinder makeFinder(const std::vector<Variable> &bigger_group) const;

  std::vector<float> getProbabilities() const {
    return immutable_->getProbabilities();
  }

  std::vector<Variable> vars() const;

protected:
  FactorBase(std::shared_ptr<const factor::Immutable> immutable)
      : immutable_{immutable} {}

  std::shared_ptr<const factor::Immutable> immutable_;
};

class Factor : public Wrapper<factor::Factor>, public FactorBase {
public:
  Factor(const Group &group);

  Factor(std::shared_ptr<factor::Factor> wrapped);

  static Factor cloneTrasformedFactor(const Factor &to_clone);
  static Factor cloneTrasformedFactorExp(const FactorExponential &to_clone);

  static Factor makeSimplyCorrelated(const Group &vars);

  static Factor makeSimplyAntiCorrelated(const Group &vars);

  static Factor merge(const ImmutablesAggregator &factors);
  static Factor mergeFactors(const std::vector<Factor> &factors);
  static Factor mergeFactorsExp(const std::vector<FactorExponential> &factors);

  Factor cloneWithPermutedGroup(const Group &new_order) const;

  void set(const std::vector<std::size_t> &comb, float value) {
    wrapped_->set(comb, value);
  }

  void clear() { wrapped_->clear(); }

  void replaceVariables(const std::vector<Variable> &new_variables);
};

class FactorExponential : public Wrapper<factor::FactorExponential>,
                          public FactorBase {
public:
  FactorExponential(const Factor &factor, float weigth);

  FactorExponential(std::shared_ptr<factor::FactorExponential> wrapped);

  void setWeight(float w) { wrapped_->setWeight(w); }

  float getWeight() const { return wrapped_->getWeight(); }

  void replaceVariables(const std::vector<Variable> &new_variables);
};
} // namespace EFG::python
