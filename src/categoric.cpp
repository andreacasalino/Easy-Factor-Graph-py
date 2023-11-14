#include <EasyFactorGraph/python/categoric.h>

namespace EFG::python {
categoric::VariablesSoup make_soup(const std::vector<Variable> &group) {
  categoric::VariablesSoup res;
  res.reserve(group.size());
  for (const auto &var : group) {
    res.push_back(var.get());
  }
  return res;
}

Group::Group(const std::vector<python::Variable> &group)
    : categoric::Group{make_soup(group)} {}

Group Group::make(const std::vector<python::Variable> &group) {
  return Group{group};
}

void Group::replaceVariables(
    const std::vector<python::Variable> &new_variables) {
  this->categoric::Group::replaceVariables(make_soup(new_variables));
}

std::vector<python::Variable> Group::getVariables() const {
  std::vector<python::Variable> res;
  for (const auto &var : categoric::Group::getVariables()) {
    res.emplace_back(var);
  }
  return res;
}

void GroupRange::for_each(const std::vector<Variable> &variables, Pred pred) {
  categoric::Group g{make_soup(variables)};
  categoric::GroupRange r{g};
  categoric::for_each_combination(r, pred);
}

void GroupRange::for_each_from_sizes(const std::vector<std::size_t> &sizes,
                                     Pred pred) {
  categoric::GroupRange r{sizes};
  categoric::for_each_combination(r, pred);
}

} // namespace EFG::python
