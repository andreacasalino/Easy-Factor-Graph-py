#include <EasyFactorGraph/python/factor.h>

#include <EasyFactorGraph/misc/Visitor.h>

namespace EFG::python {
ImageFinder::ImageFinder(factor::ImageFinder &&finder)
    : Wrapper<factor::ImageFinder>{
          std::make_shared<factor::ImageFinder>(std::move(finder))} {}

std::vector<const factor::Immutable *> ImmutablesAggregator::access() const {
  std::vector<const factor::Immutable *> res;
  res.reserve(elements.size());
  for (const auto &ptr : elements) {
    res.push_back(ptr.get());
  }
  return res;
}

void ImmutablesAggregator::addFactor(const Factor &factor) {
  elements.push_back(factor.get());
}

void ImmutablesAggregator::addFactorExp(const FactorExponential &factor) {
  elements.push_back(factor.get());
}

ImageFinder
FactorBase::makeFinder(const std::vector<Variable> &bigger_group) const {
  return immutable_->makeFinder(make_soup(bigger_group));
}

std::vector<Variable> FactorBase::vars() const {
  std::vector<Variable> res;
  const auto &vars_ptr = immutable_->function().vars();
  res.reserve(res.size());
  for (const auto &v : vars_ptr.getVariables()) {
    res.emplace_back(v);
  }
  return res;
}

Factor::Factor(std::shared_ptr<factor::Factor> wrapped)
    : Wrapper<factor::Factor>(wrapped), FactorBase(wrapped) {}

Factor::Factor(const Group &group)
    : Factor(std::make_shared<factor::Factor>(group)) {}

Factor Factor::cloneTrasformedFactor(const Factor &to_clone) {
  return std::make_shared<factor::Factor>(
      *to_clone.get(), factor::Factor::CloneTrasformedImagesTag{});
}

Factor Factor::cloneTrasformedFactorExp(const FactorExponential &to_clone) {
  return std::make_shared<factor::Factor>(
      *to_clone.get(), factor::Factor::CloneTrasformedImagesTag{});
}

Factor Factor::makeSimplyCorrelated(const Group &vars) {
  return std::make_shared<factor::Factor>(
      vars, factor::Factor::SimplyCorrelatedTag{});
}

Factor Factor::makeSimplyAntiCorrelated(const Group &vars) {
  return std::make_shared<factor::Factor>(
      vars, factor::Factor::SimplyAntiCorrelatedTag{});
}

Factor Factor::merge(const ImmutablesAggregator &aggregator) {
  return std::make_shared<factor::Factor>(aggregator.access());
}

namespace {
template <typename T>
std::vector<const factor::Immutable *>
extract_immutables(const std::vector<T> &subject) {
  std::vector<const factor::Immutable *> res;
  res.reserve(subject.size());
  for (const auto &element : subject) {
    res.push_back(element.get().get());
  }
  return res;
}
} // namespace

Factor Factor::mergeFactors(const std::vector<Factor> &factors) {
  return std::make_shared<factor::Factor>(extract_immutables(factors));
}

Factor Factor::mergeFactorsExp(const std::vector<FactorExponential> &factors) {
  return std::make_shared<factor::Factor>(extract_immutables(factors));
}

Factor Factor::cloneWithPermutedGroup(const Group &new_order) const {
  return std::make_shared<factor::Factor>(
      wrapped_->cloneWithPermutedGroup(make_soup(new_order.getVariables())));
}

void Factor::replaceVariables(const std::vector<Variable> &new_variables) {
  wrapped_->replaceVariables(make_soup(new_variables));
}

FactorExponential::FactorExponential(const Factor &factor, float weigth)
    : Wrapper<factor::FactorExponential>(
          std::make_shared<factor::FactorExponential>(*factor.get(), weigth)),
      FactorBase{wrapped_} {}

FactorExponential::FactorExponential(
    std::shared_ptr<factor::FactorExponential> wrapped)
    : Wrapper<factor::FactorExponential>(wrapped), FactorBase(wrapped) {}

void FactorExponential::replaceVariables(
    const std::vector<Variable> &new_variables) {
  wrapped_->replaceVariables(make_soup(new_variables));
}
} // namespace EFG::python
