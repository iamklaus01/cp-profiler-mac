#ifndef CPPROFILER_EXECUTION_HH
#define CPPROFILER_EXECUTION_HH

#include "solver_data.hh"
#include "tree/node.hh"
#include "tree/node_tree.hh"
#include "user_data.hh"
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

namespace cpprofiler
{

class Execution
{

    const ExecID id_;

    const std::string m_name;

    std::unique_ptr<tree::NodeTree> m_tree;

    std::shared_ptr<SolverData> solver_data_;

    std::shared_ptr<const NameMap> name_map_;

    std::unique_ptr<UserData> user_data_;

    /// Whether the execution contains restarts
    bool m_is_restarts;

  public:
    std::string name();

    ExecID id() { return id_; }

    explicit Execution(const std::string &name, ExecID id = 0, bool restarts = false);

    void setNameMap(std::shared_ptr<const NameMap> nm);

    SolverData &solver_data();
    const SolverData &solver_data() const;

    inline UserData &userData() { return *user_data_; }
    inline const UserData &userData() const { return *user_data_; };

    tree::NodeTree &tree();
    const tree::NodeTree &tree() const;

    bool doesRestarts() const;
};

} // namespace cpprofiler

#endif
