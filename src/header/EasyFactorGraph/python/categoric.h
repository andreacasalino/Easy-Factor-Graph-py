#pragma once

#include <EasyFactorGraph/categoric/Group.h>
#include <EasyFactorGraph/categoric/GroupRange.h>
#include <EasyFactorGraph/categoric/Variable.h>

#include <EasyFactorGraph/python/Wrapper.h>

#include <functional>

namespace EFG::python {
class Variable : public Wrapper<categoric::Variable> {
public:
  using Wrapper<categoric::Variable>::Wrapper;

  std::size_t size() const { return wrapped_->size(); }
  std::string name() const { return wrapped_->name(); }
};

class Group : public categoric::Group {
public:
  Group(const std::vector<python::Variable> &group);
  static Group make(const std::vector<python::Variable> &group);

  Group(const python::Variable &var) : categoric::Group{var.get()} {};

  void replaceVariables(const std::vector<python::Variable> &new_variables);

  void add(const python::Variable &var) {
    this->categoric::Group::add(var.get());
  }

  std::vector<python::Variable> getVariables() const;
};

categoric::VariablesSoup make_soup(const std::vector<Variable> &group);

struct GroupRange {
  using Pred = std::function<void(const std::vector<std::size_t> &)>;

  static void for_each(const std::vector<Variable> &variables, Pred pred);

  static void for_each_from_sizes(const std::vector<std::size_t> &sizes,
                                  Pred pred);
};
} // namespace EFG::python
