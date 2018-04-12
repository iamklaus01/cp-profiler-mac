#include "tree_builder.hh"

#include "cpp-integration/message.hpp"

#include "utils/perf_helper.hh"
#include "utils/debug.hh"
#include "execution.hh"

#include <thread>

namespace cpprofiler {

static std::ostream& operator<<(std::ostream& os, const NodeUID& uid) {
    return os << "{" << uid.nid << ", " << uid.rid << ", " << uid.tid << "}";
}

static std::ostream& operator<<(std::ostream& os, const NodeStatus& status) {
    switch (status) {
        case SOLVED: os << "SOLVED"; break;
        case FAILED: os << "FAILED"; break;
        case BRANCH: os << "BRANCH"; break;
        case SKIPPED: os << "SKIPPED"; break;
    }
    return os;
}

/// works correctly for node messages only atm
static std::ostream& operator<<(std::ostream& os, const Message& msg) {
    os << "nid: " << msg.nodeUID() << ", pid: " << msg.parentUID();
    os << ", alt: " << msg.alt() << ", kids: " << msg.kids();
    os << ", " << msg.status();
    // if (msg.has_label()) os << ", label: " << msg.label();
    // if (msg.has_nogood()) os << ", nogood: " << msg.nogood();
    // if (msg.has_info()) os << ", info: " << msg.info();
    return os;
}

void TreeBuilder::startBuilding(Execution* e) {
    m_execution = e;
    perfHelper.begin("tree building");
    std::cerr << "  Builder: start building\n";
}

void TreeBuilder::finishBuilding() {
    perfHelper.end();
    debug("done") << "  Builder: done building\n";
}

void TreeBuilder::handleNode(Message* node) {

    static bool done = false;

    if (!done) {
        debug("thread") << "BuilderThread:handleNode thread:" << std::this_thread::get_id() << std::endl;
        done = true;
    }

    // qDebug() << "handle node";

    std::unique_ptr<Message> node_msg{node};
    // std::cerr << *node << std::endl;

    auto n_uid = node->nodeUID();
    auto p_uid = node->parentUID();

    auto& tree = m_execution->tree();

    tree::NodeID pid = tree::NodeID::NoNode;

    /// who else has access to solver_data?
    if (p_uid.nid != -1) {
        pid = m_execution->solver_data().getNodeId({p_uid.nid, p_uid.rid, p_uid.tid});
    }


    const auto kids = node->kids();
    const auto alt = node->alt();
    const auto status = static_cast<tree::NodeStatus>(node->status());

    NodeID nid;

    {
        utils::MutexLocker tree_lock(&tree.treeMutex(), "builder");

        if (pid == NodeID::NoNode) {

            if (p_uid.rid == -1) {
                nid = tree.createRoot(kids);
            } else {
                nid = tree.addNodeNew(NodeID{0}, n_uid.rid, kids, status);
            }

        } else {
            nid = tree.addNodeNew(pid, alt, kids, status);
        }
    }

    m_execution->solver_data().setNodeId({n_uid.nid, n_uid.rid, n_uid.tid}, nid);

}

}